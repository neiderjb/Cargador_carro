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

#include "esp_log.h"

#include "SC18IS602B.h"
#include "software_i2c.h"
#include "Parameters.h"

static const char *TAG = "SC18IS602B";

// SC18IS601B_GPIO_WRITE_CMD =  0xF4;
// SC18IS601B_GPIO_READ_CMD =  0xF5;
// SC18IS601B_GPIO_ENABLE_CMD  = 0xF6;
// SC18IS601B_GPIO_CONFIGURATION_CMD =  0xF7;

bool setLowPowerMode()
{
    SC18IS601B_IDLE_CMD = 0xF2;
    if (xSemaphoreTake(Semaphore_I2C, 10))
    {
        sw_i2c_write(SC18IS601B_address, &SC18IS601B_IDLE_CMD, 1);
        xSemaphoreGive(Semaphore_I2C);
    }
    return true;
}

void clearInterrupt_SC18IS602B()
{
    SC18IS601B_CLEAR_INTERRUPT_CMD = 0xF1;
    if (xSemaphoreTake(Semaphore_I2C, 10))
    {
        sw_i2c_write(SC18IS601B_address, &SC18IS601B_CLEAR_INTERRUPT_CMD, 1);
        xSemaphoreGive(Semaphore_I2C);
    }
}

/* 
 * @brief  Write SPI
 * @Parameter:
 * cmdByte, 
 * data, Pointer data Send
 * len, Size Pointer data send
 */
bool i2c_write_SC18IS602B(uint8_t cmdByte, uint8_t *data, size_t len)
{
    sw_i2c_spi_write(SC18IS601B_address, cmdByte, data, len);
    return true;
}

bool configureSPI(bool lsbFirst, enum SC18IS601B_SPI_Mode spiMode, enum SC18IS601B_SPI_Speed clockSpeed)
{
    SC18IS601B_CONFIG_SPI_CMD = 0xF0;
    //sanity check on parameters
    if (spiMode > SC18IS601B_SPIMODE_3)
        return false;
    uint8_t clk = (uint8_t)((uint8_t)(clockSpeed)&0B11);

    //see chapter 7.1.5
    uint8_t configByte = (lsbFirst << 5) | (spiMode << 2) | clk;

    uint8_t dataWrite[2];
    dataWrite[0] = SC18IS601B_CONFIG_SPI_CMD;
    dataWrite[1] = configByte;
    //printf("SPI CONFIG: %x \n", configByte);

    sw_i2c_write(SC18IS601B_address, dataWrite, sizeof(dataWrite));

    return true;
}

void begin_SC18IS602B()
{
    if (xSemaphoreTake(Semaphore_I2C, 10))
    {
        if (configureSPI(false, SC18IS601B_SPIMODE_3, SC18IS601B_SPICLK_58_kHz))
        {
            ESP_LOGI(TAG, "begin_SC18IS602B -OK");
        }
        else
        {
            ESP_LOGI(TAG, "begin_SC18IS602B -FAIL");
        }
        xSemaphoreGive(Semaphore_I2C);
    }
}

uint8_t spiTransfer(int slaveNum, uint8_t txByte)
{
    uint8_t readBuf = 0;
    spiTransferData(slaveNum, &txByte, 1, &readBuf);
    return readBuf;
}

bool i2c_read_SC18IS602B(uint8_t *readBuf, size_t len)
{
    //while (Wire.requestFrom(address,len) == 0);
    // return Wire.readBytes(readBuf, len);
    if (xSemaphoreTake(Semaphore_I2C, 10))
    {
        sw_i2c_spi_read(SC18IS601B_address, readBuf, len);
        xSemaphoreGive(Semaphore_I2C);
    }
    return true;
}

bool spiTransferData(int slaveNum, uint8_t *txData, size_t txLen, uint8_t *readBuf)
{
    //sanity check
    if (slaveNum < 0 || slaveNum > 3)
        return false;

    //Overly long data?
    if (txLen > SC18IS601B_DATABUFFER_DEPTH)
        return false;

    //the function ID will have the lower 4 bits set to the
    //activated slave selects. We use only 1 at a time here.
    uint8_t functionID = (1 << slaveNum);
    if (xSemaphoreTake(Semaphore_I2C, 10))
    {
        //transmit our TX buffer
        sw_i2c_write(SC18IS601B_address, &functionID, txLen);
        //read in the data that came from MISO
        sw_i2c_spi_read(SC18IS601B_address, readBuf, txLen);
        xSemaphoreGive(Semaphore_I2C);
    }

    return true;
}