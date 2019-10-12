#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h" 


void begin_spiffs();
void start_spiffs(const char *path);
void close_spiffs();

esp_err_t save_data(const char *path, char *data);
void createFile(char *path);
void getconfig(char *path);
