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

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#define PCD85063TP_I2C_ADDRESS 0x51
#define PCD85063TP_OFFSET 0x02
#define REG_CTRL1 0x00
#define REG_CTRL1_STOP 0x20
#define REG_CTRL2 0x01
#define REG_OFFSET 0x02
#define REG_SEC 0x04
#define REG_SEC_OS 0x80
#define REG_MIN 0x05
#define REG_HOUR 0x06
#define REG_DAY_MONTH 0x07
#define REG_DAY_WEEK 0x08
#define REG_MON 0x09
#define REG_YEAR 0x0A
#define CAP_SEL_7PF 0
#define CAP_SEL_12_5PF 1

#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6
#define SUN 0



uint8_t decToBcd(uint8_t val);
uint8_t bcdToDec(uint8_t val);

void begin_PCF85063TP();
void startClock(void);
void stopClock(void);
void setTime(void);
void getTime(uint8_t *data);
void setcalibration(int mode, float Fmeas);
uint8_t calibratBySeconds(int mode, float offset_sec);
uint8_t readCalibrationReg(void);
uint8_t cap_sel(uint8_t value);
void reset();
void fillByHMS(uint8_t _hour, uint8_t _minute, uint8_t _second);
void fillByYMD(uint16_t _year, uint8_t _month, uint8_t _day);
void fillDayOfWeek(uint8_t _dow);
uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t dayOfWeek; // day of week, 1 = Monday
uint8_t dayOfMonth;
uint8_t month;
uint16_t year;

uint8_t readReg(uint8_t reg);
void writeReg(uint8_t reg, uint8_t data);

void Time_Task_Control(void *p);
void Set_Time_Reference(char *s);
