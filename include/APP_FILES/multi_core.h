#ifndef multicore_h
#define multicore_h

#include <Arduino.h>

extern SemaphoreHandle_t analog_mutex;
extern SemaphoreHandle_t spiffs_mutex;
extern SemaphoreHandle_t bme_mutex;
extern SemaphoreHandle_t chart_mutex;
extern SemaphoreHandle_t time_sem_start;
extern SemaphoreHandle_t time_sem_end;

inline void task_delay(unsigned ms){
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

#endif