#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/spi_master.h"

#define FIRMWARE_VERSION	0.1
#define UPDATE_JSON_URL		"https://esp32tutorial.netsons.org/https_ota/firmwarex.json"

#define NUMBER_COMMANDS_QUEUE 32

#define WIFI_SSID	"CEMUSA"
#define WIFI_PASS	"Ofiled@8031"
#define BT_NAME		"CC-AIRIS-01"
//#define WIFI_SSID	"UNE_HFC_D860"
//#define WIFI_PASS	"AAA0CBF7"

#define PIN_SDA 0		//ESP Internal
#define PIN_SCL 4

// #define PIN_SDA 21		//ESP External
// #define PIN_SCL 22


//Grid Analizer
unsigned short LineFreq;
unsigned short PGAGain;
unsigned short VoltageGain;
unsigned short CurrentGain;


// //static TaskHandle_t task_handles[portNUM_PROCESSORS];
// static TaskHandle_t task_http_server;
// static TaskHandle_t task_wifi_manager;

// static TaskHandle_t task_network;

// static TaskHandle_t task_analyzer;
// static TaskHandle_t task_phoenix;

// static TaskHandle_t task_sim800_rx;

// //timer Control

// xSemaphoreHandle Semaphore_control;
// xSemaphoreHandle Semaphore_control_timer;
// xQueueHandle Queue_control;

// struct Config config_network;

// struct Config
// {
// 	bool IP_mode;
// 	char *ssid;
// 	char *password;
// 	int ip_c[4];
// 	int gw_c[4];
// 	int dns_c[4];
// 	int msk_c[4];
// 	char *pin_bt;
// 	char *broker_mqtt;
// 	char *Username;
// 	char *Password_broker;
// 	char *Output_topic;
// 	char *Input_topic;
// 	char *ID;
// 	char *SoftVersion;
// 	char *HardVersion;
// 	char *time_zone;
// };

// #define PIN_NUM_MOSI 19
// #define PIN_NUM_MISO 23
// #define PIN_NUM_CLK 18
// #define PIN_NUM_CS 5
#define WRITE false
#define READ true

bool ready;
bool read_time;

// #define CTP_INT 2
// #define GPIO_INPUT_PIN_SEL_CTP_INT  (1ULL<<CTP_INT)
// #define ESP_INTR_FLAG_DEFAULT 0

// xSemaphoreHandle Semaphore_control_touch;
// xSemaphoreHandle Semaphore_control_screen;
// bool cambio;
// bool finish_print;
// spi_device_handle_t spi_handle;
// bool FinishTrans;