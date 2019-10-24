/****************************************************************************/
//  Function: file for ZDU0210RJX
//  Hardware: AIRIS - RTC
//  ESP32 MCU: ESPRESSIF
//  Author:   Diego Ochoa
//  Date:    July 24,2019
//  Version: v1.0
//  by Agrum.SAS
/****************************************************************************/
//https://www.digikey.com/product-detail/en/zilog/ZDU0210RJX/269-5021-ND/5878591

//UART0 -> 485 Modbus
//UART1 -> GPRS ?

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ZDU0210RJX.h"
#include "software_i2c.h"

#include "esp_log.h"

static const char *TAG = "ZDU0210RJX";

uint8_t UART0_ConfigurationData[2] = {0x86, 0x30};
uint8_t UART1_ConfigurationData[2] = {0x86, 0x30};

uint8_t UART0_BaudrateData[2] = {0x00, 0x60}; //0x00, 0x60- 9600
uint8_t UART1_BaudrateData[2] = {0x00, 0x60}; //0x04, 0x57- 115200

uint8_t EnableUartData = 0x03; // Rx Tx Enable

uint8_t UART_RESET_FIFO[2] = {0x06, 0x03}; //0x04, 0x57- 115200



void begin_ZDU0210RJX(uint8_t gpio, uint8_t mode)
{

    uint8_t _gpio = gpio; //pin select
    uint8_t _mode = mode; //Input or ouput

    //i2c_gpio_configuration(I2C_NUM_0, ZDU0210RJX_address, _gpio, _mode);

    Write_Configuration_ZDU0210RJX(UART0_ConfigurationData, 0);
    uint8_t  confUart0[2];
    Read_Configuration_ZDU0210RJX(confUart0,0);
    int value = confUart0[0] << 8 | confUart0[1];
    printf("Configuration UART0: %d\n", value);

    Write_Configuration_ZDU0210RJX(UART1_ConfigurationData, 1);
    uint8_t  confUart1[2];
    Read_Configuration_ZDU0210RJX(confUart1,1);
    value = confUart1[0] << 8 | confUart1[1];
    printf("Configuration UART1: %d\n", value);

    Write_Baud_Rate_Register_ZDU0210RJX(UART0_BaudrateData, 0);
    uint8_t  baudUart0[2];
    Read_Actual_baud_Rate_ZDU0210RJX(baudUart0,0);
    value = baudUart0[0] << 8 | baudUart0[1];
    printf("Baudrate UART0: %d\n", value);

    Write_Baud_Rate_Register_ZDU0210RJX(UART1_BaudrateData, 1);
    uint8_t  baudUart1[2];
    Read_Actual_baud_Rate_ZDU0210RJX(baudUart1,0);
    value = baudUart1[0] << 8 | baudUart1[1];
    printf("Baudrate UART0: %d\n", value);

    Enable_Uart_ZDU0210RJX(EnableUartData, 0);
    Enable_Uart_ZDU0210RJX(EnableUartData, 1);

    Enable_Interrupts_ZDU0210RJX(0xFF, 1);

    ESP_LOGI(TAG, "begin_ZDU0210RJX OK");
}

//FUNCTION GPIO Command Detail

bool gpio_write_ZDU0210RJX(uint8_t gpio, uint8_t state)
{
    return i2c_gpio_write(ZDU0210RJX_address, gpio, state);
}

uint8_t gpio_read_ZDU0210RJX()
{
    return i2c_gpio_read(ZDU0210RJX_address);
}

//FUNCTION UART Command Detail

bool Read_UART_STATUS_REGISTER_ZDU0210RJX(uint8_t uart, uint8_t *dataRead)
{
    
    if (uart == 0)
        uart = UART0_ReadStatusRegister;
    else
        uart = UART1_ReadStatusRegister;
    return i2c_uart_read_ZDU0210RJX(ZDU0210RJX_address, &dataRead, 1, uart);
}

bool Enable_Interrupts_ZDU0210RJX(uint8_t data, uint8_t uart)
{
    if (uart == 0)
        uart = Uart0_EnableInterrupts;
    else
        uart = Uart1_EnableInterrupts;

    return i2c_uart_enable_ZDU0210RJX(ZDU0210RJX_address, data, uart);
}

bool Interrupt_Status_Register_ZDU0210RJX(uint8_t uart, uint8_t *dataRead)
{
    if (uart == 0)
        uart = Uart0_InterruptStatusRegister;
    else
        uart = Uart1_InterruptStatusRegister;
    return i2c_uart_read_ZDU0210RJX(ZDU0210RJX_address, &dataRead, 1, uart);
}

/*
Writes all bytes received to TX FIFO until the packet is terminated or TX Buffer is full.
I2C terminates by Stop condition. If the buffer is full, the I2C transmits a Negative
Acknowledgement (NAK) for any byte sent until the buffer has been emptied enough to
take more data. Any byte that has been NAK’d is not added to the buffer.
 */
bool Write_Data_TX_FIFO_ZDU0210RJX(uint8_t data, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteDataTXFIFO;
    else
        uart = UART1_WriteDataTXFIFO;

    //printf("Write_Data_TX_FIFO_ZDU0210RJX: %x \n", data);
    return i2c_uart_write_ZDU0210RJX_8(ZDU0210RJX_address, data, uart);
}

bool Write_Multiple_Data_TX_FIFO_ZDU0210RJX(uint8_t *data, int count, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteDataTXFIFO;
    else
        uart = UART1_WriteDataTXFIFO;

    //printf("Write_Data_TX_FIFO_ZDU0210RJX: %x \n", data);
    return i2c_uart_write_ZDU0210RJX(ZDU0210RJX_address, data, count, uart);
}

bool Write_Read_Data_TX_FIFO_ZDU0210RJX(uint8_t *dataWrite, int countWrite, uint8_t *dataRead, int countRead, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteDataTXFIFO;
    else
        uart = UART1_WriteDataTXFIFO;

    return i2c_uart_write_read_ZDU0210RJX(ZDU0210RJX_address, dataWrite, countWrite, uart, dataRead, countRead);
}

/*
Reads bytes from the RX FIFO until either a Stop Condition or a NAK is received. If the
reads exceed the number of bytes in the FIFO, the returning value will be 0x00.
 */
uint8_t Read_Data_RX_FIFO_ZDU0210RJX(uint8_t uart, uint8_t *data, int count)
{

    if (uart == 0)
        uart = UART0_ReadDataRXFIFO;
    else
        uart = UART1_ReadDataRXFIFO;
    
    return i2c_uart_read_ZDU0210RJX(ZDU0210RJX_address, data, count, uart);
}

/*
Sets the baud rate for the requested UART, in multiples of 100.
 */
bool Write_Baud_Rate_Register_ZDU0210RJX(uint8_t *data, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteBaudRateRegister;
    else
        uart = UART1_WriteBaudRateRegister;

    return i2c_uart_write_ZDU0210RJX(ZDU0210RJX_address, data, sizeof(UART0_BaudrateData), uart);
}

/*
Reads the actual baud rate in the baud rate register (in multiple of 100s).
 */
uint8_t Read_Actual_baud_Rate_ZDU0210RJX(uint8_t *data, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_ReadActualBaudRateRegister;
    else
        uart = UART1_ReadActualBaudRateRegister;

    return i2c_uart_read_16_ZDU0210RJX(ZDU0210RJX_address, data, uart);
}

/*
The configuration for the UART can either be set with all parameters in one command or
individual parameters, by the use of sub commands. The first byte, bit 7, determines if this
is a sub command request or data (configuring all parameters). If the bit 7 is cleared, the
data is a sub command; otherwise, the data is all configuration parameters. The sub commands also provides the ability to reset the FIFO buffers and a soft reset of the UART to
return to default settings.
 */
bool Write_Configuration_ZDU0210RJX(uint8_t *data, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteConfiguration;
    else
        uart = UART1_WriteConfiguration;

    return i2c_uart_write_ZDU0210RJX(ZDU0210RJX_address, data, sizeof(UART0_ConfigurationData), uart);
}

/*
Reads the current configuration of the UART
 */
uint8_t Read_Configuration_ZDU0210RJX(uint8_t *data, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_ReadConfiguration;
    else
        uart = UART1_ReadConfiguration;

    return i2c_uart_read_16_ZDU0210RJX(ZDU0210RJX_address, data, uart);
}

/*
Writes the Transmit Watermark Register.
Values between 1 and 64 are acceptable. Sending a 0x00 will keep the TX Watermark
Interrupt always on and any value greater than 64 will not ever set the Interrupt. The
default value is 63.
 */
bool Write_Transmit_Watermark_Register_ZDU0210RJX(uint8_t *data, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteTransmitWatermarkRegister;
    else
        uart = UART1_WriteTransmitWatermarkRegister;

    return i2c_uart_write_ZDU0210RJX(ZDU0210RJX_address, data, sizeof(data), uart);
}

/*
Reads the Transmit Watermark Register.
 */
uint8_t Read_Transmit_Watermark_Register_ZDU0210RJX(uint8_t uart)
{
    uint8_t data = 0;
    if (uart == 0)
        uart = UART0_ReadTransmitWatermarkRegister;
    else
        uart = UART1_ReadTransmitWatermarkRegister;

    i2c_uart_read_16_ZDU0210RJX(ZDU0210RJX_address, &data, uart);
    return data;
}

bool Write_Receive_Watermark_Register_ZDU0210RJX(uint8_t *data, uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteReceiveWatermarkRegister;
    else
        uart = UART1_WriteReceiveWatermarkRegister;

    return i2c_uart_write_ZDU0210RJX(ZDU0210RJX_address, data, sizeof(data), uart);
}

/*
Reads the Receive Watermark Register.
 */
uint8_t Read_Receive_Watermark_Register_ZDU0210RJX(uint8_t uart)
{
    uint8_t data = 0;
    if (uart == 0)
        uart = UART0_ReadReceiveWatermarkRegister;
    else
        uart = UART1_ReadReceiveWatermarkRegister;

    i2c_uart_read_16_ZDU0210RJX(ZDU0210RJX_address, &data, uart);
    return data;
}

/*
Enables the UART for receiving and/or transmitting.
 */
bool Enable_Uart_ZDU0210RJX(uint8_t data, uint8_t uart)
{
    //printf("enter Enable uart");
    if (uart == 0)
    {
        uart = UART0_EnableUart;
    }
    else
    {
        uart = UART1_EnableUart;
    }
    return i2c_uart_enable_ZDU0210RJX(ZDU0210RJX_address, data, uart);
}

/*
Retrieves the current FIFO level for the Receive and Transmit FIFOs.
The maximum value is 64 and the minimum value is 0.
RxTx= 0; Rx FIFO
RxTx= 1; Tx FIFO
 */
uint8_t Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(uint8_t uart, uint8_t RxTx)
{
    uint8_t dataout = 0;
    if (uart == 0)
        uart = UART0_ReadReceiveTransmitFIFOLevelRegisters;
    else
        uart = UART1_ReadReceiveTransmitFIFOLevelRegisters;

    uint8_t data[2];
    i2c_uart_read_16_ZDU0210RJX(ZDU0210RJX_address, data, uart);

    if (RxTx == 0)
        dataout = data[0];
    else
        dataout = data[1];

    return dataout;
}

bool uart_reset_FIFO_ZDU0210RJX(uint8_t uart)
{
    if (uart == 0)
        uart = UART0_WriteConfiguration;
    else
        uart = UART1_WriteConfiguration;

    return i2c_uart_write_ZDU0210RJX(ZDU0210RJX_address, UART_RESET_FIFO, sizeof(UART_RESET_FIFO), uart);
}

//---------------------------------