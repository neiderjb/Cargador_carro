//****************************************************************************/
//  Function: file for SC18IS602B
//  Hardware: AIRIS - RTC
//  ESP32 MCU: ESPRESSIF
//  Author:   Diego Ochoa
//  Date:    July 11,2019
//  Version: v1.0
//  by Agrum.SAS
//****************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 


/* Describes the pin modes a pin of the SC18IS602B can be in */
enum SC18IS601B_GPIOPinMode
{
    SC18IS601B_GPIO_MODE_QUASI_BIDIRECTIONAL = 0B00,
    SC18IS601B_GPIO_MODE_PUSH_PULL = 0B01,
    SC18IS601B_GPIO_MODE_INPUT_ONLY = 0B10,
    SC18IS601B_GPIO_MODE_OPEN_DRAIN = 0B11
};

/* Describes the possible SPI speeds */
enum SC18IS601B_SPI_Speed
{
    SC18IS601B_SPICLK_1843_kHz = 0B00, /* 1.8 MBit/s */
    SC18IS601B_SPICLK_461_kHz = 0B01,  /* 461 kbit/s */
    SC18IS601B_SPICLK_115_kHz = 0B10,  /* 115 kbit/s */
    SC18IS601B_SPICLK_58_kHz = 0B11    /* 58 kbit/s */
};

/* Describes the possible SPI modes */
enum SC18IS601B_SPI_Mode
{
    SC18IS601B_SPIMODE_0 = 0B00, /* CPOL: 0  CPHA: 0 */
    SC18IS601B_SPIMODE_1 = 0B01, /* CPOL: 0  CPHA: 1 */
    SC18IS601B_SPIMODE_2 = 0B10, /* CPOL: 1  CPHA: 0 */
    SC18IS601B_SPIMODE_3 = 0B11  /* CPOL: 1  CPHA: 1 */
};

/* address of the module */
#define SC18IS601B_address 0x2B        

/* Function IDs */
uint8_t SC18IS601B_CONFIG_SPI_CMD;
uint8_t SC18IS601B_CLEAR_INTERRUPT_CMD;
uint8_t SC18IS601B_IDLE_CMD;
uint8_t SC18IS601B_GPIO_WRITE_CMD;
uint8_t SC18IS601B_GPIO_READ_CMD;
uint8_t SC18IS601B_GPIO_ENABLE_CMD;
uint8_t SC18IS601B_GPIO_CONFIGURATION_CMD;

#define SC18IS601B_DATABUFFER_DEPTH 200


/* calls into Wire.begin() */
void begin_SC18IS602B();

/* Sets the chip into low power mode */
bool setLowPowerMode();

/* Clears the INT pin asserted HIGH after every SPI transfer */
void clearInterrupt_SC18IS602B();

/* SPI functions. spiMode should be SPI_MODE0 to SPI_MODE3 */
bool configureSPI(bool lsbFirst, enum SC18IS601B_SPI_Mode spiMode, enum SC18IS601B_SPI_Speed clockSpeed);

/* Executes a SPI transfer Test Mode.
     * Slave select number `slaveNum` will be used.
     * The txLen bytes from the txData buffer will be sent,
     * then txLen bytes will be read into readBuf.
     * Returns success (true/false).
     * */
bool spiTransferData(int slaveNum, uint8_t *txData, size_t txLen, uint8_t *readBuf);

/* Transfers a single byte to a slave. returns the read value. */
uint8_t spiTransfer(int slaveNum, uint8_t txByte);

bool i2c_write_SC18IS602B(uint8_t cmdByte, uint8_t *data, size_t len);
bool i2c_read_SC18IS602B(uint8_t *readBuf, size_t len);




