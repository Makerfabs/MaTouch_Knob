# SHIVERS Makerfabs MaTouch Knob

## Introduction

Inspired by [Scott Bezek](https://github.com/scottbez1)'s [SmartKnob](https://github.com/scottbez1/smartknob), [Makerfabs](https://www.makerfabs.com/)' original intention was to create a desktop force feedback knob that could interact with a computer. The project starts with the most important motor, and chooses a large and cheap 3205 motor (the stock of Xiaomi Pentie motor) to achieve a larger form factor, and increases the practical function of Bluetooth HID (Roulette function), which can achieve the same function as the Microsoft Surface Dial. 

The [SHIVERS Group](https://github.com/ucalgary-shivers) at the University of Calgary brings new software features:
- updated documentation
- firmware compilation verified via Continuous Integration with [.github/workflows/firmware.yml](.github/workflows/firmware.yml)

## Hardware Features

- Controller: ESP32-S3
- Motor: 3205B Brushless Motor
- Wireless: WiFi & Bluetooth 5.0
- LCD: 1.28", 240*240 resolution, driver GC9A01
- LCD interface: SPI
- Touch Panel Driver: CST816S
- USB: USB Native
- Power Supply: USB Type-C 5.0V(4.0V~5.25V); 3.7V LiPo battery
- Button: Function button, Flash button and reset button

## Development

- Open [example/matouch_knob/matouch_knob.ino](example/matouch_knob/matouch_knob.ino) in Arduino IDE.
- Tools > Board > Board Manager: search "esp32" and install [esp32 2.0.17](https://github.com/espressif/arduino-esp32/releases/tag/2.0.17) the latest version containing the following 2.0.0 libraries:          
  - SPI
  - Wire
  - EEPROM
  - BLE
  - USB
  - WiFi
  - FS
  - ESPmDNS
  - Preferences
  - Update
  - FFat
  - SPIFFS
- Tools > Manage libraries: search and install libraries：
  - Simple_FOC 2.3.2
  - lvgl 8.3.11
    - note: [libraries/lvgl/src/lv_conf.h](libraries/lvgl/src/lv_conf.h) is superseded by [example/matouch_knob/build_opt.h](example/matouch_knob/build_opt.h)
  - GFX_Library_for_Arduino 1.3.1
  - OneButton 2.5.0
  - AsyncTCP 1.1.4
  - ESPAsyncWebSrv 1.2.7             
  - ArduinoJson 7.0.4
- Connect a type-c USB cable between your computer and the device.
- With the device upright, you will see the following inputs from left to right: USB connector, round button, then a pair of small buttons (reset and boot). Press and hold the boot button, press and release the reset button, release the boot button to enter flashing mode.
- Tools > Board > esp32: select "ESP32S3 Dev Module"
- Tools: select the following settings:
  - USB CDC On Boot: "Enabled"
  - CPU Frequency: "240MHz (WiFi)"
  - Core Debug Level: "None"
  - USB DFU On Boot: "Disabled"
  - Erase All Flash Before Sketch Upload: "Enabled"
  - Events Run On: "Core 1"
  - Flash Mode: "QIO 80MHz"
  - Flash Size: "16MB (128Mb)"
  - JTAG Adapter: "Disabled"
  - Arduino Runs On: "Core 1"
  - USB Firmware MSC On Boot: "Disabled"
  - Partition Scheme: "16M Flash (3MB APP/9.9MB FATFS)"
  - PSRAM: "OPI PSRAM"
  - Upload Mode: "UARTO / Hardware CDC"
  - Upload Speed: "921600"
  - USB Mode: "USB-OTG (TinyUSB)"
- Compile.
- Tools > Port: select your device.
  - Note: compilation is verified via Continuous Integration with [.github/workflows/firmware.yml](.github/workflows/firmware.yml), please update documentation and workflow accordingly.
- Upload.

## Provenance

### Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://wiki.makerfabs.com/)

### matouch_knob

Modified from the [智能旋钮_superdial](https://gitee.com/coll45/super-dial-motor-knob-screen) project.
Functions include setting motor feedback force, rotation Angle, screen brightness.
And can connect to Windows via Bluetooth, similar to the Surface Dial function.
