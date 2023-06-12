#ifndef display_manager_
#define display_manager_

#include <SPI.h>
#include <TFT_eSPI.h>

#include "APP_FILES/list_of_communicates.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define RIGHT_ARROW_PRESSED 0x01
#define LEFT_ARROW_PRESSED 0x02
#define CHOICE_1_PRESSED 0x04
#define CHOICE_2_PRESSED 0x08
#define CHOICE_3_PRESSED 0x10
#define SET_ALM_ON_OFF_PRESSED 0x20
#define GET_FROM_NTP_PRESSED 0x40
#define SAVE_DATETIME_PRESSED 0x80
#define INC_LO_TR_PRESSED 0x100
#define DEC_LO_TR_PRESSED 0x200
#define INC_HI_TR_PRESSED 0x400
#define DEC_HI_TR_PRESSED 0x800
#define INC_HOUR_PRESSED 0x1000
#define DEC_HOUR_PRESSED 0x2000
#define INC_MIN_PRESSED 0x4000
#define DEC_MIN_PRESSED 0x8000
#define INC_DAY_PRESSED 0x10000
#define DEC_DAY_PRESSED 0x20000
#define INC_MON_PRESSED 0x40000
#define DEC_MON_PRESSED 0x80000
#define INC_YEAR_PRESSED 0x100000
#define DEC_YEAR_PRESSED 0x200000

struct press_point{
  uint16_t x;
  uint16_t y;
};

float get_sensor_reading(uint16_t sensor_voltage);

class display_manager{
public:
  static TFT_eSPI tft;
  static TFT_eSprite main_background_sprite;
  static TFT_eSprite left_arrow_sprite; 
  static TFT_eSprite right_arrow_sprite; 
  static TFT_eSprite humidity_gauge_sprite; 
  static TFT_eSprite temperature_gauge_sprite; 

  static bool refresh_window;
  static Linked_List message_list;
  static Adafruit_BME280& bme_ref;
  static press_point coords;

  static void main();
  static void append_and_print_start_window(const char* message, bool status = true, bool add_status = false);

private:
  typedef void (*screen_function)(void);
  static screen_function array_of_screens[7];
  static void touch_debouncer();
  static void touch_manager();
  static void draw_temperature_screen();
  static void draw_temperature_graph_screen();
  static void draw_chart_config_screen();
  static void draw_alarm_config_screen();
  static void draw_time_config_screen();
  static void draw_logging_screen();
  static void draw_weather_forecast_screen();
  static void write_status(bool ok, int x, int y);
  static bool pressed;
  static uint32_t touch_flags;
};

#endif