#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/task.h"
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

#include "lvgl.h"

#define FIRMWARE_VERSION	0.1
#define UPDATE_JSON_URL		"https://esp32tutorial.netsons.org/https_ota/firmwarex.json"

#define littleOpt
#define NUMBER_COMMANDS_QUEUE 32

#define WIFI_SSID	"CEMUSA"
#define WIFI_PASS	"Ofiled@8031"
#define BT_NAME		"CC-AIRIS-01"

/* address of the module */
uint8_t ZDU0210RJX_address; 	//0x5c     //58    //59  //5c  //5b   //5f
uint8_t ZDU0210RJX_address1; 	//0x54    //50    //51  //54  //53   //57


//Touch screen
#define GPIO_INPUT_IO_1     13	//14 // 15
#define GPIO_INPUT_PIN_SEL_1  (1ULL<<GPIO_INPUT_IO_1)
#define ESP_INTR_FLAG_DEFAULT 1
bool cambio_touch_interrupt;
xSemaphoreHandle Semaphore_control_touch;

//I2c pins
// #define PIN_SDA 21
// #define PIN_SCL 22
#define PIN_SDA 0
#define PIN_SCL 4


bool detectAnalizer;
bool detectTouch;
bool detectRtc;
bool detectModbus;



bool finish_print;


//Network Analizer

unsigned short LineFreq;
unsigned short PGAGain;
unsigned short VoltageGain;
unsigned short CurrentGain;
double frequencyValue;
bool lecturaCorrecta;
int contador_faseA;
int contador_faseB;
int contador_faseC;



//SPI
//External ESP32
// #define PIN_NUM_MOSI 23
// #define PIN_NUM_MISO 19
// #define PIN_NUM_CLK 18
// #define PIN_NUM_CS 5
// #define PIN_RESET_SCREEN 2
//Maqueta
#define PIN_NUM_MOSI 19
#define PIN_NUM_MISO 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5
#define PIN_RESET_SCREEN 14
#define TOUCH_RESET 15
#define DMA_CHAN 2
#define LCD_HOST HSPI_HOST
spi_device_handle_t spi_handle;

#define CONFIG_LCD_OVERCLOCK

#define WRITE false
#define READ true

bool ready_information;
bool read_time;


struct Config
{
	bool IP_mode;
	char *ssid;
	char *password;
	int ip_c[4];
	int gw_c[4];
	int dns_c[4];
	int msk_c[4];
	char *pin_bt;
	char *broker_mqtt;
	char *Username;
	char *Password_broker;
	char *Output_topic;
	char *Input_topic;
	char *ID;
	char *SoftVersion;
	char *HardVersion;
	char *time_zone;
};



struct Timer_control
{
	int TimerPulseControl;
	int InterruptValue;
};

struct Timer_control control_timer;

struct Config ConfigurationObject;

float voltageA, currentA, powerfactorA, powerA, powerReacA, powerAppA,
		voltageB, currentB, powerfactorB, powerB, powerReacB, powerAppB,
		voltageC, currentC, powerfactorC, powerC, powerReacC, powerAppC,
		temperature;

float PowerConsumeA, PowerConsumeB, PowerConsumeC;

//9200-1000-7400-1000 --- 60
//10700-1000-9000-1000 --- 50
//Frequency Detection
bool frequency;// = false;
bool start_count;// = false;
int count_value;// = 0;
int64_t timer_register[10];
bool salida;/// = false;
bool check;// = false;
bool state;// = false;

//Phoenix Contact
xSemaphoreHandle Semaphore_Start_Charging;
xSemaphoreHandle Semaphore_Stop_Charging;
xSemaphoreHandle Semaphore_Out_Phoenix;

xSemaphoreHandle Semaphore_Out_Rele;
xSemaphoreHandle Semaphore_Out_Led;

uint16_t PStatus;
uint16_t EStatus;
uint8_t PHour;
uint8_t PMinute;
uint8_t PSecond;
bool charging;


//LittleVgl 
#define DISP_BUF_SIZE (LV_HOR_RES_MAX * 20)

lv_obj_t *labelPotencia;
lv_obj_t *labelCarga;
lv_obj_t *labelCoste;
lv_obj_t *labelTiempo;

lv_obj_t *labelPhoenix;

//timers
esp_timer_handle_t Timer_Screen_Control;
esp_timer_handle_t Timer_Memory_Control;
esp_timer_handle_t Timer_Charge_Control;

//power control
float total_time;
float total_power;
int contador_power_read;
float power_charge_value;
float power_actual_value;
float total_cost;
#define PRICE_ENERGY	0.05
#define FAKE_DATA
