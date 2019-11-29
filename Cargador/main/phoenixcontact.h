/****************************************************************************/
//  Function: file for PhoenixContact
//  Hardware: AIRIS - RTC 
//  ESP32 MCU: ESPRESSIF
//  Author:   Diego Ochoa
//  Date:    July 24,2019
//  Version: v1.0
//  by Agrum.SAS
/****************************************************************************/

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "modbusMaster.h"


//INPUT READ   16bits
#define YearManufacture 0x0403      //1027
#define ManufacturingDate 0x404     //1028
#define HardwareIdentification 0x0405   //1029
#define HardwareVersion 0x0406      //1030
#define FirmwareMaxVersion 0x0407   //1031
#define FirmwareVNumber 0x0408      //1032
#define FirmwareMinVersion 0x0409   //1033

//Holding 16bits
#define ConfigurationSwitchS1 2000  //Read
#define ConfigurationSwitchS2 2001  //Read
#define EnableChargingConfig 0xFA0      //4000   //Read/write
#define ExternalReleaseConfig 0xFA1     //4001   //Read/write
#define LockingConfig 0xFA2             //4002   //Read/write
#define WeldedContactDetectionConfig 0xFA6         //4006  //Read/write
#define WeldedContactDetectionDelayConfig 0xFA7    //4007  //Read/write
#define RejectingInvalidChargingCable 0xFA8        //4008  //Read
#define ConfiguringConnectionCaseBorC 0xFA9        //4009
#define AllowStateVehiclesConfig 0xFAA             //4010
#define RCMConfig 0xFAB                            //4011
#define ChangeCurrentReductionConfig 0xFAC         //4012
#define ChangeCurrentReductionAnalogValueForMinCurrent 0xFAD   //4013
#define ChangeCurrentReductionAnalogValueForMaxCurrent 0xFAE   //4014
#define ChangeCurrentReductionRefreshCycle 0xFAF               //4015
#define Input5Configuration 0x138C                             //5004

#define FunctionDefinitionOut0 0x157C   //5500
#define FunctionDefinitionOut1 0x157D   //5501
#define FunctionDefinitionOut2 0x157E   //5502
#define FunctionDefinitionOut3 0x157F   //5503
#define OutputBehaviorOut0 0x15E0   //5600
#define OutputBehaviorOut1 0x15E1   //5601
#define OutputBehaviorOut2 0x15E2   //5602
#define OutputBehaviorOut3 0x15E3   //5603

#define LockPulseLengthMiliseconds 0x1B59   //7001
#define LockCyclesWithoutConfig 0x1B5A      //7002
#define LockPause 0x1B5B                    //7003
#define LockOnTime 0x1B5c                   //7004
#define ReferenceResistorValueLocked 0x1B63 //7011
#define Led1PWMFrecuency 0x1D4C     //7500
#define Led2PWMFrecuency 0x1D4D     //7501 
#define Led3PWMFrecuency 0x1D4E     //7502
#define Led4PWMFrecuency 0x1D4F     //7503

//Coils Register 1bits
#define EnablingChargingProcess 0x4E20      //20000        //Read/Write
#define SettingSystemState 0x4E21           //20001        //Read/Write
#define ControllingLockingSctuator 0x4E22   //20002        //Read/Write
#define EVRCMFunctionTest 0x4E23            //20003        //Read/Write
#define RestartingChargingController 0x5208    //21000     //Read/Write

//Holding Register 16bits
#define ResettingDefaultSettings 0x526C     //21100
#define SettingMaximumPermissibleChargingCurrent 0x55F0     //22000
#define SettingDigitalOutputRegisterOutputOUT 0x59D8        //23000
#define SettingDigitalOutputRegisterOutputERR 0x59D9        //23001
#define SettingDigitalOutputRegisterOutputCHG 0x59DA        //23002
#define SettingDigitalOutputRegisterOutputCON 0x59DB        //23003

/*
Function assignment of output registers
The digital inputs can be assigned different functions by entering values according to
Table 9-3 to registers 5500 – 5503. 
Value Function
0 Control from the assigned output register (23000 – 23003)
1 Charging controller in status A
2 Charging controller in status B
3 Charging controller in status B and PWM ON
4 Charging controller in status B and PWM OFF
5 Charging controller in status C
6 Charging controller in status D
7 Charging controller in status E
8 Charging controller in status F
9 Charging controller in status A or B
10 Charging controller in status A or B and PWM ON
11 Charging controller in status A or B and PWM OFF
12 Charging controller in status A - C
13 Charging controller in status A - B or D
14 Charging controller in status A - D
15 Charging controller in status E - F
16 Charging controller in status C or D
17 PWM ON
18 Charging controller has detected a valid PP value
19 Charging controller has detected an invalid PP value
20 Charging controller has detected a 13 A connector at PP
21 Charging controller has detected a 20 A connector at PP
22 Charging controller has detected a 32 A connector at PP
23 Charging controller has detected a 63 A connector at PP
24 Charging controller has detected a 13 A or 20 A connector at PP
25 Charging controller has detected a 13 A or 20 A connector at PP
26 Insufficient current carrying capacity of the charging cable
27 Charging controller switches the charging contactor ON
28 Cannot be used for this device
29 Locking active
30 Cannot be used for this device
31 Cannot be used for this device
32 Cannot be used for this device
33 Cannot be used for this device
34 Cannot be used for this device
35 Charging contactor monitoring triggered
36 Status D, vehicle rejected
37 Vehicle connected in status B or C or D
38 EV-RCM: Test and reset function
39 EV-RCM: Error (system error or residual current detected)
≥ 39 Not permitted*/


void begin_phoenixcontact();

void Year_manufacture();
void Hardware_version();

void GetDataPhoenix();

uint16_t phoenixcontact_Set_EnableChargingConfig(int state);
uint16_t phoenixcontact_Get_EnableChargingConfig();

uint16_t phoenixcontact_Set_ExternalReleaseConfig(int state);
uint16_t phoenixcontact_Get_ExternalReleaseConfig();

uint16_t phoenixcontact_Set_LockingConfig(int state);
uint16_t phoenixcontact_Get_LockingConfig();

void phoenixcontact_WeldedContactDetectionConfig();
void phoenixcontact_WeldedContactDetectionDelayConfig();
void phoenixcontact_AllowStateVehiclesConfig();
void phoenixcontact_RCMConfig();
void phoenixcontact_ChangeCurrentReductionConfig();
void phoenixcontact_ChangeCurrentReductionAnalogValueForMinCurrent();
void phoenixcontact_ChangeCurrentReductionAnalogValueForMaxCurrent();
void phoenixcontact_ChangeCurrentReductionRefreshCycle();
void phoenixcontact_Input5Configuration();

void phoenixcontact_FunctionDefinitionOut0(int function);
void phoenixcontact_FunctionDefinitionOut1(int function);
void phoenixcontact_FunctionDefinitionOut2(int function);
void phoenixcontact_FunctionDefinitionOut3(int function);
void phoenixcontact_OutputBehaviorOut0(int state);
void phoenixcontact_OutputBehaviorOut1(int state);
void phoenixcontact_OutputBehaviorOut2(int state);
void phoenixcontact_OutputBehaviorOut3(int state);

void phoenixcontact_LockPulseLengthMiliseconds();
void phoenixcontact_LockCyclesWithoutConfig();
void phoenixcontact_LockPause();
void phoenixcontact_LockOnTime();
void phoenixcontact_ReferenceResistorValueLocked();

uint8_t phoenixcontact_Led1PWMFrecuency(int porcen);
uint8_t phoenixcontact_Led2PWMFrecuency(int porcen);
uint8_t phoenixcontact_Led3PWMFrecuency(int porcen);
uint8_t phoenixcontact_Led4PWMFrecuency(int porcen);

void phoenixcontact_SettingOutputERR();

void phoenixcontact_Digital_OutputBehaviorOut0(int state);
void phoenixcontact_Digital_OutputBehaviorOut1(int state);
void phoenixcontact_Digital_OutputBehaviorOut2(int state);
void phoenixcontact_Digital_OutputBehaviorOut3(int state);

//COILS REGISTER

uint8_t phoenixcontact_Set_Enable_charging_process(int state);
uint8_t phoenixcontact_Get_Enable_charging_process();

uint8_t phoenixcontact_Set_Setting_System_StateF(int state);
uint8_t phoenixcontact_Get_Setting_System_StateF();

uint8_t phoenixcontact_Set_Controlling_Locking_Actuator(int state);
uint8_t phoenixcontact_Get_Controlling_Locking_Actuator();

uint8_t phoenixcontact_Set_EV_RCM(int state);
uint8_t phoenixcontact_Get_EV_RCM();

uint8_t phoenixcontact_Set_Reset(int state);
uint8_t phoenixcontact_Get_Reset();







