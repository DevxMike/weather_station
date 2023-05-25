#ifndef file_management_
#define file_management_

#include <SD.h>
#include <FS.h>
#include "SPIFFS.h"

#include "APP_FILES/main.h"

void readFile(fs::FS &fs, const char * path);

void writeFile(fs::FS &fs, const char * path, const char * message);

void appendFile(fs::FS &fs, const char * path, const char * message);

void renameFile(fs::FS &fs, const char * path1, const char * path2);

void deleteFile(fs::FS &fs, const char * path);

void get_json_file(char* buffer, const char* fname);

void read_system_config();

void update_system_config();

void init_system_config();

#endif