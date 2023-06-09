#ifndef alarm_h_
#define alarm_h_

#include "main.h"
#include "APP_FILES/comm.h"
#include "APP_FILES/display_manager.h"

#define ALM_HI_TRIG 0x01
#define ALM_LO_TRIG 0x02

class alarm_manager{
public:
    static uint8_t alm;
    static void main(void* arg);
};

#endif