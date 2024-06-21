#ifndef __MOTOR_TASK_H__
#define __MOTOR_TASK_H__
#include <Arduino.h>
#include <SimpleFOC.h>
#include "Wire.h"
#include <SPI.h>
#include "EEPROM.h"

#define MO1 GPIO_NUM_17
#define MO2 GPIO_NUM_16
#define MO3 GPIO_NUM_15
static const int spiClk = 1000000; // 400KHz
SPIClass* hspi = NULL;
#define MT6701_SDA 1
#define MT6701_SCL 2
#define MT6701_SS 42
uint8_t sleep_flag = 0;
xQueueHandle knob_state_queue_;
xQueueHandle queue_;
struct KnobConfig {
  int32_t num_positions;
  int32_t position;
  float position_width_radians;
  float detent_strength_unit;
  float endstop_strength_unit;
  float snap_point;
};
struct KnobState {
  int32_t current_position;
  float sub_position_unit;
  KnobConfig config;
};
enum class CommandType {
  CONFIG,
  HAPTIC,
};
struct HapticData {
  bool press;
};
struct Command {
  CommandType command_type;
  union CommandData {
    KnobConfig config;
    HapticData haptic;
  };
  CommandData data;
};
void publish(const KnobState& state);
void calibrate();
template<typename T> T CLAMP(const T& value, const T& low, const T& high) {
  return value < low ? low : (value > high ? high : value);
}
static const float DEAD_ZONE_DETENT_PERCENT = 0.2;
static const float DEAD_ZONE_RAD = 1 * _PI / 180;

static const float IDLE_VELOCITY_EWMA_ALPHA = 0.001;
static const float IDLE_VELOCITY_RAD_PER_SEC = 0.05;
static const uint32_t IDLE_CORRECTION_DELAY_MILLIS = 500;
static const float IDLE_CORRECTION_MAX_ANGLE_RAD = 5 * PI / 180;
static const float IDLE_CORRECTION_RATE_ALPHA = 0.0005;

//目标变量
float readMySensorCallback() {
    hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
    digitalWrite(hspi->pinSS(), LOW); //pull SS slow to prep other end for transfer
    uint16_t ag = hspi->transfer16(0);
    digitalWrite(hspi->pinSS(), HIGH); //pull ss high to signify end of data transfer
    hspi->endTransaction();
    ag = ag >> 2;
    float rad = (float)ag * 2 * PI / 16384;
    if (rad < 0) {
        rad += 2 * PI;
    }
  return rad;
}
void initMySensorCallback() {
  // do the init
    hspi = new SPIClass(HSPI);
    hspi->begin(MT6701_SCL, MT6701_SDA, -1, MT6701_SS); //SCLK, MISO, MOSI, SS
    pinMode(hspi->pinSS(), OUTPUT); //HSPI SS
}
BLDCMotor motor = BLDCMotor(7);
BLDCDriver3PWM driver = BLDCDriver3PWM(MO1, MO3, MO2);
GenericSensor sensor = GenericSensor(readMySensorCallback, initMySensorCallback);
float idle_check_velocity_ewma = 0;
uint32_t last_idle_start = 0;
uint32_t last_debug = 0;
float target_velocity = 0;
float current_detent_center, last_angle;
uint32_t last_publish = 0;

KnobConfig config = {
  .num_positions = 0,
  .position = 0,
  .position_width_radians = 1 * _PI / 180,
  .detent_strength_unit = 1,
  .endstop_strength_unit = 1,
  .snap_point = 1.1,
};
void init_foc()
{
  sensor.init();
  //连接motor对象与传感器对象
  motor.linkSensor(&sensor);
  // PWM 频率 [Hz]
  driver.pwm_frequency = 50000;
  //供电电压设置 [V]

  driver.voltage_power_supply = 5;
  driver.init();
  motor.linkDriver(&driver);
  //FOC模型选择
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
  //运动控制模式设置
  motor.controller = MotionControlType::torque;
  //速度PI环设置
  motor.PID_velocity.P = 2;
  motor.PID_velocity.I = 0;
  motor.PID_velocity.D = 0.08;
  motor.PID_velocity.output_ramp = 10000;
  motor.PID_velocity.limit = 10;
  //最大电机限制电机
  motor.voltage_limit = 5;
  //速度低通滤波时间常数
  motor.LPF_velocity.Tf = 0.01;
  //设置最大速度限制
  motor.velocity_limit = 40;

  motor.useMonitoring(Serial);
  //初始化电机
  motor.init();
  //初始化 FOC 
  // calibrate();
 if(isnan(EEPROM.readFloat(0))||EEPROM.readFloat(0)>=100)
  {
      if(motor.initFOC())
      {
        Serial.println(motor.pole_pairs);
        motor.pole_pairs = 7;
      }
  }
  else
  {
    float zero_electric_offset = EEPROM.readFloat(0);
    motor.pole_pairs = 7;
    Direction foc_direction = Direction::CW;
    // motor.initFOC(zero_electric_offset, foc_direction);
    motor.initFOC();  //Vincent
  }

  //   calibrate();
  motor.monitor_downsample = 0;  // disable monitor at first - optional
  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target velocity using serial terminal:"));

  current_detent_center = motor.shaft_angle;
}
void motor_task_init() {

    init_foc();
  // Update derivative factor of torque controller based on detent width.
  // If the D factor is large on coarse detents, the motor ends up making noise because the P&D factors amplify the noise from the sensor.
  // This is a piecewise linear function so that fine detents (small width) get a higher D factor and coarse detents get a small D factor.
  // Fine detents need a nonzero D factor to artificially create "clicks" each time a new value is reached (the P factor is small
  // for fine detents due to the smaller angular errors, and the existing P factor doesn't work well for very small angle changes (easy to
  // get runaway due to sensor noise & lag)).
  // TODO: consider eliminating this D factor entirely and just "play" a hardcoded haptic "click" (e.g. a quick burst of torque in each
  // direction) whenever the position changes when the detent width is too small for the P factor to work well.
  const float derivative_lower_strength = config.detent_strength_unit * 0.08;
  const float derivative_upper_strength = config.detent_strength_unit * 0.02;
  const float derivative_position_width_lower = radians(3);
  const float derivative_position_width_upper = radians(8);
  const float raw = derivative_lower_strength + (derivative_upper_strength - derivative_lower_strength) / (derivative_position_width_upper - derivative_position_width_lower) * (config.position_width_radians - derivative_position_width_lower);
  motor.PID_velocity.D = CLAMP(
                           raw,
                           min(derivative_lower_strength, derivative_upper_strength),
                           max(derivative_lower_strength, derivative_upper_strength));
}
void motor_run(void* parameter) {
  motor_task_init();
  while (1) {
    if (sleep_flag == 0) {
      Command command;
      if (xQueueReceive(queue_, &command, 0) == pdTRUE) {
        switch (command.command_type) {
          case CommandType::CONFIG:
            {
              config = command.data.config;
              Serial.println("Got new config");
              current_detent_center = motor.shaft_angle;
#if SK_INVERT_ROTATION
              current_detent_center = -motor.shaft_angle;
#endif

              // Update derivative factor of torque controller based on detent width.
              // If the D factor is large on coarse detents, the motor ends up making noise because the P&D factors amplify the noise from the sensor.
              // This is a piecewise linear function so that fine detents (small width) get a higher D factor and coarse detents get a small D factor.
              // Fine detents need a nonzero D factor to artificially create "clicks" each time a new value is reached (the P factor is small
              // for fine detents due to the smaller angular errors, and the existing P factor doesn't work well for very small angle changes (easy to
              // get runaway due to sensor noise & lag)).
              // TODO: consider eliminating this D factor entirely and just "play" a hardcoded haptic "click" (e.g. a quick burst of torque in each
              // direction) whenever the position changes when the detent width is too small for the P factor to work well.
              const float derivative_lower_strength = config.detent_strength_unit * 0.08;
              const float derivative_upper_strength = config.detent_strength_unit * 0.02;
              const float derivative_position_width_lower = radians(3);
              const float derivative_position_width_upper = radians(8);
              const float raw = derivative_lower_strength + (derivative_upper_strength - derivative_lower_strength) / (derivative_position_width_upper - derivative_position_width_lower) * (config.position_width_radians - derivative_position_width_lower);
              motor.PID_velocity.D = CLAMP(
                                       raw,
                                       min(derivative_lower_strength, derivative_upper_strength),
                                       max(derivative_lower_strength, derivative_upper_strength));
              break;
            }
          case CommandType::HAPTIC:
            {
              float strength = command.data.haptic.press ? 5 : 1.5;
              motor.move(strength);
              for (uint8_t i = 0; i < 3; i++) {
                motor.loopFOC();
                delay(2);
              }
              motor.move(-strength);
              for (uint8_t i = 0; i < 3; i++) {
                motor.loopFOC();
                delay(2);
              }
              motor.move(0);
              motor.loopFOC();
              break;
            }
        }
      }
      sensor.update();
      idle_check_velocity_ewma = motor.shaft_velocity * IDLE_VELOCITY_EWMA_ALPHA + idle_check_velocity_ewma * (1 - IDLE_VELOCITY_EWMA_ALPHA);
      if (fabsf(idle_check_velocity_ewma) > IDLE_VELOCITY_RAD_PER_SEC) {
        last_idle_start = 0;
      } else {
        if (last_idle_start == 0) {
          last_idle_start = millis();
        }
      }

      // If we are not moving and we're close to the center (but not exactly there), slowly adjust the centerpoint to match the current position
      if (last_idle_start > 0 && millis() - last_idle_start > IDLE_CORRECTION_DELAY_MILLIS && fabsf(motor.shaft_angle - current_detent_center) < IDLE_CORRECTION_MAX_ANGLE_RAD) {
        current_detent_center = motor.shaft_angle * IDLE_CORRECTION_RATE_ALPHA + current_detent_center * (1 - IDLE_CORRECTION_RATE_ALPHA);
        if (millis() - last_debug > 100) {
            last_debug = millis();
            Serial.print("Moving detent center. ");
            Serial.print(current_detent_center);
            Serial.print(" ");
            Serial.println(motor.shaft_angle);
        }
      }

      float angle_to_detent_center = motor.shaft_angle - current_detent_center;
#if SK_INVERT_ROTATION
      angle_to_detent_center = -motor.shaft_angle - current_detent_center;
#endif
      if (angle_to_detent_center > config.position_width_radians * config.snap_point && (config.num_positions <= 0 || config.position > 0)) {
        current_detent_center += config.position_width_radians;
        angle_to_detent_center -= config.position_width_radians;
        config.position--;
      } else if (angle_to_detent_center < -config.position_width_radians * config.snap_point && (config.num_positions <= 0 || config.position < config.num_positions - 1)) {
        current_detent_center -= config.position_width_radians;
        angle_to_detent_center += config.position_width_radians;
        config.position++;
      }

      float dead_zone_adjustment = CLAMP(
                                     angle_to_detent_center,
                                     fmaxf(-config.position_width_radians * DEAD_ZONE_DETENT_PERCENT, -DEAD_ZONE_RAD),
                                     fminf(config.position_width_radians * DEAD_ZONE_DETENT_PERCENT, DEAD_ZONE_RAD));

      bool out_of_bounds = config.num_positions > 0 && ((angle_to_detent_center > 0 && config.position == 0) || (angle_to_detent_center < 0 && config.position == config.num_positions - 1));
      motor.PID_velocity.limit = 10;  //out_of_bounds ? 10 : 3;
      motor.PID_velocity.P = out_of_bounds ? config.endstop_strength_unit * 4 : config.detent_strength_unit * 4;

      if (fabsf(motor.shaft_velocity) > 60) {
        // Don't apply torque if velocity is too high (helps avoid positive feedback loop/runaway)
        motor.move(0);
      } else {
        float torque = motor.PID_velocity(-angle_to_detent_center + dead_zone_adjustment);
#if SK_INVERT_ROTATION
        torque = -torque;
#endif
        // Serial.print(torque);
        // Serial.print("-");
        // Serial.print(-angle_to_detent_center);
        // Serial.print("-");
        // Serial.println(config.position);
        motor.move(torque);
      }
      if (millis() - last_publish > 20) {
        publish({
          .current_position = config.position,
          .sub_position_unit = -angle_to_detent_center / config.position_width_radians,
          .config = config,
        });
        last_publish = millis();
      }
      motor.loopFOC();
      motor.monitor();
      //  Serial.println(millis());
    }
    // taskYIELD();
     vTaskDelay(1);
  }

}
void setConfig(const KnobConfig& config) {
  Command command = {
    .command_type = CommandType::CONFIG,
    .data = {
      .config = config,
    }
  };
  xQueueSend(queue_, &command, portMAX_DELAY);
}
void playHaptic(bool press) {
  Command command = {
    .command_type = CommandType::HAPTIC,
    .data = {
      .haptic = {
        .press = press,
      },
    }
  };
  xQueueSend(queue_, &command, portMAX_DELAY);
}
void publish(const KnobState& state) {
  xQueueOverwrite(knob_state_queue_, &state);
}
void calibrate() {
  // SimpleFOC is supposed to be able to determine this automatically (if you omit params to initFOC), but
  // it seems to have a bug (or I've misconfigured it) that gets both the offset and direction very wrong!
  // So this value is based on experimentation.
  // TODO: dig into SimpleFOC calibration and find/fix the issue

  Serial.println("\n\n\nStarting calibration, please do not touch to motor until complete!");

  motor.controller = MotionControlType::angle_openloop;
  motor.pole_pairs = 1;
  // motor.initFOC(0, Direction::CW);
  motor.initFOC();  //Vincent

  float a = 0;

  // #### Determine direction motor rotates relative to angle sensor
  for (uint8_t i = 0; i < 200; i++) {
    sensor.update();
    motor.move(a);
    delay(1);
  }
  float start_sensor = sensor.getAngle();

  for (; a < 3 * _2PI; a += 0.01) {
    sensor.update();
    motor.move(a);
    delay(1);
  }

  for (uint8_t i = 0; i < 200; i++) {
    sensor.update();
    delay(1);
  }
  float end_sensor = sensor.getAngle();


  motor.voltage_limit = 0;
  motor.move(a);

  Serial.println();

  // TODO: check for no motor movement!

  Serial.print("Sensor measures positive for positive motor rotation: ");
  if (end_sensor > start_sensor) {
    Serial.println("YES, Direction=CW");
    // motor.initFOC(0, Direction::CW);
    motor.initFOC();  //Vincent
  } else {
    Serial.println("NO, Direction=CCW");
    // motor.initFOC(0, Direction::CCW);
    motor.initFOC();  //Vincent
  }


  // #### Determine pole-pairs
  // Rotate 20 electrical revolutions and measure mechanical angle traveled, to calculate pole-pairs
  uint8_t electrical_revolutions = 20;
  Serial.printf("Going to measure %d electrical revolutions...\n", electrical_revolutions);
  motor.voltage_limit = 5;
  motor.move(a);
  Serial.println("Going to electrical zero...");
  float destination = a + _2PI;
  for (; a < destination; a += 0.03) {
    sensor.update();
    motor.move(a);
    delay(1);
  }
  Serial.println("pause..."); // Let momentum settle...
  for (uint16_t i = 0; i < 1000; i++) {
    sensor.update();
    delay(1);
  }
  Serial.println("Measuring...");

  start_sensor = motor.sensor_direction * sensor.getAngle();
  destination = a + electrical_revolutions * _2PI;
  for (; a < destination; a += 0.03) {
    sensor.update();
    motor.move(a);
    delay(1);
  }
  for (uint16_t i = 0; i < 1000; i++) {
    sensor.update();
    motor.move(a);
    delay(1);
  }
  end_sensor = motor.sensor_direction * sensor.getAngle();
  motor.voltage_limit = 0;
  motor.move(a);

  if (fabsf(motor.shaft_angle - motor.target) > 1 * PI / 180) {
    Serial.println("ERROR: motor did not reach target!");
    while (1) {}
  }

  float electrical_per_mechanical = electrical_revolutions * _2PI / (end_sensor - start_sensor);
  Serial.print("Electrical angle / mechanical angle (i.e. pole pairs) = ");
  Serial.println(electrical_per_mechanical);

  int measured_pole_pairs = (int)round(electrical_per_mechanical);
  Serial.printf("Pole pairs set to %d\n", measured_pole_pairs);

  delay(1000);


  // #### Determine mechanical offset to electrical zero
  // Measure mechanical angle at every electrical zero for several revolutions
  motor.voltage_limit = 5;
  motor.move(a);
  float offset_x = 0;
  float offset_y = 0;
  float destination1 = (floor(a / _2PI) + measured_pole_pairs / 2.) * _2PI;
  float destination2 = (floor(a / _2PI)) * _2PI;
  for (; a < destination1; a += 0.4) {
    motor.move(a);
    delay(100);
    for (uint8_t i = 0; i < 100; i++) {
      sensor.update();
      delay(1);
    }
    float real_electrical_angle = _normalizeAngle(a);
    float measured_electrical_angle = _normalizeAngle( (float)(motor.sensor_direction * measured_pole_pairs) * sensor.getMechanicalAngle()  - 0);

    float offset_angle = measured_electrical_angle - real_electrical_angle;
    offset_x += cosf(offset_angle);
    offset_y += sinf(offset_angle);

    Serial.print(degrees(real_electrical_angle));
    Serial.print(", ");
    Serial.print(degrees(measured_electrical_angle));
    Serial.print(", ");
    Serial.println(degrees(_normalizeAngle(offset_angle)));
  }
  for (; a > destination2; a -= 0.4) {
    motor.move(a);
    delay(100);
    for (uint8_t i = 0; i < 100; i++) {
      sensor.update();
      delay(1);
    }
    float real_electrical_angle = _normalizeAngle(a);
    float measured_electrical_angle = _normalizeAngle( (float)(motor.sensor_direction * measured_pole_pairs) * sensor.getMechanicalAngle()  - 0);

    float offset_angle = measured_electrical_angle - real_electrical_angle;
    offset_x += cosf(offset_angle);
    offset_y += sinf(offset_angle);

    Serial.print(degrees(real_electrical_angle));
    Serial.print(", ");
    Serial.print(degrees(measured_electrical_angle));
    Serial.print(", ");
    Serial.println(degrees(_normalizeAngle(offset_angle)));
  }
  motor.voltage_limit = 0;
  motor.move(a);

  float avg_offset_angle = atan2f(offset_y, offset_x);


  // #### Apply settings
  // TODO: save to non-volatile storage
  motor.pole_pairs = measured_pole_pairs;
  motor.zero_electric_angle = avg_offset_angle + _3PI_2;
  motor.voltage_limit = 5;
  motor.controller = MotionControlType::torque;

  Serial.print("\n\nRESULTS:\n  Update these constants at the top of " __FILE__ "\n  ZERO_ELECTRICAL_OFFSET: ");
  Serial.println(motor.zero_electric_angle);
  Serial.print("  FOC_DIRECTION: ");
  if (motor.sensor_direction == Direction::CW) {
    Serial.println("Direction::CW");
  } else {
    Serial.println("Direction::CCW");
  }
  Serial.printf("  MOTOR_POLE_PAIRS: %d\n", motor.pole_pairs);
  delay(2000);
}

#endif