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
#define SET_ON_PRESSED 0x20
#define SET_OFF_PRESSED 0x40

struct press_point{
  uint16_t x;
  uint16_t y;
};

class display_manager{
public:
  static TFT_eSPI tft;
  static bool refresh_window;
  static Linked_List message_list;
  static Adafruit_BME280& bme_ref;
  static press_point coords;

  static void main();
  static void append_and_print_start_window(const char* message, bool status = true, bool add_status = false);

private:
  typedef void (*screen_function)(void);
  static screen_function array_of_screens[5];
  static void touch_debouncer();
  static void touch_manager();
  static void draw_temperature_screen();
  static void draw_logging_screen();
  static void write_status(bool ok, int x, int y);
  static bool pressed;
  static uint16_t touch_flags;
};

#endif