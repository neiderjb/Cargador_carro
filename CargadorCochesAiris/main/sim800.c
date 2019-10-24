//https://github.com/vshymanskyy/TinyGSM/blob/master/src/TinyGsmClientSIM800.h

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_system.h"
#include "esp_log.h"

#include "string.h"
#include "cJSON.h"

#include "ZDU0210RJX.h"
#include "sim800.h"

#include "esp_log.h"

#include "Parameters.h"
#include "Function.h"

static const char *TAG = "sim800";

bool connected_value_gprs = false;

bool begin_800()
{

    //xTaskCreate(sim800_rx_task, "sim800_rx_task", 4096, NULL, 5, &task_sim800_rx);

    char CommandToSend[] = "AT+CFUN=1\n";
    sendCommandAT(CommandToSend, "OK");

    char CommandToSend1[] = "AT+CREG=1\n";
    sendCommandAT(CommandToSend1, "OK");

    char CommandToSend2[] = "AT+CIPMUX=0\n";
    sendCommandAT(CommandToSend2, "OK");

    //char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"web.colombiamovil.com.co\"\n"; //TIGO
    //char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"internet.movistar.com.co\"\n";   //MOVISTAR
    char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"internet.comcel.com.co\"\n"; //CLARO
    sendAT(CommandToSend3, strlen(CommandToSend3), 1);
    //responseAT();

    //char CommandToSend4[] = "AT+CSTT=\"web.colombiamovil.com.co\"\n"; //TIGO
    //char CommandToSend4[] = "AT+CSTT=\"internet.movistar.com.co\"\n";             //MOVISTAR
    char CommandToSend4[] = "AT+CSTT=\"internet.comcel.com.co\"\n"; //CLARO
    sendAT(CommandToSend4, strlen(CommandToSend4), 1);
    //responseAT();

    char CommandToSend5[] = "AT+CIICR\n";
    sendAT(CommandToSend5, strlen(CommandToSend5), 1);
    //responseAT();

    char CommandToSend6[] = "AT+CIFSR\n";
    sendAT(CommandToSend6, strlen(CommandToSend6), 1);
    //responseAT();

    char CommandToSend7[] = "AT+CIPSTATUS\n";
    sendAT(CommandToSend7, strlen(CommandToSend7), 1);
    //responseAT();

    connected_value_gprs = true;
    ESP_LOGI(TAG, "begin_800 OK");
    return true;
}

/*
Metodo Post de MQTT, recibe los mismos parametros, calcula los mismos tamaños

*/
void sim800_postMQTT(uint8_t *topic, int TopicSize, uint8_t *data, int DataSize)
{
    uint8_t command2[] = "AT+CIPSEND\n";
    sendAT((char *)command2, sizeof(command2), 1);
    //responseAT();

    //30 13 00 08 76 61 6C 65 74 72 6F 6E 68 65 6C 6C 6F 72 61 76 69 1A ---Publish
    char headPub[4];
    // printf("Topic Size: %x\n",TopicSize);
    // printf("Data Size: %x\n",DataSize);
    // printf("Mat Size: %x\n",(DataSize+TopicSize));
    headPub[0] = 0x30;
    headPub[1] = (char)DataSize + TopicSize + 1;
    headPub[2] = (char)TopicSize >> 8;
    headPub[3] = (char)TopicSize - 1;
    sendAT(headPub, strlen(headPub), 1);
    //responseAT();

    sendAT((char *)topic, strlen((char *)topic), 1);
    //responseAT();

    sendAT((char *)data, strlen((char *)data), 1);
    //responseAT();

    char FinPub[2] = {0x1A, 0x0D};
    sendAT(FinPub, strlen(FinPub), 1);
    //responseAT();
    //ready = true;
}

void subtopic()
{
    uint8_t command2[] = "AT+CIPSEND\n";
    sendAT((char *)command2, sizeof(command2), 1);
    //responseAT();

    char sub[] = {0x82, 0x17, 0x00, 0x02, 0x00, 0x12, 0x61, 0x69, 0x72, 0x69, 0x73, 0x2F, 0x31, 0x31, 0x35, 0x35, 0x2F, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64, 0x00, 0x1A, 0x0D};
    sendAT(sub, strlen(sub), 1);
    //responseAT();
}

void startCon()
{
    uint8_t command[] = "AT+CIPSTART=\"TCP\",\"platform.agrum.co\",\"1883\"\n";
    sendAT((char *)command, sizeof(command), 1);
    //responseAT();
    uint8_t command2[] = "AT+CIPSEND\n";
    sendAT((char *)command2, sizeof(command2), 1);
    //responseAT();

    char headMQTT[] = {0x10, 0x12, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0x02, 0x00, 0x3C, 0x00, 0x06, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x1A, 0x0D};
    sendAT((char *)headMQTT, strlen(headMQTT), 1);
    // responseAT();
}

void sim800_rx_task(void *pvParameters)
{
    uint8_t rx_size = 0;
    uint8_t status = 0;
    uint8_t statusInter = 0;

    printf("RX size >0 TASK");
    for (;;)
    {
        Read_UART_STATUS_REGISTER_ZDU0210RJX(1, status);
        Interrupt_Status_Register_ZDU0210RJX(1, statusInter);
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(1, 0);
        printf("STATUS UART 1: %x -Interrupt: %x -SizeRx %d\n", status, statusInter, rx_size);

        if (statusInter != 0)
        {
            printf("========================== \n");
            printf("STATUS UART 1: %x -Interrupt: %x\n", status, statusInter);
            rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(1, 0);
            if (rx_size > 0)
            {
                //responseAT();
                rx_size = 0;
            }
        }
        uart_reset_FIFO_ZDU0210RJX(1);
        vTaskDelay(100);
    }
}

bool Isconnected_gprs()
{
    if (connected_value_gprs)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool factoryDefault()
{
    // sendAT("&FZE0&W");  // Factory + Reset + Echo Off + Write
    // //waitResponse();
    // sendAT("+IPR=0");   // Auto-baud
    // //waitResponse();
    // sendAT("+IFC=0,0"); // No Flow Control
    // //waitResponse();
    // sendAT("+ICF=3,3"); // 8 data 0 parity 1 stop
    // //waitResponse();
    // sendAT("+CSCLK=0"); // Disable Slow Clock
    // //waitResponse();
    // sendAT("&W");       // Write configuration
    return true;
}

bool restart()
{
    //Enable Local Time Stamp for getting network time
    // TODO: Find a better place for this
    // sendAT("+CLTS=1");
    // // if (waitResponse(10000L) != 1) {
    // //   return false;
    // // }
    // sendAT("&W");
    // //waitResponse();
    // sendAT(GF("+CFUN=0"));
    // // if (waitResponse(10000L) != 1) {
    // //   return false;
    // // }
    // sendAT(GF("+CFUN=1,1"));
    // // if (waitResponse(10000L) != 1) {
    // //   return false;
    // // }
    // vTaskDelay(1000);
    return true;
}

bool poweroff()
{
    char CommandToSend[] = "AT+CPOWD=1\n";
    sendAT(CommandToSend, sizeof(CommandToSend), 1);
    vTaskDelay(200);
    return true;
}

bool radioOff()
{
    char CommandToSend[] = "AT+CFUN=0\n";
    sendAT(CommandToSend, sizeof(CommandToSend), 1);
    vTaskDelay(200);
    return true;
}

/*
    During sleep, the SIM800 module has its serial communication disabled. In order to reestablish communication
    pull the DRT-pin of the SIM800 module LOW for at least 50ms. Then use this function to disable sleep mode.
    The DTR-pin can then be released again.
*/
bool sleepEnable()
{
    char CommandToSend[] = "AT+CSCLK=\n";
    sendAT(CommandToSend, sizeof(CommandToSend), 1);
    vTaskDelay(200);
    return true;
}

bool testAT()
{
    return true;
}

/*
Send and Get Data to AT command
*/
void sendAT(char *dataWrite, int countWrite, int uart)
{
    //Write_Multiple_Data_TX_FIFO_ZDU0210RJX((uint8_t *)dataWrite, countWrite, 1);
    //Send to AT
    for (int i = 0; i < countWrite; i++)
    {
        Write_Data_TX_FIFO_ZDU0210RJX(dataWrite[i], 1);
    }
    countWrite = 0;
}

void sendCommandAT(char *command, char *responsecommand)
{
    uint8_t countAT = 0;
    while (countAT < 3)
    {
        sendAT(command, strlen(command), 1);
        if (responseAT(command, responsecommand))
        {
            countAT = 0;
            break;
        }
        countAT++;
    }
}

bool responseAT(char *ATcommand, char *ATcommandResponse)
{
    char dataRxGPRS[64];
    uint8_t rx_size = 0;
    uint8_t tx_size = 0;
    uint8_t status = 0;
    uint8_t statusInter = 0;
    bool AT_echo = false;

    vTaskDelay(600);
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(1, 0);
    printf("RX size AT: %d \n", rx_size);

    Read_Data_RX_FIFO_ZDU0210RJX(1, (uint8_t *)dataRxGPRS, rx_size);
    // printf("Response AT: ");
    // for (int i = 0; i < rx_size; i++)
    // {
    //     printf("%c", dataRxGPRS[i]);
    // }
    // printf("\n");

    uart_reset_FIFO_ZDU0210RJX(1);

    const char s[] = "\n";
    char *token;
    token = strtok(dataRxGPRS, s);
    /* walk through other tokens */
    while (token != NULL)
    {
        printf("TOKEN: %s\n", token);
        token = strtok(NULL, s);

        if (strcmp(token, ATcommand) == 0)
        {
            printf("Response = CommandAT");
            AT_echo = true;
        }

        if (AT_echo && strcmp(token, ATcommandResponse) == 0)
        {
            printf("Response AT = Response OK");
            return true;
        }
        else
        {
            printf("Response AT = Response ERROR");
        }
    }
    AT_echo = false;

    memset(dataRxGPRS, 0, sizeof(dataRxGPRS));
    return false;
}