#ifndef list_of_logs_h
#define list_of_logs_h

#include "APP_FILES/list_of_logs.h"
#include "APP_FILES/main.h"

#define MAX_LINES 10

typedef struct temperature_log{
  float temperature;
  int16_t time_ago;
  struct temperature_log* next;
};

class list_of_logs{
private:
  temperature_log* head;
  const uint8_t max_count;
  uint8_t count;

public:
  list_of_logs():
    max_count{ MAX_LINES }, head{ nullptr }, count{ 0 }{}
  ~list_of_logs(){
    clear_list();
  }

  void append(int16_t temperature, char interval);

  void clear_list();

  temperature_log* get_elements();
};


#endif