/*
 * ESP32看门狗对象使用方法:
 * 1、引用对象定义文件(本文件)：#include "Watchdog.C"
 * 2、申明看门狗对象变量：WATCHDOG Watchdog;
 * 3、初始化对象：Watchdog.begin();//默认使用定时器0，10秒超时。//也可以Watchdog.begin(1,1000);//指定定时器和超时时间。
 * 4、喂狗：Watchdog.feed();
 */
#ifndef ESP_RT_WDT_H  //避免重复定义.
  #define ESP_RT_WDT_H  
  #include "esp_system.h"
  class WATCHDOG {
    private:
      hw_timer_t *timer = NULL;//定时器对象.
      uint8_t Timerindex=0;//硬件定时器编号.默认定时器0.
      uint16_t Timeout=15000;//定时器计数.默认15秒超时.
    protected:
      static void Callback(){
        //定时器溢出回调函数,直接复位.
        esp_restart();
      }
      void Init(){
        if(timer!=NULL){timerEnd(timer);}//如果之前有设置过定时器,则关闭相应的定时器.
        timer=timerBegin(Timerindex,80,true);//使用硬件定时器，预分频80，向上计数。
        timerAttachInterrupt(timer,Callback,true);//设置回调函数，边延触发。
        timerAlarmWrite(timer,Timeout*1000,true);//设定中断计数器值，自动重启计数（循环定时）。CPU主频80MHz/80/1000=1毫秒。
        timerAlarmEnable(timer);//开启定时器。
      }
    public:
      WATCHDOG(){Timerindex=0;Timeout=15000;}
      void begin(){Init();}
      void begin(uint8_t Esp_Timerindex,uint16_t Esp_Timerout){Timerindex=Esp_Timerindex,Timeout=Esp_Timerout;begin();}
      void feed(void){timerWrite(timer,0);}//喂狗.
  };
#endif