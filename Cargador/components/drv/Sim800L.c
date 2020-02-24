//https://bitbucket.org/mbari_peter/sim800-mqtt-ravi/src/master/
//https://openlabpro.com/guide/mqtt-packet-format/
//http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html

#include "Parameters.h"
#include "Sim800L.h"
#include "uart_lib.h"
#include "ZDU0210RJX.h"
#include "Functions.h"
#include "FunctionsCC.h"
#include <esp_log.h>

#include "esp_timer.h"

static const char *TAG = "SIM800";

bool conGPRS = false;

bool sendCommandATToken(char *command, char *responsecommand, bool debug)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;
    if (debug)
    {
        printf("\nCommand to sendCommandATToken: %s", command);
    }
    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, strlen(command), 0); //SEND ALL DATA
    //Write_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address,u8ModbusADU[i], 0); //SEND TO BYTE TO BYTE
    vTaskDelay(200);
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
    int try
        = 2;
    while (rx_size == 0)
    {
        if (try == 0)
        {
            break;
        }
        try
            --;
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
        vTaskDelay(100);
    }

    if (try != 0)
    {
        // printf("RX size AT: %d \n", rx_size);
        Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

        char *ReadValue = substring((char *)dataRxGPRS, 3, strlen(responsecommand)); //(rx_size - 4)
        char *ReadValueComplete = substring((char *)dataRxGPRS, 3, rx_size);         //(rx_size - 4)

        if (debug)
        {
            printf("Response AT Hex:");
            for (int i = 0; i < rx_size; i++)
                printf("%x", dataRxGPRS[i]);
            printf("\n");
            printf("Response AT COMPLETE ASCII: %s\n", ReadValueComplete);
            printf("Response AT ASCII: %s\n", ReadValue);
            //printf("\n\n");
        }
        memset(dataRxGPRS, 0, 64);

        if (strcmp(responsecommand, ReadValue) == 0)
        {
            if (debug)
            {
                printf("Response sendCommandATToken AT OK\n\n");
            }
            return true;
        }
        else
        {
            if (debug)
            {
                printf("Response sendCommandATToken AT ERROR\n\n");
            }
            return false;
        }
    }
    else
    {

        if (debug)
        {
            printf("NO Response sendCommandATToken\n\n");
        }
        memset(dataRxGPRS, 0, 64);
        return false;
    }
}

bool sendCommandATSize(char *command, int MinimunSize, bool debug)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;
    if (debug)
    {
        printf("\nCommand to sendCommandATSize: %s", command);
    }

    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, strlen(command), 0);
    vTaskDelay(200);
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
    int try
        = 2;
    while (rx_size == 0)
    {
        if (try == 0)
        {
            break;
        }
        try
            --;
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
        vTaskDelay(100);
    }

    if (try != 0)
    {
        //printf("RX size AT: %d \n", rx_size);
        Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

        char *ReadValue = substring((char *)dataRxGPRS, 3, (rx_size - 4));
        char *ReadValueComplete = substring((char *)dataRxGPRS, 3, rx_size);

        if (debug)
        {
            printf("Response AT Hex:");
            for (int i = 0; i < rx_size; i++)
                printf("%x", dataRxGPRS[i]);
            printf("\n");
            printf("Response AT Complete ASCII: %s\n", ReadValueComplete);
            printf("Response AT ASCII: %s\n", ReadValue);
        }
        memset(dataRxGPRS, 0, 64);

        if (rx_size > MinimunSize)
        {
            if (debug)
            {
                printf("Response sendCommandATSize AT OK\n\n");
            }
            return true;
        }
        else
        {
            if (debug)
            {
                printf("Response sendCommandATSize AT ERROR\n\n");
            }
            memset(dataRxGPRS, 0, 64);
            return false;
        }
    }
    else
    {
        if (debug)
        {
            printf("NO Response sendCommandATSize AT ERROR\n\n");
        }
        return false;
    }
}

//Send command to response task
bool sendCommandAT(char *command, int sizeCommand, char *response, bool debug)
{
    uint8_t rx_size = 0;
    printf("\nCommand to sendCommandAT: %s", command);
    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, sizeCommand, 0);

    //vTaskDelay(50);
    int try
        = 3;
    while (try != 0) //Semaphore ">", "SEND OK"
    {
        //Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, sizeCommand, 0);

        if (strcmp("-", response) == 0)
        {
            return true;
        }
        if (xSemaphoreTake(Semaphore_WAIT, 10) && (strcmp(">", response) == 0))
        {
            printf("Command %s, >OK\n", command);
            return true;
        }
        if (xSemaphoreTake(Semaphore_SENDOK, 10) && (strcmp("SEND OK", response) == 0))
        {
            printf("Command %s, SENDOK OK\n", command);
            return true;
        }
        try
            --;
        vTaskDelay(100);
    }
    printf("NO Response Command\n");
    return false;
}

void sendATValue(char *value, int sizeCommand)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;

    printf("\nCommand to sendCommandATSize: %s", value);

    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)value, sizeCommand, 0);

    vTaskDelay(50);
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
    //printf("RX size AT: %d \n", rx_size);
    Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

    char *ReadValue = substring((char *)dataRxGPRS, 3, rx_size);

    printf("Response MQTT Hex:");
    for (int i = 0; i < rx_size; i++)
        printf("%x", dataRxGPRS[i]);
    printf("\n");
    printf("Response MQTT ASCII: %s\n", ReadValue);
    memset(dataRxGPRS, 0, 64);
}

bool sendATValueResponse(char *value, int sizeCommand, char *responsecommand, bool debug)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;

    if (debug)
    {
        printf("\nCommand to sendATValue: %s", value);
    }

    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)value, sizeCommand, 0);

    vTaskDelay(200);
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);

    int try
        = 2;
    while (rx_size == 0)
    {
        if (try == 0)
        {
            break;
        }
        try
            --;
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
        vTaskDelay(100);
    }

    if (try != 0)
    {
        //printf("RX size AT: %d \n", rx_size);
        Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

        char *ReadValue = substring((char *)dataRxGPRS, 3, strlen(responsecommand));
        char *ReadValueComplete = substring((char *)dataRxGPRS, 3, rx_size);

        if (debug)
        {
            printf("Response MQTT Hex:");
            for (int i = 0; i < rx_size; i++)
                printf("%x", dataRxGPRS[i]);
            printf("\n");
            printf("Response MQTT Complete ASCII: %s\n", ReadValueComplete);
            printf("Response MQTT ASCII: %s\n", ReadValue);
            //printf("\n\n");
        }
        memset(dataRxGPRS, 0, 64);

        if (strcmp(responsecommand, ReadValue) == 0)
        {
            if (debug)
            {
                printf("Response sendCommandATToken AT OK\n\n");
            }
            return true;
        }
        else
        {
            if (debug)
            {
                printf("Response sendCommandATToken AT ERROR\n\n");
            }
            memset(dataRxGPRS, 0, 64);
            return false;
        }
    }
    else
    {
        if (debug)
        {
            printf("NO DATA sendATValue\n\n");
        }
        return false;
    }
}

void sendATValueNoRESPONSE(char *value, int sizeCommand, bool debug)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;

    if (debug)
    {
        printf("\nCommand to sendATValue: %s", value);
    }

    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)value, sizeCommand, 0);

    vTaskDelay(50);
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
    int try
        = 5;

    while (rx_size == 0)
    {
        if (try == 0)
        {
            break;
        }
        try
            --;
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
        vTaskDelay(50);
    }

    if (try != 0)
    {
        printf("RX size AT: %d \n", rx_size);
        Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

        char *ReadValue = substring((char *)dataRxGPRS, 3, (rx_size - 4));
        char *ReadValueComplete = substring((char *)dataRxGPRS, 3, (rx_size));

        if (debug)
        {
            printf("Response MQTT Hex:");
            for (int i = 0; i < rx_size; i++)
                printf("%x", dataRxGPRS[i]);
            printf("\n");
            printf("Response MQTT Complete ASCII: %s\n", ReadValueComplete);
            printf("Response MQTT ASCII: %s\n", ReadValue);
            //printf("\n\n");
        }
        memset(dataRxGPRS, 0, 64);
    }
    else
    {
        memset(dataRxGPRS, 0, 64);
        printf("NO DATA MQTT");
    }
}

//---------------------------------------------------------------------//

void gprsRead_task(void *p)
{
    ESP_LOGI(TAG, "Gprs READ Task");
    while (1)
    {
        if (conGPRS)
        {
            uint8_t dataRxGPRS[64];

            uint8_t rx_size = 0;
            vTaskDelay(10);
            rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
            Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

            //char *ReadValue = substring((char *)dataRxGPRS, 3, (rx_size));
            char *ReadValue = (char *)dataRxGPRS;

            
            char delimitador[] = "0xda 0x20"; //"0xda";
            char *token = strtok(ReadValue, delimitador);

            if (token != NULL)
            {
                printf("RX size AT TASK: %d \n", rx_size);

                printf("Response COMPLETE ASCII TASK: %s\n", ReadValue);
                for (int i = 0; i < strlen(ReadValue); i++)
                {
                    printf(" %x", ReadValue[i]);
                }
                printf("\n");

                while (token != NULL)
                {

                    // printf("Token  ASCII: %s , %d\n", token, strlen(token));
                    // for (int i = 0; i < strlen(token); i++)
                    // {
                    //     printf(" %x", token[i]);
                    // }
                    // printf("\n");
                    char *token1 = substring(token, 3, 2);
                    printf("Token 1  ASCII: %s , %d\n", token1, strlen(token1));
                    if (strcmp(">", token1) == 0)
                    {
                        printf("> Compare OK\n");
                        xSemaphoreGive(Semaphore_WAIT);
                        break;
                    }
                    char *token2 = substring(token, 3, 7);
                    printf("Token 2  ASCII: %s , %d\n", token2, strlen(token2));
                    if (strcmp("SEND OK", token2) == 0)
                    {
                        printf("SEND OK Compare OK\n");
                        xSemaphoreGive(Semaphore_SENDOK);
                        break;
                    }
                    // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
                    token = strtok(NULL, delimitador);
                }
            }
        }
        else
        {
            vTaskDelay(500);
        }

        //SearchCommand(dataRxGPRS);
    }
}

void readDataMQTT2G(char *command, int timeout)
{
    unsigned long nowMillis = esp_timer_get_time();

    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;
    //ESP_LOGI(TAG, "--------READ DATA GPRS-------------");

    //Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, strlen(command), 0); //SEND ALL DATA
    rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);

    
    while (rx_size == 0)
    {
        if (esp_timer_get_time()-nowMillis > timeout)
        {
             //ESP_LOGI(TAG, "TIME OUT NO DATA MQTT\n");
            break;
        }
        
        rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
        vTaskDelay(1);
    }

    if (rx_size != 0)
    {
        ESP_LOGI(TAG, "--------DATA READ 2G-------------\n");
        printf("RX size AT: %d \n", rx_size);
        Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

        char *ReadValue = substring((char *)dataRxGPRS, 0, (rx_size));

        printf("Response MQTT 2G Hex:");
        for (int i = 0; i < rx_size; i++)
            printf("%02x", dataRxGPRS[i]);
        printf("\n");
        printf("Response MQTT ASCII: %s\n", dataRxGPRS);

        if(dataRxGPRS[0] == 0x30 && rx_size > 5 ){
            SearchCommand(dataRxGPRS);
        }
        memset(dataRxGPRS, 0, 64);
    }
    else
    {
        //ESP_LOGI(TAG, "NO DATA MQTT\n");
        memset(dataRxGPRS, 0, 64);
    }
}

void sim800l_PowerOn()
{
    //start sim800l
    gpio_write_ZDU0210RJX(ZDU0210RJX_address2G, 0x08, 0x08);
    vTaskDelay(100);
    gpio_write_ZDU0210RJX(ZDU0210RJX_address2G, 0x08, 0x00);
    vTaskDelay(1000);
    gpio_write_ZDU0210RJX(ZDU0210RJX_address2G, 0x80, 0x80);
    ESP_LOGI(TAG, "POWER ON sim800");
}

bool sim800l_begin()
{
    ESP_LOGI(TAG, "sim800l_begin");
    int try
        = 5;
    ESP_LOGI(TAG, "Init BEGIN sim800");
    char CommandToSendx[] = "ATE0\n";
    sendCommandATSize(CommandToSendx, 6, false);
    char CommandToSendy[] = "AT\n";
    // while(!sendCommandATToken(CommandToSendy, "OK")){
    //     //validate synchronization
    // }
    //led PowerOn

    char CommandToSendSim[] = "AT+CPIN?\n"; //Pin de seguridad
    sendCommandATToken(CommandToSendSim, "+CPIN: READY", true);
    char CommandToSendSim0[] = "AT+CSQ\n"; //Calidad de señal
    sendCommandATToken(CommandToSendSim0, "+CSQ:", true);
    char CommandToSendSim1[] = "AT+CREG?\n"; //3 GSM con EGPRS
    sendCommandATToken(CommandToSendSim1, "+CREG:", true);

    char CommandToSendSim2[] = "AT+CGATT?\n"; //Estado GPRS
    sendCommandATToken(CommandToSendSim2, "+CGATT:", true);
    char CommandToSendSim3[] = "AT+CIPMUX?\n";
    sendCommandATToken(CommandToSendSim3, "+CIPMUX:", true);
    char CommandToSendSim4[] = "AT+CCID=?\n";
    sendCommandATToken(CommandToSendSim4, "OK", true);

    char CommandToSend[] = "AT+CFUN=1\n";
    sendCommandATToken(CommandToSend, "OK", true);
    char CommandToSend1[] = "AT+CREG=1\n";
    sendCommandATToken(CommandToSend1, "OK", true);

    char CommandToSend2[] = "AT+CIPMUX=0\n"; // Selects Single-connection mode
    if (sendCommandATToken(CommandToSend2, "OK", true))
    {
        //if (sendCommandATToken("AT+CIPRXGET=1\n", "OK", true)) //RECEIVE DATA manually FROM THE REMOTE SERVER
        //{
            // sendCommandATToken("AT+CIPMODE=0\n", "OK", true);
            // sendCommandATToken("AT+CIPSRIP=0\n", "OK", true);
            vTaskDelay(500);
            while (!sendCommandATToken("AT+CGATT?\n", "+CGATT: 1", true))
            {
                if (try == 0)
                {
                    ESP_LOGI(TAG, " AT+CGATT NO CONNECTION");
                    return false;
                }
                try
                    --;
                vTaskDelay(500);
            }
            ESP_LOGI(TAG, "+CGATT: 1 CONECTION GPRS");
            sendCommandATToken("AT+CIPSTATUS\n", "OK", true);

            char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"internet.comcel.com.co\"\n"; //cambiar por APN del operador
            //char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"web.colombiamovil.com.co\"\n"; //cambiar por APN del operador
            //char CommandToSend3[] = "AT+CGDCONT=1,\"IP\",\"telefonica.es\"\n"; //cambiar por APN del operador
            sendCommandATToken(CommandToSend3, "OK", true);

            char CommandToSend4[] = "AT+CSTT=\"internet.comcel.com.co\"\n"; //cambiar por APN del operador
            //char CommandToSend4[] = "AT+CSTT=\"web.colombiamovil.com.co\"\n"; //cambiar por APN del operador
            //char CommandToSend4[] = "AT+CSTT=\"telefonica.es\"\n"; //cambiar por APN del operador
            if (sendCommandATToken(CommandToSend4, "OK", true))
            {
                // Waits for status IP START
                sendCommandATToken("AT+CIPSTATUS\n", "OK", true);
                vTaskDelay(1000);

                // Brings Up Wireless Connection
                char CommandToSend5[] = "AT+CIICR\n";
                try
                    = 5;
                while (!sendCommandATToken(CommandToSend5, "OK", true))
                {
                    if (try == 0)
                    {
                        ESP_LOGI(TAG, "AT+CIICR ERROR BRINGING UP WIRELESS CONNECTION");
                        return false;
                    }
                    try
                        --;
                    vTaskDelay(1000);
                }
                // Waits for status IP GPRSACT
                try
                    = 5;
                while (!sendCommandATToken("AT+CIPSTATUS\n", "OK", true))
                {
                    if (try == 0)
                    {
                        ESP_LOGI(TAG, "AT+CIPSTATUS ERROR");
                        return false;
                    }
                    try
                        --;
                    vTaskDelay(500);
                }
                vTaskDelay(300);
                // Gets Local IP Address
                char CommandToSend6[] = "AT+CIFSR\n";
                sendCommandATSize(CommandToSend6, 10, true);

                // Waits for status IP STATUS
                char CommandToSend7[] = "AT+CIPSTATUS\n";
                try
                    = 5;
                while (!sendCommandATToken(CommandToSend7, "OK", true))
                {
                    if (try == 0)
                    {
                        ESP_LOGI(TAG, "AT+CIPSTATUS ERROR");
                        return false;
                    }
                    try
                        --;
                    vTaskDelay(500);
                }
                //vTaskDelay(1000);
                gpio_write_ZDU0210RJX(ZDU0210RJX_address2G, 0x40, 0x40);
                ESP_LOGI(TAG, "CONECTION GPRS OK");
                return true;
            }
            else
            {
                ESP_LOGI(TAG, "Error setting the APN\n");
                return false;
            }
        // }
        // else
        // {
        //     printf("Error setting CIPRXGET");
        //     return false;
        // }
    }
    else
    {
        ESP_LOGI(TAG, "sim800l_begin FAIL \n");
        return false;
    }
}

bool subscribeTopic()
{
    ESP_LOGI(TAG, "--------Init subscribeTopic-------------\n");
    char command2[] = "AT+CIPSEND\n";
    int try
        = 5;
    while (!sendCommandATToken(command2, ">", true))
    {
        if (try == 0)
        {
            ESP_LOGI(TAG, "ERROR AT+CIPSEND");
            return false;
        }
        try
            --;
        vTaskDelay(300);
    }
    //sendATValue(command2, strlen(command2), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(300); //No es necesario en la version i2c-uart

    char sub[] = {0x82, 0x15, 0x00, 0x02, 0x00, 0x10, 0x61, 0x69, 0x72, 0x69, 0x73, 0x2F, 0x63, 0x63, 0x2F, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64, 0X00, 0x1A, 0x0D};
    // char sub[25];

    // char headPub[6];
    // headPub[0] = 0x82; //Header subscribeTopic
    // headPub[1] = 0x15; //Length remaning byte 3 to end

    // sub[2] = 0x00; //PACKET ID
    // sub[3] = 0x02; //PACKET ID

    // sub[4] = 0x00;                                          //Length PAYLOAD
    // sub[5] = 0x10;                                          //Length PAYLOAD
    // sendATValue(headPub, sizeof(headPub), "SEND OK", true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    // vTaskDelay(100);

    // sendATValue("airis/cc/command", strlen("airis/cc/command"), "SEND OK", true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    // vTaskDelay(100);

    // char FinPub[3] = {0X00, 0x1A, 0x0D};

    // sub[6] = 0x61;  //a
    // sub[7] = 0x69;  //i
    // sub[8] = 0x72;  //r
    // sub[9] = 0x69;  //i
    // sub[10] = 0x73; //s
    // sub[11] = 0x2F; ///
    // sub[12] = 0x63; //c
    // sub[13] = 0x63; //c
    // sub[14] = 0x2F; ///
    // sub[15] = 0x63; //c
    // sub[16] = 0x6F; //o
    // sub[17] = 0x6D; //m
    // sub[18] = 0x6D; //m
    // sub[19] = 0x61; //a
    // sub[20] = 0x6E; //n
    // sub[21] = 0x64; //d

    // sub[22] = 0X00; //QOS
    // sub[23] = 0x1A; //END
    // sub[24] = 0x0D; //END
    try
        = 5;
    while (!sendATValueResponse(sub, 25, "SEND OK", true))
    {
        if (try == 0)
        {
            ESP_LOGI(TAG, "Exit subscribeTopic FAIL\n");
            return false;
        }
        try
            --;
        vTaskDelay(500);
    }

    if (try != 0)
    {
        ESP_LOGI(TAG, "subscribeTopic OK\n");
        vTaskDelay(200);
        return true;
    }
    else
    {
        ESP_LOGI(TAG, "subscribeTopic FAIL\n");
        return false;
    }
    //sendATValue(sub, 25, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    //ready = true;
}

unsigned short MQTTProtocolNameLength;
const char *MQTTHost = "platform.agrum.co";
const char *MQTTPort = "1883";
const char *MQTTProtocolName = "MQTT"; //"MQIsdp";
const char MQTTFlags = 0x02;           //Clean Sesion

bool StartGPRSMQTTConnectionNew()
{
    ESP_LOGI(TAG, "\n-------Init StartGPRSMQTT ---------\n");
    if (sendCommandATToken("AT+CIPSTATUS\n", "OK", false))
    {

        char command[] = "AT+CIPSTART=\"TCP\",\"platform.agrum.co\",\"1883\"\n";
        if (sendCommandATToken(command, "OK", true))
        {
            vTaskDelay(500);
            int try
                = 3;
            while (!sendCommandATToken("AT+CIPSEND\n", "CONNECT OK", true))
            {
                if (try == 0)
                {
                    ESP_LOGI(TAG, "StartGPRSMQTTConnection ERROR CONNECT AT+CIPSEND");
                    return false;
                }
                try
                    --;
                vTaskDelay(100);
            }

            // vTaskDelay(500);
            // char command2[] = "AT+CIPSEND\n";
            // //sendATValue(command2, strlen(command2), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
            // int try
            //     = 3;
            // while (!sendCommandATToken(command2, ">", true))
            // {
            //     if (try == 0)
            //     {
            //         ESP_LOGI(TAG, "StartGPRSMQTTConnection ERROR AT+CIPSEND");
            //         return false;
            //     }
            //     try
            //         --;
            //     vTaskDelay(300);
            // }

            vTaskDelay(100); //No es necesario en la version i2c-uart

            char headMQTT[] = {0x10, 0x12, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0x02, 0x04, 0xB0, 0x00, 0x06, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x1A, 0x0D};
            // char headMQTT[21];

            // MQTTProtocolNameLength = strlen(MQTTProtocolName);

            // headMQTT[0] = 0x10; //Header connection
            // headMQTT[1] = 0x11; //Length remaning byte 3 to end

            // headMQTT[2] = 0x00; //(MQTTProtocolNameLength >> 8);   //Length of protocol NAME
            // headMQTT[3] = 0x04; // (MQTTProtocolNameLength & 0xFF); //Length of protocol NAME

            // headMQTT[4] = 0x4D; //M
            // headMQTT[5] = 0x51; //Q
            // headMQTT[6] = 0x54; //T
            // headMQTT[7] = 0x54; //T
            // headMQTT[8] = 0x04; //Version

            // headMQTT[9] = MQTTFlags; //Connect Flags

            // headMQTT[10] = 0x00; //Keep Alive
            // headMQTT[11] = 0x3C; //Keep Alive   60 Seconds

            // headMQTT[12] = 0x00; //Length Payload
            // headMQTT[13] = 0X05; //Length Payload

            // headMQTT[14] = 0x41; //A
            // headMQTT[15] = 0x47; //G
            // headMQTT[16] = 0x52; //R
            // headMQTT[17] = 0x55; //U
            // headMQTT[18] = 0x4d; //M

            // headMQTT[19] = 0x1A; //END
            // headMQTT[20] = 0x0D; //END

            // sendATValue(headMQTT, 22, true); //OPTIMIZAR
            // printf("Connected TCP\n");
            // vTaskDelay(200); //No es necesario en la version i2c-uart
            // return true;

            try
                = 5;
            while (!sendATValueResponse(headMQTT, 22, "SEND OK", true))
            {
                if (try == 0)
                {
                    ESP_LOGI(TAG, "ERROR Send headMQTT FAIL Connection TCP");
                    conGPRS = false;
                    return false;
                }
                try
                    --;
                vTaskDelay(100);
            }
            if (try != 0)
            {
                ESP_LOGI(TAG, "Connected TCP BROKER\n");
                conGPRS = true;
                vTaskDelay(100); //No es necesario en la version i2c-uart
                return true;
            }
            else
            {
                ESP_LOGI(TAG, "Fail Connected TCP BROKER\n");
                conGPRS = false;
                return false;
            }
        }
        else
        {
            ESP_LOGI(TAG, "ERROR AT+CIPSTART Connection TCP\n");
            return false;
        }
    }
    else
    {
        return false;
    }
}

void StartGPRSMQTTConnection()
{
    printf("Init StartGPRSMQTTConnection\n");
    if (sendCommandATSize("AT+CIPSTATUS\n", 10, true))
    {
        printf("IP OK StartGPRSMQTTConnection\n");
        char command[] = "AT+CIPSTART=\"TCP\",\"platform.agrum.co\",\"1883\"\n";
        sendCommandATSize(command, 3, true);
        vTaskDelay(400);
        char command2[] = "AT+CIPSEND\n";
        sendATValue(command2, strlen(command2)); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(300);                         //No es necesario en la version i2c-uart

        char headMQTT[] = {0x10, 0x12, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0x02, 0x00, 0x3C, 0x00, 0x06, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x1A, 0x0D};
        sendATValue(headMQTT, 22); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(200);           //No es necesario en la version i2c-uart
    }

    printf("Exit StartGPRSMQTTConnection\n");
}

bool PublishMqtt2GTask(char *topic, int TopicSize, char *data, int DataSize)
{
    ESP_LOGI(TAG, "--------Init Publish-------------\n");
    char command2[] = "AT+CIPSEND\n";
    //sendATValueNoRESPONSE(command2, strlen(command2), true);
    //vTaskDelay(100);

    int try
        = 2;
    while (1)
    {
        if (try == 0)
        {
            ESP_LOGI(TAG, "Publish FAIL NO >");
            return false;
        }

        if (sendCommandAT(command2, strlen(command2), ">", true))
        {
            break;
        }
    }

    vTaskDelay(300); //No es necesario en la version i2c-uart

    //30 13 00 08 76 61 6C 65 74 72 6F 6E 68 65 6C 6C 6F 72 61 76 69 1A ---Publish

    try
        = 2;
    while (1)
    {
        if (try == 0)
        {
            return false;
        }
        try
            --;
        char headPub[4];
        headPub[0] = 0x30;
        headPub[1] = (char)DataSize + TopicSize + 2;
        headPub[2] = (char)TopicSize >> 8;
        headPub[3] = (char)TopicSize; //- 1

        sendCommandAT(headPub, sizeof(headPub), "-", true);
        //sendATValueNoRESPONSE(headPub, sizeof(headPub), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(100); //No es necesario en la version i2c-uart

        sendCommandAT(topic, TopicSize, "-", true);
        //sendATValueNoRESPONSE(topic, TopicSize, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(100); //No es necesario en la version i2c-uart

        sendCommandAT(data, DataSize, "-", true);
        //sendATValueNoRESPONSE(data, DataSize, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(100); //No es necesario en la version i2c-uart

        char FinPub[2] = {0x1A, 0x0D};

        //sendATValueNoRESPONSE(FinPub, sizeof(FinPub), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        //return true;

        if (sendCommandAT(FinPub, 2, "SEND OK", true))
        {
            ESP_LOGI(TAG, "Publish OK");
            vTaskDelay(200);
            return true;
        }
        else
        {
            ESP_LOGI(TAG, "Publish FAIL PAYLOAD");
            //return false;
        }
    }
    //ready = true;						 ///solo valida que el dato ya se envio
}

bool PublishMqtt2G(char *topic, int TopicSize, char *data, int DataSize)
{
    ESP_LOGI(TAG, "--------Init Publish-------------\n");
    char command2[] = "AT+CIPSEND\n";
    //sendATValueNoRESPONSE(command2, strlen(command2), true);
    //vTaskDelay(100);
    int try
        = 3;
    while (!sendCommandATToken(command2, ">", true))
    {
        if (try == 0)
        {
            ESP_LOGI(TAG, "PublishMqtt2G ERROR AT+CIPSEND");
            return false;
        }
        try
            --;
        vTaskDelay(100);
    }
    vTaskDelay(100); //No es necesario en la version i2c-uart

    //30 13 00 08 76 61 6C 65 74 72 6F 6E 68 65 6C 6C 6F 72 61 76 69 1A ---Publish

    try
        = 2;
    while (1)
    {
        if (try == 0)
        {
            return false;
        }
        try
            --;
        char headPub[4];
        headPub[0] = 0x30;
        headPub[1] = (char)DataSize + TopicSize + 2;
        headPub[2] = (char)TopicSize >> 8;
        headPub[3] = (char)TopicSize; //- 1

        sendATValueNoRESPONSE(headPub, sizeof(headPub), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(100);                                       //No es necesario en la version i2c-uart

        sendATValueNoRESPONSE(topic, TopicSize, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(100);                               //No es necesario en la version i2c-uart

        sendATValueNoRESPONSE(data, DataSize, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        vTaskDelay(100);                             //No es necesario en la version i2c-uart

        char FinPub[2] = {0x1A, 0x0D};

        //sendATValueNoRESPONSE(FinPub, sizeof(FinPub), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
        //return true;

        if (sendATValueResponse(FinPub, 2, "SEND OK", true))
        {
            ESP_LOGI(TAG, "Publish OK");
            vTaskDelay(200);
            return true;
        }
        else
        {
            ESP_LOGI(TAG, "Publish FAIL");
            //return false;
        }
    }
    //ready = true;						 ///solo valida que el dato ya se envio
}