#include "APP_FILES/list_of_communicates.h"

void Linked_List::append(const char* message, bool status, bool add_status){
    if(head == nullptr){
      list_of_messages* tmp = new list_of_messages;
      tmp->next = nullptr;
      strcpy(tmp->line, message);
      tmp->has_status = add_status;
      tmp->status = status;
      head = tmp;

      ++count;
    }
    else if(count < max_count){
      list_of_messages* tmp = new list_of_messages;
      tmp->next = nullptr;

      strcpy(tmp->line, message);
      tmp->has_status = add_status;
      tmp->status = status;

      list_of_messages* ptr = head;

      while(ptr->next != nullptr){
        ptr = ptr->next;
      }

      ptr->next = tmp;
      ++count;
    }
    else{
      list_of_messages* tmp = new list_of_messages;
      tmp->next = nullptr;

      strcpy(tmp->line, message);
      tmp->has_status = add_status;
      tmp->status = status;

      list_of_messages* ptr = head->next;
      delete head;
      head = ptr;

      while(ptr->next != nullptr){
        ptr = ptr->next;
      }

      ptr->next = tmp;
    }
  }

  void Linked_List::clear_list(){
    count = 0;

    list_of_messages* tmp = head;

    while(tmp != nullptr){
      tmp = tmp->next;
      delete head;
      head = tmp;
    }
  }

  list_of_messages* Linked_List::get_elements(){
    return head;
  }