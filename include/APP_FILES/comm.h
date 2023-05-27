#ifndef comm_h_
#define comm_h_

#include "APP_FILES/main.h"

#define STACK_SIZE 10

struct msg_t{
    String topic;
    String payload;
};

class message_stack{
private:
    msg_t messages[STACK_SIZE];
    uint8_t count;

public:
    message_stack(): count{ 0 } {}
    bool is_empty() { return count == 0; }
    bool is_full() { return count == STACK_SIZE; }
    void push(String topic, String payload);
    void pop(msg_t& msg);
};

class comm{
private:
    static void callback(char* topic, byte* payload, unsigned int length);
    static message_stack messages;
public:
    static bool connect();
    static void send_message(const char* topic, const char* mesg);
    static void main();
};

#endif