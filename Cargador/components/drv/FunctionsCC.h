#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "mdns.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "cJSON.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include <math.h>

int contador_power_read;
float power_charge_value;

void decodeCommand(char *s);

void postMQTT(char *topic, int TopicSize, char *data, int DataSize);
char * createjsonFaseA(double volA, double currA, double factor, double potenA, double potenR, double potenApp, double potenMat, double temp, uint16_t Psta, uint16_t Esta, uint8_t phour, uint8_t pminute, uint8_t psecond);
char * createjsonFaseB(double volB, double currB, double factor, double potenB, double potenR, double potenApp, double potenMat, double temp, uint16_t Psta, uint16_t Esta, uint8_t phour, uint8_t pminute, uint8_t psecond);
char * createjsonFaseC(double volC, double currC, double factor, double potenC, double potenR, double potenApp, double potenMat, double temp, uint16_t Psta, uint16_t Esta, uint8_t phour, uint8_t pminute, uint8_t psecond);

int *decToBinary(int n);
void SearchCommand(uint8_t *data);
int ParityDetector(int value);

void begin_analizer();
void begin_calibration_analizer();
double ReadFrequency();
void ReadInformation();
void set_PhaseControl();
void Charge_Power_Control(bool start);
bool compare_ticket(char *ticket);
