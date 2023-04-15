#ifndef main_h
#define main_h

#include <Arduino.h>

#include "WiFi.h"
#include "FS.h"
#include "SD.h"

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
#define REPEAT_CAL false

extern uint32_t system_flags;

#endif