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


// #define WRITE 0 // WRITE SPI
// #define READ 1  // READ SPI

/*! \brief This Status Registers using configuration
 */
#define MeterEn 0x00       // Metering Enable
#define ChannelMapI 0x01   // Current Channel Mapping Configuration
#define ChannelMapU 0x02   // Voltage Channel Mapping Configuration
#define SagPeakDetCfg 0x05 // Sag and Peak Detector Period Configuration
#define OVth 0x06          // Over Voltage Threshold
#define ZXConfig 0x07      // Zero-Crossing Config
#define SagTh 0x08         // Voltage Sag Th
#define PhaseLossTh 0x09   // Voltage Phase Losing Th
#define INWarnTh 0x0A      // Neutral Current (Calculated) Warning Threshold
#define OIth 0x0B          // Over Current Threshold
#define FreqLoTh 0x0C      // Low Threshold for Frequency Detection
#define FreqHiTh 0x0D      // High Threshold for Frequency Detection
#define PMPwrCtrl 0x0E     // Partial Measurement Mode Power Control
#define IRQ0MergeCfg 0x0F  // IRQ0 Merge Configuration

/*! \brief This Low Power Mode Registers Registers using configuration
 */
#define DetectCtrl 0x10  // Current Detect Control
#define DetectTh1 0x11   // Channel 1 Current Threshold in Detection Mode
#define DetectTh2 0x12   // Channel 2 Current Threshold in Detection Mode
#define DetectTh3 0x13   // Channel 3 Current Threshold in Detection Mode
#define IDCoffsetA 0x14  // Phase A Current DC offset
#define IDCoffsetB 0x15  // Phase B Current DC offset
#define IDCoffsetC 0x16  // Phase C Current DC offset
#define UDCoffsetA 0x17  // Voltage DC offset for Channel A
#define UDCoffsetB 0x18  // Voltage DC offset for Channel B
#define UDCoffsetC 0x19  // Voltage DC offset for Channel C
#define UGainTAB 0x1A    // Voltage Gain Temperature Compensation for Phase A/B
#define UGainTC 0x10B    // Voltage Gain Temperature Compensation for Phase C
#define PhiFreqComp 0x1C // Phase Compensation for Frequency
#define LOGIrms0 0x20    // Current (Log Irms0) Configuration for Segment Compensation
#define LOGIrms1 0x21    // Current (Log Irms1) Configuration for Segment Compensation
#define F0 0x22          // Nominal Frequency
#define T0 0x23          // Nominal Temperature
#define PhiAIrms01 0x24  // Phase A Phase Compensation for Current Segment 0 and 1
#define PhiAIrms2 0x25   // Phase A Phase Compensation for Current Segment 2
#define GainAIrms01 0x26 // Phase A Gain Compensation for Current Segment 0 and 1
#define GainAIrms2 0x27  // Phase A Gain Compensation for Current Segment 2
#define PhiBIrms01 0x28  // Phase B Phase Compensation for Current Segment 0 and 1
#define PhiBIrms2 0x29   // Phase B Phase Compensation for Current Segment 2
#define GainBIrms01 0x2A // Phase B Gain Compensation for Current Segment 0 and 1
#define GainBIrms2 0x2B  // Phase B Gain Compensation for Current Segment 2
#define PhiCIrms01 0x2C  // Phase C Phase Compensation for Current Segment 0 and 1
#define PhiCIrms2 0x2D   // Phase C Phase Compensation for Current Segment 2
#define GainCIrms01 0x2E // Phase C Gain Compensation for Current Segment 0 and 1
#define GainCIrms2 0x2F  // Phase C Gain Compensation for Current Segment 2

/*! \brief This Configuration Registers Registers using configuration
 */
#define PLconstH 0x31 // High Word of PL_Constant
#define PLconstL 0x32 // Low Word of PL_Constant
#define MMode0 0x33   // Metering Mode Config
#define MMode1 0x34   // PGA Gain Configuration for Current Channels
#define PStartTh 0x35 // Startup Power Th (P)
#define QStartTh 0x36 // Startup Power Th (Q)
#define SStartTh 0x37 // Startup Power Th (S)
#define PPhaseTh 0x38 // Startup Power Accum Th (P)
#define QPhaseTh 0x39 // Startup Power Accum Th (Q)
#define SPhaseTh 0x3A // Startup Power Accum Th (S)

/*! \brief This Calibration Registers Registers using configuration
 */
#define PoffsetA 0x41 // A Line Power Offset (P)
#define QoffsetA 0x42 // A Line Power Offset (Q)
#define PoffsetB 0x43 // B Line Power Offset (P)
#define QoffsetB 0x44 // B Line Power Offset (Q)
#define PoffsetC 0x45 // C Line Power Offset (P)
#define QoffsetC 0x46 // C Line Power Offset (Q)
#define PQGainA 0x47  // A Line Calibration Gain
#define PhiA 0x48     // A Line Calibration Angle
#define PQGainB 0x49  // B Line Calibration Gain
#define PhiB 0x4A     // B Line Calibration Angle
#define PQGainC 0x4B  // C Line Calibration Gain
#define PhiC 0x4C     // C Line Calibration Angle

/*! \brief This Fundamental/Harmonic Energy Calibration Registers Registers using configuration
 */
#define POffsetAF 0x51 // A Fund Power Offset (P)
#define POffsetBF 0x52 // B Fund Power Offset (P)
#define POffsetCF 0x53 // C Fund Power Offset (P)
#define PGainAF 0x54   // A Fund Power Gain (P)
#define PGainBF 0x55   // B Fund Power Gain (P)
#define PGainCF 0x56   // C Fund Power Gain (P)

/*! \brief This Measurement Calibration Registers Registers using configuration
 */
#define UgainA 0x61   // A Voltage RMS Gain
#define IgainA 0x62   // A Current RMS Gain
#define UoffsetA 0x63 // A Voltage RMS Offset
#define IoffsetA 0x64 // A Current RMS Offset
#define UgainB 0x65   // B Voltage RMS Gain
#define IgainB 0x66   // B Current RMS Gain
#define UoffsetB 0x67 // B Voltage RMS Offset
#define IoffsetB 0x68 // B Current RMS Offset
#define UgainC 0x69   // C Voltage RMS Gain
#define IgainC 0x6A   // C Current RMS Gain
#define UoffsetC 0x6B // C Voltage RMS Offset
#define IoffsetC 0x6C // C Current RMS offset

/*! \brief This EMM Status Registers Registers using configuration
 */
#define SoftReset 0x70    // Software Reset
#define EMMState0 0x71    // EMM State 0
#define EMMState1 0x72    // EMM State 1
#define EMMIntState0 0x73 // EMM Interrupt Status 0
#define EMMIntState1 0x74 // EMM Interrupt Status 1
#define EMMIntEn0 0x75    // EMM Interrupt Enable 0
#define EMMIntEn1 0x76    // EMM Interrupt Enable 1
#define LastSPIData 0x78  // Last Read/Write SPI Value
#define CRCErrStatus 0x79 // CRC Error Status
#define CRCDigest 0x7A    // CRC Digest
#define CfgRegAccEn 0x7F  // Configure Register Access Enable

/*! \brief This Energy Registers Registers using configuration
 */
#define APenergyT 0x80 // Total Forward Active
#define APenergyA 0x81 // A Forward Active
#define APenergyB 0x82 // B Forward Active
#define APenergyC 0x83 // C Forward Active
#define ANenergyT 0x84 // Total Reverse Active
#define ANenergyA 0x85 // A Reverse Active
#define ANenergyB 0x86 // B Reverse Active
#define ANenergyC 0x87 // C Reverse Active
#define RPenergyT 0x88 // Total Forward Reactive
#define RPenergyA 0x89 // A Forward Reactive
#define RPenergyB 0x8A // B Forward Reactive
#define RPenergyC 0x8B // C Forward Reactive
#define RNenergyT 0x8C // Total Reverse Reactive
#define RNenergyA 0x8D // A Reverse Reactive
#define RNenergyB 0x8E // B Reverse Reactive
#define RNenergyC 0x8F // C Reverse Reactive
#define SAenergyT 0x90 // Total Apparent Energy
#define SenergyA 0x91  // A Apparent Energy
#define SenergyB 0x92  // B Apparent Energy
#define SenergyC 0x93  // C Apparent Energy

/*! \brief This Fundamental/Harmonic Energy Registers Registers using configuration
 */
#define APenergyTF 0xA0 // Total Forward Fund. Energy
#define APenergyAF 0xA1 // A Forward Fund. Energy
#define APenergyBF 0xA2 // B Forward Fund. Energy
#define APenergyCF 0xA3 // C Forward Fund. Energy
#define ANenergyTF 0xA4 // Total Reverse Fund Energy
#define ANenergyAF 0xA5 // A Reverse Fund. Energy
#define ANenergyBF 0xA6 // B Reverse Fund. Energy
#define ANenergyCF 0xA7 // C Reverse Fund. Energy
#define APenergyTH 0xA8 // Total Forward Harm. Energy
#define APenergyAH 0xA9 // A Forward Harm. Energy
#define APenergyBH 0xAA // B Forward Harm. Energy
#define APenergyCH 0xAB // C Forward Harm. Energy
#define ANenergyTH 0xAC // Total Reverse Harm. Energy
#define ANenergyAH 0xAD // A Reverse Harm. Energy
#define ANenergyBH 0xAE // B Reverse Harm. Energy
#define ANenergyCH 0xAF // C Reverse Harm. Energy

/*! \brief This Power & P.F. Registers Registers using configuration
 */
#define PmeanA 0xB1     // A Mean Power (P)
#define PmeanT 0xB0     // Total Mean Power (P)
#define PmeanB 0xB2     // B Mean Power (P)
#define PmeanC 0xB3     // C Mean Power (P)
#define QmeanT 0xB4     // Total Mean Power (Q)
#define QmeanA 0xB5     // A Mean Power (Q)
#define QmeanB 0xB6     // B Mean Power (Q)
#define QmeanC 0xB7     // C Mean Power (Q)
#define SmeanT 0xB8     // Total Mean Power (S)
#define SmeanA 0xB9     // A Mean Power (S)
#define SmeanB 0xBA     // B Mean Power (S)
#define SmeanC 0xBB     // C Mean Power (S)
#define PFmeanT 0xBC    // Mean Power Factor
#define PFmeanA 0xBD    // A Power Factor
#define PFmeanB 0xBE    // B Power Factor
#define PFmeanC 0xBF    // C Power Factor
#define PmeanTLSB 0xC0  // Lower Word (Tot. Act. Power)
#define PmeanALSB 0xC1  // Lower Word (A Act. Power)
#define PmeanBLSB 0xC2  // Lower Word (B Act. Power)
#define PmeanCLSB 0xC3  // Lower Word (C Act. Power)
#define QmeanTLSB 0xC4  // Lower Word (Tot. React. Power)
#define QmeanALSB 0xC5  // Lower Word (A React. Power)
#define QmeanBLSB 0xC6  // Lower Word (B React. Power)
#define QmeanCLSB 0xC7  // Lower Word (C React. Power)
#define SAmeanTLSB 0xC8 // Lower Word (Tot. App. Power)
#define SmeanALSB 0xC9  // Lower Word (A App. Power)
#define SmeanBLSB 0xCA  // Lower Word (B App. Power)
#define SmeanCLSB 0xCB  // Lower Word (C App. Power)

/*! \brief This Fund/Harm Power & V/I RMS Registers Registers using configuration
 */
#define PmeanTF 0xD0    // Total Active Fund. Power
#define PmeanAF 0xD1    // A Active Fund. Power
#define PmeanBF 0xD2    // B Active Fund. Power
#define PmeanCF 0xD3    // C Active Fund. Power
#define PmeanTH 0xD4    // Total Active Harm. Power
#define PmeanAH 0xD5    // A Active Harm. Power
#define PmeanBH 0xD6    // B Active Harm. Power
#define PmeanCH 0xD7    // C Active Harm. Power
#define UrmsA 0xD9      // A RMS Voltage
#define UrmsB 0xDA      // B RMS Voltage
#define UrmsC 0xDB      // C RMS Voltage
#define IrmsN 0xDC      // N Line Cakculated Current RMS
#define IrmsA 0xDD      // A RMS Current
#define IrmsB 0xDE      // B RMS Current
#define IrmsC 0xDF      // C RMS Current
#define PmeanTFLSB 0xE0 // Lower Word (Tot. Act. Fund. Power)
#define PmeanAFLSB 0xE1 // Lower Word (A Act. Fund. Power)
#define PmeanBFLSB 0xE2 // Lower Word (B Act. Fund. Power)
#define PmeanCFLSB 0xE3 // Lower Word (C Act. Fund. Power)
#define PmeanTHLSB 0xE4 // Lower Word (Tot. Act. Harm. Power)
#define PmeanAHLSB 0xE5 // Lower Word (A Act. Harm. Power)
#define PmeanBHLSB 0xE6 // Lower Word (B Act. Harm. Power)
#define PmeanCHLSB 0xE7 // Lower Word (C Act. Harm. Power)
///////////////// 0xE8	    // Reserved Register
#define UrmsALSB 0xE9 // Lower Word (A RMS Voltage)
#define UrmsBLSB 0xEA // Lower Word (B RMS Voltage)
#define UrmsCLSB 0xEB // Lower Word (C RMS Voltage)
///////////////// 0xEC	    // Reserved Register
#define IrmsALSB 0xED // Lower Word (A RMS Current)
#define IrmsBLSB 0xEE // Lower Word (B RMS Current)
#define IrmsCLSB 0xEF // Lower Word (C RMS Current)

/*! \brief This Peak, Frequency, Angle & Temperature Registers Registers using configuration
 */
#define UPeakA 0xF1 // Channel A Voltage Peak
#define UPeakB 0xF2 // Channel B Voltage Peak
#define UPeakC 0xF3 // Channel C Voltage Peak
///////////////// 0xF4	    // Reserved Register
#define IPeakA 0xF5  // Channel A Current Peak
#define IPeakB 0xF6  // Channel B Current Peak
#define IPeakC 0xF7  // Channel C Current Peak
#define Freq 0xF8    // Frequency
#define PAngleA 0xF9 // A Mean Phase Angle
#define PAngleB 0xFA // B Mean Phase Angle
#define PAngleC 0xFB // C Mean Phase Angle
#define Temp 0xFC    // Measured Temperature
#define UangleA 0xFD // A Voltage Phase Angle
#define UangleB 0xFE // B Voltage Phase Angle
#define UangleC 0xFF // C Voltage Phase Angle




/*! \brief This function initialize the configuration.
 *
 *  \param lineFreq         Configure working frequency. 
 *  \param pgagain          .
 *  \param ugain            .
 *  \param igainA           .
 *  \param igainB           .
 *  \param igainC           .
 */
void begin_M90E32AS(unsigned short lineFreq, unsigned short pgagain, unsigned short ugain, unsigned short igainA, unsigned short igainB, unsigned short igainC);

/*! \brief This function Calculate VIO ffset.
 *
 *  For getting the lower registers of energy and calculating the offset
 *  this should only be run when all inputs are disconnected
 * 
 *  \param regh_addr        Address high register. 
 *  \param regl_addr        Address low register.
 *  \return uint16_t        Voltage Offset
 */
uint16_t  CalculateVIOffset(unsigned short regh_addr, unsigned short regl_addr /*, unsigned short offset_reg*/);

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
uint16_t CalculatePowerOffset(unsigned short regh_addr, unsigned short regl_addr /*, unsigned short offset_reg*/);

/*! \brief This function Calibrate VI.
 *
 *   Input the Voltage or Current register, and the actual value that it should be
 *   actualVal can be from a calibration meter or known value from a power supply
 * 
 *  \param reg              Address high register. 
 *  \param actualVal        Address low register.
 *  \return uint16_t        Gain Calibration
 */
uint16_t CalibrateVI(unsigned short reg, unsigned short actualVal);

double GetRegister(uint8_t addr);

/*! \brief This function get Voltage Line A.
 *
 *  \return double          Voltage Line A.
 */
double GetLineVoltageA();

/*! \brief This function get Voltage Line B.
 *
 *  \return double          Voltage Line B.
 */
double GetLineVoltageB();

/*! \brief This function get Voltage Line C.
 *
 *  \return double          Voltage Line C.
 */
double GetLineVoltageC();


/*! \brief This function get Current Line A.
 *
 *  \return double          Current Line A.
 */
double GetLineCurrentA();

/*! \brief This function get Current Line B.
 *
 *  \return double          Current Line B.
 */
double GetLineCurrentB();

/*! \brief This function get Current Line C.
 *
 *  \return double          Current Line C.
 */
double GetLineCurrentC();

/*! \brief This function get Current Line Neutral.
 *
 *  \return double          Current Line Neutral.
 */
double GetLineCurrentN();

double GetActivePowerA();
double GetActivePowerB();
double GetActivePowerC();
double GetTotalActivePower();

double GetTotalActiveFundPower();
double GetTotalActiveHarPower();

double GetReactivePowerA();
double GetReactivePowerB();
double GetReactivePowerC();
double GetTotalReactivePower();

double GetApparentPowerA();
double GetApparentPowerB();
double GetApparentPowerC();
double GetTotalApparentPower();

double GetFrequency();

double GetPowerFactorA();
double GetPowerFactorB();
double GetPowerFactorC();
double GetTotalPowerFactor();

double GetPhaseA();
double GetPhaseB();
double GetPhaseC();

double GetTemperature();

/* Gain Parameters (GET)*/
double GetValueRegister(unsigned short registerRead);

/* Energy Consumption */
double GetImportEnergy();
double GetImportReactiveEnergy();
double GetImportApparentEnergy();
double GetExportEnergy();
double GetExportReactiveEnergy();

/* System Status */
unsigned short GetSysStatus0();
unsigned short GetSysStatus1();
unsigned short GetMeterStatus0();
unsigned short GetMeterStatus1();

/* Checksum Function */
bool calibrationError();

void read_initial_analyzer();
/* grid_analyzer_task */
void grid_analyzer_task(void *arg);