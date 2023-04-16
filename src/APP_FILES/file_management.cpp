#include "APP_FILES/file_management.h"

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print(message);

    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    file.print(message);
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void get_json_file(char* buffer, const char* fname){
  File file = SD.open(fname);

  unsigned i = 0;

  if (!file) {
        Serial.println(F("Failed to read file"));
        return;
  }

  while (file.available()) {
        buffer[i] = file.read();
        ++i;
  }

  file.close();
}

void read_system_config(){
    char buffer[6] = { 0 };

    File f = SPIFFS.open(CONFIG_FILE, "rb");
    
    if(!f){
        Serial.println("File does not exist");
        return;
    }
    
    auto tmp = f.readBytes(buffer, 6);
    
    Serial.println("Config succesfully read");

    char debug_buff[100];

    sprintf(debug_buff, "%u, %u, %u, %u, %u, %u\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);

    Serial.println(debug_buff);

    system_configuration.alarm_low = buffer[0];
    system_configuration.alarm_high = buffer[1];
    system_configuration.logging_config = buffer[2];
    system_configuration.messaging_config = buffer[3];
    system_configuration.graph_config = buffer[4];
    system_configuration.alarm_set = buffer[5];

    f.close();
}

void update_system_config(){
    char buffer[6] = { 
        system_configuration.alarm_low,
        system_configuration.alarm_high,
        system_configuration.logging_config,
        system_configuration.messaging_config,
        system_configuration.graph_config,
        system_configuration.alarm_set
    };

    char debug_buff[100];

    sprintf(debug_buff, "%u, %u, %u, %u, %u, %u\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);

    Serial.println(debug_buff);

    File f = SPIFFS.open(CONFIG_FILE, "wb");

    if(f){
        f.write((const unsigned char *)buffer, 6);

        Serial.println("Config saved");
    }

    f.close();
}

void init_system_config(){
    if(SPIFFS.exists(CONFIG_FILE)){
        read_system_config();
    }
    else{
        system_configuration.alarm_low = 17;
        system_configuration.alarm_high = 25;
        system_configuration.logging_config = 1;
        system_configuration.messaging_config = 1;
        system_configuration.graph_config = 1;
        system_configuration.alarm_set = 0;

        update_system_config();
    }
}