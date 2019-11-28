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
//#include "Sim800L.h"

void decodeCommand(char *s)
{
	// int values[32];
	// cJSON *root, *dataarray;
	// root = cJSON_CreateObject();
	// root = cJSON_Parse(s);
	// printf("Command: %s\n", cJSON_PrintUnformatted(root));
	// dataarray = cJSON_GetObjectItem(root, "command");
	
		// if (MONOPHASE)
		// {
		// 	//Control FaseA

		// 	for (int i = 0; i < cJSON_GetArraySize(dataarray); i++)
		// 	{
		// 		values[i] = (cJSON_GetArrayItem(dataarray, i)->valueint);
		// 		printf("Send to Queue %d: %d\n", i, values[i]);
		// 		if (!xQueueSend(Queue_control_FaseA, &values[i], 100 / portTICK_PERIOD_MS))
		// 		{
		// 			printf("Error Send Queue!!\n");
		// 		}
		// 	}
		// }
		// else
		// {
		// 	//----------------------------------------------------------------------------
		// 	printf("Send to Queue Fase A ");
		// 	for (int i = 0; i < cJSON_GetArraySize(dataarray); i++)
		// 	{
		// 		values[i] = (cJSON_GetArrayItem(dataarray, i)->valueint);
		// 		printf("%d ", values[i]);
		// 		if (!xQueueSend(Queue_control_FaseA, &values[i], 100 / portTICK_PERIOD_MS))
		// 		{
		// 			printf("Error Send Queue!!\n");
		// 		}
		// 	}

		// 	//----------------------------------------------------------------------------
		// 	printf("\nSend to Queue Fase B ");
		// 	for (int i = 0; i < cJSON_GetArraySize(dataarray); i++)
		// 	{
		// 		values[i] = (cJSON_GetArrayItem(dataarray, i)->valueint);
		// 		printf("%d ", values[i]);
		// 		if (!xQueueSend(Queue_control_FaseB, &values[i], 100 / portTICK_PERIOD_MS))
		// 		{
		// 			printf("Error Send Queue!!\n");
		// 		}
		// 	}

		// 	//----------------------------------------------------------------------------
		// 	printf("\nSend to Queue Fase C ");
		// 	for (int i = 0; i < cJSON_GetArraySize(dataarray); i++)
		// 	{
		// 		values[i] = (cJSON_GetArrayItem(dataarray, i)->valueint);
		// 		printf("%d ", values[i]);
		// 		if (!xQueueSend(Queue_control_FaseC, &values[i], 100 / portTICK_PERIOD_MS))
		// 		{
		// 			printf("Error Send Queue!!\n");
		// 		}
		// 	}
		// 	printf("\n");
		// }
		// sizeCommand = 6 + (values[2] >> 2 & 0x0F);
	
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
char *createjsonFaseA(double volA, double currA, double factor, double potenA, double potenR, double potenApp, double potenMat, double temp)
{

	cJSON *root;
	char *datatoreturn;

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

	datatoreturn = cJSON_PrintUnformatted(root);
	return datatoreturn;
	//sendMessage(datatoreturn, "airis/1155/power_analizer");
}

char *createjsonFaseB(double volB, double currB, double factor, double potenB, double potenR, double potenApp, double potenMat, double temp)
{

	cJSON *root;
	char *datatoreturn;

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

	datatoreturn = cJSON_PrintUnformatted(root);
	return datatoreturn;
}

char *createjsonFaseC(double volC, double currC, double factor, double potenC, double potenR, double potenApp, double potenMat, double temp)
{

	cJSON *root;
	char *datatoreturn;

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

	datatoreturn = cJSON_PrintUnformatted(root);
	return datatoreturn;
}

/*! \brief ReadInformation.
 *
 *  This function read variables from the network analizer
 */
void ReadInformation()
{
	ready = false;
	double voltageA = GetLineVoltageA();
	while (!FinishTrans)
	{
	}
	double voltageB = GetLineVoltageB();
	while (!FinishTrans)
	{
	}
	double voltageC = GetLineVoltageC();
	while (!FinishTrans)
	{
	}
	double currentA = GetLineCurrentA();
	while (!FinishTrans)
	{
	}
	double currentB = GetLineCurrentB();
	while (!FinishTrans)
	{
	}
	double currentC = GetLineCurrentC();
	while (!FinishTrans)
	{
	}
	double temperature = GetTemperature();
	while (!FinishTrans)
	{
	}
	double powerfactorA = GetPowerFactorA();
	while (!FinishTrans)
	{
	}
	double powerfactorB = GetPowerFactorB();
	while (!FinishTrans)
	{
	}
	double powerfactorC = GetPowerFactorC();
	while (!FinishTrans)
	{
	}
	double powerA = GetActivePowerA();
	while (!FinishTrans)
	{
	}
	double powerB = GetActivePowerB();
	while (!FinishTrans)
	{
	}
	double powerC = GetActivePowerC();
	while (!FinishTrans)
	{
	}
	double powerReacA = GetReactivePowerA();
	while (!FinishTrans)
	{
	}
	double powerReacB = GetReactivePowerB();
	while (!FinishTrans)
	{
	}
	double powerReacC = GetReactivePowerC();
	while (!FinishTrans)
	{
	}
	double powerAppA = GetApparentPowerA();
	while (!FinishTrans)
	{
	}
	double powerAppB = GetApparentPowerB();
	while (!FinishTrans)
	{
	}
	double powerAppC = GetApparentPowerC();
	while (!FinishTrans)
	{
	}

	printf("=============FASE A=============== \n");
	printf("Voltage 1: %.1f [V] \n", voltageA);
	printf("Current 1: %.1f [A] \n", currentA);
	printf("Power 1: %.1f [W] \n", powerA);
	printf("Factor 1: %.1f  \n", powerfactorA);
	printf("PowerR 1: %.1f [VAR] \n", powerReacA);
	printf("PowerApp 1: %.1f [VA] \n", powerAppA);
	printf("=============FASE B=============== \n");
	printf("Voltage 2: %.1f [V] \n", voltageB);
	printf("Current 2: %.1f [A] \n", currentB);
	printf("Power 2: %.1f [W] \n", powerB);
	printf("Factor 2: %.1f [V] \n", powerfactorB);
	printf("PowerR 2: %.1f [VAR] \n", powerReacB);
	printf("PowerApp 2: %.1f [VA] \n", powerAppB);
	printf("=============FASE C=============== \n");
	printf("Voltage 3: %.1f [V] \n", voltageC);
	printf("Current 3: %.1f [A] \n", currentC);
	printf("Power 3: %.1f [W] \n", powerC);
	printf("Factor 3: %.1f [V] \n", powerfactorC);
	printf("PowerR 3: %.1f [VAR] \n", powerReacC);
	printf("PowerApp 3: %.1f [VA] \n", powerAppC);
	printf("============================== \n");
	printf("Temperature: %.1f [C] \n", temperature);
	uint8_t topic[] = "airis/1155/power_analizer";

	if (MONOPHASE)
	{
		// char *dataMQTTA = createjsonFaseA(voltageA, currentA, powerfactorA, powerA, powerReacA, powerAppA, (voltageA * currentA), temperature);
		// printf("Value to send faseA: %s\n", dataMQTTA);
		// sendMessage(dataMQTTA,(char *)topic);
	}
	else
	{
		char *dataMQTTA = createjsonFaseA(voltageA, currentA, powerfactorA, powerA, powerReacA, powerAppA, (voltageA * currentA), temperature);
		char *dataMQTTB = createjsonFaseB(voltageB, currentB, powerfactorB, powerB, powerReacB, powerAppB, (voltageB * currentB), temperature);
		char *dataMQTTC = createjsonFaseC(voltageC, currentC, powerfactorC, powerC, powerReacC, powerAppC, (voltageC * currentC), temperature);

		if (voltageA < 90 || voltageA > 235 || !Isconnected())
		{
			printf("\033[1;31m");
			printf("Lectura Erronea\n");
			printf("\033[0m");
		}
		else
		{
			sendMessage(dataMQTTA, (char *)topic);
			sendMessage(dataMQTTB, (char *)topic);
			sendMessage(dataMQTTC, (char *)topic);
		}
	}

	if (voltageA < 100)
	{
		contador_faseA++;
	}
	else
	{
		if(contador_faseA > 0)
		{
			contador_faseA--;
		}
	}

	if (voltageB < 100)
	{
		contador_faseB++;
	}
	else
	{
		if(contador_faseB > 0)
		{
			contador_faseB--;
		}
	}

	if (voltageC < 100)
	{
		contador_faseC++;
	}
	else
	{
		if(contador_faseC> 0)
		{
			contador_faseC--;
		}
	}

	if (contador_faseB >= 6 || contador_faseC >= 6)
	{
		if (contador_faseA < 6)
		{
			MONOPHASE = true;
			contador_faseB = 6;
			contador_faseC = 6;
		}
	}
	else
	{
		if (contador_faseB < 6 && contador_faseC < 6)
		{
			if (contador_faseA < 6)
			{
				MONOPHASE = false;
			}
		}
	}

	ready = true;
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
	vTaskDelay(200);
	frequencyValue = ReadFrequency();
	while (frequencyValue < 49 || frequencyValue > 61)
	{
		frequencyValue = ReadFrequency();
		while (!FinishTrans)
		{
		}
		printf("\033[1;31m");
		printf("FrequencyValue = %f\n", frequencyValue);
		printf("\033[0m");
	}
	printf("FrequencyValue = %f\n", frequencyValue);

	if (frequencyValue > 49 && frequencyValue < 51)
	{
		LineFreq = 5255;
		PGAGain = 0x002A;
		VoltageGain = 50597;
		CurrentGain = 53685;
	}
	else
	{
		LineFreq = 5255;
		PGAGain = 0x002A;
		VoltageGain = 50391;
		CurrentGain = 51571;
	}
	
	begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //
	ESP_LOGI("Network Analizer", "Calibration OK");
}

void set_PhaseControl()
{
	double VoltageA = GetLineVoltageA();
	while (!FinishTrans)
	{
	}

	while (VoltageA < 110 || VoltageA > 300)
	{
		VoltageA = GetLineVoltageA();
		while (!FinishTrans)
		{
		}
	}

	double VoltageB = GetLineVoltageA();
	while (!FinishTrans)
	{
	}
	double VoltageC = GetLineVoltageC();

	printf("=====CALIBRATION===== \n");

	if (VoltageB < 100 || VoltageC < 100)
	{
		MONOPHASE = true;
		printf("======MONOPHASE====== \n");
	}
	else
	{
		MONOPHASE = false;
		printf("=== THREE PHASE ==== \n");
	}
	printf("===================== \n");
}