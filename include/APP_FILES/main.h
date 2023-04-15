#ifndef main_h
#define main_h

#include <Arduino.h>

#include "WiFi.h"
#include "FS.h"
#include "SD.h"

#include <SD.h>
#include <FS.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>


#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#define USE_DEBUG_MSG 

#include <string.h>
#include "time.h"

#define DEV_STATE_SENSOR_OK 0x0001
#define DEV_STATE_SD_OK 0x0002
#define DEV_STATE_CONF_FILE_OK 0x0004
#define DEV_STATE_WIFI_OK 0x0008
#define DEV_STATE_RTC_OK 0x0010
#define DEV_STATE_MQTT_OK 0x0020
#define DEV_STATE_ADC_OK 0x0040

#define CALIBRATION_FILE "/TouchCalData3"
#define CONFIG_FILE "/ConfigData1.bin"
#define REPEAT_CAL false

#define DEG2RAD 0.0174532925 

struct config_info{
    char alarm_low; // lower threshold of temperature
    char alarm_high; // upper threshold of temperature
    char logging_config; // logging frequency 
    char messaging_config; // messaging frequency
    char graph_config; // drawing frequency
    char alarm_set; // is alarm set
};

extern uint32_t system_flags;
extern config_info system_configuration;

#endif