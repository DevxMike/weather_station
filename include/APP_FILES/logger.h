#ifndef logger_
#define logger_

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "APP_FILES/file_management.h"
#include "APP_FILES/main.h"
#include "APP_FILES/list_of_logs.h"

extern SemaphoreHandle_t bme_semaphore;

class Logging{
public:
  static list_of_logs chart_list;
  static Adafruit_BME280& bme_ref;
  static struct tm time_info;
  static void main(void* args);
  static void log_for_chart(void* args);
};

#endif