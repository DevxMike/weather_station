#include "APP_FILES/display_manager.h"
#include "APP_FILES/logger.h"
#include "GRAPHICS/arrow_left.h"
#include "GRAPHICS/arrow_right.h"
#include "GRAPHICS/background.h"
#include "GRAPHICS/background_graph.h"

#define MAX_LEFT_ARROW_X 50
#define MIN_LEFT_ARROW_Y 190

#define MIN_RIGHT_ARROW_X 260
#define MIN_RIGHT_ARROW_Y 190

TFT_eSprite main_background_sprite = TFT_eSprite(&display_manager::tft);
TFT_eSprite left_arrow_sprite = TFT_eSprite(&display_manager::tft);
TFT_eSprite right_arrow_sprite = TFT_eSprite(&display_manager::tft);


  display_manager::screen_function display_manager::array_of_screens[5]{
    draw_temperature_screen,
    draw_logging_screen,
    draw_temperature_graph_screen,
    draw_chart_config_screen,
    draw_alarm_config_screen
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

        init_sprite = true;

        tft.fillScreen(TFT_BLACK);
      }


      sprintf(buffer, 
          "%02i.%02i.%02i        %02i:%02i", 
          tmp.tm_mday, tmp.tm_mon + 1, tmp.tm_year+1900,
          tmp.tm_hour, tmp.tm_min
        );

      main_background_sprite.fillSprite(TFT_BLACK);
      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      // tft.fillScreen(TFT_BLUE); 
      // tft.setTextColor(TFT_WHITE, TFT_BLUE);

      // display_manager::tft.setTextSize(2);
      // tft.drawString(buffer, 2, 0, 2);

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
      sprintf(buffer, "Pressure: %.1f", press);
      main_background_sprite.drawString(buffer, 30, 100, 2);

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
      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("LOGGING CONFIG", 30, 50, 2);

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
      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("TEMPERATURE GRAPH", 30, 50, 2);

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
      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("CHART CONFIG", 30, 50, 2);

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
      left_arrow_sprite.pushImage(0, 0, 48, 48, arrow_left);
      right_arrow_sprite.pushImage(0, 0, 48, 48, arrow_right);
      left_arrow_sprite.pushToSprite(&main_background_sprite, 10, 190);
      right_arrow_sprite.pushToSprite(&main_background_sprite, 260, 190);

      main_background_sprite.setTextColor(TFT_WHITE, TFT_BLACK);
      main_background_sprite.setTextSize(2);
      main_background_sprite.drawString(buffer, 2, 0, 2);
      main_background_sprite.drawLine(0, 28, 320, 28, TFT_WHITE);
      main_background_sprite.drawString("SET_ALARM", 30, 50, 2);

      main_background_sprite.pushSprite(0, 0);
  }


  
  void display_manager::main(){ 
    static uint8_t refresh_state = 0;
    static unsigned long refresh_timer = 0;
    static uint8_t current_screen = 0;

    touch_debouncer();
    touch_manager();

    if(LEFT_ARROW_PRESSED & touch_flags){
      if(current_screen == 0){
        current_screen = 4;
      }
      else{
        current_screen = current_screen - 1;
      }
    }
    else if(RIGHT_ARROW_PRESSED & touch_flags){
      current_screen = (current_screen + 1) % 5;
    }

    touch_flags = 0;

    array_of_screens[current_screen]();
    
    // draw_temperature_screen(tft);
    switch(refresh_state){
      case 0:
        refresh_state = 1;
        refresh_timer = millis();
      break;

      case 1:
        if(millis() - refresh_timer >= 500){
          refresh_state = 0;
          Serial.println(current_screen);
        }
      break;
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
      else{
        //TODO
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