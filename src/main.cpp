#include "APP_FILES/main.h"
#include "APP_FILES/file_management.h"
#include "APP_FILES/list_of_communicates.h"
#include "APP_FILES/display_manager.h"
#include "APP_FILES/logger.h"
#include "APP_FILES/bme_manager.h"
#include "APP_FILES/time_manager.h"
#include "APP_FILES/analog.h"
#include "APP_FILES/comm.h"
#include "APP_FILES/alarm.h"

uint32_t system_flags = 0;
volatile config_info system_configuration{ 0 };

void touch_calibrate(TFT_eSPI& tft)
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

TFT_eSPI display_manager::tft = TFT_eSPI(); 
Linked_List display_manager::message_list;
list_of_logs Logging::chart_list;

uint16_t analog_measures::input_voltage{ 0 };
uint16_t analog_measures::sensor_voltage{ 0 };
uint8_t alarm_manager::alm{ 0 };

bool connected{ false };

TFT_eSprite display_manager::main_background_sprite = TFT_eSprite(&display_manager::tft);
TFT_eSprite display_manager::left_arrow_sprite = TFT_eSprite(&display_manager::tft);
TFT_eSprite display_manager::right_arrow_sprite = TFT_eSprite(&display_manager::tft);
TFT_eSprite display_manager::humidity_gauge_sprite = TFT_eSprite(&display_manager::tft);
TFT_eSprite display_manager::temperature_gauge_sprite = TFT_eSprite(&display_manager::tft);

Adafruit_BME280 bme;
Adafruit_BME280& Logging::bme_ref{bme};
Adafruit_BME280& display_manager::bme_ref{bme};
bool display_manager::pressed{false};
press_point display_manager::coords;
uint32_t display_manager::touch_flags{ 0 };
message_stack comm::messages;

SemaphoreHandle_t bme_semaphore {NULL};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

NTPClient& time_manager::client_ref{ timeClient };
long time_manager::gmtOffset_sec;
int time_manager::daylightOffset_sec;

int loging_freq, chart_freq;

// const char* mqtt_broker = "570d3f595a3e473790594292a6d5494d.s2.eu.hivemq.cloud";
// const char* mqtt_user_name = "michael";
// const char* mqtt_password = "7^UCSDU')_7@7eU";

struct tm Logging::time_info{ 0 };

unsigned long delayTime;

void setup(){
  Serial.begin(115200);
  
  // bme_semaphore = xSemaphoreCreateMutex();

  display_manager::tft.init();
  display_manager::tft.setRotation(1);
  display_manager::tft.setTextSize(2);
  touch_calibrate(display_manager::tft);

  display_manager::append_and_print_start_window("System init...");
  display_manager::append_and_print_start_window("ILI9341 initialized", true, true);
  display_manager::append_and_print_start_window("Sensor init");
    
  if(bme.begin(0x76)){
    display_manager::append_and_print_start_window("BME280 initialized", true, true);
    system_flags |= DEV_STATE_SENSOR_OK;
  }
  else{
    display_manager::append_and_print_start_window("Hard fault", false, true);
    appendFile(SD, "/error_logs.txt", "Could not initialize BME280 sensor\n");
    while(1);
  }

  display_manager::append_and_print_start_window("Initializing SD Card");


  if(SD.begin()){
    display_manager::append_and_print_start_window("SD Card initialized", true, true);
    system_flags |= DEV_STATE_SD_OK;

  }
  else{
    display_manager::append_and_print_start_window("SD Card not initialized", false, true);
    display_manager::append_and_print_start_window("Restricted mode");
  }
   StaticJsonBuffer<300> JSONBuffer;   
   
   char buffer[500] = {0};
    // obj = getJSonFromFile(&json_document, "/config.jso");
    // JsonArray data;
    get_json_file(buffer, "/config.jso");
    Serial.println(buffer);
    JsonObject& parsed = JSONBuffer.parseObject(buffer);

    if(parsed.success()){
      display_manager::append_and_print_start_window("Read config", true, true);
    }
    else{
      display_manager::append_and_print_start_window("no config on SD", false, true);
      appendFile(SD, "/error_logs.txt", "Could not read config file or did not find it\n");
      system_flags &= ~DEV_STATE_SD_OK;
    }

  display_manager::append_and_print_start_window("Initializing ADC");
  init_pwm();

  display_manager::append_and_print_start_window("Initializing WiFi");
  WiFi.mode(WIFI_STA); //Client
  const char* ssid = parsed["ssid"];
  const char* pass = parsed["pass"]; 
  // Logging::logging_interval = parsed["log"];
  // chart_freq = parsed["chart"];

  Serial.println(ssid);
  Serial.println(pass);
  if(system_flags & DEV_STATE_SD_OK){
    WiFi.begin(ssid, pass);
    for(int i = 0; i < 5; ++i){
        if(WiFi.status() == WL_CONNECTED){
          break;
        }
        delay(400);
        display_manager::append_and_print_start_window(".");
      }

      if(WiFi.status() != WL_CONNECTED){
        display_manager::append_and_print_start_window("WiFi error", false, true);
        appendFile(SD, "/error_logs.txt", "Could not connect to WiFi");

      }
      else{
        display_manager::append_and_print_start_window("WiFi connected as:", true, true);
        display_manager::append_and_print_start_window(WiFi.localIP().toString().c_str());

        system_flags |= DEV_STATE_WIFI_OK;
      }
      
      if(system_flags & DEV_STATE_WIFI_OK){
        time_manager::gmtOffset_sec = parsed["gmt_offset"];
        time_manager::daylightOffset_sec = parsed["daylight_offset"];
        
        struct tm timeinfo;

        display_manager::append_and_print_start_window("Initializing RTC from NTP");
        timeClient.begin();

        time_manager::get_time_from_ntp();

        if(!getLocalTime(&timeinfo)){
          display_manager::append_and_print_start_window("Time error", false, true);  
          appendFile(SD, "/error_logs.txt", "Could not obtain time from NTP\n");
        }
        else{
          char buffer[40];
          display_manager::append_and_print_start_window("Obtained the time", true, true);  
          sprintf(buffer, 
          "%02i/%02i/%02i-%02i:%02i:%02i", 
          timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
          display_manager::append_and_print_start_window(buffer);  

          system_flags |= DEV_STATE_RTC_OK;
        }

        display_manager::append_and_print_start_window("Reaching MQTT Broker");

        
        if(comm::connect()){
          display_manager::append_and_print_start_window("Connected to broker", true, true);
          comm::send_message("ws_test", "connected");
        }
        else{
          display_manager::append_and_print_start_window("Broker error", false, true);
          appendFile(SD, "/error_logs.txt", "Could not connect to MQTT broker\n");
        }
      }
  }
  else{
    display_manager::append_and_print_start_window("Config error", false, true);

    time_manager::set_local_time(2023, 4, 23, 16, 16, 0, 1);
  }

  init_system_config();

  delay(3000);
  display_manager::message_list.clear_list();
}

void printValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void loop() {
  analog_measures::main();

  Logging::main(NULL);
  Logging::log_for_chart(NULL);

  display_manager::main();

  comm::main();

  alarm_manager::main(NULL);
}
