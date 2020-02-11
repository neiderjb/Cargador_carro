/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      Library M90E32AS M90E32AS Enhanced Poly-Phase High-Performance Wide-Span 
 *      Energy Metering IC
 *
 *      This file contains the function prototypes and definitions
 *      for various configuration parameters for the ATMEL M90E32AS.
 *
 * \par Application note:
 *      ESP32 WROVER-B: Using the conversor I2C-SPI SC18IS601B
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      DSD: http:// \n
 *      Support email: 
 *
 * $Revision: 1 $
 * $Date: 2019-08-03 10:00:00 +0200 (ti, 22 apr 2008) $  \n
 *
 * Copyright (c) 2019, DSD All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of DSD may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DSD "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL DSD BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

//https://github.com/CircuitSetup/ATM90E32
//https://github.com/CircuitSetup/Split-Single-Phase-Energy-Meter
//Aplication Note http://ww1.microchip.com/downloads/en/AppNotes/Atmel-46103-SE-M90E32AS-ApplicationNote.pdf

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <math.h>

#include "esp_log.h"

#include "M90E32AS.h"
#include "SC18IS602B.h"
#include "PCF85063TP.h"
#include "Parameters.h"

#include "little.h"

static const char *TAG = "M90E32AS";
/* 
 * @brief  Write or Read Data SPI
 * @Parameter:
 *   RW = 1, Read Data
 *   RW = 0, Write Data
 *   address, register to read/write
 *   val, data to write  
 */
unsigned short CommEnergyIC(bool RW, uint8_t address, unsigned short val)
{
    unsigned char *data = (unsigned char *)&val;
    unsigned short output = 0;
    uint8_t datos[4] = {0x80, 0x00, 0x00, 0x00};

    if (RW) //READ DATA
    {
        uint8_t dataRead[10];
        datos[1] = address;
        i2c_write_SC18IS602B(0x01, datos, sizeof(datos));
        vTaskDelay(10 / portTICK_RATE_MS);
        i2c_read_SC18IS602B(dataRead, sizeof(dataRead));
        output = dataRead[2] << 8 | dataRead[3];
    }
    else
    {
        datos[0] = 0x00;
        datos[1] = address;
        datos[2] = data[1];
        datos[3] = data[0];
        //printf("WRITE  Data 0: %x , Data 1: %x \n", datos[2], datos[3]);
        i2c_write_SC18IS602B(0x01, datos, sizeof(datos));
        vTaskDelay(10 / portTICK_RATE_MS);
    }

    return output;
}

/* 
 * @brief  Get data to Register
 * @Parameter:
 *   addr, address to register read
 */
double GetRegister(uint8_t addr)
{
    unsigned short registerRead = CommEnergyIC(READ, addr, 0xFFFF);
    return (double)registerRead;
}

int Read32Register(signed short regh_addr, signed short regl_addr)
{
    int val, val_h, val_l;
    val_h = CommEnergyIC(READ, regh_addr, 0xFFFF);
    val_l = CommEnergyIC(READ, regl_addr, 0xFFFF);
    val = CommEnergyIC(READ, regh_addr, 0xFFFF);

    val = val_h << 16;
    val |= val_l; //concatenate the 2 registers to make 1 32 bit number

    /*
  // returns positive value if negative
  if ((val & 0x80000000) != 0) { 
		val = (~val) + 1; //2s compliment
  }
  */
    return (val);
}

/*! \brief This function Calculate VIO ffset.
 *
 *  For getting the lower registers of energy and calculating the offset
 *  this should only be run when all inputs are disconnected
 * 
 *  \param regh_addr        Address high register. 
 *  \param regl_addr        Address low register.
 *  \return uint16_t        Voltage Offset
 */
uint16_t CalculateVIOffset(unsigned short regh_addr, unsigned short regl_addr /*, unsigned short offset_reg*/)
{
    uint32_t val, val_h, val_l;
    uint16_t offset;
    val_h = CommEnergyIC(READ, regh_addr, 0xFFFF);
    val_l = CommEnergyIC(READ, regl_addr, 0xFFFF);
    val = CommEnergyIC(READ, regh_addr, 0xFFFF);

    val = val_h << 16; //move high register up 16 bits
    val |= val_l;      //concatenate the 2 registers to make 1 32 bit number
    val = val >> 7;    //right shift 7 bits - lowest 7 get ignored
    val = (~val) + 1;  //2s compliment

    offset = val; //keep lower 16 bits
    //CommEnergyIC(WRITE, offset_reg, (signed short)val);
    return offset;
}

/*! \brief This function Calculate Power Offset.
 *
 *  For getting the lower registers of energy and calculating the offset
 *  should only be run when CT sensors are connected to the meter,
 *  but not connected around wires
 * 
 *  \param regh_addr        Address high register. 
 *  \param regl_addr        Address low register.
 *  \return uint16_t        Power Offset
 */
uint16_t CalculatePowerOffset(unsigned short regh_addr, unsigned short regl_addr /*, unsigned short offset_reg*/)
{

    uint32_t val, val_h, val_l;
    uint16_t offset;
    val_h = CommEnergyIC(READ, regh_addr, 0xFFFF);
    val_l = CommEnergyIC(READ, regl_addr, 0xFFFF);
    val = CommEnergyIC(READ, regh_addr, 0xFFFF);

    val = val_h << 16; //move high register up 16 bits
    val |= val_l;      //concatenate the 2 registers to make 1 32 bit number
    val = (~val) + 1;  //2s compliment

    offset = val; //keep lower 16 bits
    //CommEnergyIC(WRITE, offset_reg, (signed short)val);
    return offset;
}

/*! \brief This function Calibrate VI.
 *
 *   Input the Voltage or Current register, and the actual value that it should be
 *   actualVal can be from a calibration meter or known value from a power supply
 * 
 *  \param reg              Address high register. 
 *  \param actualVal        Address low register.
 *  \return uint16_t        Gain Calibration
 */
uint16_t CalibrateVI(unsigned short reg, unsigned short actualVal)
{
    uint16_t gain, gain_old, val, m, gainReg;
    //sample the reading
    val = CommEnergyIC(READ, reg, 0xFFFF);
    val += CommEnergyIC(READ, reg, 0xFFFF);
    val += CommEnergyIC(READ, reg, 0xFFFF);
    val += CommEnergyIC(READ, reg, 0xFFFF);
    printf("Calibrate Val: %d \n", val);

    //get value currently in gain register
    switch (reg)
    {
    case UrmsA:
    {
        gainReg = UgainA;
    }
    case UrmsB:
    {
        gainReg = UgainB;
    }
    case UrmsC:
    {
        gainReg = UgainC;
    }
    case IrmsA:
    {
        gainReg = IgainA;
    }
    case IrmsB:
    {
        gainReg = IgainB;
    }
    case IrmsC:
    {
        gainReg = IgainC;
    }
    }

    gain = CommEnergyIC(READ, gainReg, 0xFFFF);
    gain_old = gain;
    m = actualVal;
    m = ((m * gain) / val);
    gain = m;

    printf("Calibrate gain_old: %d, gain_new: %d \n", gain_old, gain);
    //write new value to gain register
    CommEnergyIC(WRITE, CfgRegAccEn, 0x55AA); // enable register config access
    CommEnergyIC(WRITE, gainReg, gain);
    CommEnergyIC(WRITE, CfgRegAccEn, 0x0000); // end configuration

    return (gain);
}

/*! \brief This function get Voltage Line A.
 *
 *  \return double          Voltage Line A.
 */
double GetLineVoltageA()
{
    double voltage = (double)CommEnergyIC(READ, UrmsA, 0xFFFF);
    unsigned short voltageLSB = CommEnergyIC(READ, UrmsALSB, 0xFFFF);
    voltageLSB = voltageLSB << 8 | voltageLSB >> 8;
    double voltageRMS = (voltage * 0.01) + (((double)voltageLSB * 0.01) / 256);
    if (voltageRMS < 0)
    {
        voltageRMS = 0;
    }
    else if (voltageRMS > 65535)
    {
        voltageRMS = 65535;
    }
    return voltageRMS;
}

/*! \brief This function get Voltage Line B.
 *
 *  \return double          Voltage Line B.
 */
double GetLineVoltageB()
{
    unsigned short voltage = CommEnergyIC(READ, UrmsB, 0xFFFF);
    unsigned short voltageLSB = CommEnergyIC(READ, UrmsBLSB, 0xFFFF);
    voltageLSB = voltageLSB << 8 | voltageLSB >> 8;
    double voltageRMS = (voltage * 0.01) + ((voltageLSB * 0.01) / 256);
    if (voltageRMS < 0)
    {
        voltageRMS = 0;
    }
    else if (voltageRMS > 65535)
    {
        voltageRMS = 65535;
    }
    return voltageRMS;
}

/*! \brief This function get Voltage Line C.
 *
 *  \return double          Voltage Line C.
 */
double GetLineVoltageC()
{
    unsigned short voltage = CommEnergyIC(READ, UrmsC, 0xFFFF);
    unsigned short voltageLSB = CommEnergyIC(READ, UrmsCLSB, 0xFFFF);
    voltageLSB = voltageLSB << 8 | voltageLSB >> 8;
    double voltageRMS = (voltage * 0.01) + ((voltageLSB * 0.01) / 256);
    if (voltageRMS < 0)
    {
        voltageRMS = 0;
    }
    else if (voltageRMS > 65535)
    {
        voltageRMS = 65535;
    }
    return (double)voltageRMS;
}

/*! \brief This function get Current Line A.
 *
 *  \return double          Current Line A.
 */
double GetLineCurrentA()
{
    unsigned short current = CommEnergyIC(READ, IrmsA, 0xFFFF);
    unsigned short currentLSB = CommEnergyIC(READ, IrmsALSB, 0xFFFF);
    currentLSB = currentLSB << 8 | currentLSB >> 8;
    double currentRMS = (current * 0.001) + ((currentLSB * 0.001) / 256);
    if (currentRMS < 0)
    {
        currentRMS = 0;
    }
    else if (currentRMS > 65535)
    {
        currentRMS = 65535;
    }
    return currentRMS;
}

/*! \brief This function get Current Line B.
 *
 *  \return double          Current Line B.
 */
double GetLineCurrentB()
{
    unsigned short current = CommEnergyIC(READ, IrmsB, 0xFFFF);
    unsigned short currentLSB = CommEnergyIC(READ, IrmsBLSB, 0xFFFF);
    currentLSB = currentLSB << 8 | currentLSB >> 8;
    double currentRMS = (current * 0.001) + ((currentLSB * 0.001) / 256);
    if (currentRMS < 0)
    {
        currentRMS = 0;
    }
    else if (currentRMS > 65535)
    {
        currentRMS = 65535;
    }
    return currentRMS;
}

/*! \brief This function get Current Line A.
 *
 *  \return double          Current Line A.
 */
double GetLineCurrentC()
{
    unsigned short current = CommEnergyIC(READ, IrmsC, 0xFFFF);
    unsigned short currentLSB = CommEnergyIC(READ, IrmsCLSB, 0xFFFF);
    currentLSB = currentLSB << 8 | currentLSB >> 8;
    double currentRMS = (current * 0.001) + ((currentLSB * 0.001) / 256);
    if (currentRMS < 0)
    {
        currentRMS = 0;
    }
    else if (currentRMS > 65535)
    {
        currentRMS = 65535;
    }
    return currentRMS;
}

/*! \brief This function get Current Line Neutral.
 *
 *  \return double          Current Line Neutral.
 */
double GetLineCurrentN()
{
    unsigned short current = CommEnergyIC(READ, IrmsN, 0xFFFF);
    unsigned short currentRMS = (current * 0.001);
    return (double)currentRMS;
}

// ACTIVE POWER
double GetActivePowerA()
{
    int val = Read32Register(PmeanA, PmeanALSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}
double GetActivePowerB()
{
    int val = Read32Register(PmeanB, PmeanBLSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}
double GetActivePowerC()
{
    int val = Read32Register(PmeanC, PmeanCLSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}
double GetTotalActivePower()
{
    int val = Read32Register(PmeanT, PmeanTLSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}

// Active Fundamental Power
double GetTotalActiveFundPower()
{
    int val = Read32Register(PmeanTF, PmeanTFLSB);
    return (double)val * 0.00032;
}

// Active Harmonic Power
double GetTotalActiveHarPower()
{
    int val = Read32Register(PmeanTH, PmeanTHLSB);
    return (double)val * 0.00032;
}

// REACTIVE POWER
double GetReactivePowerA()
{
    int val = Read32Register(QmeanA, QmeanALSB);
    return (double)val * 0.00032;
}
double GetReactivePowerB()
{
    int val = Read32Register(QmeanB, QmeanBLSB);
    return (double)val * 0.00032;
}
double GetReactivePowerC()
{
    int val = Read32Register(QmeanC, QmeanCLSB);
    return (double)val * 0.00032;
}
double GetTotalReactivePower()
{
    int val = Read32Register(QmeanT, QmeanTLSB);
    return (double)val * 0.00032;
}

// APPARENT POWER
double GetApparentPowerA()
{
    int val = Read32Register(SmeanA, SmeanALSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}
double GetApparentPowerB()
{
    int val = Read32Register(SmeanB, SmeanBLSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}
double GetApparentPowerC()
{
    int val = Read32Register(SmeanC, SmeanCLSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}
double GetTotalApparentPower()
{
    int val = Read32Register(SmeanT, SAmeanTLSB);
    if ((val & 0x80000000) != 0)
    {
        val = (~val) + 1; //2s compliment
    }
    return (double)val * 0.00032;
}

// FREQUENCY
double GetFrequency()
{
    unsigned short freq = CommEnergyIC(READ, Freq, 0xFFFF);
    return (double)freq / 100;
}

// POWER FACTOR
double GetPowerFactorA()
{
    signed short pf = (signed short)CommEnergyIC(READ, PFmeanA, 0xFFFF);
    //if negative
    if (pf & (0x8000 != 0))
    {
        pf = (~pf) + 1;
    }
    return (double)pf / 1000;
}
double GetPowerFactorB()
{
    signed short pf = (signed short)CommEnergyIC(READ, PFmeanB, 0xFFFF);
    //if negative
    if (pf & (0x8000 != 0))
    {
        pf = (~pf) + 1;
    }
    return (double)pf / 1000;
}
double GetPowerFactorC()
{
    signed short pf = (signed short)CommEnergyIC(READ, PFmeanC, 0xFFFF);
    //if negative
    if (pf & (0x8000 != 0))
    {
        pf = (~pf) + 1;
    }
    return (double)pf / 1000;
}
double GetTotalPowerFactor()
{
    signed short pf = (signed short)CommEnergyIC(READ, PFmeanT, 0xFFFF);
    //if negative
    if (pf & (0x8000 != 0))
    {
        pf = (~pf) + 1;
    }
    return (double)pf / 1000;
}

// MEAN PHASE ANGLE
double GetPhaseA()
{
    //unsigned short angleA = (unsigned short)CommEnergyIC(READ, PAngleA, 0xFFFF);
    unsigned short angleA = (unsigned short)CommEnergyIC(READ, UangleA, 0xFFFF);

    return (double)angleA / 10;
}
double GetPhaseB()
{
    //unsigned short angleB = (unsigned short)CommEnergyIC(READ, PAngleB, 0xFFFF);
    unsigned short angleB = (unsigned short)CommEnergyIC(READ, UangleB, 0xFFFF);

    return (double)angleB / 10;
}
double GetPhaseC()
{
    //unsigned short angleC = (unsigned short)CommEnergyIC(READ, PAngleC, 0xFFFF);
    unsigned short angleC = (unsigned short)CommEnergyIC(READ, UangleC, 0xFFFF);

    return (double)angleC / 10;
}

// TEMPERATURE
double GetTemperature()
{
    short int atemp = (short int)CommEnergyIC(READ, Temp, 0xFFFF);
    return (double)atemp;
}

/* Gets the Register Value if Desired */
// REGISTER
double GetValueRegister(unsigned short registerRead)
{
    return (double)CommEnergyIC(READ, registerRead, 0xFFFF); //returns value register
}

// REGULAR ENERGY MEASUREMENT

// FORWARD ACTIVE ENERGY
double GetImportEnergy()
{
    unsigned short ienergyT = CommEnergyIC(READ, APenergyT, 0xFFFF);
    return (double)ienergyT / 100 / 3200; //returns kWh
}
// unsigned short ienergyA = CommEnergyIC(READ, APenergyA, 0xFFFF);
// unsigned short ienergyB = CommEnergyIC(READ, APenergyB, 0xFFFF);
// unsigned short ienergyC = CommEnergyIC(READ, APenergyC, 0xFFFF);

// FORWARD REACTIVE ENERGY
double GetImportReactiveEnergy()
{
    unsigned short renergyT = CommEnergyIC(READ, RPenergyT, 0xFFFF);
    return (double)renergyT / 100 / 3200; //returns kWh
}
// unsigned short renergyA = CommEnergyIC(READ, RPenergyA, 0xFFFF);
// unsigned short renergyB = CommEnergyIC(READ, RPenergyB, 0xFFFF);
// unsigned short renergyC = CommEnergyIC(READ, RPenergyC, 0xFFFF);

// APPARENT ENERGY
double GetImportApparentEnergy()
{
    unsigned short senergyT = CommEnergyIC(READ, SAenergyT, 0xFFFF);
    return (double)senergyT / 100 / 3200; //returns kWh
}
// unsigned short senergyA = CommEnergyIC(READ, SenergyA, 0xFFFF);
// unsigned short senergyB = CommEnergyIC(READ, SenergyB, 0xFFFF);
// unsigned short senergyC = CommEnergyIC(READ, SenergyC, 0xFFFF);

// REVERSE ACTIVE ENERGY
double GetExportEnergy()
{
    unsigned short eenergyT = CommEnergyIC(READ, ANenergyT, 0xFFFF);
    return (double)eenergyT / 100 / 3200; //returns kWh
}
// unsigned short eenergyA = CommEnergyIC(READ, ANenergyA, 0xFFFF);
// unsigned short eenergyB = CommEnergyIC(READ, ANenergyB, 0xFFFF);
// unsigned short eenergyC = CommEnergyIC(READ, ANenergyC, 0xFFFF);

// REVERSE REACTIVE ENERGY
double GetExportReactiveEnergy()
{
    unsigned short reenergyT = CommEnergyIC(READ, RNenergyT, 0xFFFF);
    return (double)reenergyT / 100 / 3200; //returns kWh
}
// unsigned short reenergyA = CommEnergyIC(READ, RNenergyA, 0xFFFF);
// unsigned short reenergyB = CommEnergyIC(READ, RNenergyB, 0xFFFF);
// unsigned short reenergyC = CommEnergyIC(READ, RNenergyC, 0xFFFF);

/* System Status Registers */
unsigned short GetSysStatus0()
{
    return CommEnergyIC(READ, EMMIntState0, 0xFFFF);
}
unsigned short GetSysStatus1()
{
    return CommEnergyIC(READ, EMMIntState1, 0xFFFF);
}
unsigned short GetMeterStatus0()
{
    return CommEnergyIC(READ, EMMState0, 0xFFFF);
}
unsigned short GetMeterStatus1()
{
    return CommEnergyIC(READ, EMMState1, 0xFFFF);
}

/* Checksum Error Function */
bool calibrationError()
{
    bool CS0, CS1, CS2, CS3;
    unsigned short systemstatus0 = GetSysStatus0();

    if (systemstatus0 & 0x4000)
    {
        CS0 = true;
    }
    else
    {
        CS0 = false;
    }

    if (systemstatus0 & 0x0100)
    {
        CS1 = true;
    }
    else
    {
        CS1 = false;
    }
    if (systemstatus0 & 0x0400)
    {
        CS2 = true;
    }
    else
    {
        CS2 = false;
    }
    if (systemstatus0 & 0x0100)
    {
        CS3 = true;
    }
    else
    {
        CS3 = false;
    }

    if (CS0 || CS1 || CS2 || CS3)
        return (true);
    else
        return (false);
}

/*! \brief This function initialize the configuration.
 *
 *  \param lineFreq         Configure working frequency. 
 *  \param pgagain          .
 *  \param ugain            .
 *  \param igainA           .
 *  \param igainB           .
 *  \param igainC           .
 */
void begin_M90E32AS(unsigned short lineFreq, unsigned short pgagain, unsigned short ugain, unsigned short igainA, unsigned short igainB, unsigned short igainC)
{

    unsigned short _lineFreq = lineFreq; //frequency of power
    unsigned short _pgagain = pgagain;   //PGA Gain for current channels
    unsigned short _ugain = ugain;       //voltage rms gain
    unsigned short _igainA = igainA;     //CT1
    unsigned short _igainB = igainB;     //CT2 - not used for single split phase meter
    unsigned short _igainC = igainC;     //CT2 for single split phase meter - CT3 otherwise

    //printf("Connecting to ATM-M90E32AS \n");

    //determine proper low and high frequency threshold
    unsigned short FreqHiThresh;
    unsigned short FreqLoThresh;

    if (_lineFreq == 4485 || _lineFreq == 5231 || _lineFreq == 5255)
    {
        //North America power frequency
        FreqHiThresh = 61 * 100;
        FreqLoThresh = 59 * 100;
    }
    else
    {
        FreqHiThresh = 51 * 100;
        FreqLoThresh = 49 * 100;
    }

    //calculation for voltage sag threshold - assumes we do not want to go under 90v for split phase and 190v otherwise
    unsigned short vSagTh;
    unsigned short sagV;
    if (_lineFreq == 4485 || _lineFreq == 5231 || _lineFreq == 5255)
    {
        sagV = 90;
    }
    else
    {
        sagV = 190;
    }

    vSagTh = (sagV * 100 * sqrt(2)) / (2 * _ugain / 32768);

    //Initialize registers
    CommEnergyIC(WRITE, SoftReset, 0x789A);   // Perform soft reset
    CommEnergyIC(WRITE, CfgRegAccEn, 0x55AA); // enable register config access
    CommEnergyIC(WRITE, MeterEn, 0x0001);     // Enable Metering

    CommEnergyIC(WRITE, SagTh, vSagTh);          // Voltage sag threshold
    CommEnergyIC(WRITE, FreqHiTh, FreqHiThresh); // High frequency threshold - 61.00Hz
    CommEnergyIC(WRITE, FreqLoTh, FreqLoThresh); // Lo frequency threshold - 59.00Hz
    CommEnergyIC(WRITE, EMMIntEn0, 0xFFFF);      //0xB76F   // 0xFFFF All Enable interrupts
    CommEnergyIC(WRITE, EMMIntEn1, 0xFFFF);      //0xDDFD   // 0xffff Enable interrupts
    CommEnergyIC(WRITE, EMMIntState0, 0x0001);   // Clear interrupt flags
    CommEnergyIC(WRITE, EMMIntState1, 0x0001);   // Clear interrupt flags
    CommEnergyIC(WRITE, ZXConfig, 0x0A55);       // ZX2, ZX1, ZX0 pin config  DISABLE

    //Set metering config values (CONFIG)
    CommEnergyIC(WRITE, PLconstH, 0x0861);  // PL Constant MSB (default) - Meter Constant = 3200 - PL Constant = 140625000
    CommEnergyIC(WRITE, PLconstL, 0xC468);  // PL Constant LSB (default) - this is 4C68 in the application note, which is incorrect
    CommEnergyIC(WRITE, MMode0, _lineFreq); // Mode Config (frequency set in main program)
    CommEnergyIC(WRITE, MMode1, _pgagain);  // PGA Gain Configuration for Current Channels - 0x002A (x4) // 0x0015 (x2) // 0x0000 (1x)
    CommEnergyIC(WRITE, PStartTh, 0x0AFC);  // All phase Active Startup Power Threshold - 50% of startup current = 0.09W/0.00032 = 2812.5
    CommEnergyIC(WRITE, QStartTh, 0x0000);  // All phase Reactive Startup Power Threshold
    CommEnergyIC(WRITE, SStartTh, 0x0AEC);  // All phase Apparent Startup Power Threshold
    CommEnergyIC(WRITE, PPhaseTh, 0x00BC);  // Each phase Active Phase Threshold = 10% of startup current = 0.06W/0.00032 = 187.5
    CommEnergyIC(WRITE, QPhaseTh, 0x0000);  // Each phase Reactive Phase Threshold
    CommEnergyIC(WRITE, SPhaseTh, 0x00BC);  // Each phase Apparent  Phase Threshold

    //Set metering calibration values (CALIBRATION)
    CommEnergyIC(WRITE, PQGainA, 0x0000);  // Line calibration gain
    CommEnergyIC(WRITE, PhiA, 0x0032);     // Line calibration angle - accounts for a ~2.5 degree error from 9V AC transformer
    CommEnergyIC(WRITE, PQGainB, 0x0000);  // Line calibration gain
    CommEnergyIC(WRITE, PhiB, 0x0032);     // Line calibration angle
    CommEnergyIC(WRITE, PQGainC, 0x0000);  // Line calibration gain
    CommEnergyIC(WRITE, PhiC, 0x0032);     // Line calibration angle
    CommEnergyIC(WRITE, PoffsetA, 0xFFDC); // A line active power offset
    CommEnergyIC(WRITE, QoffsetA, 0xFFDC); // A line reactive power offset
    CommEnergyIC(WRITE, PoffsetB, 0xFFDC); // B line active power offset
    CommEnergyIC(WRITE, QoffsetB, 0xFFDC); // B line reactive power offset
    CommEnergyIC(WRITE, PoffsetC, 0xFFDC); // C line active power offset
    CommEnergyIC(WRITE, QoffsetC, 0xFFDC); // C line reactive power offset

    //Set metering calibration values (HARMONIC)
    CommEnergyIC(WRITE, POffsetAF, 0xFFDC); // A Fund. active power offset
    CommEnergyIC(WRITE, POffsetBF, 0xFFDC); // B Fund. active power offset
    CommEnergyIC(WRITE, POffsetCF, 0xFFDC); // C Fund. active power offset
    CommEnergyIC(WRITE, PGainAF, 0x0000);   // A Fund. active power gain
    CommEnergyIC(WRITE, PGainBF, 0x0000);   // B Fund. active power gain
    CommEnergyIC(WRITE, PGainCF, 0x0000);   // C Fund. active power gain

    //Set measurement calibration values (ADJUST)
    CommEnergyIC(WRITE, UgainA, _ugain);   // A Voltage rms gain
    CommEnergyIC(WRITE, IgainA, _igainA);  // A line current gain
    CommEnergyIC(WRITE, UoffsetA, 0x0000); // 0x1D4C A Voltage offset
    CommEnergyIC(WRITE, IoffsetA, 0x0000); // 0xFC60 A line current offset

    CommEnergyIC(WRITE, UgainB, _ugain);   // B Voltage rms gain
    CommEnergyIC(WRITE, IgainB, _igainB);  // B line current gain
    CommEnergyIC(WRITE, UoffsetB, 0x0000); // 0x1D4C B Voltage offset
    CommEnergyIC(WRITE, IoffsetB, 0x0000); // 0xFC60B line current offset

    CommEnergyIC(WRITE, UgainC, _ugain);   // C Voltage rms gain
    CommEnergyIC(WRITE, IgainC, _igainC);  // C line current gain
    CommEnergyIC(WRITE, UoffsetC, 0x0000); // 0x1D4CC Voltage offset
    CommEnergyIC(WRITE, IoffsetC, 0x0000); // 0xFC60C line current offset

    CommEnergyIC(WRITE, CfgRegAccEn, 0x0000); // end configuration

    ESP_LOGI(TAG, "begin_M90E32AS OK");
}

uint8_t secondCharger, minuteCharger, hourCharger, secondChargerOld, minuteChargerOld, hourChargerOld;
uint8_t hourstamp, minutestamp, secondstamp;
bool timestamp = true;

void read_initial_analyzer()
{
    double PhaseA = 0;
    double PhaseB = 0;
    double PhaseC = 0;

    if (!SincI2C) //Only take value when  SincI2C = false
    {             //Sincronizate with Phoenix read task

        unsigned short sys0 = GetSysStatus0();  //EMMState0
        unsigned short sys1 = GetSysStatus1();  //EMMState1
        // unsigned short en0 = GetMeterStatus0(); //EMMIntState0
        // unsigned short en1 = GetMeterStatus1(); //EMMIntState1

        //printf("Sys Status: S0:0x %d, S1:0x %d \n", sys0, sys1);
        //printf("Meter Status: E0:0x %d, E1:0x %d \n", en0, en1);
        vTaskDelay(10 / portTICK_RATE_MS);

        //if true the MCU is not getting data from the energy meter
        if (sys0 == 65535 || sys0 == 0)
        {
            // led_state_maxV(2, 2);
            ESP_LOGI(TAG, "Error: Not receiving data from energy meter - check your connections \n");
            voltageA = 0;
            currentA = 0;
            powerfactorA = 0;
            powerA = 0;
            powerReacA = 0;
            powerAppA = 0;
            voltageB = 0;
            currentB = 0;
            powerfactorB = 0;
            powerB = 0;
            powerReacB = 0;
            powerAppB = 0;
            voltageC = 0;
            currentC = 0;
            powerfactorC = 0;
            powerC = 0;
            powerReacC = 0;
            powerAppC = 0;
            temperature = 0;
        }
        else
        {
            voltageA = GetLineVoltageA();
            voltageB = GetLineVoltageB();
            voltageC = GetLineVoltageC();
            PhaseA = GetPhaseA();
            PhaseB = GetPhaseB();
            PhaseC = GetPhaseC();

            printf("Voltage A: %.1f [V] \n", voltageA);
            printf("Voltage B: %.1f [V] \n", voltageB);
            printf("Voltage C: %.1f [V] \n", voltageC);

            printf("PhaseAngle A: %f [°] \n", PhaseA);
            printf("PhaseAngle B: %f [°] \n", PhaseB);
            printf("PhaseAngle C: %f [°] \n", PhaseC);

            if (voltageA < 100)
            {
                printf("No se detecto la Fase 1");
                Phase1 = false;
            }
            else
            {
                Phase1 = true;
            }
            if (voltageB < 100)
            {
                printf("No se detecto la Fase 2");
                Phase2 = false;
            }
            else
            {
                Phase2 = true;
            }
            if (voltageC < 100)
            {
                printf("No se detecto la Fase 3");
                Phase3 = false;
            }
            else
            {
                Phase3 = true;
            }
        }
    }
}

void grid_analyzer_task(void *arg)
{
    ESP_LOGI(TAG, "Initiation grid_analyzer_task");
    vTaskDelay(7000 / portTICK_RATE_MS);
    // float voltageA, currentA, powerfactorA, powerA, powerReacA, powerAppA,
    // 	voltageB, currentB, powerfactorB, powerB, powerReacB, powerAppB,
    // 	voltageC, currentC, powerfactorC, powerC, powerReacC, powerAppC,
    // 	temperature;

    for (;;)
    {
        if (!SincI2C) //Only take value when  SincI2C = false
        {             //Sincronizate with Phoenix read task

            unsigned short sys0 = GetSysStatus0();  //EMMState0
            unsigned short sys1 = GetSysStatus1();  //EMMState1
            unsigned short en0 = GetMeterStatus0(); //EMMIntState0
            unsigned short en1 = GetMeterStatus1(); //EMMIntState1

            //printf("Sys Status: S0:0x %d, S1:0x %d \n", sys0, sys1);
            //printf("Meter Status: E0:0x %d, E1:0x %d \n", en0, en1);
            vTaskDelay(10 / portTICK_RATE_MS);

            //if true the MCU is not getting data from the energy meter
            if (sys0 == 65535 || sys0 == 0)
            {
                // led_state_maxV(2, 2);
                ESP_LOGI(TAG, "Error: Not receiving data from energy meter - check your connections \n");
                voltageA = 0;
                currentA = 0;
                powerfactorA = 0;
                powerA = 0;
                powerReacA = 0;
                powerAppA = 0;
                voltageB = 0;
                currentB = 0;
                powerfactorB = 0;
                powerB = 0;
                powerReacB = 0;
                powerAppB = 0;
                voltageC = 0;
                currentC = 0;
                powerfactorC = 0;
                powerC = 0;
                powerReacC = 0;
                powerAppC = 0;
                temperature = 0;
            }
            else
            {
                voltageA = GetLineVoltageA();
                voltageB = GetLineVoltageB();
                voltageC = GetLineVoltageC();
                currentA = GetLineCurrentA();
                currentB = GetLineCurrentB();
                currentC = GetLineCurrentC();
                temperature = GetTemperature();
                powerfactorA = GetPowerFactorA();
                powerfactorB = GetPowerFactorB();
                powerfactorC = GetPowerFactorC();
                powerA = GetActivePowerA();
                powerB = GetActivePowerB();
                powerC = GetActivePowerC();
                powerReacA = GetReactivePowerA();
                powerReacB = GetReactivePowerB();
                powerReacC = GetReactivePowerC();
                powerAppA = GetApparentPowerA();
                powerAppB = GetApparentPowerB();
                powerAppC = GetApparentPowerC();

                uint8_t dataTime[6];
                getTime(dataTime);

                if (voltageA < 100)
                {
                    printf("No se detecto la Fase 1");
                    Phase1 = false;
                }
                else
                {
                    Phase1 = true;
                }
                if (voltageB < 100)
                {
                    printf("No se detecto la Fase 2");
                    Phase2 = false;
                }
                else
                {
                    Phase2 = true;
                }
                if (voltageC < 100)
                {
                    printf("No se detecto la Fase 3");
                    Phase3 = false;
                }
                else
                {
                    Phase3 = true;
                }

#ifdef DEBUG
                printf("\033[0;32m");
                printf("System Time: %d:%d:%d \n", dataTime[2], dataTime[1], dataTime[0]);
                printf("\033[0m");

                printf("=============FASE A=============== \n");
                printf("Voltage 1: %.1f [V] \n", voltageA);
                printf("Current 1: %.1f [A] \n", currentA);
                printf("Power 1: %.1f [W] \n", powerA);
                printf("Factor 1: %.1f  \n", powerfactorA);
                printf("PowerR 1: %.1f [VAR] \n", powerReacA);
                printf("PowerApp 1: %.1f [VA] \n", powerAppA);
                printf("=============FASE B=============== \n");
                printf("Voltage 2: %.1f [V] \n", voltageB);
                printf("Current 2: %.1f [A] \n", currentB);
                printf("Power 2: %.1f [W] \n", powerB);
                printf("Factor 2: %.1f [V] \n", powerfactorB);
                printf("PowerR 2: %.1f [VAR] \n", powerReacB);
                printf("PowerApp 2: %.1f [VA] \n", powerAppB);
                printf("=============FASE C=============== \n");
                printf("Voltage 3: %.1f [V] \n", voltageC);
                printf("Current 3: %.1f [A] \n", currentC);
                printf("Power 3: %.1f [W] \n", powerC);
                printf("Factor 3: %.1f [V] \n", powerfactorC);
                printf("PowerR 3: %.1f [VAR] \n", powerReacC);
                printf("PowerApp 3: %.1f [VA] \n", powerAppC);
                printf("============================== \n");
                printf("Temperature: %.1f [C] \n", temperature);
                printf("============================== \n");
#endif
            }

            SincI2C = true;
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}