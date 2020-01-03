#include <string.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ZDU0210RJX.h"
#include "modbusMaster.h"

#include "esp_log.h"

//https://github.com/4-20ma/ModbusMaster/tree/master/src
//https://www.modbustools.com/modbus.html#function06

static const char *TAG = "modbusMaster";

uint8_t ku8MaxBufferSize = 64; ///< size of response/transmit buffers

uint8_t u8ModbusADUSizeTx = 0;
uint8_t u8ModbusADUSizeRx = 0;

uint8_t ku8MBIllegalFunction = 0x01;
uint8_t ku8MBIllegalDataAddress = 0x02;
uint8_t ku8MBIllegalDataValue = 0x03;
uint8_t ku8MBSlaveDeviceFailure = 0x04;
uint8_t ku8MBSuccess = 0x00;
uint8_t ku8MBInvalidSlaveID = 0xE0;
uint8_t ku8MBInvalidFunction = 0xE1;
uint8_t ku8MBResponseTimedOut = 0xE2;
uint8_t ku8MBInvalidCRC = 0xE3;

uint8_t ku8MBReadCoils = 0x01;
uint8_t ku8MBReadDiscreteInputs = 0x02;   ///< Modbus function 0x02 Read Discrete Inputs
uint8_t ku8MBReadHoldingRegisters = 0x03; ///< Modbus function 0x03 Read Holding Registers
uint8_t ku8MBReadInputRegisters = 0x04;   ///< Modbus function 0x04 Read Input Registers
uint8_t ku8MBWriteSingleCoil = 0x05;      ///< Modbus function 0x05 Write Single Coil
uint8_t ku8MBWriteSingleRegister = 0x06;  ///< Modbus function 0x06 Write Single Register

uint8_t ku8MBWriteMultipleCoils = 0x0F;         ///< Modbus function 0x0F Write Multiple Coils
uint8_t ku8MBWriteMultipleRegisters = 0x10;     ///< Modbus function 0x10 Write Multiple Registers
uint8_t ku8MBMaskWriteRegister = 0x16;          ///< Modbus function 0x16 Mask Write Register
uint8_t ku8MBReadWriteMultipleRegisters = 0x17; ///< Modbus function 0x17 Read Write Multiple Registers
uint16_t ku16MBResponseTimeout = 2000;          ///< Modbus timeout [milliseconds]

/**
Initialize class object.
Assigns the Modbus slave ID and serial port.
Call once class has been instantiated, typically within setup().
@param slave Modbus slave ID (1..255)
@param &serial reference to serial port object (Serial, Serial1, ... Serial3)
@ingroup setup
*/

void begin_modbusMaster(uint8_t slave)
{
    //xTaskCreate(modbusResponse_task, "modbusResponse_task", 2048, NULL, 5, NULL);
    _u8MBSlave = slave;
    _u8TransmitBufferIndex = 0;
    u16TransmitBufferLength = 0;
    ESP_LOGI(TAG, "begin_modbusMaster OK");
}

void modbusResponse_task(void *arg)
{
    ESP_LOGI(TAG, "Initiation modbusResponse_task");
    //uint8_t u8ModbusADUSize = 0;

    // for (;;)
    // {
    //     //u8ModbusADUSize = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(0, 0);
    //     if (u8ModbusADUSize > 0)
    //     {

    //         //responseModbusTask(uint8_t u8MBFunction, uint8_t *data_rd, bool coil, uint8_t u8ModbusADUSize)
    //     }
    // }
}
/**
Modbus function 0x01 Read Coils.
This function code is used to read from 1 to 2000 contiguous status of 
coils in a remote device. The request specifies the starting address, 
i.e. the address of the first coil specified, and the number of coils. 
Coils are addressed starting at zero.
The coils in the response buffer are packed as one coil per bit of the 
data field. Status is indicated as 1=ON and 0=OFF. The LSB of the first 
data word contains the output addressed in the query. The other coils 
follow toward the high order end of this word and from low order to high 
order in subsequent words.
If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).
@param u16ReadAddress address of first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t readCoils(uint16_t u16ReadAddress, uint16_t u16BitQty)
{
    _u16ReadAddress = u16ReadAddress;
    _u16ReadQty = u16BitQty;
    return ModbusMasterTransaction(ku8MBReadCoils);
}

/**
Modbus function 0x02 Read Discrete Inputs.
This function code is used to read from 1 to 2000 contiguous status of 
discrete inputs in a remote device. The request specifies the starting 
address, i.e. the address of the first input specified, and the number 
of inputs. Discrete inputs are addressed starting at zero.
The discrete inputs in the response buffer are packed as one input per 
bit of the data field. Status is indicated as 1=ON; 0=OFF. The LSB of 
the first data word contains the input addressed in the query. The other 
inputs follow toward the high order end of this word, and from low order 
to high order in subsequent words.
If the returned quantity is not a multiple of sixteen, the remaining 
bits in the final data word will be padded with zeros (toward the high 
order end of the word).
@param u16ReadAddress address of first discrete input (0x0000..0xFFFF)
@param u16BitQty quantity of discrete inputs to read (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t readDiscreteInputs(uint16_t u16ReadAddress, uint16_t u16BitQty)
{
    _u16ReadAddress = u16ReadAddress;
    _u16ReadQty = u16BitQty;
    return ModbusMasterTransaction(ku8MBReadDiscreteInputs);
}

/**
Modbus function 0x03 Read Holding Registers.
This function code is used to read the contents of a contiguous block of 
holding registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.
The register data in the response buffer is packed as one word per 
register.
@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t readHoldingRegisters(uint16_t u16ReadAddress, uint16_t u16ReadQty)
{
    _u16ReadAddress = u16ReadAddress;
    _u16ReadQty = u16ReadQty;
    return ModbusMasterTransaction(ku8MBReadHoldingRegisters);
}

/**
Modbus function 0x04 Read Input Registers.
This function code is used to read from 1 to 125 contiguous input 
registers in a remote device. The request specifies the starting 
register address and the number of registers. Registers are addressed 
starting at zero.
The register data in the response buffer is packed as one word per 
register.
@param u16ReadAddress address of the first input register (0x0000..0xFFFF)
@param u16ReadQty quantity of input registers to read (1..125, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t readInputRegisters(uint16_t u16ReadAddress, uint8_t u16ReadQty)
{
    _u16ReadAddress = u16ReadAddress;
    _u16ReadQty = u16ReadQty;
    return ModbusMasterTransaction(ku8MBReadInputRegisters);
}

/**
Modbus function 0x05 Write Single Coil.
This function code is used to write a single output to either ON or OFF 
in a remote device. The requested ON/OFF state is specified by a 
constant in the state field. A non-zero value requests the output to be 
ON and a value of 0 requests it to be OFF. The request specifies the 
address of the coil to be forced. Coils are addressed starting at zero.
@param u16WriteAddress address of the coil (0x0000..0xFFFF)
@param u8State 0=OFF, non-zero=ON (0x00..0xFF)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t writeSingleCoil(uint16_t u16WriteAddress, uint8_t u8State)
{
    _u16WriteAddress = u16WriteAddress;
    _u16WriteQty = (u8State ? 0xFF00 : 0x0000);
    return ModbusMasterTransaction(ku8MBWriteSingleCoil);
}

/**
Modbus function 0x06 Write Single Register.
This function code is used to write a single holding register in a 
remote device. The request specifies the address of the register to be 
written. Registers are addressed starting at zero.
@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteValue value to be written to holding register (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t writeSingleRegister(uint16_t u16WriteAddress, uint16_t u16WriteValue)
{
    _u16WriteAddress = u16WriteAddress;
    _u16WriteQty = 0;
    _u16TransmitBuffer[0] = u16WriteValue;
    return ModbusMasterTransaction(ku8MBWriteSingleRegister);
}

/**
Modbus function 0x0F Write Multiple Coils.
This function code is used to force each coil in a sequence of coils to 
either ON or OFF in a remote device. The request specifies the coil 
references to be forced. Coils are addressed starting at zero.
The requested ON/OFF states are specified by contents of the transmit 
buffer. A logical '1' in a bit position of the buffer requests the 
corresponding output to be ON. A logical '0' requests it to be OFF.
@param u16WriteAddress address of the first coil (0x0000..0xFFFF)
@param u16BitQty quantity of coils to write (1..2000, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup discrete
*/
uint8_t writeMultipleCoilsAdd(uint16_t u16WriteAddress, uint16_t u16BitQty)
{
    _u16WriteAddress = u16WriteAddress;
    _u16WriteQty = u16BitQty;
    return ModbusMasterTransaction(ku8MBWriteMultipleCoils);
}

uint8_t writeMultipleCoils()
{
    _u16WriteQty = u16TransmitBufferLength;
    return ModbusMasterTransaction(ku8MBWriteMultipleCoils);
}

/**
Modbus function 0x10 Write Multiple Registers.
This function code is used to write a block of contiguous registers (1 
to 123 registers) in a remote device.
The requested written values are specified in the transmit buffer. Data 
is packed as one word per register.
@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..123, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t writeMultipleRegistersAdd(uint16_t u16WriteAddress, uint16_t u16WriteQty)
{
    _u16WriteAddress = u16WriteAddress;
    _u16WriteQty = u16WriteQty;
    return ModbusMasterTransaction(ku8MBWriteMultipleRegisters);
}

// new version based on Wire.h
uint8_t writeMultipleRegisters()
{
    _u16WriteQty = _u8TransmitBufferIndex;
    return ModbusMasterTransaction(ku8MBWriteMultipleRegisters);
}

/**
Modbus function 0x16 Mask Write Register.
This function code is used to modify the contents of a specified holding 
register using a combination of an AND mask, an OR mask, and the 
register's current contents. The function can be used to set or clear 
individual bits in the register.
The request specifies the holding register to be written, the data to be 
used as the AND mask, and the data to be used as the OR mask. Registers 
are addressed starting at zero.
The function's algorithm is:
Result = (Current Contents && And_Mask) || (Or_Mask && (~And_Mask))
@param u16WriteAddress address of the holding register (0x0000..0xFFFF)
@param u16AndMask AND mask (0x0000..0xFFFF)
@param u16OrMask OR mask (0x0000..0xFFFF)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t maskWriteRegister(uint16_t u16WriteAddress, uint16_t u16AndMask, uint16_t u16OrMask)
{
    _u16WriteAddress = u16WriteAddress;
    _u16TransmitBuffer[0] = u16AndMask;
    _u16TransmitBuffer[1] = u16OrMask;
    return ModbusMasterTransaction(ku8MBMaskWriteRegister);
}

/**
Modbus function 0x17 Read Write Multiple Registers.
This function code performs a combination of one read operation and one 
write operation in a single MODBUS transaction. The write operation is 
performed before the read. Holding registers are addressed starting at 
zero.
The request specifies the starting address and number of holding 
registers to be read as well as the starting address, and the number of 
holding registers. The data to be written is specified in the transmit 
buffer.
@param u16ReadAddress address of the first holding register (0x0000..0xFFFF)
@param u16ReadQty quantity of holding registers to read (1..125, enforced by remote device)
@param u16WriteAddress address of the first holding register (0x0000..0xFFFF)
@param u16WriteQty quantity of holding registers to write (1..121, enforced by remote device)
@return 0 on success; exception number on failure
@ingroup register
*/
uint8_t readWriteMultipleRegistersAdd(uint16_t u16ReadAddress, uint16_t u16ReadQty, uint16_t u16WriteAddress, uint16_t u16WriteQty)
{
    _u16ReadAddress = u16ReadAddress;
    _u16ReadQty = u16ReadQty;
    _u16WriteAddress = u16WriteAddress;
    _u16WriteQty = u16WriteQty;
    return ModbusMasterTransaction(ku8MBReadWriteMultipleRegisters);
}

uint8_t readWriteMultipleRegisters(uint16_t u16ReadAddress, uint16_t u16ReadQty)
{
    _u16ReadAddress = u16ReadAddress;
    _u16ReadQty = u16ReadQty;
    _u16WriteQty = _u8TransmitBufferIndex;
    return ModbusMasterTransaction(ku8MBReadWriteMultipleRegisters);
}

uint16_t crc16_update(uint16_t crc, uint8_t a)
{
    int i;

    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    return crc;
}

/* _________________________________________________________ */
/**
Modbus transaction engine.
Sequence:
  - assemble Modbus Request Application Data Unit (ADU),
    based on particular function called
  - transmit request over selected serial port
  - wait for/retrieve response
  - evaluate/disassemble response
  - return status (success/exception)
@param u8MBFunction Modbus function (0x01..0xFF)
@return 0 on success; exception number on failure
*/

uint8_t ModbusMasterTransaction(uint8_t u8MBFunction)
{
    vTaskDelay(100);
    uint8_t u8ModbusADU[64];
    uint8_t u8ModbusADUSize = 0;
    uint8_t i;
    uint16_t u16CRC;

    uint8_t u8MBStatus = ku8MBSuccess;

    // assemble Modbus Request Application Data Unit
    u8ModbusADU[u8ModbusADUSize++] = _u8MBSlave;
    u8ModbusADU[u8ModbusADUSize++] = u8MBFunction;

    //Add address to Read
    if (u8MBFunction == ku8MBReadCoils)
    {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
    }
    else if (u8MBFunction == ku8MBReadDiscreteInputs)
    {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
    }
    else if (u8MBFunction == ku8MBReadHoldingRegisters)
    {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
    }
    else if (u8MBFunction == ku8MBReadInputRegisters)
    {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadAddress);
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16ReadQty);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16ReadQty);
    }
    //Or Add address - Data to Write
    else if (u8MBFunction == ku8MBWriteSingleCoil)
    {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteAddress);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteAddress);
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteQty);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteQty);
    }
    else if (u8MBFunction == ku8MBWriteSingleRegister)
    {
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16WriteAddress);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16WriteAddress);
        u8ModbusADU[u8ModbusADUSize++] = highByte(_u16TransmitBuffer[0]);
        u8ModbusADU[u8ModbusADUSize++] = lowByte(_u16TransmitBuffer[0]);
    }
    else if (u8MBFunction == ku8MBWriteMultipleCoils)
    {
    }
    else if (u8MBFunction == ku8MBWriteMultipleRegisters)
    {
    }

    // ADD append CRC
    u16CRC = 0xFFFF;
    for (i = 0; i < u8ModbusADUSize; i++)
    {
        u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
    }
    u8ModbusADU[u8ModbusADUSize++] = lowByte(u16CRC);
    u8ModbusADU[u8ModbusADUSize++] = highByte(u16CRC);

    //u8ModbusADU[u8ModbusADUSize] = 0;

    //Send to RS485 Mod Bus
    //printf("send MODBUS: \n");
    for (i = 0; i < u8ModbusADUSize; i++)
    {
        Write_Data_TX_FIFO_ZDU0210RJX(u8ModbusADU[i], 0);
        //printf("%x \n", u8ModbusADU[i]);
    }
    //printf("\n");
    u8ModbusADUSizeTx = u8ModbusADUSize;
    u8ModbusADUSize = 0;

    return u8ModbusADUSize;
}

void responseModbus(uint8_t u8MBFunction, uint8_t *data_rd, bool coil)
{
    vTaskDelay(100);
    uint8_t u8ModbusADU[20];
    uint8_t u8MBStatus = ku8MBSuccess;
    uint8_t u8ModbusADUSize = 0;
    uint16_t u16CRC;
    bool crc = false;
    uint8_t i;
    uint8_t size = 7;

    //El tamaño de los coils es de 6 bytes, los otros registros tienen un tamaño de 7 bytes
    if (coil)
        size = 6;

    u8ModbusADUSize = Read_Receive_Transmit_FIFO_Level_Registers_ZDU0210RJX(0, 0);
    //printf("RX size MODBUS: %d \n", u8ModbusADUSize);

    if (u8ModbusADUSize > 0)
    {
        Read_Data_RX_FIFO_ZDU0210RJX(0, u8ModbusADU, u8ModbusADUSize);
        //printf("----------------------------------\n");
        //printf("Response Phoenix: \n");
        //for (int i = 0; i < u8ModbusADUSize; i++)
        //{
        //    printf("%x \n", u8ModbusADU[i]);
        //}
        //printf("\n");

        // evaluate slave ID, function code once enough bytes have been read
        if (u8ModbusADUSize >= 7)
        {
            // verify response is for correct Modbus slave
            if (u8ModbusADU[u8ModbusADUSize - size] != _u8MBSlave)
            {
                u8MBStatus = ku8MBInvalidSlaveID;
                ESP_LOGI(TAG, "FAIL SLAVE %x -NO VALID %x \n ", u8ModbusADU[u8ModbusADUSize - size], u8MBStatus);
            }

            // verify response is for correct Modbus function code (mask exception bit 7)
            if ((u8ModbusADU[u8ModbusADUSize - (size - 1)] & 0x7F) != u8MBFunction)
            {
                u8MBStatus = ku8MBInvalidFunction;
                ESP_LOGI(TAG, "FAIL function code  %x -NO VALID %x \n ", u8ModbusADU[u8ModbusADUSize - (size - 1)], u8MBStatus);
            }

            // check whether Modbus exception occurred; return Modbus Exception Code
            if ((u8ModbusADU[u8ModbusADUSize - (size - 1)] >> 7) & 0x01)
            {
                u8MBStatus = u8ModbusADU[u8ModbusADUSize - (size - 2)];
                ESP_LOGI(TAG, "FAIL function code Exception %x -NO VALID %x \n ", u8ModbusADU[u8ModbusADUSize - (size - 2)], u8MBStatus);
            }

            // verify response is large enough to inspect further
            if (!u8MBStatus && u8ModbusADUSize >= 7)
            {
                // calculate CRC
                u16CRC = 0xFFFF;
                for (i = (u8ModbusADUSize - size); i < (u8ModbusADUSize - 2); i++)
                {
                    u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
                    //printf("CRC-UPDATE: %x - %x \n", u16CRC, u8ModbusADU[i]);
                }

                //printf("CRC-COMPARE: %x - %x \n", (uint8_t)(u16CRC), u8ModbusADU[u8ModbusADUSize - 2]);
                //printf("CRC-COMPARE: %x - %x \n", (uint8_t)(u16CRC >> 8), u8ModbusADU[u8ModbusADUSize - 1]);
                // verify CRC
                if (!u8MBStatus && ((uint8_t)(u16CRC) != u8ModbusADU[u8ModbusADUSize - 2] ||
                                    (uint8_t)(u16CRC >> 8) != u8ModbusADU[u8ModbusADUSize - 1]))
                {
                    u8MBStatus = ku8MBInvalidCRC;
                    ESP_LOGI(TAG, "Invalidate CRC State:  %d \n", u8MBStatus);
                }
                else
                {
                    crc = true;
                    //ESP_LOGI(TAG, "Validate CRC State:  %d \n", u8MBStatus);
                }
            }

            if (crc && u8ModbusADUSize >= 7)
            {
                // evaluate returned Modbus function code
                //ESP_LOGI(TAG, "Read function %x \n", u8ModbusADU[u8ModbusADUSizeTx + 1]);
                if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadCoils)
                {
                    //ESP_LOGI(TAG, "ReadCoils");
                    if (u8ModbusADU[u8ModbusADUSize - (size - 2)] == 1)
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        //ESP_LOGI(TAG, "DATA 1 Byte- Modbus: %x  \n", data_rd[0]);
                    }
                    else
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        data_rd[1] = u8ModbusADU[u8ModbusADUSize - (size - 4)];
                        //ESP_LOGI(TAG, "DATA 2 Bytes- Modbus: %x - %x \n", data_rd[0], data_rd[1]);
                    }
                }
                else if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadDiscreteInputs)
                {
                    //ESP_LOGI(TAG, "ReadDiscreteInputs");
                }
                else if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadInputRegisters)
                {
                    //ESP_LOGI(TAG, "ReadInputRegisters");
                    if (u8ModbusADU[u8ModbusADUSize - (size - 2)] == 1)
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        //ESP_LOGI(TAG, "DATA 1 Byte- Modbus: %x  \n", data_rd[0]);
                    }
                    else
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        data_rd[1] = u8ModbusADU[u8ModbusADUSize - (size - 4)];
                        //ESP_LOGI(TAG, "DATA 2 Bytes- Modbus: %x - %x \n", data_rd[0], data_rd[1]);
                    }
                }
                else if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadHoldingRegisters)
                {
                    //ESP_LOGI(TAG, "ReadHoldingRegisters");
                    if (u8ModbusADU[u8ModbusADUSize - (size - 2)] == 1)
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        //ESP_LOGI(TAG, "DATA 1 Byte- Modbus: %x  \n", data_rd[0]);
                    }
                    else
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        data_rd[1] = u8ModbusADU[u8ModbusADUSize - (size - 4)];
                        //ESP_LOGI(TAG, "DATA 2 Bytes- Modbus: %x - %x \n", data_rd[0], data_rd[1]);
                    }
                }
            }
            else
            {
                ESP_LOGI(TAG, "NO CRC-FAIL DATA MODBUS");
                ESP_LOGI(TAG, "SizeBuffer:%d \n", u8ModbusADUSize);
            }
            memset(u8ModbusADU, 0, sizeof(u8ModbusADU));
        }
        else
        {
            memset(u8ModbusADU, 0, sizeof(u8ModbusADU));
            ESP_LOGI(TAG, "NO SIZE DATA MODBUS");
            ESP_LOGI(TAG, "SizeBuffer:%d \n", u8ModbusADUSize);
            data_rd[0] = 0;
            data_rd[1] = 0;
        }
    }
    else
    {
        ESP_LOGI(TAG, "NO DATA MODBUS");
        ESP_LOGI(TAG, "SizeBuffer:%d \n", u8ModbusADUSize);
        data_rd[0] = 0;
        data_rd[1] = 0;
    }
}

void responseModbusTask(uint8_t u8MBFunction, uint8_t *data_rd, bool coil, uint8_t u8ModbusADUSize)
{
    uint8_t u8ModbusADU[64];
    uint8_t u8MBStatus = ku8MBSuccess;
    uint16_t u16CRC;
    bool crc = false;
    uint8_t i;
    uint8_t size = 7;

    //El tamaño de los coils es de 6 bytes, los otros registros tienen un tamaño de 7 bytes
    if (coil)
        size = 6;

    if (u8ModbusADUSize > 0)
    {
        Read_Data_RX_FIFO_ZDU0210RJX(0, u8ModbusADU, u8ModbusADUSize);
        //printf("----------------------------------\n");
        //printf("Response Phoenix: \n");
        //for (int i = 0; i < u8ModbusADUSize; i++)
        //{
        //    printf("%x \n", u8ModbusADU[i]);
        //}
        //printf("\n");

        // evaluate slave ID, function code once enough bytes have been read
        if (u8ModbusADUSize >= 8)
        {
            // verify response is for correct Modbus slave
            if (u8ModbusADU[u8ModbusADUSize - size] != _u8MBSlave)
            {
                u8MBStatus = ku8MBInvalidSlaveID;
                ESP_LOGE(TAG, "FAIL SLAVE %x -NO VALID %x \n ", u8ModbusADU[u8ModbusADUSize - size], u8MBStatus);
            }

            // verify response is for correct Modbus function code (mask exception bit 7)
            if ((u8ModbusADU[u8ModbusADUSize - (size - 1)] & 0x7F) != u8MBFunction)
            {
                u8MBStatus = ku8MBInvalidFunction;
                ESP_LOGE(TAG, "FAIL function code  %x -NO VALID %x \n ", u8ModbusADU[u8ModbusADUSize - (size - 1)], u8MBStatus);
            }

            // check whether Modbus exception occurred; return Modbus Exception Code
            if ((u8ModbusADU[u8ModbusADUSize - (size - 1)] >> 7) & 0x01)
            {
                u8MBStatus = u8ModbusADU[u8ModbusADUSize - (size - 2)];
                ESP_LOGE(TAG, "FAIL function code Exception %x -NO VALID %x \n ", u8ModbusADU[u8ModbusADUSize - (size - 2)], u8MBStatus);
            }

            // verify response is large enough to inspect further
            if (!u8MBStatus)
            {
                // calculate CRC
                u16CRC = 0xFFFF;
                for (i = (u8ModbusADUSize - size); i < (u8ModbusADUSize - 2); i++)
                {
                    u16CRC = crc16_update(u16CRC, u8ModbusADU[i]);
                    //printf("CRC-UPDATE: %x - %x \n", u16CRC, u8ModbusADU[i]);
                }

                //printf("CRC-COMPARE: %x - %x \n", (uint8_t)(u16CRC), u8ModbusADU[u8ModbusADUSize - 2]);
                //printf("CRC-COMPARE: %x - %x \n", (uint8_t)(u16CRC >> 8), u8ModbusADU[u8ModbusADUSize - 1]);
                // verify CRC
                if (!u8MBStatus && ((uint8_t)(u16CRC) != u8ModbusADU[u8ModbusADUSize - 2] ||
                                    (uint8_t)(u16CRC >> 8) != u8ModbusADU[u8ModbusADUSize - 1]))
                {
                    u8MBStatus = ku8MBInvalidCRC;
                    ESP_LOGE(TAG, "Invalidate CRC State:  %d \n", u8MBStatus);
                }
                else
                {
                    crc = true;
                    //ESP_LOGI(TAG, "Validate CRC State:  %d \n", u8MBStatus);
                }
            }

            if (!u8MBStatus && crc)
            {
                // evaluate returned Modbus function code
                //ESP_LOGI(TAG, "Read function %x \n", u8ModbusADU[u8ModbusADUSizeTx + 1]);
                if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadCoils)
                {
                    //ESP_LOGI(TAG, "ReadCoils");
                    if (u8ModbusADU[u8ModbusADUSize - (size - 2)] == 1)
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        //ESP_LOGI(TAG, "DATA 1 Byte- Modbus: %x  \n", data_rd[0]);
                    }
                    else
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        data_rd[1] = u8ModbusADU[u8ModbusADUSize - (size - 4)];
                        //ESP_LOGI(TAG, "DATA 2 Bytes- Modbus: %x - %x \n", data_rd[0], data_rd[1]);
                    }
                }
                else if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadDiscreteInputs)
                {
                    //ESP_LOGI(TAG, "ReadDiscreteInputs");
                }
                else if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadInputRegisters)
                {
                    //ESP_LOGI(TAG, "ReadInputRegisters");
                    if (u8ModbusADU[u8ModbusADUSize - (size - 2)] == 1)
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        //ESP_LOGI(TAG, "DATA 1 Byte- Modbus: %x  \n", data_rd[0]);
                    }
                    else
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        data_rd[1] = u8ModbusADU[u8ModbusADUSize - (size - 4)];
                        //ESP_LOGI(TAG, "DATA 2 Bytes- Modbus: %x - %x \n", data_rd[0], data_rd[1]);
                    }
                }
                else if (u8ModbusADU[u8ModbusADUSize - (size - 1)] == ku8MBReadHoldingRegisters)
                {
                    //ESP_LOGI(TAG, "ReadHoldingRegisters");
                    if (u8ModbusADU[u8ModbusADUSize - (size - 2)] == 1)
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        //ESP_LOGI(TAG, "DATA 1 Byte- Modbus: %x  \n", data_rd[0]);
                    }
                    else
                    {
                        data_rd[0] = u8ModbusADU[u8ModbusADUSize - (size - 3)];
                        data_rd[1] = u8ModbusADU[u8ModbusADUSize - (size - 4)];
                        //ESP_LOGI(TAG, "DATA 2 Bytes- Modbus: %x - %x \n", data_rd[0], data_rd[1]);
                    }
                }
            }
            else
            {
                ESP_LOGE(TAG, "NO CRC-FAIL DATA MODBUS");
            }
            memset(u8ModbusADU, 0, sizeof(u8ModbusADU));
        }
        else
        {
            ESP_LOGE(TAG, "NO SIZE DATA MODBUS");
        }
    }
    else
    {
        ESP_LOGE(TAG, "NO DATA MODBUS");
    }
}

void resetBufferTx() //optimizar
{
    uint8_t u8ModbusADU[1];
    uint8_t u8ModbusADUSize = 0;
    Read_Data_RX_FIFO_ZDU0210RJX(0, u8ModbusADU, u8ModbusADUSize);
    //uart_reset_FIFO_ZDU0210RJX(0);
}

/** @ingroup util_word
    Return low word of a 32-bit integer.
    @param uint32_t ww (0x00000000..0xFFFFFFFF)
    @return low word of input (0x0000..0xFFFF)
*/
uint16_t lowWord(uint32_t ww)
{
    return (uint16_t)((ww)&0xFFFF);
}

uint8_t lowByte(uint16_t ww)
{
    return (uint8_t)((ww)&0xFFFF);
}

uint8_t highByte(uint16_t ww)
{
    return (uint8_t)((ww) >> 8);
}

/** @ingroup util_word
    Return high word of a 32-bit integer.
    @param uint32_t ww (0x00000000..0xFFFFFFFF)
    @return high word of input (0x0000..0xFFFF)
*/
uint16_t highWord(uint32_t ww)
{
    return (uint16_t)((ww) >> 16);
}
