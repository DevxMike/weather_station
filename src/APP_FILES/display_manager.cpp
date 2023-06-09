#include "APP_FILES/display_manager.h"
#include "APP_FILES/logger.h"
#include "APP_FILES/main.h"
#include "APP_FILES/file_management.h"
#include "APP_FILES/time_manager.h"
#include "APP_FILES/analog.h"

#include "GRAPHICS/arrow_left.h"
#include "GRAPHICS/arrow_right.h"
#include "GRAPHICS/background.h"
#include "GRAPHICS/background_graph.h"
#include "GRAPHICS/sky_bg.h"

#include "GRAPHICS/active.h"
#include "GRAPHICS/not_active.h"

#include "GRAPHICS/set_alm.h"
#include "GRAPHICS/clear_alm.h"
#include "GRAPHICS/save_button.h"
#include "GRAPHICS/ntp_button.h"

#include "GRAPHICS/increment.h"
#include "GRAPHICS/decrement.h"

#include "GRAPHICS/sm_increment.h"
#include "GRAPHICS/sm_decrement.h"

#include "GRAPHICS/chart.h"

#include "GRAPHICS/gauge1_sm.h"
#include "GRAPHICS/gauge2_sm.h"

#include <math.h>

/* NAVIGATION BUTTONS START */

#define MAX_LEFT_ARROW_X 50
#define MIN_LEFT_ARROW_Y 190

#define MIN_RIGHT_ARROW_X 260
#define MIN_RIGHT_ARROW_Y 190

/* NAVIGATION BUTTONS END */

/* RADIO BUTTONS START */

#define MIN_CHOICE_X 20
#define MAX_CHOICE_X 70
#define MIN_CHOICE_1_Y 70
#define MAX_CHOICE_1_Y 108
#define MIN_CHOICE_2_Y 110
#define MAX_CHOICE_2_Y 148
#define MIN_CHOICE_3_Y 150
#define MAX_CHOICE_3_Y 188

/* RADIO BUTTONS END */

/* ALARM SCREEN START */

#define MIN_INC_X 230
#define MAX_INC_X 270
#define MIN_INC_1_Y 80
#define MAX_INC_1_Y 120
#define MIN_INC_2_Y 125
#define MAX_INC_2_Y 165

#define MIN_DEC_X 180
#define MAX_DEC_X 220
#define MIN_DEC_1_Y 80
#define MAX_DEC_1_Y 120
#define MIN_DEC_2_Y 125
#define MAX_DEC_2_Y 165

#define MIN_SET_CLR_X 125
#define MAX_SET_CLR_X 195
#define MIN_SET_CLR_Y 200
#define MAX_SET_CLR_Y 238

/* ALARM SCREEN END */

/* DATETIME SCREEN START */

#define MIN_SM_DEC_X 250
#define MAX_SM_DEC_X 275

#define MIN_SM_INC_X 280
#define MAX_SM_INC_X 305

#define MIN_SM_DEC_1_Y 40
#define MAX_SM_DEC_1_Y 65
#define MIN_SM_DEC_2_Y 70
#define MAX_SM_DEC_2_Y 95
#define MIN_SM_DEC_3_Y 100
#define MAX_SM_DEC_3_Y 125
#define MIN_SM_DEC_4_Y 130
#define MAX_SM_DEC_4_Y 155
#define MIN_SM_DEC_5_Y 160
#define MAX_SM_DEC_5_Y 185

#define MIN_SM_INC_1_Y 40
#define MAX_SM_INC_1_Y 65
#define MIN_SM_INC_2_Y 70
#define MAX_SM_INC_2_Y 95
#define MIN_SM_INC_3_Y 100
#define MAX_SM_INC_3_Y 125
#define MIN_SM_INC_4_Y 130
#define MAX_SM_INC_4_Y 155
#define MIN_SM_INC_5_Y 160
#define MAX_SM_INC_5_Y 185

#define MIN_SAVE_X 70
#define MAX_SAVE_X 145
#define MIN_NTP_X 175
#define MAX_NTP_X 250

#define MIN_SAVE_Y 200
#define MAX_SAVE_Y 238
#define MIN_NTP_Y 200 
#define MAX_NTP_Y 238

/* DATETIME SCREEN END */

#define BARO_X0 100
#define BARO_Y0 115
#define BARO_RADIUS 72
#define BARO_STEP 4.15

#define BARO_MIN 978
#define BARO_MAX 1050.5

#define BARO_MIN_ANGLE 135

#define CHART_START_Y 175

const char config_strings[3][30]{
  " EVERY 10 MIN ",
  " EVERY 30 MIN ",
  " EVERY HOUR   ",
};

  int16_t get_height(float degrees){ // get height of bar on a chart
    if(degrees > 55){
      degrees = 55;
    }
    else if(degrees < -20){
      degrees = -19;
    }

    return int16_t((abs(((-20 - degrees)* 16.5)/ 10.0)) );
  }
  int16_t get_starting_point(int16_t height){ // get starting point of bar on a chart
    return CHART_START_Y - height;
  }

  float get_sensor_reading(uint16_t sensor_voltage){ // linear function to get reading of air quality sensor
    auto tmp = (0.000165 * sensor_voltage - 0.095);
    if(tmp < 0) tmp = 0;

    return tmp;
  }

  display_manager::screen_function display_manager::array_of_screens[6]{ // function pointer array - screen drawing functions
    draw_temperature_screen,
    draw_temperature_graph_screen,
    draw_chart_config_screen,
    draw_logging_screen,
    draw_alarm_config_screen,
    draw_time_config_screen
  };


struct Point { // point on barometer
    double x;
    double y;
};

struct Point get_point_on_circle(struct Point center, double radius, double angle_degrees) { // used to get point on barometer according to actual angle
    double angle_radians = angle_degrees * M_PI / 180.0;
    double x = center.x + radius * cos(angle_radians);
    double y = center.y + radius * sin(angle_radians);
    struct Point p = {x, y};
    return p;
}
  void display_manager::draw_temperature_screen(){
      char buffer[100];
      static float temp = display_manager::bme_ref.readTemperature();
      static float hum = display_manager::bme_ref.readHumidity();
      static float press = display_manager::bme_ref.readPressure() / 100;
      static float dust_voltage = get_sensor_reading(analog_measures::sensor_voltage);
      static uint16_t voltage = analog_measures::input_voltage;

      static unsigned long temperature_timer;

      auto& tmp = Logging::time_info; // get time

      static bool init_sprite = false;

      if(!init_sprite){ // init buffers 

        main_background_sprite.setColorDepth(8);
        main_background_sprite.createSprite(tft.width(), tft.height());

        left_arrow_sprite.createSprite(48, 48);
        left_arrow_sprite.setSwapBytes(true);
        right_arrow_sprite.createSprite(48, 48);
        right_arrow_sprite.setSwapBytes(true);
        main_background_sprite.setSwapBytes(true);
        display_manager::tft.setSwapBytes(true);

        humidity_gauge_sprite.createSprite(80, 80);
        temperature_gauge_sprite.createSprite(80, 80);

        init_sprite = true;

        tft.fillScreen(TFT_BLACK);
      }


      sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left); // draw arrows
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2); // draw time and date
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);

      if(millis() - temperature_timer > 1000){ // read every 1000ms
        temp = display_manager::bme_ref.readTemperature();
        hum = hum * 0.9 + 0.1 * display_manager::bme_ref.readHumidity();
        press = press * 0.9 + 0.1 * (display_manager::bme_ref.readPressure() / 100);
        voltage = voltage * 0.7 + 0.3 * analog_measures::input_voltage;
        dust_voltage = dust_voltage * 0.85 + 0.15 * analog_measures::sensor_voltage;
        temperature_timer = millis();
      }
      // draw readings
      sprintf(buffer, "%.1f*C", temp);
      main_background_sprite.drawString("T: ", 192, 35, 2);

      if(temp >= 26){
        main_background_sprite.setTextColor(TFT_RED, TFT_BLACK);
      }
      else if(temp >= 19){
        main_background_sprite.setTextColor(TFT_GREEN, TFT_BLACK);
      }
      else{
        main_background_sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
      }

      main_background_sprite.drawString(buffer, 220, 35, 2);
      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);

      sprintf(buffer, "%.1f%%", hum);
      main_background_sprite.drawString("H: ", 192, 70, 2);

      if(hum >= 70){
        main_background_sprite.setTextColor(TFT_RED, TFT_BLACK);
      }
      else if(hum >= 40){
        main_background_sprite.setTextColor(TFT_GREEN, TFT_BLACK);
      }
      else{
        main_background_sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
      }

      main_background_sprite.drawString(buffer, 220, 70, 2);

      sprintf(buffer, "%.1f", press);
      
      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);

      main_background_sprite.drawString("P:", 192, 100, 2);


      if(press >= 1020){
        main_background_sprite.setTextColor(TFT_GREEN, TFT_BLACK);
      }
      else if(press >= 1005){
        main_background_sprite.setTextColor(TFT_RED, TFT_BLACK);
      }
      else{
        main_background_sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
      }

      main_background_sprite.drawString(buffer, 220, 100, 2);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);

      main_background_sprite.drawString("Q:", 192, 130, 2);
      main_background_sprite.drawString("V:", 192, 160, 2);

      main_background_sprite.drawString(buffer, 220, 130, 2);

      sprintf(buffer, "%.2fV", voltage / 1000.0);
      main_background_sprite.drawString(buffer, 220, 160, 2);

      main_background_sprite.pushImage(20, 35, 160, 160, gauge1_sm);

      auto dust_percent = get_sensor_reading(dust_voltage);

      if(dust_percent >= 0.5){
        main_background_sprite.setTextColor(TFT_RED, TFT_BLACK);
      }
      else if(dust_percent >= 0.3){
        main_background_sprite.setTextColor(TFT_YELLOW, TFT_BLACK);
      }
      else{
        main_background_sprite.setTextColor(TFT_GREEN, TFT_BLACK);
      }

      sprintf(buffer, "%.2fmg", dust_percent);
      main_background_sprite.drawString(buffer, 220, 130, 2);

      // double end_x, end_y;

      if(press > BARO_MAX){
        press = BARO_MAX;
      }
      else if(press < BARO_MIN){
        press = BARO_MIN;
      }

      double delta = press - BARO_MIN; // get delta between min baro and actual


      double angle = BARO_MIN_ANGLE + (delta * BARO_STEP); // get angle on baro

      auto point = get_point_on_circle({BARO_X0, BARO_Y0}, BARO_RADIUS, angle); // get end points baro


      // main_background_sprite.drawLine(BARO_X0, BARO_Y0, end_x + 20, end_y + 42, TFT_BLACK);
      main_background_sprite.drawWideLine(BARO_X0, BARO_Y0, point.x, point.y , 2, TFT_BLACK); // draw baro needle

      main_background_sprite.drawSmoothCircle(100, 115, 81, TFT_BLACK, TFT_BLACK); // smooth edges of baro
      main_background_sprite.drawSmoothCircle(100, 115, 82, TFT_BLACK, TFT_BLACK);
      main_background_sprite.drawSmoothCircle(100, 115, 83, TFT_BLACK, TFT_BLACK);
      main_background_sprite.drawSmoothCircle(100, 115, 84, TFT_BLACK, TFT_BLACK);


      main_background_sprite.drawLine(190, 28, 190, 190, TFT_WHITE);


      main_background_sprite.pushSprite(0, 0); // swap buffers of display
  }

  void display_manager::draw_logging_screen(){
    auto& tmp = Logging::time_info;
    char buffer[100];
    
    sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);
      // main_background_sprite.pushImage(0, 0, 320, 240, sky_bg);

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left); // draw arrows
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2); // draw date time
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("LOG CONFIG", 80, 35, 2);

      bool changed = false;

      if(CHOICE_1_PRESSED & touch_flags){
        system_configuration.logging_config = 0;
        changed = true;
      }
      else if(CHOICE_2_PRESSED & touch_flags){
        system_configuration.logging_config = 1;
        changed = true;
      }
      else if(CHOICE_3_PRESSED & touch_flags){
        system_configuration.logging_config = 2;
        changed = true;
      }

      if(changed){ // change config if button was pressed
        update_system_config();
      }

      for(uint8_t i = 0; i < 3; ++i){ // draw buttons
        if(system_configuration.logging_config == i){
          main_background_sprite.pushImage(20, 70 + i * 40, 50, 38, active);  
          main_background_sprite.setTextColor(TFT_BLACK, TFT_GREEN);
        }
        else{
          main_background_sprite.pushImage(20, 70 + i * 40, 50, 38, not_active);
          main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
        }

        main_background_sprite.drawString(config_strings[i], 75, 72 + i * 40, 2);
      }

      main_background_sprite.pushSprite(0, 0); // swap buffers
  }

  void display_manager::draw_temperature_graph_screen(){
    auto& tmp = Logging::time_info;
    char buffer[100];

      sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);
      // main_background_sprite.pushImage(0, 0, 320, 240, sky_bg);

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left); // draw arrows
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2); // draw date time
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      // main_background_sprite.drawString("TEMPERATURE GRAPH", 30, 50, 2);
      main_background_sprite.pushImage(20, 36, 280, 160, chart);
      
      auto values = Logging::chart_list.get_elements(); // get pointer to first element

      int i = 0;

      main_background_sprite.setTextSize(1);

      while(values != nullptr){ // draw chart
        auto height = get_height(values->temperature);
        auto start_y = get_starting_point(height);

        main_background_sprite.fillRect(48 + 27 * i, start_y, 25, height, TFT_GREEN);
        
        auto hours = values->time_ago / 60; // get time for chart
        auto minutes = values->time_ago % 60;

        if(hours){
          sprintf(buffer, "%2dh", hours);
          main_background_sprite.drawString(buffer, 49 + 27 * i, 175, 1);
          sprintf(buffer, "%2dm", minutes);
          main_background_sprite.drawString(buffer, 49 + 27* i, 183, 1);
        }
        else{
          sprintf(buffer, "%2dm", minutes);
          main_background_sprite.drawString(buffer, 49 + 27 * i, 175, 1);
        }

        //Serial.println(values->time_ago);

        ++i;
        values = values->next;
      }

      main_background_sprite.pushSprite(0, 0);
  }

  void display_manager::draw_chart_config_screen(){ 
    auto& tmp = Logging::time_info;
    char buffer[100];

    sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);
      // main_background_sprite.pushImage(0, 0, 320, 240, sky_bg);

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left); // draw arrows 
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2); //draw date time
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("CHART UPDATE", 70, 35, 2);

      bool changed = false;

      if(CHOICE_1_PRESSED & touch_flags){
        system_configuration.graph_config = 0;
        changed = true;
      }
      else if(CHOICE_2_PRESSED & touch_flags){
        system_configuration.graph_config = 1;
        changed = true;
      }
      else if(CHOICE_3_PRESSED & touch_flags){
        system_configuration.graph_config = 2;
        changed = true;
      }

      if(changed){ // change config if button was pressed
        update_system_config();
      }

      for(uint8_t i = 0; i < 3; ++i){ // draw buttons
        if(system_configuration.graph_config == i){
          main_background_sprite.pushImage(20, 70 + i * 40, 50, 38, active);  
          main_background_sprite.setTextColor(TFT_BLACK, TFT_GREEN);
        }
        else{
          main_background_sprite.pushImage(20, 70 + i * 40, 50, 38, not_active);
          main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
        }

        main_background_sprite.drawString(config_strings[i], 75, 72 + i * 40, 2);
      }

      main_background_sprite.pushSprite(0, 0); // swap buffers
  }
  
  void display_manager::draw_alarm_config_screen(){
    auto& tmp = Logging::time_info;
    char buffer[100];

    auto& cfg = system_configuration;

      bool change = false;

      if(INC_LO_TR_PRESSED & touch_flags){
        if(cfg.alarm_low < 50 && (cfg.alarm_low + 1) < cfg.alarm_high){
          ++cfg.alarm_low;
          change = true;
        }
      }
      else if(INC_HI_TR_PRESSED & touch_flags){
        if(cfg.alarm_high < 60){
          ++cfg.alarm_high;
          change =  true;
        }
      }
      else if(DEC_HI_TR_PRESSED & touch_flags){
        if((cfg.alarm_high - 1) > cfg.alarm_low && cfg.alarm_high > -20){
          --cfg.alarm_high;
          change = true;
        }
      }
      else if(DEC_LO_TR_PRESSED & touch_flags){
        if(cfg.alarm_low > -30){
          --cfg.alarm_low;
          change = true;
        }
      }
      else if(SET_ALM_ON_OFF_PRESSED & touch_flags){
        cfg.alarm_set ^= 0x01;
        change = true;
      }

      if(change){ // change alarms, set/clear alarm if button was pressed
        update_system_config();
      }
    sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);
      // main_background_sprite.pushImage(0, 0, 320, 240, sky_bg);

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right); // draw arrows
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2); // draw date time
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("ALM SETTINGS", 70, 35, 2); // draw tresholds and buttons

      sprintf(buffer, "LO TR: %i", system_configuration.alarm_low);
      
      main_background_sprite.drawString(buffer, 50, 85, 2);
      main_background_sprite.pushImage(180, 80, 40, 40, decrement);
      main_background_sprite.pushImage(230, 80, 40, 40, increment);


      sprintf(buffer, "HI TR: %i", system_configuration.alarm_high);

      main_background_sprite.drawString(buffer, 50, 130, 2);

      main_background_sprite.pushImage(180, 125, 40, 40, decrement);
      main_background_sprite.pushImage(230, 125, 40, 40, increment);

      main_background_sprite.pushImage(110, 185, 100, 60,
        !system_configuration.alarm_set? set_alm : clear_alm      
      );

      main_background_sprite.pushSprite(0, 0); // swap buffer
  }

  void display_manager::draw_time_config_screen(){
    auto& tmp = Logging::time_info;
    char buffer[100];
    static auto static_time = Logging::time_info;

    sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);
      // main_background_sprite.pushImage(0, 0, 320, 240, sky_bg);

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left); // draw arrows
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2); // draw date time
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);

      if(INC_HOUR_PRESSED & touch_flags){ // change displayed values if button was pressed
        static_time.tm_hour = (static_time.tm_hour + 1) % 24;
      }
      else if(INC_MIN_PRESSED & touch_flags){
        static_time.tm_min = (static_time.tm_min + 1) % 60; 
      }
      else if(INC_DAY_PRESSED & touch_flags){
        static_time.tm_mday = (static_time.tm_mday + 1) % 31;
        if(static_time.tm_mday == 0){
          static_time.tm_mday = 1;
        } 
      }
      else if(INC_MON_PRESSED & touch_flags){
        static_time.tm_mon = (static_time.tm_mon + 1) % 12; 
      }
      else if(INC_YEAR_PRESSED & touch_flags){
        static_time.tm_year = (static_time.tm_year + 1) % 300;
      } 
      else if(DEC_HOUR_PRESSED & touch_flags){
        if(static_time.tm_hour == 0){
          static_time.tm_hour = 23;
        }
        else{
          static_time.tm_hour -= 1;
        }
      }
      else if(DEC_MIN_PRESSED & touch_flags){
        if(static_time.tm_min == 0){
          static_time.tm_min = 59;
        }
        else{
          static_time.tm_min -= 1;
        }
      }
      else if(DEC_DAY_PRESSED & touch_flags){
        if(static_time.tm_mday == 1){
          static_time.tm_mday = 31;
        }
        else{
          static_time.tm_mday -= 1;
        }
      }
      else if(DEC_MON_PRESSED & touch_flags){
        if(static_time.tm_mon == 0){
          static_time.tm_mon = 11;
        }
        else{
          static_time.tm_mon -= 1;
        }
      }
      else if(DEC_YEAR_PRESSED & touch_flags){
        if(static_time.tm_year == 0){
          static_time.tm_year = 299;
        }
        else{
          static_time.tm_year -= 1;
        }
      } 
      else if(SAVE_DATETIME_PRESSED & touch_flags){ // set system values to displayed vaules
        time_manager::set_local_time(
          static_time.tm_year, static_time.tm_mon, static_time.tm_mday,
          static_time.tm_hour, static_time.tm_min, 0, 1
          );
      }
      else if(GET_FROM_NTP_PRESSED & touch_flags){ // get time from ntp
        time_manager::get_time_from_ntp();
      }

      sprintf(buffer, "HOUR: %02i", static_time.tm_hour); // display values
      main_background_sprite.drawString(buffer, 20, 35, 2);
      
      sprintf(buffer, "MIN : %02i", static_time.tm_min);
      main_background_sprite.drawString(buffer, 20, 65, 2);

      sprintf(buffer, "DAY : %02i", static_time.tm_mday);
      main_background_sprite.drawString(buffer, 20, 95, 2);

      sprintf(buffer, "MON : %02i", static_time.tm_mon + 1);
      main_background_sprite.drawString(buffer, 20, 125, 2);

      sprintf(buffer, "YEAR: %i", static_time.tm_year + 1900);
      main_background_sprite.drawString(buffer, 20, 155, 2);

      for(int i = 0; i < 5; ++i){ // dislpay buttons
        main_background_sprite.pushImage(250, 40 + 30 * i, 25, 25, sm_decrement);
        main_background_sprite.pushImage(280, 40 + 30 * i, 25, 25, sm_increment);
      }

      // dislpay buttons
      main_background_sprite.pushImage(60, 185, 100, 60, save_button);
      main_background_sprite.pushImage(160, 185, 100, 60, ntp_button);


      main_background_sprite.pushSprite(0, 0);
  }
  
  void display_manager::main(){ 
    static uint8_t refresh_state = 0;
    static unsigned long refresh_timer = 0;
    static uint8_t current_screen = 0;

    static bool time_init = false;

    touch_debouncer(); // check for touch
    touch_manager(); // check if certain button was pressed

    // switching between the screens
    if(LEFT_ARROW_PRESSED & touch_flags){ // navigate left
      if(current_screen == 0){
        current_screen = 5;
      }
      else{
        current_screen = current_screen - 1;
      }
    }
    else if(RIGHT_ARROW_PRESSED & touch_flags){ // navigate right
      current_screen = (current_screen + 1) % 6;
    }


    array_of_screens[current_screen](); // draw current screen

    touch_flags = 0; // reset touch flags
    
    // draw_temperature_screen(tft);
    switch(refresh_state){ // refresh screen every 500 ms
      case 0:
        refresh_state = 1;
        refresh_timer = millis();
      break;

      case 1:
        if(millis() - refresh_timer >= 500){
          refresh_state = 0;
          // Serial.println(current_screen);
        }
      break;
    }

    if(!time_init){ // start with time date set screen if time was not set properly
      // check if the time has been initialised
      if(!(system_flags & DEV_STATE_RTC_OK)){
        // start with date setting screen
        current_screen = 5;
      } 
      time_init = true;
    }
    // if(pressed){
    //   Serial.print(coords.x);
    //   Serial.print(", ");
    //   Serial.print(coords.y);
    //   Serial.print("\n\r");

    //   pressed = false;
    // }
  }

  void display_manager::append_and_print_start_window(const char* message, bool status, bool add_status){
    static bool init_sprite = false;
    if(!init_sprite){
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);

      main_background_sprite.setColorDepth(8);
      main_background_sprite.createSprite(tft.width(), tft.height());
      main_background_sprite.setSwapBytes(true);
      main_background_sprite.setTextSize(2);
      init_sprite = true;
    }
    int line = 0;

    
    main_background_sprite.fillSprite(TFT_BLACK); // clear buffer with black color

    message_list.append(message, status, add_status); // create new list element

    auto tmp = message_list.get_elements(); // get pointer to elements

    while(tmp != nullptr){
      if(tmp->has_status){ // draw message with status
        write_status(tmp->status, 0, line*18);
        main_background_sprite.drawString(tmp->line, 65, line*18, 1);
      }
      else{ // draw regular message
        main_background_sprite.drawString(tmp->line, 0, line * 18, 1);
      }
      ++line;
      tmp = tmp->next;
    }

    main_background_sprite.pushSprite(0, 0);
  }

  void display_manager::touch_debouncer(){ 
    static uint8_t debouncer_state = 0;
    static unsigned long timer = 0;
    uint16_t dummy_x, dummy_y;

    switch(debouncer_state){
      case 0:
        if(tft.getTouch(&coords.x, &coords.y)){ // get touch and set timer to 30ms
          timer = millis();
          debouncer_state = 1;
        }
      break;

      case 1:
        if(millis() - timer > 30){
          if(tft.getTouch(&coords.x, &coords.y)){ // if touch is still present set pressed to true
            debouncer_state = 2;
            pressed = true;
            // Serial.println(coords.x);
            // Serial.println(coords.y);
          }
        }

      break;

      case 2:
        if(!tft.getTouch(&dummy_x, &dummy_y)){ // wait till there is no touch
          debouncer_state = 0;
        }
      break;
    }
  }

  void display_manager::touch_manager(){
    if(pressed){ //if touch has been detected
      if(coords.x <= MAX_LEFT_ARROW_X && coords.y >= MIN_LEFT_ARROW_Y){
        touch_flags |= LEFT_ARROW_PRESSED;
      }
      if(coords.x >= MIN_RIGHT_ARROW_X && coords.y >= MIN_RIGHT_ARROW_Y){
        touch_flags |= RIGHT_ARROW_PRESSED;
      }
      if(coords.x >= MIN_CHOICE_X && coords.x <= MAX_CHOICE_X && coords.y >= MIN_CHOICE_1_Y && coords.y <= MAX_CHOICE_1_Y){
        touch_flags |= CHOICE_1_PRESSED;
      }
      if(coords.x >= MIN_CHOICE_X && coords.x <= MAX_CHOICE_X && coords.y >= MIN_CHOICE_2_Y && coords.y <= MAX_CHOICE_2_Y){
        touch_flags |= CHOICE_2_PRESSED;
      }
      if(coords.x >= MIN_CHOICE_X && coords.x <= MAX_CHOICE_X && coords.y >= MIN_CHOICE_3_Y && coords.y <= MAX_CHOICE_3_Y){
        touch_flags |= CHOICE_3_PRESSED;
      }
      if(coords.x >= MIN_INC_X && coords.x <= MAX_INC_X && coords.y >= MIN_INC_1_Y && coords.y <= MAX_INC_1_Y){
        touch_flags |= INC_LO_TR_PRESSED;
      }
      if(coords.x >= MIN_INC_X && coords.x <= MAX_INC_X && coords.y >= MIN_INC_2_Y && coords.y <= MAX_INC_2_Y){
        touch_flags |= INC_HI_TR_PRESSED;
      }
      if(coords.x >= MIN_DEC_X && coords.x <= MAX_DEC_X && coords.y >= MIN_DEC_1_Y && coords.y <= MAX_DEC_1_Y){
        touch_flags |= DEC_LO_TR_PRESSED;
      }
      if(coords.x >= MIN_DEC_X && coords.x <= MAX_DEC_X && coords.y >= MIN_DEC_2_Y && coords.y <= MAX_DEC_2_Y){
        touch_flags |= DEC_HI_TR_PRESSED;
      }
      if(coords.x >= MIN_SET_CLR_X && coords.x <= MAX_SET_CLR_X && coords.y >= MIN_SET_CLR_Y && coords.y <= MAX_SET_CLR_Y){
        touch_flags |= SET_ALM_ON_OFF_PRESSED;
      }
      if(coords.x >= MIN_SM_DEC_X && coords.x <= MAX_SM_DEC_X && coords.y >= MIN_SM_DEC_1_Y && coords.y <= MAX_SM_DEC_1_Y){
        touch_flags |= DEC_HOUR_PRESSED;
      }
      if(coords.x >= MIN_SM_DEC_X && coords.x <= MAX_SM_DEC_X && coords.y >= MIN_SM_DEC_2_Y && coords.y <= MAX_SM_DEC_2_Y){
        touch_flags |= DEC_MIN_PRESSED;
      }
      if(coords.x >= MIN_SM_DEC_X && coords.x <= MAX_SM_DEC_X && coords.y >= MIN_SM_DEC_3_Y && coords.y <= MAX_SM_DEC_3_Y){
        touch_flags |= DEC_DAY_PRESSED;
      }
      if(coords.x >= MIN_SM_DEC_X && coords.x <= MAX_SM_DEC_X && coords.y >= MIN_SM_DEC_4_Y && coords.y <= MAX_SM_DEC_4_Y){
        touch_flags |= DEC_MON_PRESSED;
      }
      if(coords.x >= MIN_SM_DEC_X && coords.x <= MAX_SM_DEC_X && coords.y >= MIN_SM_DEC_5_Y && coords.y <= MAX_SM_DEC_5_Y){
        touch_flags |= DEC_YEAR_PRESSED;
      }
      if(coords.x >= MIN_SM_INC_X && coords.x <= MAX_SM_INC_X && coords.y >= MIN_SM_INC_1_Y && coords.y <= MAX_SM_INC_1_Y){
        touch_flags |= INC_HOUR_PRESSED;
      }
      if(coords.x >= MIN_SM_INC_X && coords.x <= MAX_SM_INC_X && coords.y >= MIN_SM_INC_2_Y && coords.y <= MAX_SM_INC_2_Y){
        touch_flags |= INC_MIN_PRESSED;
      }
      if(coords.x >= MIN_SM_INC_X && coords.x <= MAX_SM_INC_X && coords.y >= MIN_SM_INC_3_Y && coords.y <= MAX_SM_INC_3_Y){
        touch_flags |= INC_DAY_PRESSED;
      }
      if(coords.x >= MIN_SM_INC_X && coords.x <= MAX_SM_INC_X && coords.y >= MIN_SM_INC_4_Y && coords.y <= MAX_SM_INC_4_Y){
        touch_flags |= INC_MON_PRESSED;
      }
      if(coords.x >= MIN_SM_INC_X && coords.x <= MAX_SM_INC_X && coords.y >= MIN_SM_INC_5_Y && coords.y <= MAX_SM_INC_5_Y){
        touch_flags |= INC_YEAR_PRESSED;
      }
      if(coords.x >= MIN_SAVE_X && coords.x <= MAX_SAVE_X && coords.y >= MIN_SAVE_Y && coords.y <= MAX_SAVE_Y){
        touch_flags |= SAVE_DATETIME_PRESSED;
      }
      if(coords.x >= MIN_NTP_X && coords.x <= MAX_NTP_X && coords.y >= MIN_NTP_Y && coords.y <= MAX_NTP_Y){
        touch_flags |= GET_FROM_NTP_PRESSED;
      }

      pressed = false;
    }
  }

  void display_manager::write_status(bool ok, int x, int y){
    main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    main_background_sprite.drawString("[", x, y, 1);
    
    if(ok){
      main_background_sprite.setTextColor(TFT_GREEN, TFT_BLACK);
      main_background_sprite.drawString("OK", x+17, y, 1);
    }
    else{
      main_background_sprite.setTextColor(TFT_RED, TFT_BLACK);
      main_background_sprite.drawString("NOK", x+14, y, 1);
    }
    main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    main_background_sprite.drawString("]", x+50, y, 1);
  }