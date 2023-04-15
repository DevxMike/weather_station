#include "APP_FILES/main.h"
#include "APP_FILES/file_management.h"
#include "APP_FILES/list_of_communicates.h"
#include "APP_FILES/display_manager.h"
#include "APP_FILES/logger.h"
#include "APP_FILES/bme_manager.h"
#include "APP_FILES/time_manager.h"


static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

uint32_t system_flags = 0;
config_info system_configuration{ 0 };

void read_system_config(){
    static char buffer[6] = { 0 };

    File f = SPIFFS.open(CONFIG_FILE, "rb");
    
    if(!f){
        Serial.println("File does not exist");
        return;
    }
    
    auto tmp = f.readBytes(buffer, 6);
    
    Serial.println("Config succesfully read");

    char debug_buff[100];

    sprintf(debug_buff, "%u, %u, %u, %u, %u, %u\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);

    Serial.println(debug_buff);

    system_configuration.alarm_low = buffer[0];
    system_configuration.alarm_high = buffer[1];
    system_configuration.logging_config = buffer[2];
    system_configuration.messaging_config = buffer[3];
    system_configuration.graph_config = buffer[4];
    system_configuration.alarm_set = buffer[5];

    f.close();
}

void update_system_config(const config_info& c){
    static char buffer[6] = { 
        c.alarm_low,
        c.alarm_high,
        c.logging_config,
        c.messaging_config,
        c.graph_config,
        c.alarm_set
    };

    File f = SPIFFS.open(CONFIG_FILE, "wb");

    if(f){
        f.write((const unsigned char *)buffer, 6);

        Serial.println("Config saved");
    }

    f.close();
}

void init_system_config(){
    if(SPIFFS.exists(CONFIG_FILE)){
        read_system_config();

        Logging::logging_interval = (uint8_t)system_configuration.logging_config;
    }
    else{
        system_configuration.alarm_low = 17;
        system_configuration.alarm_high = 25;
        system_configuration.logging_config = 1;
        system_configuration.messaging_config = 1;
        system_configuration.graph_config = 1;
        system_configuration.alarm_set = 0;

        update_system_config(system_configuration);

        Logging::logging_interval = (uint8_t)system_configuration.logging_config;
    }
}

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
uint16_t display_manager::touch_flags{ 0 };

SemaphoreHandle_t bme_semaphore {NULL};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

NTPClient& time_manager::client_ref{ timeClient };
long time_manager::gmtOffset_sec;
int time_manager::daylightOffset_sec;

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
int loging_freq, chart_freq;

const char* mqtt_broker = "570d3f595a3e473790594292a6d5494d.s2.eu.hivemq.cloud";
const char* mqtt_user_name = "michael";
const char* mqtt_password = "7^UCSDU')_7@7eU";

struct tm Logging::time_info{ 0 };
uint8_t Logging::logging_interval{ 0 };

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
  //LOGIC TODO

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

        espClient.setCACert(root_ca);
        mqttClient.setServer(mqtt_broker, 8883);
        
        if(mqttClient.connect("Weather", mqtt_user_name, mqtt_password)){
          display_manager::append_and_print_start_window("Connected to broker", true, true);
          system_flags |= DEV_STATE_MQTT_OK;
          // mqttClient.publish("esp32/test", "System initialized properly.");
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
  // printValues();
  static unsigned long timer = 0;
  static unsigned long display_timer = 0;
  static char buffer[50];

  Logging::main(NULL);

  display_manager::main();

  if(millis() - timer >= 1000){
    const auto& tmp = Logging::time_info;

    sprintf(buffer, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
    Serial.println(buffer);
    timer = millis();
  }
}
