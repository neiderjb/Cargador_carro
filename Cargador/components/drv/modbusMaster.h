/**
@file
Arduino library for communicating with Modbus slaves over RS232/485 (via RTU protocol).
@defgroup setup ModbusMaster Object Instantiation/Initialization
@defgroup buffer ModbusMaster Buffer Management
@defgroup discrete Modbus Function Codes for Discrete Coils/Inputs
@defgroup register Modbus Function Codes for Holding/Input Registers
@defgroup constant Modbus Function Codes, Exception Codes
*/
/*
  ModbusMaster.h - Arduino library for communicating with Modbus slaves
  over RS232/485 (via RTU protocol).
  Library:: ModbusMaster
  Copyright:: 2009-2016 Doc Walker
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
      http://www.apache.org/licenses/LICENSE-2.0
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 


void begin_modbusMaster(uint8_t slave);


void idle(void (*)());


// Modbus exception codes
/**
    Modbus protocol illegal function exception.
    
    The function code received in the query is not an allowable action for
    the server (or slave). This may be because the function code is only
    applicable to newer devices, and was not implemented in the unit
    selected. It could also indicate that the server (or slave) is in the
    wrong state to process a request of this type, for example because it is
    unconfigured and is being asked to return register values.
    
    @ingroup constant
    */
//static const uint8_t ku8MBIllegalFunction = 0x01;

/**
    Modbus protocol illegal data address exception.
    
    The data address received in the query is not an allowable address for 
    the server (or slave). More specifically, the combination of reference 
    number and transfer length is invalid. For a controller with 100 
    registers, the ADU addresses the first register as 0, and the last one 
    as 99. If a request is submitted with a starting register address of 96 
    and a quantity of registers of 4, then this request will successfully 
    operate (address-wise at least) on registers 96, 97, 98, 99. If a 
    request is submitted with a starting register address of 96 and a 
    quantity of registers of 5, then this request will fail with Exception 
    Code 0x02 "Illegal Data Address" since it attempts to operate on 
    registers 96, 97, 98, 99 and 100, and there is no register with address 
    100. 
    
    @ingroup constant
    */
//static const uint8_t ku8MBIllegalDataAddress = 0x02;

/**
    Modbus protocol illegal data value exception.
    
    A value contained in the query data field is not an allowable value for 
    server (or slave). This indicates a fault in the structure of the 
    remainder of a complex request, such as that the implied length is 
    incorrect. It specifically does NOT mean that a data item submitted for 
    storage in a register has a value outside the expectation of the 
    application program, since the MODBUS protocol is unaware of the 
    significance of any particular value of any particular register.
    
    @ingroup constant
    */
//static const uint8_t ku8MBIllegalDataValue = 0x03;

/**
    Modbus protocol slave device failure exception.
    
    An unrecoverable error occurred while the server (or slave) was
    attempting to perform the requested action.
    
    @ingroup constant
    */
//static const uint8_t ku8MBSlaveDeviceFailure = 0x04;

// Class-defined success/exception codes
/**
    ModbusMaster success.
    
    Modbus transaction was successful; the following checks were valid:
      - slave ID
      - function code
      - response code
      - data
      - CRC
      
    @ingroup constant
    */
//static const uint8_t ku8MBSuccess = 0x00;

/**
    ModbusMaster invalid response slave ID exception.
    
    The slave ID in the response does not match that of the request.
    
    @ingroup constant
    */
//static const uint8_t ku8MBInvalidSlaveID = 0xE0;

/**
    ModbusMaster invalid response function exception.
    
    The function code in the response does not match that of the request.
    
    @ingroup constant
    */
//static const uint8_t ku8MBInvalidFunction = 0xE1;

/**
    ModbusMaster response timed out exception.
    
    The entire response was not received within the timeout period, 
    ModbusMaster::ku8MBResponseTimeout. 
    
    @ingroup constant
    */
//static const uint8_t ku8MBResponseTimedOut = 0xE2;

/**
    ModbusMaster invalid response CRC exception.
    
    The CRC in the response does not match the one calculated.
    
    @ingroup constant
    */
//static const uint8_t ku8MBInvalidCRC = 0xE3;



uint8_t readCoils(uint16_t, uint16_t);
uint8_t readDiscreteInputs(uint16_t, uint16_t);
uint8_t readHoldingRegisters(uint16_t, uint16_t);
uint8_t readInputRegisters(uint16_t u16ReadAddress, uint8_t u16ReadQty);
uint8_t writeSingleCoil(uint16_t u16WriteAddress, uint8_t u8State);
uint8_t writeSingleRegister(uint16_t, uint16_t);
uint8_t writeMultipleCoilsAdd(uint16_t, uint16_t);
uint8_t writeMultipleCoils();
uint8_t writeMultipleRegistersAdd(uint16_t, uint16_t);
uint8_t writeMultipleRegisters();
uint8_t maskWriteRegister(uint16_t, uint16_t, uint16_t);
uint8_t readWriteMultipleRegistersAdd(uint16_t, uint16_t, uint16_t, uint16_t);
uint8_t readWriteMultipleRegisters(uint16_t, uint16_t);

void resetBufferTx();


//Stream *_serial;                               ///< reference to serial port object
uint8_t _u8MBSlave;                            ///< Modbus slave (1..255) initialized in begin()
//uint8_t ku8MaxBufferSize = 64;    ///< size of response/transmit buffers
uint16_t _u16ReadAddress;                      ///< slave register from which to read
uint16_t _u16ReadQty;                          ///< quantity of words to read
//uint16_t _u16ResponseBuffer[ku8MaxBufferSize]; ///< buffer to store Modbus slave response; read via GetResponseBuffer()
uint16_t _u16ResponseBuffer[64];
uint16_t _u16WriteAddress;                     ///< slave register to which to write
uint16_t _u16WriteQty;                         ///< quantity of words to write
//uint16_t _u16TransmitBuffer[ku8MaxBufferSize]; ///< buffer containing data to transmit to Modbus slave; set via SetTransmitBuffer()
uint16_t _u16TransmitBuffer[64]; ///< buffer containing data to transmit to Modbus slave; set via SetTransmitBuffer()
uint16_t *txBuffer;                            // from Wire.h -- need to clean this up Rx
uint8_t _u8TransmitBufferIndex;
uint16_t u16TransmitBufferLength;
uint16_t *rxBuffer; // from Wire.h -- need to clean this up Rx
uint8_t _u8ResponseBufferIndex;
uint8_t _u8ResponseBufferLength;

// Modbus function codes for bit access
//static const uint8_t ku8MBReadCoils = 0x01;          ///< Modbus function 0x01 Read Coils
// static const uint8_t ku8MBReadDiscreteInputs = 0x02; ///< Modbus function 0x02 Read Discrete Inputs
// static const uint8_t ku8MBWriteSingleCoil = 0x05;    ///< Modbus function 0x05 Write Single Coil
// static const uint8_t ku8MBWriteMultipleCoils = 0x0F; ///< Modbus function 0x0F Write Multiple Coils


// Modbus function codes for 16 bit access
// static const uint8_t ku8MBReadHoldingRegisters = 0x03;       ///< Modbus function 0x03 Read Holding Registers
// static const uint8_t ku8MBReadInputRegisters = 0x04;         ///< Modbus function 0x04 Read Input Registers
// static const uint8_t ku8MBWriteSingleRegister = 0x06;        ///< Modbus function 0x06 Write Single Register
// static const uint8_t ku8MBWriteMultipleRegisters = 0x10;     ///< Modbus function 0x10 Write Multiple Registers
// static const uint8_t ku8MBMaskWriteRegister = 0x16;          ///< Modbus function 0x16 Mask Write Register
// static const uint8_t ku8MBReadWriteMultipleRegisters = 0x17; ///< Modbus function 0x17 Read Write Multiple Registers

// Calcule CRC
uint16_t crc16_update(uint16_t crc, uint8_t a);

// Modbus timeout [milliseconds]
//static const uint16_t ku16MBResponseTimeout = 2000; ///< Modbus timeout [milliseconds]

// master function that conducts Modbus transactions
uint8_t ModbusMasterTransaction(uint8_t u8MBFunction);
uint8_t ModbusMasterTransactionOLD(uint8_t u8MBFunction);

// // idle callback function; gets called during idle time between TX and RX
// void (*_idle)(void);
// // preTransmission callback function; gets called before writing a Modbus message
// void (*_preTransmission)(void);
// // postTransmission callback function; gets called after a Modbus message has been sent
// void (*_postTransmission)(void);


void responseModbus(uint8_t u8MBFunction, uint8_t *data_rd);

uint16_t lowWord(uint32_t ww);
uint8_t lowByte(uint16_t ww);
uint16_t highWord(uint32_t ww);
uint8_t highByte(uint16_t ww);
