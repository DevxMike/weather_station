#include "APP_FILES/logger.h"
#include "APP_FILES/bme_manager.h"

char buffer[100];

long time_diff(const struct timeval& old_time, const struct timeval& new_time){
    //function assumes no diff in days
    unsigned long long old_in_sec = old_time.tv_sec;
    unsigned long long new_in_sec = new_time.tv_sec;

    return (new_in_sec - old_in_sec) / 60; // return diff in minutes
}

void Logging::main(void* args){
    static uint8_t logger_state = 0;
    static unsigned long timer = 0;
    static struct timeval old_seconds;
    static struct timeval now_seconds;
    static bool get_old_time = true;
    
      switch(logger_state){
        case 0:
        getLocalTime(&time_info);
        gettimeofday(&now_seconds, NULL);


        if(get_old_time){
          get_old_time = false;
          gettimeofday(&old_seconds, NULL);
             

        //   xSemaphoreTake(bme_semaphore, portMAX_DELAY);

            auto temp = bme_ref.readTemperature();
            auto hum = bme_ref.readHumidity();
            auto press = bme_ref.readPressure();

            sprintf(buffer, "%02d/%02d/%02d-%02d:%02d; %.2f; %.2f; %.2f\n", 
                time_info.tm_mday, time_info.tm_mon + 1, time_info.tm_year + 1900,
                time_info.tm_hour, time_info.tm_min,
                temp, hum, press);

            appendFile(SD, "/logs.csv", buffer);

        //   xSemaphoreGive(bme_semaphore);
        }

        timer = millis();
        logger_state = 1;
        break;

        case 1:
        if(millis() - timer >= 1000){
          logger_state = 0;

          auto tmp = time_diff(old_seconds, now_seconds);

          switch(logging_interval){
            case 0:
                if(tmp >= 10){
                    get_old_time = true;
                }
            break;

            case 1:
                if(tmp >= 30){
                    get_old_time = true;
                }
            break;
            
            case 2:
                if(tmp >= 60){
                    get_old_time = true;
                }
            break;
          }
        }
        break;
      }
  }