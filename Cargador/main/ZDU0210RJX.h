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

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 

/* address of the module */
#define ZDU0210RJX_address 0x58     //59
#define ZDU0210RJX_address1 0x50    //51

//EEPROM Command Detail
#define WriteEEPROMCurrentLocation 0x00
#define ReadEEPROMCurrentLocation 0x01
#define WriteEEPROMCurrentLocationRegister 0x02
#define ReadEEPROMCurrentLocationRegister 0x03
#define EraseEEPROM 0x04

//GPIO Command Detail
#define SettingGPIOOutRegister 0x06
#define ReadingGPIOInRegister 0x07
#define WriteGPIOConfiguration 0x08
#define ReadGPIOConfiguration 0x09
#define ReadGPIOInterruptStatusRegister 0x0F

//UART Command Detail
#define UART0_ReadStatusRegister 0x21
#define UART1_ReadStatusRegister 0x41

#define Uart0_EnableInterrupts 0x22
#define Uart1_EnableInterrupts 0x42

#define Uart0_InterruptStatusRegister 0x23
#define Uart1_InterruptStatusRegister 0x42

#define UART0_WriteDataTXFIFO 0x24
#define UART1_WriteDataTXFIFO 0x44

#define UART0_ReadDataRXFIFO 0x25 
#define UART1_ReadDataRXFIFO 0x45 

#define UART0_WriteBaudRateRegister 0x26
#define UART1_WriteBaudRateRegister 0x46

#define UART0_ReadActualBaudRateRegister 0x27
#define UART1_ReadActualBaudRateRegister 0x47

#define UART0_WriteConfiguration 0x28
#define UART1_WriteConfiguration 0x48

#define UART0_ReadConfiguration 0x29
#define UART1_ReadConfiguration 0x49

#define UART0_WriteTransmitWatermarkRegister 0x2A
#define UART1_WriteTransmitWatermarkRegister 0x4A

#define UART0_ReadTransmitWatermarkRegister 0x2B
#define UART1_ReadTransmitWatermarkRegister 0x4B

#define UART0_WriteReceiveWatermarkRegister 0x2C
#define UART1_WriteReceiveWatermarkRegister 0x4C

#define UART0_ReadReceiveWatermarkRegister 0X2D
#define UART1_ReadReceiveWatermarkRegister 0X4D

#define UART0_EnableUart 0x2E
#define UART1_EnableUart 0x4E

#define UART0_ReadReceiveTransmitFIFOLevelRegisters 0x31
#define UART1_ReadReceiveTransmitFIFOLevelRegisters 0x51

//System Command Detail

#define ReadSystemStatusRegister 0xE1
#define ReadLastOperationResultRegister 0xE3

#define ReadSystemVersion 0xE5
#define ReadInterruptSourceRegister 0xEF





void begin_ZDU0210RJX( uint8_t gpio, uint8_t mode);

bool Enable_Interrupts_ZDU0210RJX(uint8_t data, uint8_t uart);

bool i2c_gpio_write_ZDU0210RJX(uint8_t gpio, uint8_t state);
uint8_t i2c_gpio_read_ZDU0210RJX();

bool Read_UART_STATUS_REGISTER_ZDU0210RJX(uint8_t uart, uint8_t *dataRead);
bool Interrupt_Status_Register_ZDU0210RJX(uint8_t uart, uint8_t *dataRead);

bool Write_Data_TX_FIFO_ZDU0210RJX(uint8_t data, uint8_t uart);
bool Write_Multiple_Data_TX_FIFO_ZDU0210RJX(uint8_t *data, int count, uint8_t uart);
bool Write_Read_Data_TX_FIFO_ZDU0210RJX(uint8_t *dataWrite, int countWrite, uint8_t *dataRead, int countRead , uint8_t uart);

uint8_t Read_Data_RX_FIFO_ZDU0210RJX(uint8_t uart, uint8_t *data, int count);

bool Write_Baud_Rate_Register_ZDU0210RJX(uint8_t *data, uint8_t uart);
uint8_t Read_Actual_baud_Rate_ZDU0210RJX(uint8_t *data, uint8_t uart);
bool Write_Configuration_ZDU0210RJX(uint8_t *data, uint8_t uart);
uint8_t Read_Configuration_ZDU0210RJX(uint8_t *data, uint8_t uart);
bool Write_Transmit_Watermark_Register_ZDU0210RJX(uint8_t *data, uint8_t uart);
uint8_t Read_Transmit_Watermark_Register_ZDU0210RJX(uint8_t uart);
bool Write_Receive_Watermark_Register_ZDU0210RJX(uint8_t *data, uint8_t uart);
uint8_t Read_Receive_Watermark_Register_ZDU0210RJX(uint8_t uart);

bool Enable_Uart_ZDU0210RJX(uint8_t data, uint8_t uart);

uint8_t Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(uint8_t uart, uint8_t RxTx);
bool uart_reset_FIFO_ZDU0210RJX(uint8_t uart);