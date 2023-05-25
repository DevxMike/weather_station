#include "APP_FILES/analog.h"
#include "APP_FILES/multi_core.h"

#define VIN_MEASURE_PIN 34
#define DUST_MEASURE_PIN 35
#define DUST_PWM_PIN 32 
#define DUST_PWM_CHANNEL 0
#define DUST_PWM_RESOLUTION 16
// Hz
#define DUST_PWM_FREQ 100
// in %
#define DUST_PWM_WIDTH 0.032 
#define ADC_MAX 4095.0

void init_pwm(){
    // init channel 0 for PWM on pin 32, 100Hz, 0.32ms duty
    ledcSetup(DUST_PWM_CHANNEL, DUST_PWM_FREQ, DUST_PWM_RESOLUTION);
    ledcAttachPin(DUST_PWM_PIN, DUST_PWM_CHANNEL);
    ledcWrite(DUST_PWM_CHANNEL, 65535 * DUST_PWM_RESOLUTION);
}

void analog_measures::main(void* args){
    while(1){
        xSemaphoreTake(analog_mutex, portMAX_DELAY);

        uint16_t raw_dust = analogRead(DUST_MEASURE_PIN); // read analog values
        uint16_t raw_vin = analogRead(VIN_MEASURE_PIN);
        // 3V3 * ratio * 1000mV * 2 (measured on 2 by 2 voltage divider)
        input_voltage = (3.3 * raw_vin / ADC_MAX) * 2000; // representation in mV
        sensor_voltage = (3.3 * raw_dust / ADC_MAX) * 2000; // representation in mV

        xSemaphoreGive(analog_mutex);

        task_delay(50);
    }
}