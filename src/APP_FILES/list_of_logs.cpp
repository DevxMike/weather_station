#include "APP_FILES/list_of_logs.h"

static const int16_t intervals[3]{
    10, 30, 60
};

void list_of_logs::append(int16_t temperature, char interval){
    if(head == nullptr){ // if list is empty
      temperature_log* tmp = new temperature_log;
      tmp->next = nullptr;
      tmp->temperature = temperature;
      tmp->time_ago = 0;
      head = tmp;

      ++count;
    }
    else if(count < max_count){ // append new element at the end if list is not full
      temperature_log* tmp = new temperature_log;
      tmp->next = nullptr;

      tmp->temperature = temperature;
      tmp->time_ago = 0;

      temperature_log* ptr = head;

      while(ptr->next != nullptr){
        ptr = ptr->next;
      }

      ptr->next = tmp;
      ++count;
    }
    else{ // delete element at head and append element at the end of the list
      temperature_log* tmp = new temperature_log;
      tmp->next = nullptr;

      tmp->temperature = temperature;
      tmp->time_ago = 0;

      temperature_log* ptr = head->next;
      delete head;
      head = ptr;

      while(ptr->next != nullptr){
        ptr = ptr->next;
      }

      ptr->next = tmp;
    }

    if(head != nullptr){
      temperature_log* ptr = head;

      while(ptr->next != nullptr){
        // add time to element so it could be displayed at chart
        ptr->time_ago += intervals[interval];
        ptr = ptr->next;
      }
    }
  }

  // free memory
  void list_of_logs::clear_list(){
    count = 0;

    temperature_log* tmp = head;

    while(tmp != nullptr){
      tmp = tmp->next;
      delete head;
      head = tmp;
    }
  }

  // get pointer to first element
  temperature_log* list_of_logs::get_elements(){
    return head;
  }