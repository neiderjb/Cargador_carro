// /*!
//  * @file Adafruit_LIS3DH.cpp
//  *
//  *  @mainpage Adafruit LIS3DH breakout board
//  *
//  *  @section intro_sec Introduction
//  *
//  *  This is a library for the Adafruit LIS3DH Accel breakout board
//  *
//  *  Designed specifically to work with the Adafruit LIS3DH Accel breakout board.
//  *
//  *  Pick one up today in the adafruit shop!
//  *  ------> https://www.adafruit.com/product/2809
//  *
//  *  This sensor communicates over I2C or SPI (our library code supports both) so
//  * you can share it with a bunch of other sensors on the same I2C bus.
//  *
//  *  Adafruit invests time and resources providing this open source code,
//  *  please support Adafruit andopen-source hardware by purchasing products
//  *  from Adafruit!
//  *
//  *  @section author Author
//  *
//  *  K. Townsend / Limor Fried (Adafruit Industries)
//  *
//  *  @section license License
//  *
//  *  BSD license, all text above must be included in any redistribution
//  */

// //https://github.com/adafruit/Adafruit_LIS3DH

// #include "i2c.h"
// #include <esp_log.h>
// #include <freertos/FreeRTOS.h>
// #include <freertos/task.h>
// #include <stdio.h>

// #include "LIS3DH.h"


// /*!
//  *  @brief  Setups the HW (reads coefficients values, etc.)
//  *  @param  i2caddr
//  *          i2c address (optional, fallback to default)
//  *  @param  nWAI
//  *          Who Am I register value - defaults to 0x33 (LIS3DH)
//  *  @return true if successful
//  */
// bool begin(uint8_t i2caddr, uint8_t nWAI) {
//   _i2caddr = i2caddr;
//   _wai = nWAI;

//   if (_cs == -1) {
//     // i2c
//     I2Cinterface->begin();
//   } else {
//     digitalWrite(_cs, HIGH);
//     pinMode(_cs, OUTPUT);


//   }

//   /*
//   Serial.println("Debug");
//   for (uint8_t i=0; i<0x30; i++) {
//     Serial.print("$");
//     Serial.print(i, HEX); Serial.print(" = 0x");
//     Serial.println(readRegister8(i), HEX);
//   }
//   */

//   /* Check connection */
//   uint8_t deviceid = readRegister8(LIS3DH_REG_WHOAMI);
//   if (deviceid != _wai) {
//     /* No LIS3DH detected ... return false */
//     // Serial.println(deviceid, HEX);
//     return false;
//   }

//   // enable all axes, normal mode
//   writeRegister8(LIS3DH_REG_CTRL1, 0x07);
//   // 400Hz rate
//   setDataRate(LIS3DH_DATARATE_400_HZ);

//   // High res & BDU enabled
//   writeRegister8(LIS3DH_REG_CTRL4, 0x88);

//   // DRDY on INT1
//   writeRegister8(LIS3DH_REG_CTRL3, 0x10);

//   // Turn on orientation config
//   // writeRegister8(LIS3DH_REG_PL_CFG, 0x40);

//   // enable adcs
//   writeRegister8(LIS3DH_REG_TEMPCFG, 0x80);

//   /*
//   for (uint8_t i=0; i<0x30; i++) {
//     Serial.print("$");
//     Serial.print(i, HEX); Serial.print(" = 0x");
//     Serial.println(readRegister8(i), HEX);
//   }
//   */

//   return true;
// }

// /*!
//  *  @brief  Get Device ID from LIS3DH_REG_WHOAMI
//  *  @return WHO AM I value
//  */
// uint8_t getDeviceID() {
//   return readRegister8(LIS3DH_REG_WHOAMI);
// }

// /*!
//  *  @brief  Check to see if new data available
//  *  @return true if there is new data available, false otherwise
//  */
// bool haveNewData() {
// 	// checking ZYXDA in REG_STATUS2 tells us if data available
//  	return (readRegister8(LIS3DH_REG_STATUS2) & 0x8) >> 3;
// }

// /*!
//  *  @brief  Reads x y z values at once
//  */
// void read() {
//   if (_cs == -1) {
//     // i2c                                                                               //Cambiar
//     // I2Cinterface->beginTransmission(_i2caddr);
//     // I2Cinterface->write(LIS3DH_REG_OUT_X_L | 0x80); // 0x80 for autoincrement
//     // I2Cinterface->endTransmission();

//     // I2Cinterface->requestFrom(_i2caddr, 6);
//     // x = I2Cinterface->read();
//     // x |= ((uint16_t)I2Cinterface->read()) << 8;
//     // y = I2Cinterface->read();
//     // y |= ((uint16_t)I2Cinterface->read()) << 8;
//     // z = I2Cinterface->read();
//     // z |= ((uint16_t)I2Cinterface->read()) << 8;
//   }

//   uint8_t range = getRange();
//   uint16_t divider = 1;
//   if (range == LIS3DH_RANGE_16_G)
//     divider = 1365; // different sensitivity at 16g
//   if (range == LIS3DH_RANGE_8_G)
//     divider = 4096;
//   if (range == LIS3DH_RANGE_4_G)
//     divider = 8190;
//   if (range == LIS3DH_RANGE_2_G)
//     divider = 16380;

//   x_g = (float)x / divider;
//   y_g = (float)y / divider;
//   z_g = (float)z / divider;
// }

// /*!
//  *  @brief  Read the auxilary ADC
//  *  @param  adc
//  *          adc index. possible values (1, 2, 3).
//  *  @return auxilary ADC value
//  */
// int16_t readADC(uint8_t adc) {
//   if ((adc < 1) || (adc > 3))
//     return 0;
//   uint16_t value;

//   adc--;

//   uint8_t reg = LIS3DH_REG_OUTADC1_L + adc * 2;

//   if (_cs == -1) {
//     // i2c
//     // I2Cinterface->beginTransmission(_i2caddr);                       //Cambiar
//     // I2Cinterface->write(reg | 0x80); // 0x80 for autoincrement
//     // I2Cinterface->endTransmission();
//     // I2Cinterface->requestFrom(_i2caddr, 2);
//     // value = I2Cinterface->read();
//     // value |= ((uint16_t)I2Cinterface->read()) << 8;
//   }
//   return value;
// }

// /*!
//  *   @brief  Set INT to output for single or double click
//  *   @param  c
//  *					 0 = turn off I1_CLICK
//  *           1 = turn on all axes & singletap
//  *					 2 = turn on all axes & doubletap
//  *   @param  clickthresh
//  *           CLICK threshold value
//  *   @param  timelimit
//  *           sets time limit (default 10)
//  *   @param  timelatency
//  *   				 sets time latency (default 20)
//  *   @param  timewindow
//  *   				 sets time window (default 255)
//  */
// void setClick(uint8_t c, uint8_t clickthresh,
//                                uint8_t timelimit, uint8_t timelatency,
//                                uint8_t timewindow) {
//   if (!c) {
//     // disable int
//     uint8_t r = readRegister8(LIS3DH_REG_CTRL3);
//     r &= ~(0x80); // turn off I1_CLICK
//     writeRegister8(LIS3DH_REG_CTRL3, r);
//     writeRegister8(LIS3DH_REG_CLICKCFG, 0);
//     return;
//   }
//   // else...

//   writeRegister8(LIS3DH_REG_CTRL3, 0x80); // turn on int1 click
//   writeRegister8(LIS3DH_REG_CTRL5, 0x08); // latch interrupt on int1

//   if (c == 1)
//     writeRegister8(LIS3DH_REG_CLICKCFG, 0x15); // turn on all axes & singletap
//   if (c == 2)
//     writeRegister8(LIS3DH_REG_CLICKCFG, 0x2A); // turn on all axes & doubletap

//   writeRegister8(LIS3DH_REG_CLICKTHS, clickthresh);    // arbitrary
//   writeRegister8(LIS3DH_REG_TIMELIMIT, timelimit);     // arbitrary
//   writeRegister8(LIS3DH_REG_TIMELATENCY, timelatency); // arbitrary
//   writeRegister8(LIS3DH_REG_TIMEWINDOW, timewindow);   // arbitrary
// }

// /*!
//  *   @brief  Get uint8_t for single or double click
//  *   @return register LIS3DH_REG_CLICKSRC
//  */
// uint8_t getClick() {
//   return readRegister8(LIS3DH_REG_CLICKSRC);
// }

// /*!
//  *   @brief  Sets the g range for the accelerometer
//  *   @param  range
//  *           range value
//  */
// void setRange(lis3dh_range_t range) {
//   uint8_t r = readRegister8(LIS3DH_REG_CTRL4);
//   r &= ~(0x30);
//   r |= range << 4;
//   writeRegister8(LIS3DH_REG_CTRL4, r);
// }

// /*!
//  *  @brief  Gets the g range for the accelerometer
//  *  @return Returns g range value
//  */
// lis3dh_range_t getRange() {
//   /* Read the data format register to preserve bits */
//   return (lis3dh_range_t)((readRegister8(LIS3DH_REG_CTRL4) >> 4) & 0x03);
// }

// /*!
//  *  @brief  Sets the data rate for the LIS3DH (controls power consumption)
//  *  @param  dataRate
//  *          data rate value
//  */
// void setDataRate(lis3dh_dataRate_t dataRate) {
//   uint8_t ctl1 = readRegister8(LIS3DH_REG_CTRL1);
//   ctl1 &= ~(0xF0); // mask off bits
//   ctl1 |= (dataRate << 4);
//   writeRegister8(LIS3DH_REG_CTRL1, ctl1);
// }

// /*!
//  *   @brief  Gets the data rate for the LIS3DH (controls power consumption)
//  *   @return Returns Data Rate value
//  */
// lis3dh_dataRate_t getDataRate() {
//   return (lis3dh_dataRate_t)((readRegister8(LIS3DH_REG_CTRL1) >> 4) & 0x0F);
// }

// /*!
//  *  @brief  Gets the most recent sensor event
//  *  @param  *event
//  *          sensor event that we want to read
//  *  @return true if successful
//  */
// bool getEvent(sensors_event_t *event) {
//   /* Clear the event */
//   memset(event, 0, sizeof(sensors_event_t));

//   event->version = sizeof(sensors_event_t);
//   event->sensor_id = _sensorID;
//   event->type = SENSOR_TYPE_ACCELEROMETER;
//   event->timestamp = 0;

//   read();

//   event->acceleration.x = x_g * SENSORS_GRAVITY_STANDARD;
//   event->acceleration.y = y_g * SENSORS_GRAVITY_STANDARD;
//   event->acceleration.z = z_g * SENSORS_GRAVITY_STANDARD;

//   return true;
// }

// /*!
//  *   @brief  Gets the sensor_t data
//  *   @param  *sensor
//  *           sensor that we want to write data into
//  */
// void getSensor(sensor_t *sensor) {
//   /* Clear the sensor_t object */
//   memset(sensor, 0, sizeof(sensor_t));

//   /* Insert the sensor name in the fixed length char array */
//   strncpy(sensor->name, "LIS3DH", sizeof(sensor->name) - 1);
//   sensor->name[sizeof(sensor->name) - 1] = 0;
//   sensor->version = 1;
//   sensor->sensor_id = _sensorID;
//   sensor->type = SENSOR_TYPE_ACCELEROMETER;
//   sensor->min_delay = 0;
//   sensor->max_value = 0;
//   sensor->min_value = 0;
//   sensor->resolution = 0;
// }



// /*!
//  *  @brief  Writes 8-bits to the specified destination register
//  *  @param  reg
//  *          register address
//  *  @param  value
//  *          value that will be written into selected register
//  */
// void writeRegister8(uint8_t reg, uint8_t value) {
//   if (_cs == -1) {
//     // I2Cinterface->beginTransmission((uint8_t)_i2caddr);
//     // I2Cinterface->write((uint8_t)reg);
//     // I2Cinterface->write((uint8_t)value);
//     // I2Cinterface->endTransmission();
//   }

// }

// /*!
//  *  @brief  Reads 8-bits from the specified register
//  *  @param  reg
//  *          register address
//  *  @return read value
//  */
// uint8_t readRegister8(uint8_t reg) {
//   uint8_t value;

//   if (_cs == -1) {
//     I2Cinterface->beginTransmission(_i2caddr);
//     I2Cinterface->write((uint8_t)reg);
//     I2Cinterface->endTransmission();

//     I2Cinterface->requestFrom(_i2caddr, 1);
//     value = I2Cinterface->read();
//   }

//   return value;
// }