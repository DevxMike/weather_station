#ifndef time_manager_h
#define time_manager_h

#include "main.h"

class time_manager{
public:
    static NTPClient& client_ref;
    static long gmtOffset_sec;
    static int daylightOffset_sec;

    static void get_time_from_ntp(){
        const char* ntpServer = "0.europe.pool.ntp.org";

        while(!client_ref.update()) {
            client_ref.forceUpdate();
        }

        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        
    }
};

#endif