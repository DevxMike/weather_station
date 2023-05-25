#ifndef list_of_communicates_
#define list_of_communicates_

#include <string.h>

#define MAX_LINES 10

struct list_of_messages{
  char line[40];
  bool status;
  bool has_status;
  struct list_of_messages* next;
};

class Linked_List{
private:
  list_of_messages* head;
  const uint8_t max_count;
  uint8_t count;

public:
  Linked_List():
    max_count{ MAX_LINES }, head{ nullptr }, count{ 0 }{}
  ~Linked_List(){
    clear_list();
  }

  void append(const char* message, bool status = true, bool add_status = false);

  void clear_list();

  list_of_messages* get_elements();
};

#endif