//https://bitbucket.org/mbari_peter/sim800-mqtt-ravi/src/master/
//https://openlabpro.com/guide/mqtt-packet-format/
//http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html

#include "Parameters.h"
#include "Sim800L.h"
#include "uart_lib.h"
#include "ZDU0210RJX.h"
#include "Functions.h"
#include <esp_log.h>

static const char *TAG = "SIM800";

bool sendCommandATToken(char *command, char *responsecommand, bool debug)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;
    if (debug)
    {
        printf("\nCommand to sendCommandATToken: %s\n", command);
    }
    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, strlen(command), 0); //SEND ALL DATA
    //Write_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address,u8ModbusADU[i], 0); //SEND TO BYTE TO BYTE
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

        // printf("RX size AT: %d \n", rx_size);
        Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

        char *ReadValue = substring((char *)dataRxGPRS, 3, strlen(responsecommand)); //(rx_size - 4)

        if (debug)
        {
            printf("Response AT Hex:");
            for (int i = 0; i < rx_size; i++)
                printf("%x", dataRxGPRS[i]);
            printf("\n");
            printf("Response AT ASCII: %s\n", ReadValue);
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
            printf("NO Response sendCommandATToken AT ERROR\n\n");
        }
        return false;
    }
}

bool sendCommandATSize(char *command, int MinimunSize, bool debug)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;
    if (debug)
    {
        printf("\nCommand to sendCommandATSize: %s\n", command);
    }

    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)command, strlen(command), 0);
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
        //printf("RX size AT: %d \n", rx_size);
        Read_Data_RX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, 0, dataRxGPRS, rx_size);

        char *ReadValue = substring((char *)dataRxGPRS, 3, (rx_size - 4));

        if (debug)
        {
            printf("Response AT Hex:");
            for (int i = 0; i < rx_size; i++)
                printf("%x", dataRxGPRS[i]);
            printf("\n");
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

void sendATValue(char *value, int sizeCommand, bool debug)
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;

    if (debug)
    {
        printf("\nCommand to sendATValue: %s\n", value);
    }

    Write_Multiple_Data_TX_FIFO_ZDU0210RJX(ZDU0210RJX_address2G, (uint8_t *)value, sizeCommand, 0);

    vTaskDelay(50);
     rx_size = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(ZDU0210RJX_address2G, 0, 0);
    int try
        = 3;

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

        if (debug)
        {
            printf("Response MQTT Hex:");
            for (int i = 0; i < rx_size; i++)
                printf("%x", dataRxGPRS[i]);
            printf("\n");
            printf("Response MQTT ASCII: %s\n", ReadValue);
        }
        printf("\n\n");
        memset(dataRxGPRS, 0, 64);
    }
    else
    {
        if (debug)
        {
            printf("NO DATA sendATValue\n\n");
        }
    } 
}

void readDataMQTT2G()
{
    uint8_t dataRxGPRS[64];
    uint8_t rx_size = 0;
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

        printf("Response MQTT Hex:");
        for (int i = 0; i < rx_size; i++)
            printf("%x", dataRxGPRS[i]);
        printf("\n");
        printf("Response MQTT ASCII: %s\n", ReadValue);
        memset(dataRxGPRS, 0, 64);
    }
    else
    {
        printf("NO DATA MQTT\n");
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
    sendCommandATToken(CommandToSendSim, "OK", false);
    char CommandToSendSim0[] = "AT+CSQ\n"; //Calidad de señal
    sendCommandATToken(CommandToSendSim0, "OK", true);
    char CommandToSendSim1[] = "AT+CREG?\n"; //3 GSM con EGPRS
    sendCommandATToken(CommandToSendSim1, "OK", false);

    char CommandToSendSim2[] = "AT+CGATT?\n"; //Estado GPRS
    sendCommandATToken(CommandToSendSim2, "OK", false);
    char CommandToSendSim3[] = "AT+CIPM,UX?\n";
    sendCommandATToken(CommandToSendSim3, "OK", false);
    char CommandToSendSim4[] = "AT+CCID=?\n";
    sendCommandATToken(CommandToSendSim4, "OK", false);

    char CommandToSend[] = "AT+CFUN=1\n";
    sendCommandATToken(CommandToSend, "OK", false);
    char CommandToSend1[] = "AT+CREG=1\n";
    sendCommandATToken(CommandToSend1, "OK", false);

    char CommandToSend2[] = "AT+CIPMUX=0\n"; // Selects Single-connection mode
    if (sendCommandATToken(CommandToSend2, "OK", true))
    {
        if (sendCommandATToken("AT+CIPRXGET=1\n", "OK", true)) //RECEIVE DATA manually FROM THE REMOTE SERVER
        {
            // sendCommandATToken("AT+CIPMODE=0\n", "OK", true);
            // sendCommandATToken("AT+CIPSRIP=0\n", "OK", true);

            while (!sendCommandATToken("AT+CGATT?\n", "+CGATT: 1", true))
            {
                if (try == 0)
                {
                    printf("ERROR AT+CGATT NO CONNECTION");
                    return false;
                }
                try
                    --;
                vTaskDelay(500);
            }
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
                        printf("ERROR BRINGING UP WIRELESS CONNECTION");
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
                        printf("ERROR BRINGING UP WIRELESS CONNECTION");
                        return false;
                    }
                    try
                        --;
                    vTaskDelay(500);
                }
                vTaskDelay(1000);
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
                        printf("ERROR BRINGING UP WIRELESS CONNECTION");
                        return false;
                    }
                    try
                        --;
                    vTaskDelay(500);
                }
                vTaskDelay(1000);
                gpio_write_ZDU0210RJX(ZDU0210RJX_address2G, 0x40, 0x40);
                return true;
            }
            else
            {
                printf("Error setting the APN\n");
                return false;
            }
        }
        else
        {
            printf("Error setting CIPRXGET");
            return false;
        }
    }
    else
    {
        printf("Error Send AT+CIPMUX=0 Single connection IP \n");
        return false;
    }
}

void subscribeTopic()
{
    printf("Init subscribeTopic\n");
    char command2[] = "AT+CIPSEND\n";
    sendATValue(command2, strlen(command2), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(300);                               //No es necesario en la version i2c-uart

    //char sub[] = {0x82, 0x14, 0x00, 0x02, 0x00, 0x10, 0x61, 0x69, 0x72, 0x69, 0x73, 0x2F, 0x63, 0x63, 0x2F, 0x63, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64,0X00, 0x1A, 0x0D};
    char sub[25];
    sub[0] = 0x82; //Header subscribeTopic
    sub[1] = 0x14; //Length remaning byte 3 to end

    sub[2] = 0x00; //PACKET ID
    sub[3] = 0x02; //PACKET ID

    sub[4] = 0x00; //Length PAYLOAD
    sub[5] = 0x10; //Length PAYLOAD

    sub[6] = 0x61;  //a
    sub[7] = 0x69;  //i
    sub[8] = 0x72;  //r
    sub[9] = 0x69;  //i
    sub[10] = 0x73; //s
    sub[11] = 0x2F; ///
    sub[12] = 0x63; //c
    sub[13] = 0x63; //c
    sub[14] = 0x2F; ///
    sub[15] = 0x63; //c
    sub[16] = 0x6F; //o
    sub[17] = 0x6D; //m
    sub[18] = 0x6D; //m
    sub[19] = 0x61; //a
    sub[20] = 0x6E; //n
    sub[21] = 0x64; //d

    sub[22] = 0X00; //QOS
    sub[23] = 0x1A; //END
    sub[24] = 0x0D; //END

    sendATValue(sub, 25, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(100);            //No es necesario en la version i2c-uart
    printf("Exit subscribeTopic\n");
    //ready = true;
}

unsigned short MQTTProtocolNameLength;
const char *MQTTHost = "platform.agrum.co";
const char *MQTTPort = "1883";
const char *MQTTProtocolName = "MQTT"; //"MQIsdp";
const char MQTTFlags = 0x02;           //Clean Sesion

bool StartGPRSMQTTConnection()
{
    printf("\nInit StartGPRSMQTTConnection\n");
    if (sendCommandATToken("AT+CIPSTATUS\n", "OK", true))
    {
        printf("Opening TCP StartGPRSMQTTConnection\n");
        char command[100]; // = "AT+CIPSTART=\"TCP\",\"platform.agrum.co\",\"1883\"\n";
        snprintf(command, sizeof(command), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\n", MQTTHost, MQTTPort);
        if (sendCommandATToken(command, "OK", true))
        {
            vTaskDelay(500);
            char command2[] = "AT+CIPSEND\n";
            sendATValue(command2, strlen(command2), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
            vTaskDelay(500);                               //No es necesario en la version i2c-uart

            char headMQTT[] = {0x10, 0x12, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0x02, 0x00, 0x3C, 0x00, 0x06, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x1A, 0x0D};
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

            sendATValue(headMQTT, 22, true);
            printf("Connected TCP\n");
            vTaskDelay(2000); //No es necesario en la version i2c-uart
            return true;
        }
        else
        {
            printf("ERROR AT+CIPSTART Connection TCP\n");
            return false;
        }
    }
    else
    {
        return false;
    }
}

void PublishMqtt2G(char *topic, int TopicSize, char *data, int DataSize)
{
    char command2[] = "AT+CIPSEND\n";
    sendATValue(command2, strlen(command2), true);
    vTaskDelay(200); //No es necesario en la version i2c-uart

    //30 13 00 08 76 61 6C 65 74 72 6F 6E 68 65 6C 6C 6F 72 61 76 69 1A ---Publish
    char headPub[4];
    headPub[0] = 0x30;
    headPub[1] = (char)DataSize + TopicSize + 2;
    headPub[2] = (char)TopicSize >> 8;
    headPub[3] = (char)TopicSize & 0xFF; //- 1

    sendATValue(headPub, sizeof(headPub), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(100);                             //No es necesario en la version i2c-uart

    sendATValue(topic, TopicSize, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(100);                     //No es necesario en la version i2c-uart

    sendATValue(data, DataSize, true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    vTaskDelay(100);                   //No es necesario en la version i2c-uart

    char FinPub[2] = {0x1A, 0x0D};
    sendATValue(FinPub, sizeof(FinPub), true); //CAMBIAR POR LA INSTRUCCION ANTERIOR DE SEND AT
    //ready = true;						 ///solo valida que el dato ya se envio
}