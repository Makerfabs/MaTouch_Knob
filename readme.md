# MaTouch Knob

```c++
/*
Version:        V1.0
Author:            Vincent
Create Date:    2024/6/21
Note:

*/
```

[TOC]

![main](md_pic/main.jpg)

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://wiki.makerfabs.com/)

# MaTouch Knob

## Intruduce

Inspired by foreign smart konb, the original intention was to create a desktop force feedback knob that could interact with a computer. The project starts with the most important motor, and chooses a large and cheap 3205 motor (the stock of Xiaomi Pentie motor) to achieve a large structure, and increases the practical function of Bluetooth hid (Roulette function), which can achieve the same function as surface dial.

## Feature

Controller: ESP32-S3
Motor: 3205B Brushless Motor
Wireless: WiFi& Bluetooth 5.0
LCD: 1.28", 240*240 resolution, driver GC9A01
LCD interface: SPI
Touch Panel Driver: CST816S
USB: USB Native
Power Supply: USB Type-C 5.0V(4.0V~5.25V); 3.7V Lipo battery
Button: Function button, Flash button and reset button

![main](md_pic/back.jpg)

# Code

## Complier Option

- Install board : ESP32 .
- Install libraries：

[ESP32-audioI2S]([GitHub - schreibfaul1/ESP32-audioI2S: Play mp3 files from SD via I2S](https://github.com/schreibfaul1/ESP32-audioI2S))

- Use type-c use cable connect USB-TTL to PC.
- Select "ESP32-S3 DEV Module"
- Select PSRAM "OPI PSRAM"

Unzip GFX_Library_for_Arduino.zip and lvgl.zip to Arduino Libraries folder.
And install other libraries from Arduino Libraries Manager.


```c
// Use 2.3.2  Version Simple_FOC               
// Use 2.0.0  Version SPI                      
// Use 2.0.0  Version Wire                     
// Use 2.0.0  Version EEPROM                   
// Use 8.3.11 Version lvgl                     
// Use 1.3.1  Version GFX_Library_for_Arduino  
// Use 2.0.0  Version BLE                      
// Use 2.5.0  Version OneButton                
// Use 2.0.0  Version USB                      
// Use 2.0.0  Version WiFi                     
// Use 1.1.4  Version AsyncTCP                 
// Use 1.2.7  Version ESPAsyncWebSrv           
// Use 2.0.0  Version FS                       
// Use 2.0.0  Version ESPmDNS                  
// Use 7.0.4  Version ArduinoJson              
// Use 2.0.0  Version Preferences              
// Use 2.0.0  Version Update                   
// Use 2.0.0  Version FFat                       
// Use 2.0.0  Version SPIFFS     
```


## Code explain

### matouch_knob

Modified from the [智能旋钮_superdial](https://gitee.com/coll45/super-dial-motor-knob-screen) project.
Functions include setting motor feedback force, rotation Angle, screen brightness.
And can connect to Windows via Bluetooth, similar to the Surface Dial function.
