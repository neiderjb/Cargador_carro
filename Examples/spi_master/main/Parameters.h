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


//InterruptValue
#define GPIO_INPUT_IO_1     36
#define GPIO_INPUT_IO_2     35
#define GPIO_INPUT_IO_3     34
#define CTP_INT 2
#define GPIO_INPUT_PIN_SEL_1  (1ULL<<GPIO_INPUT_IO_1)
#define GPIO_INPUT_PIN_SEL_2  (1ULL<<GPIO_INPUT_IO_2)
#define GPIO_INPUT_PIN_SEL_3  (1ULL<<GPIO_INPUT_IO_3)
#define GPIO_INPUT_PIN_SEL_CTP_INT  (1ULL<<CTP_INT)
#define ESP_INTR_FLAG_DEFAULT 0

//I2c pins
#define PIN_SDA 0
#define PIN_SCL 4
xSemaphoreHandle Semaphore_control_touch;
xSemaphoreHandle Semaphore_control_screen;
bool finish_print;


uint64_t milis;

#define NUMBER_COMMANDS 32


//Define for MONOPHASE -- comment for TriPhase
// #define MONOPHASE 

//Define for Sequencial Mode
// #define SEQUENTIAL

//Network Analizer
unsigned short LineFreq;
unsigned short PGAGain;
unsigned short VoltageGain;
unsigned short CurrentGain;
bool lecturaCorrecta;

//SPI command
#define PIN_NUM_MOSI 19
#define PIN_NUM_MISO 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5
#define WRITE false
#define READ true
spi_device_handle_t spi_handle;
bool FinishTrans;

double frequencyValue;
bool ready;

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


TaskHandle_t task_http_server;
TaskHandle_t task_wifi_manager;

//timer Control

// esp_timer_handle_t Timer_Control_Pulse1;
// esp_timer_handle_t Timer_Control_Pulse2;

esp_timer_handle_t FaseA_Timer_Handle_FirstCut;
esp_timer_handle_t FaseA_Timer_Handle_FirstWidth;
esp_timer_handle_t FaseA_Timer_Handle_SecondWidth;
esp_timer_handle_t FaseB_Timer_Handle_FirstCut;
esp_timer_handle_t FaseB_Timer_Handle_FirstWidth;
esp_timer_handle_t FaseB_Timer_Handle_SecondWidth;
esp_timer_handle_t FaseC_Timer_Handle_FirstCut;
esp_timer_handle_t FaseC_Timer_Handle_FirstWidth;
esp_timer_handle_t FaseC_Timer_Handle_SecondWidth;
esp_timer_handle_t timer_control_window_handle;
xSemaphoreHandle Semaphore_control_timer_FaseA;
xSemaphoreHandle Semaphore_control_timer_FaseB;
xSemaphoreHandle Semaphore_control_timer_FaseC;
xSemaphoreHandle Semaphore_control_Second_FaseA;
xSemaphoreHandle Semaphore_control_Second_FaseB;
xSemaphoreHandle Semaphore_control_Second_FaseC;

//CONTROL UCORTES
xSemaphoreHandle Semaphore_Ucortes_FaseA;
xSemaphoreHandle Semaphore_Ucortes_FaseB;
xSemaphoreHandle Semaphore_Ucortes_FaseC;

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

int64_t usecondsA;// = 0;
int64_t usecondsB;// = 0;
int64_t usecondsC;// = 0;
int64_t comp_timer_count_A;// = 0;
int64_t comp_timer_count_B;// = 0;
int64_t comp_timer_count_C;// = 0;

//microcut
bool microcut;// = false; //send data to AC
bool cutFaseA;// = false;	  //MicroCut
bool cutFaseB;
bool cutFaseC;
xSemaphoreHandle Semaphore_control_microCutFaseA;
xSemaphoreHandle Semaphore_control_microCutFaseB;
xSemaphoreHandle Semaphore_control_microCutFaseC;
int sizeCommand;
bool FaseAMicroCutControl;
bool FaseBMicroCutControl;
bool FaseCMicroCutControl;
bool MICROCORTES;

bool cambio;
bool cambioA;
bool cambioB;
bool cambioC;

xSemaphoreHandle Semaphore_control_FaseABC;
xSemaphoreHandle Semaphore_control_FaseA;
xSemaphoreHandle Semaphore_control_FaseB;
xSemaphoreHandle Semaphore_control_FaseC;
xQueueHandle Queue_control_FaseA;
xQueueHandle Queue_control_FaseB;
xQueueHandle Queue_control_FaseC;

//macrocut
bool macrocut;
bool macrocutControl;
xSemaphoreHandle Semaphore_macrocut_control;
esp_timer_handle_t timer_control_stop_handle;


int control_send_faseA;
int control_send_faseB;
int control_send_faseC;

//Control Variables
bool CommandEnd;