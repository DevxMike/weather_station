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

    static void set_local_time(int year, int month, int mday, int hr, int minute, int sec, int is_dst){
        struct tm tm;

        tm.tm_year = year - 1900;  
        tm.tm_mon = month-1;
        tm.tm_mday = mday;
        tm.tm_hour = hr;      
        tm.tm_min = minute;
        tm.tm_sec = sec;
        tm.tm_isdst = is_dst;  // 1 or 0
        time_t t = mktime(&tm);
        struct timeval now = { .tv_sec = t };
        settimeofday(&now, NULL);
    }
};

#endif