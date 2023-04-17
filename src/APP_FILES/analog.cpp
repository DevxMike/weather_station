#include "APP_FILES/analog.h"

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
    ledcSetup(DUST_PWM_CHANNEL, DUST_PWM_FREQ, DUST_PWM_RESOLUTION);
    ledcAttachPin(DUST_PWM_PIN, DUST_PWM_CHANNEL);
    ledcWrite(DUST_PWM_CHANNEL, 65535 * DUST_PWM_RESOLUTION);
}

void analog_measures::main(){
    static uint8_t analog_state = 0;
    static unsigned long analog_timer;

    switch(analog_state){
        case 0:
            analog_timer = millis();
            analog_state = 1;
        break;

        case 1:
            if(millis() - analog_timer > 100){
                uint16_t raw_dust = analogRead(DUST_MEASURE_PIN);
                uint16_t raw_vin = analogRead(VIN_MEASURE_PIN);
                
                input_voltage = (3.3 * raw_vin / ADC_MAX) * 2000; // representation in mV
                sensor_voltage = (3.3 * raw_dust / ADC_MAX) * 2000; // representation in mV

                Serial.println(input_voltage);

                analog_state = 0;
            }
        break;
    }
}