// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

LV_FONT_DECLARE(smileysans); // 官方文档给定这个

///////////////////// VARIABLES ////////////////////
lv_obj_t *ui_Screen1;
lv_obj_t *ui_Image1;
lv_obj_t *ui_Image2;
lv_obj_t *ui_Button1;

lv_obj_t *ui_Screen2;
lv_obj_t *ui_Button2;
lv_obj_t *meter;
lv_meter_indicator_t *line_indic;
lv_obj_t *ui2_Label1;

lv_obj_t *ui_Screen0;
lv_obj_t *ui_Button0;
lv_obj_t *ui_Image0_0;
lv_obj_t *ui_Image0_1;
lv_obj_t *ui_Image0_2;
lv_obj_t *ui_Image0_3;
lv_obj_t *ui_Image0_4;

lv_obj_t *ui_Screen3;
lv_obj_t *ui_Spinner1;
lv_obj_t *ui_Label2;
lv_obj_t *ui_Label3;
///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
#error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP != 1
// #error "#error LV_COLOR_16_SWAP should be 1 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
bool check_file(const char *path)
{
  lv_fs_file_t f;
  lv_fs_res_t res;
  res = lv_fs_open(&f, path, LV_FS_MODE_RD);
  if (res != LV_FS_RES_OK)
    return 0;
  else
  {
    return 1;
    lv_fs_close(&f);
  }
}
///////////////////// FUNCTIONS ////////////////////
static void ui_event_Button0(lv_event_t *e)
{
  lv_event_code_t event = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);
  if (event == LV_EVENT_CLICKED)
  {
    _ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
  }
  if (event == LV_EVENT_RELEASED)
  {
    _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
  }
  if (event == LV_EVENT_SHORT_CLICKED)
  {
    _ui_screen_change(ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
  }
}
static void ui_event_Button1(lv_event_t *e)
{
  lv_event_code_t event = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);
  if (event == LV_EVENT_CLICKED)
  {
    _ui_screen_change(ui_Screen0, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
  }
}
static void ui_event_Button2(lv_event_t *e)
{
  lv_event_code_t event = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);
  if (event == LV_EVENT_CLICKED)
  {
    _ui_screen_change(ui_Screen0, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
  }
}

///////////////////// SCREENS ////////////////////
void ui_Screen0_screen_init(void)
{

  // ui_Screen2

  ui_Screen0 = lv_obj_create(NULL);

  lv_obj_clear_flag(ui_Screen0, LV_OBJ_FLAG_SCROLLABLE);

  if (check_file("S:img1.png"))
    lv_obj_set_style_bg_img_src(ui_Screen0, "S:img1.png", LV_PART_MAIN | LV_STATE_DEFAULT);
  else if (check_file("S:img1.jpg"))
    lv_obj_set_style_bg_img_src(ui_Screen0, "S:img1.jpg", LV_PART_MAIN | LV_STATE_DEFAULT);
  else
    lv_obj_set_style_bg_img_src(ui_Screen0, &ui_img_page1_png, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ui_Image0_0

  ui_Image0_0 = lv_img_create(ui_Screen0);
  lv_img_set_src(ui_Image0_0, &ui_img_dial_png);

  lv_obj_set_width(ui_Image0_0, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Image0_0, LV_SIZE_CONTENT);

  lv_obj_set_x(ui_Image0_0, 0);
  lv_obj_set_y(ui_Image0_0, -105);

  lv_obj_set_align(ui_Image0_0, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Image0_0, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(ui_Image0_0, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_outline_color(ui_Image0_0, lv_color_hex(0x7DB6E9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(ui_Image0_0, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_width(ui_Image0_0, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_pad(ui_Image0_0, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ui_Image0_1

  ui_Image0_1 = lv_img_create(ui_Screen0);
  lv_img_set_src(ui_Image0_1, &ui_img_dashboard_png);

  lv_obj_set_width(ui_Image0_1, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Image0_1, LV_SIZE_CONTENT);

  lv_obj_set_x(ui_Image0_1, -105);
  lv_obj_set_y(ui_Image0_1, 0);

  lv_obj_set_align(ui_Image0_1, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Image0_1, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(ui_Image0_1, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_outline_color(ui_Image0_1, lv_color_hex(0x7DB6E9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(ui_Image0_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_width(ui_Image0_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_pad(ui_Image0_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  // ui_Image0_2

  ui_Image0_2 = lv_img_create(ui_Screen0);
  if (check_file("S:pointer.png"))
    lv_img_set_src(ui_Image0_2, "S:pointer.png");
  else if (check_file("S:pointer.jpg"))
    lv_img_set_src(ui_Image0_2, "S:pointer.jpg");
  else
    lv_img_set_src(ui_Image0_2, &ui_img_point_png);

  lv_obj_set_width(ui_Image0_2, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Image0_2, LV_SIZE_CONTENT);

  lv_obj_set_x(ui_Image0_2, 0);
  lv_obj_set_y(ui_Image0_2, -69);

  lv_obj_set_align(ui_Image0_2, LV_ALIGN_CENTER);
  lv_obj_add_flag(ui_Image0_2, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(ui_Image0_2, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_outline_color(ui_Image0_2, lv_color_hex(0x7DB6E9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(ui_Image0_2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_width(ui_Image0_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_pad(ui_Image0_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  // ui_Image0_3

  ui_Image0_3 = lv_img_create(ui_Screen0);
  lv_img_set_src(ui_Image0_3, &ui_img_seting_png);

  lv_obj_set_width(ui_Image0_3, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Image0_3, LV_SIZE_CONTENT);

  lv_obj_set_x(ui_Image0_3, 105);
  lv_obj_set_y(ui_Image0_3, 0);

  lv_obj_set_align(ui_Image0_3, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Image0_3, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(ui_Image0_3, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_outline_color(ui_Image0_3, lv_color_hex(0x7DB6E9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(ui_Image0_3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_width(ui_Image0_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_pad(ui_Image0_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  // ui_Image0_4

  ui_Image0_4 = lv_img_create(ui_Screen0);
  lv_img_set_src(ui_Image0_4, &ui_img_shutdown_png);

  lv_obj_set_width(ui_Image0_4, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Image0_4, LV_SIZE_CONTENT);

  lv_obj_set_x(ui_Image0_4, 0);
  lv_obj_set_y(ui_Image0_4, 105);

  lv_obj_set_align(ui_Image0_4, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Image0_4, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(ui_Image0_4, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_outline_color(ui_Image0_4, lv_color_hex(0x7DB6E9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(ui_Image0_4, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_width(ui_Image0_4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_pad(ui_Image0_4, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  // ui_Button0

  ui_Button0 = lv_btn_create(ui_Screen1);

  lv_obj_set_width(ui_Button0, 100);
  lv_obj_set_height(ui_Button0, 50);

  lv_obj_set_x(ui_Button0, 0);
  lv_obj_set_y(ui_Button0, 0);

  lv_obj_set_align(ui_Button0, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Button0, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
  lv_obj_clear_flag(ui_Button0, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_add_event_cb(ui_Button0, ui_event_Button0, LV_EVENT_ALL, NULL);
  lv_obj_set_style_opa(ui_Button0, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Vincent

  lv_img_set_zoom(ui_Image0_0, 180);
  lv_img_set_zoom(ui_Image0_1, 180);
  lv_img_set_zoom(ui_Image0_3, 180);
  lv_img_set_zoom(ui_Image0_4, 180);
}
void ui_Screen1_screen_init(void)
{

  // ui_Screen1

  ui_Screen1 = lv_obj_create(NULL);

  lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

  // ui_Image1

  ui_Image1 = lv_img_create(ui_Screen1);

  if (check_file("S:img2.png"))
    lv_img_set_src(ui_Image1, "S:img2.png");
  else if (check_file("S:img2.jpg"))
    lv_img_set_src(ui_Image1, "S:img2.jpg");
  else
    lv_img_set_src(ui_Image1, &ui_img_page2_png);

  lv_obj_set_width(ui_Image1, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Image1, LV_SIZE_CONTENT);

  lv_obj_set_x(ui_Image1, 0);
  lv_obj_set_y(ui_Image1, 0);

  lv_obj_set_align(ui_Image1, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(ui_Image1, LV_OBJ_FLAG_SCROLLABLE);

  // ui_Image2

  ui_Image2 = lv_img_create(ui_Screen1);
  if (check_file("S:pointer.png"))
    lv_img_set_src(ui_Image2, "S:pointer.png");
  else if (check_file("S:pointer.jpg"))
    lv_img_set_src(ui_Image2, "S:pointer.jpg");
  else
    lv_img_set_src(ui_Image2, &ui_img_point_png);

  lv_obj_set_width(ui_Image2, LV_SIZE_CONTENT);
  lv_obj_set_height(ui_Image2, LV_SIZE_CONTENT);

  lv_obj_set_x(ui_Image2, 0);
  lv_obj_set_y(ui_Image2, 0);

  lv_obj_set_align(ui_Image2, LV_ALIGN_TOP_MID);

  lv_obj_add_flag(ui_Image2, LV_OBJ_FLAG_ADV_HITTEST);
  lv_obj_clear_flag(ui_Image2, LV_OBJ_FLAG_SCROLLABLE);

  // ui_Button0

  ui_Button1 = lv_btn_create(ui_Screen1);

  lv_obj_set_width(ui_Button1, 100);
  lv_obj_set_height(ui_Button1, 50);

  lv_obj_set_x(ui_Button1, 0);
  lv_obj_set_y(ui_Button1, 0);

  lv_obj_set_align(ui_Button1, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
  lv_obj_clear_flag(ui_Button1, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_add_event_cb(ui_Button1, ui_event_Button1, LV_EVENT_ALL, NULL);
  lv_obj_set_style_opa(ui_Button1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}
void ui_Screen2_screen_init(void)
{

  // ui_Screen2

  ui_Screen2 = lv_obj_create(NULL);

  lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);

  // arc
  meter = lv_meter_create(ui_Screen2);
  lv_obj_center(meter);
  lv_obj_set_size(meter, 200, 200);

  /*Add a scale first*/
  lv_meter_scale_t *scale = lv_meter_add_scale(meter);
  lv_meter_set_scale_ticks(meter, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
  lv_meter_set_scale_major_ticks(meter, scale, 8, 4, 15, lv_color_black(), 10);

  lv_meter_indicator_t *indic;

  /*Add a blue arc to the start*/
  indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_meter_set_indicator_start_value(meter, indic, 0);
  lv_meter_set_indicator_end_value(meter, indic, 20);

  /*Make the tick lines blue at the start of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                   false, 0);
  lv_meter_set_indicator_start_value(meter, indic, 0);
  lv_meter_set_indicator_end_value(meter, indic, 20);

  /*Add a red arc to the end*/
  indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
  lv_meter_set_indicator_start_value(meter, indic, 80);
  lv_meter_set_indicator_end_value(meter, indic, 100);

  /*Make the tick lines red at the end of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                   0);
  lv_meter_set_indicator_start_value(meter, indic, 80);
  lv_meter_set_indicator_end_value(meter, indic, 100);

  /*Add a needle line indicator*/
  line_indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
  // ui_Button2

  ui_Button2 = lv_btn_create(ui_Screen2);

  lv_obj_set_width(ui_Button2, 100);
  lv_obj_set_height(ui_Button2, 50);

  lv_obj_set_x(ui_Button2, 0);
  lv_obj_set_y(ui_Button2, 0);

  lv_obj_set_align(ui_Button2, LV_ALIGN_CENTER);

  lv_obj_add_flag(ui_Button2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
  lv_obj_clear_flag(ui_Button2, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_add_event_cb(ui_Button2, ui_event_Button2, LV_EVENT_ALL, NULL);
  lv_obj_set_style_opa(ui_Button2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  ui2_Label1 = lv_label_create(ui_Screen2);
  lv_obj_set_width(ui2_Label1, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(ui2_Label1, LV_SIZE_CONTENT); /// 1
  lv_obj_set_x(ui2_Label1, 0);
  lv_obj_set_y(ui2_Label1, 60);
  lv_obj_set_align(ui2_Label1, LV_ALIGN_CENTER);
  // lv_obj_set_style_text_font(ui2_Label1, &smileysans, 0); // 私有(本地)样式
  lv_obj_set_style_text_font(ui2_Label1, &lv_font_montserrat_14, 0);
  // lv_label_set_text(ui2_Label1, "力度");
  lv_label_set_text(ui2_Label1, "Feedback");
}

void ui_Screen3_screen_init(void)
{
  ui_Screen3 = lv_obj_create(NULL);
  lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_SCROLLABLE); /// Flags

  ui_Spinner1 = lv_spinner_create(ui_Screen3, 1000, 90);
  lv_obj_set_width(ui_Spinner1, 175);
  lv_obj_set_height(ui_Spinner1, 175);
  lv_obj_set_align(ui_Spinner1, LV_ALIGN_CENTER);
  lv_obj_clear_flag(ui_Spinner1, LV_OBJ_FLAG_CLICKABLE); /// Flags

  ui_Label2 = lv_label_create(ui_Screen3);
  lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT); /// 1
  lv_obj_set_x(ui_Label2, 0);
  lv_obj_set_y(ui_Label2, -10);
  lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);
  lv_label_set_text(ui_Label2, "connect wifi");

  ui_Label3 = lv_label_create(ui_Screen3);
  lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);  /// 1
  lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT); /// 1
  lv_obj_set_x(ui_Label3, 0);
  lv_obj_set_y(ui_Label3, 12);
  lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void ui_init(void)
{
  lv_disp_t *dispp = lv_disp_get_default();
  lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                            false, LV_FONT_DEFAULT);
  lv_disp_set_theme(dispp, theme);
  ui_Screen0_screen_init();
  ui_Screen1_screen_init();
  ui_Screen2_screen_init();
  ui_Screen3_screen_init();
  lv_disp_load_scr(ui_Screen0);
}
