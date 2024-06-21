#ifndef __DISPLAY_TASK_H__
#define __DISPLAY_TASK_H__

#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <esp_heap_caps.h>
#include "ui.h"
uint8_t screen_brightness = 50;
static pthread_mutex_t lvgl_mutex;

#define SCLK GPIO_NUM_11
#define MOSI GPIO_NUM_12
#define TFT_CS GPIO_NUM_10
#define TFT_BLK GPIO_NUM_13
#define TFT_DC GPIO_NUM_14
#define TFT_RST GPIO_NUM_9

Arduino_DataBus *bus = new Arduino_HWSPI(TFT_DC, TFT_CS, SCLK, MOSI, MISO);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* 屏幕方向 */, true /* IPS */);

/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

#if LV_USE_LOG != 0
/* Serial debugging */
// void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *fn_name, const char *dsc) {
//   Serial.printf("%s(%s)@%d->%s\r\n", file, fn_name, line, dsc);
//   Serial.flush();
// }
void my_print(const char *buf)
{
  Serial.printf(buf);
  Serial.flush();
}

#endif
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

  lv_disp_flush_ready(disp);
}
void display_init()
{
  // Init Display
  gfx->begin();
  gfx->fillScreen(BLACK);

  lv_init();
#if LV_USE_LOG != 0
  // lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif
  screenWidth = gfx->width();
  screenHeight = gfx->height();
  void *ptrVal = NULL;
  ptrVal = heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight, MALLOC_CAP_SPIRAM);
  lv_color_t *disp_draw_buf1 = (lv_color_t *)ptrVal;
  void *ptrVal1 = NULL;
  ptrVal1 = heap_caps_malloc(sizeof(lv_color_t) * screenWidth * screenHeight, MALLOC_CAP_SPIRAM);
  lv_color_t *disp_draw_buf2 = (lv_color_t *)ptrVal1;
  if (!disp_draw_buf1 || !disp_draw_buf2)
  {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  }
  else
  {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf1, disp_draw_buf2, screenWidth * screenHeight);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    ui_init();
    lv_img_set_pivot(ui_Image0_2, 16, 85); /*Rotate around the top left corner*/
    lv_img_set_pivot(ui_Image2, 16, 120);  /*Rotate around the top left corner*/
    lv_meter_set_indicator_value(meter, line_indic, 50);
    Serial.println("Setup done");
  }
}
void display_run(void *parameter)
{
  display_init();
  while (1)
  {
    if (sleep_flag == 0)
    {
      pthread_mutex_lock(&lvgl_mutex);
      lv_timer_handler(); /* let the GUI do its work */
      pthread_mutex_unlock(&lvgl_mutex);
    }
    vTaskDelay(10);
  }
}

#endif