#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 

char * createjson(double volA, double currA, double factor, double potenA, double potenR, double potenApp, double potenMat, double temp);

char *substring(char *string, int position, int length);

int *decToBinary(int n);
uint16_t lowWord(uint32_t ww);
uint8_t lowByte(uint16_t ww);
uint16_t highWord(uint32_t ww);
uint8_t highByte(uint16_t ww);





