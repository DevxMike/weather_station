#include "APP_FILES/alarm.h"

void set_alarm(uint8_t flag){
    alarm_manager::alm = flag;
}

void send_message(int actual){
    Serial.printf("alarm %d\n", alarm_manager::alm);
}

void alarm_manager::main(void* args){
    static uint8_t state = 0;
    static unsigned long timer;
    bool alarm_armed = system_configuration.alarm_set;
    int temp;
    static uint8_t last_alarm_state;
    

    switch(state){
        case 0:
            if(alarm_armed){
                state = 1;
            }
        break;

        case 1:
            temp = int(display_manager::bme_ref.readTemperature());

            if(temp > int(system_configuration.alarm_high)){
                set_alarm(ALM_HI_TRIG);
            }
            else if(temp < int(system_configuration.alarm_low)){
                set_alarm(ALM_LO_TRIG);
            }
            else{
                set_alarm(0);
            }

            if(alm != last_alarm_state){
                send_message(temp);
            }

            timer = millis();
            last_alarm_state = alm;
            state = 2;
        break;

        case 2:
            if(millis() - timer > 2000){
                if(alarm_armed == false){
                    state = 0;
                }
                else{
                    state = 1;
                }
            }
        break;
    }
}