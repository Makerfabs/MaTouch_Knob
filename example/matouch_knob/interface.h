#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include "BleKeyboard.h"
#include "OneButton.h"
#include "USB.h"
#include "USBHID.h"
#include "driver/rtc_io.h"
#include "display_task.h"
#include "Watchdog.h"
#include "WifiAsyncWebServer.h"

//******************Vincent
#include "touch.h"

void doubleclick();
void click();
void longPressStart();
void duringLongPress();

#define TOUCH_SDA 6
#define TOUCH_SCL 8

#define DOUBLE_CLICK_THRESHOLD 500
#define LONG_PRESS_THRESHOLD 1000

// 按键状态和时间戳
typedef struct
{
  bool isDown;
  long lastPressTime;
  long lastReleaseTime;
  int pressCount;     // 用于追踪单击的连续次数
  bool isDoubleClick; // 是否检测到双击
  bool isLongPress;   // 是否检测到长按
} ButtonState;

// 假设这是你的按键检测函数
bool isKeyPressed(void)
{
  int x, y = 0;
  if (read_touch(&x, &y) == 1)
    if (x > 60 && x < 180 && y > 60 && y < 180)
      return true;
    else
      return false;
  else
    return false;
}

long diff_time(long time1, long time2)
{
  return time1 - time2;
}

// 初始化按键状态
void initButtonState(ButtonState *state)
{
  state->isDown = false;
  state->lastPressTime = 0;
  state->lastReleaseTime = 0;
  state->pressCount = 0;
  state->isDoubleClick = false;
  state->isLongPress = false;
}

// 检查按键事件
void checkButtonPress(ButtonState *state, bool currentPress)
{
  long currentTime = millis();

  if (currentPress && !state->isDown)
  {
    // 按键被按下（从未按下到按下）
    state->isDown = true;
    state->lastPressTime = currentTime;
    state->pressCount++;

    // 如果之前释放过按键，并且当前时间与上次释放时间间隔小于双击阈值
    // 则认为是双击的一部分
    if (state->lastReleaseTime != 0 &&
        diff_time(currentTime, state->lastReleaseTime) < DOUBLE_CLICK_THRESHOLD)
    {
      state->isDoubleClick = true;
    }
  }
  else if (!currentPress && state->isDown)
  {
    // 按键被释放（从按下到未按下）
    state->isDown = false;
    state->lastReleaseTime = currentTime;

    // 如果在长按阈值内没有再次按下，则认为是长按
    if (diff_time(currentTime, state->lastPressTime) > LONG_PRESS_THRESHOLD &&
        !state->isDoubleClick)
    {
      state->isLongPress = true;
      Serial.println("Long press detected!\n");
      duringLongPress();
    }

    // 如果双击标志被设置，并且当前没有长按，则处理双击事件
    if (state->isDoubleClick && !state->isLongPress)
    {
      Serial.println("Double click detected!\n");
      doubleclick();
      state->isDoubleClick = false; // 重置双击标志
    }
    else if (state->pressCount == 1 && !state->isLongPress)
    {
      // 处理单击事件
      Serial.println("Single click detected!\n");
      click();
    }

    // 重置长按标志
    state->isLongPress = false;

    // 重置单击计数（如果需要的话）
    state->pressCount = 0;
  }
}
//******************Vincent

USBHID HID;
TaskHandle_t xTask4; // wifi任务

WATCHDOG Watchdog; // 看门狗对象.

static const uint8_t report_descriptor[] = { // 8 axis
    // DIAL
    0x05, 0x01,
    0x09, 0x0e,
    0xa1, 0x01,
    0x85, 10,
    0x05, 0x0d,
    0x09, 0x21,
    0xa1, 0x00,
    0x05, 0x09,
    0x09, 0x01,
    0x95, 0x01,
    0x75, 0x01,
    0x15, 0x00,
    0x25, 0x01,
    0x81, 0x02,
    0x05, 0x01,
    0x09, 0x37,
    0x95, 0x01,
    0x75, 0x0f,
    0x55, 0x0f,
    0x65, 0x14,
    0x36, 0xf0, 0xf1,
    0x46, 0x10, 0x0e,
    0x16, 0xf0, 0xf1,
    0x26, 0x10, 0x0e,
    0x81, 0x06,
    0xc0,
    0xc0};

class CustomHIDDevice : public USBHIDDevice
{
public:
  CustomHIDDevice(void)
  {
    static bool initialized = false;
    if (!initialized)
    {
      initialized = true;
      HID.addDevice(this, sizeof(report_descriptor));
    }
  }

  void begin(void)
  {
    HID.begin();
  }
  void end(void)
  {
    HID.end();
  }
  uint16_t _onGetDescriptor(uint8_t *buffer)
  {
    memcpy(buffer, report_descriptor, sizeof(report_descriptor));
    return sizeof(report_descriptor);
  }

  bool send(uint8_t keys)
  {

    uint8_t dial_report[2];
    dial_report[0] = keys;
    dial_report[1] = 0;
    if (keys == DIAL_L || keys == DIAL_L_F)
      dial_report[1] = 0xff;
    return HID.SendReport(10, dial_report, 2);
  }
};

CustomHIDDevice Device;

BleKeyboard bleKeyboard;

static KnobConfig configs[] = {
    // int32_t num_positions;
    // int32_t position;
    // float position_width_radians;
    // float detent_strength_unit;
    // float endstop_strength_unit;
    // float snap_point;

    {
        0,
        0,
        10 * PI / 180,
        0,
        1,
        1.1,
    },
    {
        // 1设置界面
        0,
        0,
        90 * PI / 180,
        1,
        1,
        0.55,
    },
    {
        // 2设置界面 offset调整力度
        0,
        0,
        22.5 * PI / 180,
        1,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
    },
    {
        // 3 主界面
        0,
        0,
        90 * PI / 180,
        1,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
    },
    {
        1, // 4
        0,
        60 * PI / 180,
        0.01,
        0.6,
        1.1,
    },
    {
        256, // 5
        127,
        1 * PI / 180,
        0,
        1,
        1.1,
    },
    {
        // 6surface dial
        0,
        0,
        3 * PI / 180,
        1,
        1,
        1.1,
    },
    {
        // 7
        100,
        50,
        1 * PI / 180,
        1,
        1,
        1.1,
    },
    {
        // 8
        32,
        0,
        8.225806452 * PI / 180,
        0.2,
        1,
        1.1,
    },
};

#define OFF_PIN GPIO_NUM_7
#define OFF_UP_PIN GPIO_NUM_39
#define PUSH_BUTTON GPIO_NUM_5
#define IO_ON_OFF GPIO_NUM_18
OneButton button(PUSH_BUTTON, true, true);
uint32_t interface_time;
uint8_t push_flag, push_states;
uint32_t push_time, push_in_time, push_two_time;
uint8_t dial_flag;
uint8_t power_scale = 50; // 力度
uint8_t angle_scale = 75; // 角度
uint8_t push_scale = 3;

uint8_t lv_page = 0, lv_adjust_flag = 0;
void doubleclick()
{
  push_states = 2;
  Serial.println("doubleclick");
}
void click()
{
  push_states = 1;
  Serial.println("click");
}
void longPressStart()
{
  push_states = 3;
  Serial.println("longPressStart");
}
void duringLongPress()
{
  if (button.isLongPressed())
  {
    push_states = 3;
    Serial.print("duringLongPress:");
  }
}
void longPressStop()
{
  push_states = 4;
  Serial.println("longPressStop");
}
void power_off()
{
  // digitalWrite(OFF_PIN, LOW);
  // delay(100);
  // digitalWrite(OFF_PIN, LOW);
  // delay(200);
  // digitalWrite(OFF_PIN, HIGH);
  // delay(100);
  // digitalWrite(OFF_PIN, LOW);
  // delay(200);
  // digitalWrite(OFF_PIN, HIGH);

  digitalWrite(IO_ON_OFF, LOW);

  ledcWrite(0, 0); // 通道0输出， PWM输出0~100%（0~2^10=1024）
  sleep_flag = 1;
  motor.disable();

  rtc_gpio_init(TFT_BLK);
  rtc_gpio_init(PUSH_BUTTON);

  rtc_gpio_pullup_dis(TFT_BLK);
  rtc_gpio_pulldown_en(TFT_BLK);

  rtc_gpio_pullup_en(PUSH_BUTTON);
  rtc_gpio_pulldown_dis(PUSH_BUTTON);

  gpio_deep_sleep_hold_en();

  esp_sleep_enable_ext0_wakeup(PUSH_BUTTON, 0); // 1 = High, 0 = Low
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
}
void eeprom_read()
{
  if (isnan(EEPROM.readUChar(4)))
  {
    Serial.println("write power");
    EEPROM.writeUChar(4, 50);
    delay(10);
    EEPROM.commit();
  }
  else
  {
    if (EEPROM.readUChar(4) > 100)
    {
      Serial.println("write power1");
      EEPROM.writeUChar(4, 50);
      delay(10);
      EEPROM.commit();
    }
    else
    {
      power_scale = EEPROM.readUChar(4);
      Serial.println(power_scale);
    }
  }
  if (isnan(EEPROM.readUChar(8)))
  {
    Serial.println("write angle");
    EEPROM.writeUChar(8, 75);
    delay(10);
    EEPROM.commit();
  }
  else
  {
    if (EEPROM.readUChar(8) > 100)
    {
      Serial.println("write angle1");
      EEPROM.writeUChar(8, 75);
      delay(10);
      EEPROM.commit();
    }
    else
    {
      angle_scale = EEPROM.readUChar(8);
      Serial.println(angle_scale);
    }
  }
  if (isnan(EEPROM.readUChar(12)))
  {
    Serial.println("write push");
    EEPROM.writeUChar(12, 3);
    delay(10);
    EEPROM.commit();
  }
  else
  {
    if (EEPROM.readUChar(12) > 5)
    {
      Serial.println("write push1");
      EEPROM.writeUChar(12, 3);
      delay(10);
      EEPROM.commit();
    }
    else
    {
      push_scale = EEPROM.readUChar(12);
      Serial.println(push_scale);
    }
  }
  if (isnan(EEPROM.readUChar(16)))
  {
    Serial.println("write screen_brightness");
    EEPROM.writeUChar(16, 50);
    delay(10);
    EEPROM.commit();
  }
  else
  {
    if (EEPROM.readUChar(16) > 100)
    {
      Serial.println("write screen_brightness");
      EEPROM.writeUChar(16, 50);
      delay(10);
      EEPROM.commit();
    }
    else
    {
      screen_brightness = EEPROM.readUChar(16);
      Serial.println(screen_brightness);
    }
  }
#ifdef TFT_BLK
  ledcSetup(0, 5000, 10);               // 通道0， 5KHz，10位解析度
  ledcAttachPin(TFT_BLK, 0);            // pin25定义为通道0的输出引脚
  ledcWrite(0, screen_brightness * 10); // 通道0输出， PWM输出0~100%（0~2^10=1024）
#endif
}
// 长时间休眠
void sleep_time(uint8_t move)
{
  static uint16_t time = 0;
  static uint8_t dis_flag = 0;
  if (move)
  {
    time = 0;
    ledcWrite(0, screen_brightness * 10); // 通道0输出， PWM输出0~100%（0~2^10=1024）
  }

  time++;
  // 50为一秒   3000为一分钟
  if (time > 1500)
  {
    ledcWrite(0, 0); // 通道0输出， PWM输出0~100%（0~2^10=1024）
  }
  if (time > 15000)
  {
    power_off();
  }
}
void send_config(uint8_t num)
{
  KnobConfig set_config;
  set_config = configs[num];
  set_config.detent_strength_unit = configs[num].detent_strength_unit * (100 - power_scale) * 0.02;
  set_config.position_width_radians = configs[num].position_width_radians * (100 - angle_scale) * 0.04;
  setConfig(set_config);
}
void interface_run(void *parameter)
{
  // Vincent
  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  ButtonState buttonState;
  initButtonState(&buttonState);

  uint16_t img_angle = 0, position_flag = 0, last_img_angle = 0;
  float last_adjusted_angle = 0;
  int16_t last_position = 0;
  uint8_t p; // 当前选择的位置
  KnobConfig set_config;
  KnobState state;

  button.reset(); // 清除一下按钮状态机的状态
  button.attachClick(click);
  button.attachDoubleClick(doubleclick);
  button.attachLongPressStart(longPressStart);
  //  button.attachDuringLongPress(duringLongPress);
  button.attachLongPressStop(longPressStop);

  USB.begin();

  pinMode(OFF_UP_PIN, OUTPUT);
  pinMode(OFF_PIN, OUTPUT);
  pinMode(IO_ON_OFF, OUTPUT);
  digitalWrite(IO_ON_OFF, HIGH);
  delay(10);
  digitalWrite(OFF_UP_PIN, LOW);
  delay(10);
  digitalWrite(OFF_PIN, LOW);

  eeprom_read();

  send_config(3);
  Serial.println("setConfig");
  Watchdog.begin(); // 默认定时器0,10秒超时.
  while (1)
  {
    Watchdog.feed(); // 喂狗
    sleep_time(0);
    if (xQueueReceive(knob_state_queue_, &state, 0) == pdTRUE)
    {
      pthread_mutex_lock(&lvgl_mutex);
      float adjusted_sub_position, raw_angle, adjusted_angle;

      adjusted_sub_position = state.sub_position_unit * state.config.position_width_radians;
      raw_angle = state.current_position * state.config.position_width_radians;
      adjusted_angle = -(raw_angle + adjusted_sub_position);
      if (adjusted_angle > 0)
        img_angle = (uint16_t)(adjusted_angle * 573) % 3600;
      else
        img_angle = 3600 - (uint16_t)(abs(adjusted_angle) * 573) % 3600;
      // Serial.print(last_adjusted_angle);
      // Serial.print("/");
      // Serial.println(abs(adjusted_angle-last_adjusted_angle));
      if (abs(adjusted_angle - last_adjusted_angle) > 0.05)
      {

        sleep_time(1);
        last_adjusted_angle = adjusted_angle;
      }

      // Serial.print(raw_angle);
      // Serial.print("/");
      // Serial.print(adjusted_angle);
      // Serial.print("/");
      // Serial.print(img_angle);
      // Serial.print("/");
      // Serial.print(last_img_angle);
      // Serial.print("/");
      // Serial.println((uint16_t)(adjusted_angle * 573));
      if (last_position != state.current_position)
      {
        position_flag = 1;
        sleep_time(1);
        if (state.current_position - last_position > 0)
          dial_flag = 1;
        else
          dial_flag = 2;
      }
      last_position = state.current_position;
      switch (lv_page)
      {
      case 0:
        adjusted_sub_position = state.sub_position_unit * 90 * PI / 180;
        raw_angle = state.current_position * 90 * PI / 180;
        adjusted_angle = -(raw_angle + adjusted_sub_position);
        if (adjusted_angle > 0)
          img_angle = (uint16_t)(adjusted_angle * 573) % 3600;
        else
          img_angle = 3600 - (uint16_t)(abs(adjusted_angle) * 573) % 3600;
        //   if (abs(img_angle - last_img_angle) > 50) {
        //     lv_img_set_angle(ui_Image0_2, img_angle);
        //     last_img_angle = img_angle;
        //   }
        lv_img_set_angle(ui_Image0_2, img_angle);
        if ((img_angle >= 0 && img_angle <= 450) || (img_angle > 3150 && img_angle <= 3600))
          p = 0;
        else if ((img_angle > 450 && img_angle <= 1350))
          p = 3;
        else if ((img_angle > 1350 && img_angle <= 2250))
          p = 2;
        else if ((img_angle > 2250 && img_angle <= 3150))
          p = 1;
        switch (p)
        {
        case 0:
          lv_obj_set_style_outline_width(ui_Image0_0, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          break;
        case 1:
          lv_obj_set_style_outline_width(ui_Image0_0, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_1, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          break;
        case 2:
          lv_obj_set_style_outline_width(ui_Image0_0, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_4, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
          break;
        case 3:
          lv_obj_set_style_outline_width(ui_Image0_0, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_3, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
          lv_obj_set_style_outline_width(ui_Image0_4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
          break;
        }
        break;
      case 1:

        // adjusted_sub_position = state.sub_position_unit * state.config.position_width_radians;
        // raw_angle = state.current_position * state.config.position_width_radians;
        // adjusted_angle = -(raw_angle + adjusted_sub_position);
        // if (adjusted_angle > 0)
        //   img_angle = (uint16_t)(adjusted_angle * 573) % 3600;
        // else
        //   img_angle = 3600 - (uint16_t)(abs(adjusted_angle) * 573) % 3600;
        // if (abs(img_angle - last_img_angle) > 5) {
        //   lv_img_set_angle(ui_Image2, img_angle);
        //   last_img_angle = img_angle;
        // }
        lv_img_set_angle(ui_Image2, img_angle);
        break;
      case 2:

        switch (lv_adjust_flag) //--------------------------------------------------------------------------
        {
        case 0: // 力度
          power_scale = state.current_position;
          set_config.position = power_scale;
          break;
        case 1: // 角度
          angle_scale = state.current_position;
          set_config.position = angle_scale;
          break;
        case 2: // 亮度
          screen_brightness = 100 - state.current_position;
          if (screen_brightness < 5)
            screen_brightness = 5;
          set_config.position = state.current_position;
          break;
        }
        lv_meter_set_indicator_value(meter, line_indic, 100 - state.current_position);
        break;
      }
      pthread_mutex_unlock(&lvgl_mutex);
    }

    unsigned long currentMillis = millis();

    //----------------------旋转部分
    //------------------------按键处理----------------------//
    if (digitalRead(PUSH_BUTTON) == 0)
    { // 按下
      if (push_flag == 0)
      {
        push_time = currentMillis;
        push_flag = 1;
      }
      if (currentMillis - push_time > 10)
      { // 消抖10ms
        if (push_flag == 1)
        {
          if (lv_page == 1)
          {
            if (bleKeyboard.isConnected())
            {
              bleKeyboard.sendDialReport(DIAL_PRESS);
            }
            else
            {
              Device.send(DIAL_PRESS);
            }
          }

          push_flag = 2;
          push_in_time = currentMillis;
          playHaptic(push_scale);
        }
      }
    }
    if (push_flag && digitalRead(PUSH_BUTTON))
    { // 松开

      push_two_time = currentMillis;
      push_flag = 0;
      if (lv_page == 1)
      {
        if (bleKeyboard.isConnected())
        {
          bleKeyboard.sendDialReport(DIAL_RELEASE);
        }
        else
        {
          Device.send(DIAL_RELEASE);
        }
      }
    }
    // ---- 页面处理
    switch (lv_page)
    {
    case 0:
    {
      // 左旋右旋
      if (dial_flag == 1)
      {
      }
      else if (dial_flag == 2)
      {
      }
      switch (push_states)
      {
      case 1: // 单击
        if (sleep_flag)
        {
          ledcWrite(0, screen_brightness * 10); // 通道0输出， PWM输出0~100%（0~2^10=1024）
          sleep_flag = 0;
          motor.enable();
        }
        break;
      case 2: // 双击切换
        switch (p)
        {
        case 0: // surface dial
          lv_event_send(ui_Button0, LV_EVENT_CLICKED, 0);
          configs[3].position = state.current_position;
          send_config(6);
          lv_page = 1;
          Serial.println(lv_page);
          Serial.println("Starting BLE work!");
          bleKeyboard.begin();
          Device.begin();
          break;
        case 1: // 力度设置界面
          lv_event_send(ui_Button0, LV_EVENT_RELEASED, 0);
          switch (lv_adjust_flag)
          {
          case 0:
            // lv_label_set_text(ui2_Label1, "力度");
            lv_label_set_text(ui2_Label1, "Feedback");
            configs[3].position = state.current_position;
            set_config = configs[7];
            set_config.position = power_scale;
            set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
            set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
            setConfig(set_config);
            break;
          case 1:
            // lv_label_set_text(ui2_Label1, "角度");
            lv_label_set_text(ui2_Label1, "Angle");
            configs[3].position = state.current_position;
            set_config = configs[7];
            set_config.position = angle_scale;
            set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
            set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
            setConfig(set_config);
            break;
          case 2:
            // lv_label_set_text(ui2_Label1, "亮度");
            lv_label_set_text(ui2_Label1, "Brightness");
            configs[3].position = state.current_position;
            set_config = configs[7];
            set_config.position = 100 - screen_brightness;
            set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
            set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
            setConfig(set_config);
            break;
          }

          lv_page = 2;
          Serial.println(lv_page);
          break;
        case 2:        // 关机
          power_off(); // ip5306关机

          break;
        case 3: // 跳转到设置
          // motor.disable();
          EEPROM.writeFloat(0, motor.zero_electric_angle);
          delay(1);
          EEPROM.commit();
          lv_event_send(ui_Button0, LV_EVENT_SHORT_CLICKED, 0);
          configs[3].position = state.current_position;
          send_config(1);
          lv_page = 3;
          Serial.println(lv_page);
          xTaskCreatePinnedToCore(
              wifi_server_begin,
              "wifi_server_begin", /* 任务名称. */
              8192,                /* 任务的堆栈大小 */
              NULL,                /* 任务的参数 */
              4,                   /* 任务的优先级 */
              &xTask4,             /* 跟踪创建的任务的任务句柄 */
              1);                  /* pin任务到核心0 */
          break;
        }
        break;
      case 3: // 长按
        break;
      case 4: // 松开
        break;
      default:
        break;
      }
    }

    break;
    case 1: // 页面1
    {
      // 左旋右旋
      if (dial_flag == 1)
      {
      }
      else if (dial_flag == 2)
      {
      }
      if (push_flag == 2) // 按下旋转
      {
        ;
      }
      else
      {
        // 如果蓝牙连接成功
        if (bleKeyboard.isConnected())
        {
          if (dial_flag == 1)
          {
            bleKeyboard.sendDialReport(DIAL_L);
          }
          else if (dial_flag == 2)
          {
            bleKeyboard.sendDialReport(DIAL_R);
          }
        }
        else
        {
          if (dial_flag == 1)
          {
            Device.send(DIAL_L);
          }
          else if (dial_flag == 2)
          {
            Device.send(DIAL_R);
          }
        }
      }
      switch (push_states)
      {
      case 1: // 单击

        // if (bleKeyboard.isConnected()) {
        //   bleKeyboard.sendDialReport(DIAL_PRESS);
        //   delay(50);
        //   bleKeyboard.sendDialReport(DIAL_RELEASE);
        // } else {
        //   Device.send(DIAL_PRESS);
        //   delay(50);
        //   Device.send(DIAL_RELEASE);
        // }

        break;
      case 2: // 双击切换
        lv_event_send(ui_Button1, LV_EVENT_CLICKED, 0);
        send_config(3);
        lv_page = 0;
        Serial.println(lv_page);

        if (bleKeyboard.isConnected())
        {
          bleKeyboard.sendDialReport(DIAL_RELEASE);
        }
        else
          Device.send(DIAL_RELEASE);

        bleKeyboard.end();
        Device.end();
        break;
      case 3: // 长按
        break;
      case 4: // 松开
        Serial.println("DIAL_RELEASE");
        break;
      default:
        break;
      }
    }

    break;
    case 2:
    {
      if (dial_flag == 1)
      {
        set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
        set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
        if (millis() - interface_time > 10)
        {
          interface_time = millis();
          setConfig(set_config);
        }
      }
      else if (dial_flag == 2)
      {
        set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
        set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
        if (millis() - interface_time > 10)
        {
          interface_time = millis();
          setConfig(set_config);
        }
      }
      switch (push_states)
      {
      case 1: // 单击
        lv_adjust_flag++;
        if (lv_adjust_flag > 2)
          lv_adjust_flag = 0;
        switch (lv_adjust_flag)
        {
        case 0:
          // lv_label_set_text(ui2_Label1, "力度");
          lv_label_set_text(ui2_Label1, "Feedback");
          configs[3].position = state.current_position;
          set_config = configs[7];
          set_config.position = power_scale;
          set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
          set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
          setConfig(set_config);
          break;
        case 1:
          // lv_label_set_text(ui2_Label1, "角度");
          lv_label_set_text(ui2_Label1, "Angle");
          configs[3].position = state.current_position;
          set_config = configs[7];
          set_config.position = angle_scale;
          set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
          set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
          setConfig(set_config);
          break;
        case 2:
          // lv_label_set_text(ui2_Label1, "亮度");
          lv_label_set_text(ui2_Label1, "Brightness");
          configs[3].position = state.current_position;
          set_config = configs[7];
          set_config.position = 100 - screen_brightness;
          set_config.detent_strength_unit = configs[7].detent_strength_unit * (100 - power_scale) * 0.02;
          set_config.position_width_radians = configs[7].position_width_radians * (100 - angle_scale) * 0.04;
          setConfig(set_config);
          break;
        }
        break;
      case 2: // 双击切换
        EEPROM.writeUChar(4, power_scale);
        EEPROM.commit();
        Serial.println(power_scale);

        EEPROM.writeUChar(8, angle_scale);
        EEPROM.commit();
        Serial.println(angle_scale);

        EEPROM.writeUChar(16, screen_brightness);
        EEPROM.commit();
        Serial.println(screen_brightness);
        lv_event_send(ui_Button2, LV_EVENT_CLICKED, 0);
        configs[7].position = state.current_position;
        send_config(3);
        lv_page = 0;
        break;
      case 3: // 长按
        break;
      case 4: // 松开
        Serial.println("DIAL_RELEASE");
        break;
      default:
        break;
      }
    }
    break;
    case 3:
    {
      if (dial_flag == 1)
      {
      }
      else if (dial_flag == 2)
      {
      }
      switch (push_states)
      {
      case 1: // 单击
        break;
      case 2: // 双击切换

        lv_event_send(ui_Button2, LV_EVENT_CLICKED, 0);
        send_config(3);
        lv_page = 0;
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        vTaskDelete(xTask4);

        // motor.enable();
        break;
      case 3: // 长按
        EEPROM.writeFloat(0, 100);
        EEPROM.commit();
        delay(10);
        ESP.restart();
        break;
      case 4: // 松开
        Serial.println("DIAL_RELEASE");
        break;
      default:
        break;
      }
    }
    break;
    }
    push_states = 0;
    dial_flag = 0;
    button.tick();
    // Vincent
    checkButtonPress(&buttonState, isKeyPressed());

    vTaskDelay(20);
  }
}

#endif