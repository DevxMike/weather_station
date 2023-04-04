#ifndef file_management_
#define file_management_

#include <SD.h>
#include <FS.h>

void readFile(fs::FS &fs, const char * path);

void writeFile(fs::FS &fs, const char * path, const char * message);

void appendFile(fs::FS &fs, const char * path, const char * message);

void renameFile(fs::FS &fs, const char * path1, const char * path2);

void deleteFile(fs::FS &fs, const char * path);

void get_json_file(char* buffer, const char* fname);

#endif