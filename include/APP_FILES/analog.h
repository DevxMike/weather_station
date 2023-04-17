#ifndef analog_h
#define analog_h

#include "APP_FILES/main.h"

void init_pwm();

class analog_measures{
public:
    static uint16_t input_voltage;
    static uint16_t sensor_voltage;
    static void main();
};

#endif