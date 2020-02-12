#include "Parameters.h"
#include "Sim800L.h"
#include "uart_lib.h"
#include "ZDU0210RJX.h"
#include "Functions.h"
#include <esp_log.h>

static const char *TAG = "SIM800";


bool sendCommandATToken(char *command, char *responsecommand)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;

    printf("Command to send: %s\n", command);
    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, strlen(command), 0);
    
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G,0, 0);
    while (rx_size == 0)
    {
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G,0, 0);
        vTaskDelay(50);
    }
    printf("RX size AT: %d \n", rx_size);
    Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);   

    char *ReadValue = substring((char *)dataRxGPRS,3,(rx_size-4));
    
    // printf("Response AT Hex:");
    // for (int i = 0; i < rx_size; i++)
    //     printf("%x", dataRxGPRS[i]);
    // printf("\n");
    // printf("Response AT ASCII: %s\n",ReadValue);

    memset(dataRxGPRS, 0, 64);  

    if(strcmp(responsecommand, ReadValue)==0){
        printf("Response AT OK\n");
        return true;
    }else{
        printf("Response AT ERROR\n");
        return false;
    }
    
}

bool sendCommandATSize(char *command, int MinimunSize)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;

    printf("Command to send: %s\n", command);
    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, strlen(command), 0);
    
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G,0, 0);
    while (rx_size == 0)
    {
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G,0, 0);
        vTaskDelay(50);
    }
    printf("RX size AT: %d \n", rx_size);
    Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);   

    char *ReadValue = substring((char *)dataRxGPRS,3,(rx_size-4));
    
    // printf("Response AT Hex:");
    // for (int i = 0; i < rx_size; i++)
    //     printf("%x", dataRxGPRS[i]);
    // printf("\n");
    // printf("Response AT ASCII: %s\n",ReadValue);

    memset(dataRxGPRS, 0, 64);  

    if(rx_size>MinimunSize){
        printf("Response AT OK\n");
        return true;
    }else{
        printf("Response AT ERROR\n");
        return false;
    }
}

void sendATValue(char *value, int sizeCommand)
{
    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)value, sizeCommand, 0);
}

void sim800l_PowerOn()
{
    //start sim800l
    gpio_write_ZDU0210RJX(ZDU0210RJX_address2G,0x08, 0x08);
    vTaskDelay(100);
    gpio_write_ZDU0210RJX(ZDU0210RJX_address2G,0x08, 0x00);    
    vTaskDelay(1000);
    gpio_write_ZDU0210RJX(ZDU0210RJX_address2G,0x80, 0x80);
    ESP_LOGI(TAG, "POWER ON sim800");

}

void sim800l_begin()
{
    ESP_LOGI(TAG, "Init BEGIN sim800");
    // char CommandToSendx[] = "ATE0\n";
    // sendCommandATSize(CommandToSendx, 6);
    char CommandToSendy[] = "AT\n";
    // while(!sendCommandATToken(CommandToSendy, "OK")){
    //     //validate synchronization
    // }
    //led PowerOn   

    char CommandToSendSim[] = "AT+CPIN?\n";        //Pin de seguridad
    sendCommandATToken(CommandToSendSim, "OK");
    char CommandToSendSim0[] = "AT+CSQ\n";          //Calidad de señal 
    sendCommandATToken(CommandToSendSim0, "OK");
    char CommandToSendSim1[] = "AT+CREG?\n";        //3 GSM con EGPRS 
    sendCommandATToken(CommandToSendSim1, "OK");
    char CommandToSendSim2[] = "AT+CGATT?\n";       //Estado GPRS
    sendCommandATToken(CommandToSendSim2, "OK");
    char CommandToSendSim3[] = "AT+CIPMUX?\n";
    sendCommandATToken(CommandToSendSim3, "OK");
    char CommandToSendSim4[] = "AT+CCID=?\n";
    sendCommandATToken(CommandToSendSim4, "OK");

    char CommandToSend[] = "AT+CFUN=1\n";
    sendCommandATToken(CommandToSend, "OK");
    char CommandToSend1[] = "AT+CREG=1\n";
    sendCommandATToken(CommandToSend1, "OK");
    char CommandToSend2[] = "AT+CIPMUX=0\n";
    sendCommandATToken(CommandToSend2, "OK");
    char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"internet.comcel.com.co\"\n"; //cambiar por APN del operador
    //char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"web.colombiamovil.com.co\"\n"; //cambiar por APN del operador
    //char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"telefonica.es\"\n"; //cambiar por APN del operador
    sendCommandATToken(CommandToSend3,"OK");
    char CommandToSend4[] = "AT+CSTT=\"internet.comcel.com.co\"\n"; //cambiar por APN del operador
    //char CommandToSend4[] = "AT+CSTT=\"web.colombiamovil.com.co\"\n"; //cambiar por APN del operador
    //char CommandToSend4[] = "AT+CSTT=\"telefonica.es\"\n"; //cambiar por APN del operador
    sendCommandATToken(CommandToSend4,"OK");
    char CommandToSend5[] = "AT+CIICR\n";
    sendCommandATToken(CommandToSend5,"OK");
    char CommandToSend6[] = "AT+CIFSR\n";
    sendCommandATSize(CommandToSend6,10);
    char CommandToSend7[] = "AT+CIPSTATUS\n";
    sendCommandATSize(CommandToSend7,20);
    gpio_write_ZDU0210RJX(ZDU0210RJX_address2G,0x40, 0x40);
}

void subscribeTopic()
{
    char command2[] = "AT+CIPSEND\n";
    sendATValue(command2, strlen(command2)); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(300);                 //No es necesario en la version i2c-uart

    char sub[] = {0x82, 0x17, 0x00, 0x02, 0x00, 0x12, 0x61, 0x69, 0x72, 0x69, 0x73, 0x2F, 0x31, 0x31, 0x35, 0x35, 0x2F, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64, 0x00, 0x1A, 0x0D};
    sendATValue(sub,27); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(100);                //No es necesario en la version i2c-uart
    //ready = true;
}

void StartGPRSMQTTConnection()
{
    char command[] = "AT+CIPSTART=\"TCP\",\"platform.agrum.co\",\"1883\"\n";
    sendCommandATSize(command,3);
    vTaskDelay(400);
    char command2[] = "AT+CIPSEND\n";
    sendATValue(command2, strlen(command2)); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(300);                 //No es necesario en la version i2c-uart

    char headMQTT[] = {0x10, 0x12, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0x02, 0x00, 0x3C, 0x00, 0x06, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x1A, 0x0D};
    sendATValue(headMQTT, 22); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(200);                          //No es necesario en la version i2c-uart
}