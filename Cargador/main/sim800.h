#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ZDU0210RJX.h"

bool begin_800();
void sim800_postMQTT(uint8_t *topic, int TopicSize, uint8_t *data, int DataSize);
void subtopic();



bool Isconnected_gprs();

void sim800_rx_task(void *pvParameters);

bool factoryDefault();
bool restart();
bool poweroff();
bool radioOff();
bool sleepEnable();
bool testAT();

/*
Send and Get Data to AT command
*/
void sendAT(char *dataWrite, int countWrite, int uart);
void sendCommandAT(char *command, char *responsecommand);
bool responseAT(char *ATcommand, char *ATcommandResponse);

void startCon();
void subtopic();