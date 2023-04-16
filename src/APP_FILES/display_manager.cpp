#include "APP_FILES/display_manager.h"
#include "APP_FILES/logger.h"
#include "APP_FILES/main.h"
#include "APP_FILES/file_management.h"

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

#include "GRAPHICS/temp_gauge.h"

#define MAX_LEFT_ARROW_X 50
#define MIN_LEFT_ARROW_Y 190

#define MIN_RIGHT_ARROW_X 260
#define MIN_RIGHT_ARROW_Y 190

#define MIN_CHOICE_X 20
#define MAX_CHOICE_X 70
#define MIN_CHOICE_1_Y 70
#define MAX_CHOICE_1_Y 108
#define MIN_CHOICE_2_Y 110
#define MAX_CHOICE_2_Y 148
#define MIN_CHOICE_3_Y 150
#define MAX_CHOICE_3_Y 188

#define CHART_START_Y 175

const char config_strings[3][30]{
  " EVERY 10 MIN ",
  " EVERY 30 MIN ",
  " EVERY HOUR   ",
};

  int16_t get_height(int16_t degrees){
    if(degrees > 55){
      degrees = 55;
    }
    else if(degrees < -20){
      degrees = -19;
    }

    return int16_t((abs(-20 - degrees) * 16.5) / 10.0);
  }
  int16_t get_starting_point(int16_t height){
    return CHART_START_Y - height;
  }

  display_manager::screen_function display_manager::array_of_screens[6]{
    draw_temperature_screen,
    draw_temperature_graph_screen,
    draw_chart_config_screen,
    draw_logging_screen,
    draw_alarm_config_screen,
    draw_time_config_screen
  };

  void display_manager::draw_temperature_screen(){
      char buffer[100];
      static float temp = display_manager::bme_ref.readTemperature();
      static float hum = display_manager::bme_ref.readHumidity();
      static float press = display_manager::bme_ref.readPressure();

      static unsigned long temperature_timer;

      auto& tmp = Logging::time_info;

      static bool init_sprite = false;

      if(!init_sprite){

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
      // main_background_sprite.pushImage(0, 0, 320, 240, sky_bg);
      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);

      if(millis() - temperature_timer > 3000){
        temp = temp * 0.9 + 0.1 * display_manager::bme_ref.readTemperature();
        hum = hum * 0.9 + 0.1 * display_manager::bme_ref.readHumidity();
        press = press * 0.9 + 0.1 * display_manager::bme_ref.readPressure();
        temperature_timer = millis();
      }

      sprintf(buffer, "Temperature: %.1f", temp);
      main_background_sprite.drawString(buffer, 30, 40, 2);
      sprintf(buffer, "Humidity: %.1f", hum);
      main_background_sprite.drawString(buffer, 30, 70, 2);
      sprintf(buffer, "Pressure: %.1f", press / 100.0);
      main_background_sprite.drawString(buffer, 30, 100, 2);

      // main_background_sprite.pushImage(60, 35, 200, 199, temp_gauge);
      // main_background_sprite.drawCircle(160, 155, 100, TFT_BLACK);
      // main_background_sprite.drawCircle(160, 155, 99, TFT_BLACK);
      // main_background_sprite.drawCircle(160, 155, 98, TFT_BLACK);

      main_background_sprite.pushSprite(0, 0);
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

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
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

      if(changed){
        update_system_config();
      }

      for(uint8_t i = 0; i < 3; ++i){
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

      main_background_sprite.pushSprite(0, 0);
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

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      // main_background_sprite.drawString("TEMPERATURE GRAPH", 30, 50, 2);
      main_background_sprite.pushImage(20, 36, 280, 160, chart);

      int16_t dummies[]{
        -10, -15, 20, 30, 23, 12, 5
      };

      for(int i = 0; i < 7; ++i){
        auto height = get_height(dummies[i]);
        auto start_y = get_starting_point(height);

        main_background_sprite.fillRect(48 + 12 * i, start_y, 10, height, TFT_GREEN);
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

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
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

      if(changed){
        update_system_config();
      }

      for(uint8_t i = 0; i < 3; ++i){
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

      main_background_sprite.pushSprite(0, 0);
  }
  
  void display_manager::draw_alarm_config_screen(){
    auto& tmp = Logging::time_info;
    char buffer[100];

    sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);
      // main_background_sprite.pushImage(0, 0, 320, 240, sky_bg);

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("ALM SETTINGS", 70, 35, 2);

      sprintf(buffer, "LO TR: %i", system_configuration.alarm_low);
      
      main_background_sprite.drawString(buffer, 20, 85, 2);
      main_background_sprite.pushImage(230, 80, 40, 40, decrement);
      main_background_sprite.pushImage(280, 80, 40, 40, increment);


      sprintf(buffer, "HI TR: %i", system_configuration.alarm_high);

      main_background_sprite.drawString(buffer, 20, 130, 2);

      main_background_sprite.pushImage(230, 125, 40, 40, decrement);
      main_background_sprite.pushImage(280, 125, 40, 40, increment);

      main_background_sprite.pushImage(110, 185, 100, 60,
        !system_configuration.alarm_set? set_alm : clear_alm      
      );

      main_background_sprite.pushSprite(0, 0);
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

      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);

      sprintf(buffer, "HOUR: %02i", static_time.tm_hour);
      main_background_sprite.drawString(buffer, 20, 35, 2);
      
      sprintf(buffer, "MIN : %02i", static_time.tm_min);
      main_background_sprite.drawString(buffer, 20, 65, 2);

      sprintf(buffer, "DAY : %02i", static_time.tm_mday);
      main_background_sprite.drawString(buffer, 20, 95, 2);

      sprintf(buffer, "MON : %02i", static_time.tm_mon + 1);
      main_background_sprite.drawString(buffer, 20, 125, 2);

      sprintf(buffer, "YEAR: %i", static_time.tm_year + 1900);
      main_background_sprite.drawString(buffer, 20, 155, 2);

      for(int i = 0; i < 5; ++i){
        main_background_sprite.pushImage(250, 40 + 30 * i, 25, 25, sm_decrement);
        main_background_sprite.pushImage(280, 40 + 30 * i, 25, 25, sm_increment);
      }

      main_background_sprite.pushImage(60, 185, 100, 60, save_button);
      main_background_sprite.pushImage(160, 185, 100, 60, ntp_button);


      main_background_sprite.pushSprite(0, 0);
  }
  
  void display_manager::main(){ 
    static uint8_t refresh_state = 0;
    static unsigned long refresh_timer = 0;
    static uint8_t current_screen = 0;

    static bool time_init = false;

    touch_debouncer();
    touch_manager();

    // switching between the screens
    if(LEFT_ARROW_PRESSED & touch_flags){
      if(current_screen == 0){
        current_screen = 5;
      }
      else{
        current_screen = current_screen - 1;
      }
    }
    else if(RIGHT_ARROW_PRESSED & touch_flags){
      current_screen = (current_screen + 1) % 6;
    }


    array_of_screens[current_screen]();

    touch_flags = 0;
    
    // draw_temperature_screen(tft);
    switch(refresh_state){
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

    if(!time_init){
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

    
    main_background_sprite.fillSprite(TFT_BLACK);

    message_list.append(message, status, add_status);

    auto tmp = message_list.get_elements();

    while(tmp != nullptr){
      if(tmp->has_status){
        write_status(tmp->status, 0, line*18);
        main_background_sprite.drawString(tmp->line, 65, line*18, 1);
      }
      else{
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
        if(tft.getTouch(&coords.x, &coords.y)){
          timer = millis();
          debouncer_state = 1;
        }
      break;

      case 1:
        if(millis() - timer > 50){
          if(tft.getTouch(&coords.x, &coords.y)){
            debouncer_state = 2;
            pressed = true;
            Serial.println(coords.x);
            Serial.println(coords.y);
          }
        }

      break;

      case 2:
        if(!tft.getTouch(&dummy_x, &dummy_y)){
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
      else if(coords.x >= MIN_RIGHT_ARROW_X && coords.y >= MIN_RIGHT_ARROW_Y){
        touch_flags |= RIGHT_ARROW_PRESSED;
      }
      else if(coords.x >= MIN_CHOICE_X && coords.x <= MAX_CHOICE_X && coords.y >= MIN_CHOICE_1_Y && coords.y <= MAX_CHOICE_1_Y){
        touch_flags |= CHOICE_1_PRESSED;
      }
      else if(coords.x >= MIN_CHOICE_X && coords.x <= MAX_CHOICE_X && coords.y >= MIN_CHOICE_2_Y && coords.y <= MAX_CHOICE_2_Y){
        touch_flags |= CHOICE_2_PRESSED;
      }
      else if(coords.x >= MIN_CHOICE_X && coords.x <= MAX_CHOICE_X && coords.y >= MIN_CHOICE_3_Y && coords.y <= MAX_CHOICE_3_Y){
        touch_flags |= CHOICE_3_PRESSED;
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