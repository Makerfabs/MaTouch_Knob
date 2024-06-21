// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: SquareLine_Project

#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Image1;
extern lv_obj_t * ui_Image2;
extern lv_obj_t * ui_Button1;

extern lv_obj_t * ui_Screen2;
extern lv_obj_t * ui_Button2;
extern lv_obj_t* meter;
extern lv_meter_indicator_t* line_indic;
extern lv_obj_t * ui2_Label1;

extern lv_obj_t* ui_Screen0;
extern lv_obj_t* ui_Button0;
extern lv_obj_t* ui_Image0_0;
extern lv_obj_t* ui_Image0_1;
extern lv_obj_t* ui_Image0_2;
extern lv_obj_t* ui_Image0_3;
extern lv_obj_t* ui_Image0_4;

extern lv_obj_t * ui_Screen3;
extern lv_obj_t * ui_Spinner1;
extern lv_obj_t * ui_Label2;
extern lv_obj_t * ui_Label3;

LV_IMG_DECLARE(ui_img_page1_png); 
LV_IMG_DECLARE(ui_img_page2_png);    
LV_IMG_DECLARE(ui_img_point_png);    
LV_IMG_DECLARE(ui_img_dial_png);    
LV_IMG_DECLARE(ui_img_dashboard_png);    
LV_IMG_DECLARE(ui_img_seting_png);    
LV_IMG_DECLARE(ui_img_shutdown_png);   



void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
