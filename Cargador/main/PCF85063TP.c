/****************************************************************************/
//  Function: file for PCD85063TP
//  Hardware: AIRIS - RTC
//  ESP32 MCU: ESPRESSIF
//  Author:   Diego Ochoa
//  Date:    July 11,2019
//  Version: v1.0
//  by Agrum.SAS
/****************************************************************************/
//https://github.com/Seeed-Studio/Grove_High_Precision_RTC_PCF85063TP

#include "i2c.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "esp_log.h"

#include "PCF85063TP.h"

static const char *TAG = "PCF85063TP";

uint8_t decToBcd(uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

//Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}

void begin_PCF85063TP()
{
    //i2c_uart_configuration(I2C_NUM_0, 21, 22); //Configure i2c Port    //Wire.begin();
    cap_sel(CAP_SEL_12_5PF);                   // CAP_SEL bit setting 12.5pF
    ESP_LOGI(TAG, "begin_PCF85063TP OK");

}
/*Function: The clock timing will start */
void startClock(void) // set the ClockHalt bit low to start the rtc
{
    uint8_t data[1];
    i2c_read(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, 0x00, data, 1);    // Register 0x00 holds the oscillator start/stop bit
    data[1] = data[1] & ~0x20; // save actual control_1 regitser and AND sec with bit 7 (sart/stop bit) = clock started
    uint8_t dataIn[2] = {0x00, data[1]} ;
    i2c_write(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, dataIn, sizeof(dataIn));

    //uint8_t data;
    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write((uint8_t)0x00); // Register 0x00 holds the oscillator start/stop bit
    //Wire.endTransmission();
    //Wire.requestFrom(PCD85063TP_I2C_ADDRESS, 1);
    //data = Wire.read() & ~0x20; // save actual control_1 regitser and AND sec with bit 7 (sart/stop bit) = clock started
    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write((uint8_t)0x00);
    //Wire.write((uint8_t)data); // write seconds back and start the clock
    //Wire.endTransmission();
}
/*Function: The clock timing will stop */
void stopClock(void) // set the ClockHalt bit high to stop the rtc
{
    uint8_t data[1];
    i2c_read(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, 0x00, data, 1);    // Register 0x00 holds the oscillator start/stop bit
    data[1] = data[1] | 0x20; // save actual control_1 regitser and AND sec with bit 7 (sart/stop bit) = clock started
    uint8_t dataOut[] = {0x00, data[1]} ;
    i2c_write(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, dataOut, sizeof(dataOut));

    //uint8_t data;
    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write((uint8_t)0x00); // Register 0x00 holds the oscillator start/stop bit
    //Wire.endTransmission();
    //Wire.requestFrom(PCD85063TP_I2C_ADDRESS, 1);
    //data = Wire.read() | 0x20; // save actual control_1 regitser and AND sec with bit 7 (sart/stop bit) = clock started
    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write((uint8_t)0x00);
    //Wire.write((uint8_t)data); // write seconds back and stop the clock
    //Wire.endTransmission();
}
/****************************************************************/
/*Function: Read time and date from RTC  */
void getTime()
{
    // Reset the register pointer
    uint8_t datos[7];
    i2c_read(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, 0x04, datos, 7);

    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write((uint8_t)0x04);
    //Wire.endTransmission();
    //Wire.requestFrom(PCD85063TP_I2C_ADDRESS, 7);
    // A few of these need masks because certain bits are control bits
    second = bcdToDec(datos[0] & 0x7f);      //bcdToDec(Wire.read() & 0x7f);
    minute = bcdToDec(datos[1]);             //bcdToDec(Wire.read());
    hour = bcdToDec(datos[2] & 0x3f);        //bcdToDec(Wire.read() & 0x3f); // Need to change this if 12 hour am/pm
    dayOfMonth = bcdToDec(datos[3]);         //bcdToDec(Wire.read());
    dayOfWeek = bcdToDec(datos[4]);          //bcdToDec(Wire.read());
    month =  bcdToDec(datos[5]);             //bcdToDec(Wire.read());
    year = bcdToDec(datos[6]);               //bcdToDec(Wire.read());
}
/*******************************************************************/
/*Frunction: Write the time that includes the date to the RTC chip */
void setTime()
{
    writeReg(REG_SEC, decToBcd(second)); // 0 to bit 7 starts the clock, bit 8 is OS reg
    writeReg(REG_MIN, decToBcd(minute));
    writeReg(REG_HOUR, decToBcd(hour)); // If you want 12 hour am/pm you need to set bit 6
    writeReg(REG_DAY_MONTH, decToBcd(dayOfMonth));
    writeReg(REG_DAY_WEEK, decToBcd(dayOfWeek));
    writeReg(REG_MON, decToBcd(month));
    writeReg(REG_YEAR, decToBcd(year));
}
void fillByHMS(uint8_t _hour, uint8_t _minute, uint8_t _second)
{
    // assign variables
    hour = _hour;
    minute = _minute;
    second = _second;
}
void fillByYMD(uint16_t _year, uint8_t _month, uint8_t _day)
{
    year = _year - 2000;
    month = _month;
    dayOfMonth = _day;
}
void fillDayOfWeek(uint8_t _dow)
{
    dayOfWeek = _dow;
}

void reset()
{
    uint8_t data[2] = {0x00, decToBcd(0x10)};
    i2c_write(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, data, sizeof(data));

    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write((uint8_t)0x00);
    //Wire.write(decToBcd(0x10));// software reset at bit 4
    //Wire.endTransmission();
}

/* 
 * @brief  clock calibration setting
 * @Parameter:
 *   mode = 0, calibrate every 2 hours
 *   mode = 1, calibrate every 4 minutes 
 *   offset_sec, offset value of one second. 
 *   If the RTC time too fast: offset_sec < 0     
 *   If the RTC time too slow: offset_sec > 0 
 */
uint8_t calibratBySeconds(int mode, float offset_sec)
{
    float Fmeas = 32768.0 + offset_sec * 32768.0;
    setcalibration(mode, Fmeas);
    return readCalibrationReg();
}
/* 
 * @brief: Clock calibrate setting 
 * @parm: 
 *        mode: calibration cycle, mode 0 -> every 2 hours, mode 1 -> every 4 minutes
 *        Fmeas: Real frequency you detect
 */

void setcalibration(int mode, float Fmeas)
{
    float offset = 0;
    float Tmeas = 1.0 / Fmeas;
    float Dmeas = 1.0 / 32768 - Tmeas;
    float Eppm = 1000000.0 * Dmeas / Tmeas;
    if (mode == 0)
        offset = Eppm / 4.34;
    else if (mode == 1)
        offset = Eppm / 4.069;

    uint8_t data = ((mode << 7) & 0x80) | ((int)(offset + 0.5) & 0x7f);
    writeReg(PCD85063TP_OFFSET, data);
}

uint8_t readCalibrationReg(void)
{
    return readReg(PCD85063TP_OFFSET);
}

/*
 * @brief: internal oscillator capacitor selection for
 *         quartz crystals with a corresponding load capacitance
 * @parm:        
 *       value(0 or 1): 0 - 7 pF  
 *                      1 - 12.5 pF  
 * @return: value of CAP_SEL bit
 */
uint8_t cap_sel(uint8_t value)
{
    uint8_t control_1 = readReg(REG_CTRL1);
    control_1 = (control_1 & 0xFE) | (0x01 & value);
    writeReg(REG_CTRL1, control_1);

    return readReg(REG_CTRL1) & 0x01;
}

uint8_t readReg(uint8_t reg)
{
    uint8_t datos;
    i2c_read(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, (reg & 0xFF), &datos, 1);
    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write(reg & 0xFF);
    //Wire.endTransmission();
    //Wire.requestFrom(PCD85063TP_I2C_ADDRESS, 1);

    //return Wire.read();
    return datos;
}

void writeReg(uint8_t reg, uint8_t data)
{
    uint8_t dataOut[] = {(reg & 0xFF), (data & 0xFF)};
    i2c_write(I2C_NUM_0, PCD85063TP_I2C_ADDRESS, dataOut, sizeof(dataOut));
    //Wire.beginTransmission(PCD85063TP_I2C_ADDRESS);
    //Wire.write(reg & 0xFF);
    //Wire.write(data & 0xFF);
    //Wire.endTransmission();
}