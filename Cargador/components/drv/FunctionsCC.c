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
#include "esp_http_client.h"

#include "wifi_lib.h"

//#include "http_server.h"
//#include "wifi_lib.h"
//#include "wifi_manager.h"
//#include "spiffs_lib.h"
#include "Mqtt_lib.h"
//#include "Bt_lib.h"
#include "Parameters.h"
#include "gpio_lib.h"
#include "esp_timer.h"
#include "esp_sleep.h"
#include "driver/i2s.h"

#include "EPLD.h"
#include "spi_lib.h"
//#include "uart_lib.h"

#include "FunctionsCC.h"
#include "M90E32AS.h"
#include "Functions.h"
#include "PCF85063TP.h"
//#include "Sim800L.h"
#include "lv_examples/lv_cargador/cargador/cargador.h"

static const char *TAG = "FunctionsCC";
static char topic[] = "airis/1155/power_analizer";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	}
	return ESP_OK;
}

void decodeCommand(char *s)
{
	int values[32];
	cJSON *root, *dataarray;
	root = cJSON_CreateObject();
	root = cJSON_Parse(s);
	printf("Command: %s\n", cJSON_PrintUnformatted(root));
	dataarray = cJSON_GetObjectItem(root, "command");

	for (int i = 0; i < cJSON_GetArraySize(dataarray); i++)
	{
		values[i] = (cJSON_GetArrayItem(dataarray, i)->valueint);
		printf("Send to Queue %d: %d\n", i, values[i]);
	}
}

void SearchCommand(uint8_t *data)
{
	char *topic = substring((char *)data, 4, (int)data[3] + 1);
	char *command = substring((char *)data, 5 + (int)data[3], (int)data[1] - 3);
	printf("topic: %s\n", topic);
	printf("command: %s\n", command);
	decodeCommand(command);
}

// void postMQTT(char *topic, int TopicSize, char *data, int DataSize)
// {
// 	ready = false;
// 	char command2[] = "AT+CIPSEND\n";
// 	sendATValue(command2, strlen(command2));
// 	vTaskDelay(200); //No es necesario en la version i2c-uart

// 	//30 13 00 08 76 61 6C 65 74 72 6F 6E 68 65 6C 6C 6F 72 61 76 69 1A ---Publish
// 	char headPub[4];
// 	headPub[0] = 0x30;
// 	headPub[1] = (char)DataSize + TopicSize + 1;
// 	headPub[2] = (char)TopicSize >> 8;
// 	headPub[3] = (char)TopicSize - 1;
// 	sendATValue(headPub, sizeof(headPub)); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
// 	vTaskDelay(100);					   //No es necesario en la version i2c-uart

// 	sendATValue(topic, TopicSize); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
// 	vTaskDelay(100);			   //No es necesario en la version i2c-uart

// 	sendATValue(data, DataSize); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
// 	vTaskDelay(100);			 //No es necesario en la version i2c-uart

// 	char FinPub[2] = {0x1A, 0x0D};
// 	sendATValue(FinPub, sizeof(FinPub)); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
// 	ready = true;						 ///solo valida que el dato ya se envio
// }

/*! \brief createjson.
 *
 *  This function create a json object to send to MQTT broker
 *  
 *  \param volA       Votage phase A.
 *  \param curr       Current phase A.
 *  \param potenA     Active Power phase A.
 *  \param potenA     Active Power phase A.
 *  \param potenR     Reactive Power phase A.
 *  \param potenApp   Apparent Power phase A.
 *  \param potenApp   Math Power phase A.
 *  \param temp       temperture.
 */
char *createjsonFaseA(double volA, double currA, double factor, double potenA, double potenR, double potenApp, double potenMat, double temp, uint16_t Psta, uint16_t Esta, uint8_t phour, uint8_t pminute, uint8_t psecond)
{

	cJSON *root;
	static char *datatoreturn;

	root = cJSON_CreateObject();

	double _volA = round(volA * 100) / 100.0;
	double _currA = round(currA * 100) / 100.0;
	double _factor = round(factor * 100) / 100.0;
	double _potenA = round(potenA * 100) / 100.0;
	double _potenR = round(potenR * 100) / 100.0;
	double _potenApp = round(potenApp * 100) / 100.0;
	double _potenMat = round(potenMat * 100) / 100.0;
	double _temp = round(temp * 100) / 100.0;

	cJSON_AddItemToObject(root, "V1", cJSON_CreateNumber(_volA));
	cJSON_AddItemToObject(root, "C1", cJSON_CreateNumber(_currA));
	cJSON_AddItemToObject(root, "P1", cJSON_CreateNumber(_potenA));
	cJSON_AddItemToObject(root, "PR1", cJSON_CreateNumber(_potenR));
	cJSON_AddItemToObject(root, "PA1", cJSON_CreateNumber(_potenApp));
	cJSON_AddItemToObject(root, "F1", cJSON_CreateNumber(_factor));
	cJSON_AddItemToObject(root, "PM1", cJSON_CreateNumber(_potenMat));
	cJSON_AddItemToObject(root, "TEM", cJSON_CreateNumber(_temp));
	cJSON_AddItemToObject(root, "PST", cJSON_CreateNumber(Psta));
	cJSON_AddItemToObject(root, "EST", cJSON_CreateNumber(Esta));

	datatoreturn = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	return datatoreturn;
	//sendMessage(datatoreturn, "airis/1155/power_analizer");
}

char *createjsonFaseB(double volB, double currB, double factor, double potenB, double potenR, double potenApp, double potenMat, double temp, uint16_t Psta, uint16_t Esta, uint8_t phour, uint8_t pminute, uint8_t psecond)
{

	cJSON *root;
	static char *datatoreturn;

	root = cJSON_CreateObject();

	double _volB = round(volB * 100) / 100.0;
	double _currB = round(currB * 100) / 100.0;
	double _factor = round(factor * 100) / 100.0;
	double _potenB = round(potenB * 100) / 100.0;
	double _potenR = round(potenR * 100) / 100.0;
	double _potenApp = round(potenApp * 100) / 100.0;
	double _potenMat = round(potenMat * 100) / 100.0;
	double _temp = round(temp * 100) / 100.0;

	cJSON_AddItemToObject(root, "V2", cJSON_CreateNumber(_volB));
	cJSON_AddItemToObject(root, "C2", cJSON_CreateNumber(_currB));
	cJSON_AddItemToObject(root, "P2", cJSON_CreateNumber(_potenB));
	cJSON_AddItemToObject(root, "PR2", cJSON_CreateNumber(_potenR));
	cJSON_AddItemToObject(root, "PA2", cJSON_CreateNumber(_potenApp));
	cJSON_AddItemToObject(root, "F2", cJSON_CreateNumber(_factor));
	cJSON_AddItemToObject(root, "PM2", cJSON_CreateNumber(_potenMat));
	cJSON_AddItemToObject(root, "TEM", cJSON_CreateNumber(_temp));
	cJSON_AddItemToObject(root, "PST", cJSON_CreateNumber(Psta));
	cJSON_AddItemToObject(root, "EST", cJSON_CreateNumber(Esta));

	datatoreturn = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	return datatoreturn;
}

char *createjsonFaseC(double volC, double currC, double factor, double potenC, double potenR, double potenApp, double potenMat, double temp, uint16_t Psta, uint16_t Esta, uint8_t phour, uint8_t pminute, uint8_t psecond)
{

	cJSON *root;
	static char *datatoreturn;

	root = cJSON_CreateObject();

	double _volC = round(volC * 100) / 100.0;
	double _currC = round(currC * 100) / 100.0;
	double _factor = round(factor * 100) / 100.0;
	double _potenC = round(potenC * 100) / 100.0;
	double _potenR = round(potenR * 100) / 100.0;
	double _potenApp = round(potenApp * 100) / 100.0;
	double _potenMat = round(potenMat * 100) / 100.0;
	double _temp = round(temp * 100) / 100.0;

	cJSON_AddItemToObject(root, "V3", cJSON_CreateNumber(_volC));
	cJSON_AddItemToObject(root, "C3", cJSON_CreateNumber(_currC));
	cJSON_AddItemToObject(root, "P3", cJSON_CreateNumber(_potenC));
	cJSON_AddItemToObject(root, "PR3", cJSON_CreateNumber(_potenR));
	cJSON_AddItemToObject(root, "PA3", cJSON_CreateNumber(_potenApp));
	cJSON_AddItemToObject(root, "F3", cJSON_CreateNumber(_factor));
	cJSON_AddItemToObject(root, "PM3", cJSON_CreateNumber(_potenMat));
	cJSON_AddItemToObject(root, "TEM", cJSON_CreateNumber(_temp));
	cJSON_AddItemToObject(root, "PST", cJSON_CreateNumber(Psta));
	cJSON_AddItemToObject(root, "EST", cJSON_CreateNumber(Esta));

	datatoreturn = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	return datatoreturn;
}

/*! \brief ReadInformation.
 *
 *  This function read variables from the network analizer
 */
void ReadInformation()
{
	ready_information = false;
	printf("Phonix Charger State: %x \n", PStatus);
	printf("Phonix Error State: %x \n", EStatus);
	printf("Phonix Charger Time Charger- %x:%x:%x \n", PHour, PMinute, PSecond);

	char *dataMQTTA = createjsonFaseA(voltageA, currentA, powerfactorA, powerA, powerReacA, powerAppA, (voltageA * currentA), temperature, PStatus, EStatus, PHour, PMinute, PSecond);
	char *dataMQTTB = createjsonFaseB(voltageB, currentB, powerfactorB, powerB, powerReacB, powerAppB, (voltageB * currentB), temperature, PStatus, EStatus, PHour, PMinute, PSecond);
	char *dataMQTTC = createjsonFaseC(voltageC, currentC, powerfactorC, powerC, powerReacC, powerAppC, (voltageC * currentC), temperature, PStatus, EStatus, PHour, PMinute, PSecond);

	update_label_carga_one((float)GetApparentPowerA(), (float)GetApparentPowerA(), (float)3, (float)PMinute);

	if (Isconnected())
	{

		sendMessage(dataMQTTA, topic);
		sendMessage(dataMQTTB, topic);
		sendMessage(dataMQTTC, topic);
	}
	free(dataMQTTA);
	free(dataMQTTB);
	free(dataMQTTC);
	ready_information = true;
}

double ReadFrequency()
{
	return GetFrequency();
}

int ParityDetector(int value)
{
	int ret = 0;
	if (value % 2 == 0)
		ret = 0;
	else
		ret = 1;
	return ret;
}

/*! \brief decToBinary.
 *
 *  This function convert decimal number to binary, return int array with the number in binary.
 * 	
 *  \param n number.
 */
int *decToBinary(int n)
{
	static int ret[9];
	static int salida[18];
	int contador = 0;
	for (int i = 7; i >= 0; i--)
	{
		int k = n >> i;
		if (k & 1)
		{
			ret[i] = 1;
			contador++;
		}
		else
		{
			ret[i] = 0;
		}
	}

	salida[0] = ret[0];
	salida[1] = ret[0];
	salida[2] = ret[1];
	salida[3] = ret[1];
	salida[4] = ret[2];
	salida[5] = ret[2];
	salida[6] = ret[3];
	salida[7] = ret[3];
	salida[8] = ret[4];
	salida[9] = ret[4];
	salida[10] = ret[5];
	salida[11] = ret[5];
	salida[12] = ret[6];
	salida[13] = ret[6];
	salida[14] = ret[7];
	salida[15] = ret[7];
	salida[16] = ParityDetector(contador);
	salida[17] = ParityDetector(contador);

	// ret[8] = ParityDetector(contador);
	return salida;
}

/*! \brief begin_analizer.
 *
 *  This function configure the network analizer to read frequency and start zero crossing signal.
 */
void begin_analizer()
{
	// uint16_t command[2] = {0x0070, 0x789A};
	// sendtospi(WRITE, spi, 0x70, NULL, command[1]);
	// command[1] = 0x55AA;
	// sendtospi(WRITE, spi, 0x7F, NULL, command[1]);
	// command[1] = 0x4454;
	// sendtospi(WRITE, spi, 0x07, NULL, command[1]);
	// uint16_t FreqHiThresh[2] = {0x000D, 0xEE48};
	// uint16_t FreqLoThresh[2] = {0x000C, 0xBF68};
	// sendtospi(WRITE, spi, 0x0D, NULL, FreqHiThresh[1]);
	// sendtospi(WRITE, spi, 0x0C, NULL, FreqLoThresh[1]);
	// command[0] = 0x007F;
	// command[1] = 0x0000;
	// sendtospi(WRITE, spi, 0x7F, NULL, command[1]);

	LineFreq = 5255;
	PGAGain = 42; //X4
	VoltageGain = 50306;
	CurrentGain = 40953;
	begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //

	ESP_LOGI("Network Analizer", "First Begin OK");
}

void begin_calibration_analizer()
{
	vTaskDelay(200 / portTICK_RATE_MS);
	frequencyValue = ReadFrequency();

	while (frequencyValue < 49 || frequencyValue > 61)
	{
		frequencyValue = ReadFrequency();

		printf("\033[1;31m");
		printf("FrequencyValue = %f\n", frequencyValue);
		printf("\033[0m");
		vTaskDelay(500 / portTICK_RATE_MS);
	}
	printf("FrequencyValue = %f\n", frequencyValue);

	if (frequencyValue > 61)
	{
		ESP_LOGI("Network Analizer", "Calibration FAIL");
	}

	if (frequencyValue > 49 && frequencyValue < 51)
	{
		LineFreq = 5255;
		PGAGain = 0x002A;
		VoltageGain = 50597;
		CurrentGain = 53685;
		begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //
		ESP_LOGI("Network Analizer", "Calibration OK 50Hz");
	}
	else
	{
		LineFreq = 5255;
		PGAGain = 0x002A;
		VoltageGain = 50391;
		CurrentGain = 51571;
		begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //
		ESP_LOGI("Network Analizer", "Calibration OK 60Hz");
	}
}

bool compare_ticket(char *ticket)
{
	char data[50];
	bool response;

	if (strcmp("bbbb\n", ticket) == 0)
	{
		total_power = 1000;
		total_time = 1000;
		printf("ticket default: %s\n", ticket);
		response = true;
		return true;
	}

	esp_http_client_config_t config = {
		.url = "http://192.168.0.104:3000",
		.event_handler = _http_event_handler,
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	// GET
	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGI(TAG, "HTTP  OK GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));

		esp_http_client_read(client, data, esp_http_client_get_content_length(client));
	}
	else
	{
		return false;
	}
	printf("get data: %s\n", data);
	cJSON *root, *ticket_json;
	root = cJSON_CreateObject();
	root = cJSON_Parse(data);
	ticket_json = cJSON_GetObjectItem(root, "ticket");
	total_power = cJSON_GetObjectItem(root, "consumo")->valueint;
	total_time = cJSON_GetObjectItem(root, "duracion")->valueint;
	char *valid_ticket = cJSON_GetStringValue(ticket_json);
	printf("valid ticket: %s\n", valid_ticket);

	if (strcmp(cJSON_GetStringValue(ticket_json), ticket) == 0)
	{
		printf("ticket: %s\n", ticket);
		response = true;
	}
	else
	{
		printf("ticket: %s\n", ticket);
		response = false;
	}
	cJSON_Delete(root);
	return response;
}