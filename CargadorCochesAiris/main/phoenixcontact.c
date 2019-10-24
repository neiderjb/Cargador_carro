#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "ZDU0210RJX.h"

/****************************************************************************/
//  Function: file for PhoenixContact
//  Hardware: AIRIS - RTC
//  ESP32 MCU: ESPRESSIF
//  Author:   Diego Ochoa
//  Date:    July 24,2019
//  Version: v1.0
//  by Agrum.SAS
/****************************************************************************/

//Modbus/RTU (slave)
//Transmission mode 8 data bits / N parity / 1 stop bits

//Connectors
/*
Connectors to charging
LO  - Locking Control of the locking actuator
LO  +
LD2 - Lock Detection  Digital input, for connecting the locking confirmation, can be configured
LD1
PP  - Proximity Plug-Test signal-  Current carrying capacity of the connected
                                   charging connector and charging cable according to
                                   IEC 61851-1
CP  - Control Pilot-Pilot wire signal-  Communication between charging station and
                                        vehicle according to IEC 61851-1, SAE J1772
                                        and GB/T 18487.1

Switches 
S2
1 Baud rate 1= 19200, 0=9600
2-6 Modbus address
7-8 Reserved expansions

Modbus register type  |  Value     | Access
------------------------------------------------
Input                 |  16 bits   | Read
Holding               |  16 bits   | Read/write
Coils                 |  1 bit     | Read/write

*/

//https://www.phoenixcontact.com/online/portal/es/pxc/product_detail_page/!ut/p/z1/5VdbU6MwGP0rfenTTpoA4dJHjKxWrdalauGFCSGwcbkJ2Fp__QbszlqntjPrdF9gMkNu53x8J_mYOdCHC-jndCkS2ogip6kce74RqGcKUs6xMj2zJga6vbUdHc1V5CAT3kMf-jVPOWt4RGjDk6JaQ8--umoXGhpCT2l7JWO_oDcD6hggDJAKUDedirCiLYDXvO72iUgiDFXFutaOi3hGyOUOZCOalEPPFXXDMzqI-IAVeVMVadelVUIHNhmAgXMPCAE2UcBUA-SEANf5AWbkRC79CfOwL8npzID-fg3e8OiTx0aH8J7Em59usHTo7pTqr7gVZ1wseSWZWgmrInpmzSlvqEjriZRnsq3pp8f1UfwyirMi4hvaohbtrdgKIsdz_tJs5nafsWRJaZ4806RlOpCs2aNkb3CfkjX6lGyfavamTzWL-1SzuE81i_tUs5e9usZ9-kFdKn1KVpPJehPoVcm300cbXkBfhNloxbIRGpUvbLSJG0RddkHZkkpPIx6fnnwb-q1laL9n0Yo0RCseDlFGRT5EG6CcTKXD6ICyb5OA_ZTeQuRJsLEbKa_qYCuJITKwxRRmIhBFDAOMTR2MrVADY0VX1Qhp8TiMpWvw9xmDznd0G_YYC5fXgeMGswVx3P1STa8N-LAUfAXv8qLKpLVz386gPUOdqSqlsQ6MOEQA61gDVOUx0KyxqWA-jkIzhufoUAT9ixEO0JtHpf9-XPr7o4rjIHxc-q9-_cXeq26qXylKynYXZfkPRVlmmaVtGrhbvM55-Pq-xe_W31pwfa0vtxqg2xgeWkj7ANPL5QfYfw1lrTX0iL2L5Wo1jzMSWtuv9eo3nngE_g!!/p0/IZ7_2G101H41MG8I60QQAE50T20E46=CZ6_2G101H41MG8I60QQAE50T20E07=LA0=EdownloadId!2848262=action!downloadFile==/#Z7_2G101H41MG8I60QQAE50T20E46

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "phoenixcontact.h"

#include "esp_log.h"

static const char *TAG = "phoenixcontact";

uint8_t ReadCoils = 0x01;
uint8_t ReadDiscreteInputs = 0x02;   ///< Modbus function 0x02 Read Discrete Inputs
uint8_t ReadHoldingRegisters = 0x03; ///< Modbus function 0x03 Read Holding Registers
uint8_t ReadInputRegisters = 0x04;   ///< Modbus function 0x04 Read Input Registers

void begin_phoenixcontact()
{
    ESP_LOGI(TAG, "begin_Phoenix OK");
    ESP_LOGI(TAG, "year_Phoenix");
    Year_manufacture();
    GetDataPhoenix();
}

void Year_manufacture()
{
    readInputRegisters(YearManufacture, 2);
    uint8_t dateyear[2];
    responseModbus(ReadInputRegisters, dateyear);
    uint8_t yh = (dateyear[0] << 4 | dateyear[0] >> 4);
    uint8_t yl = (dateyear[1] << 4 | dateyear[1] >> 4);
    uint16_t value = yh | yl;
    ESP_LOGI(TAG, "Phoenix Age: %x %x - %x", yh, yl, value);
}

void Hardware_version()
{
    readInputRegisters(HardwareVersion, 2);
    uint8_t dataVersion[2];
    responseModbus(ReadInputRegisters, dataVersion);
    uint16_t value = (dataVersion[0] << 4 | dataVersion[0] >> 4) | (dataVersion[1] << 4 | dataVersion[1] >> 4);
    ESP_LOGI(TAG, "Phoenix Version: %x", value);
}

void GetDataPhoenix()
{
    phoenixcontact_Get_EnableChargingConfig();
    phoenixcontact_Get_ExternalReleaseConfig();
    phoenixcontact_Get_LockingConfig();
}

/*
Charging enabled via Modbus
S1/DIP 1 = OFF 
Charging station with Infrastructure Socket Outlet
Value 3 is entered in Modbus register 4000; enable via Modbus register 20000.
Locking is carried out if the vehicle is detected at the charging station and status B is present.
The charging process starts if the locking feedback is available and value 1 is written to
Modbus register 20000. */

/*
Configuring the enable charging process function via input EN or Modbus registers 
Switching on the PWM signal if all other conditions required are met
Integer
0: Charging always enabled
1: Charging enabled if input EN =
ON (default)
2: Charging enabled in the event
of a pulsed signal at input EN,
reset upon next pulse
3: Charging enabled if value 1 is
written to register 20000.
Charging not enabled (PWM
OFF) if value 0 is written to register 20000.
*/

uint16_t phoenixcontact_Set_EnableChargingConfig(int state)
{
    writeSingleRegister(EnableChargingConfig, state);
    return phoenixcontact_Get_EnableChargingConfig();
}

uint16_t phoenixcontact_Get_EnableChargingConfig()
{
    readInputRegisters(EnableChargingConfig, 2);
    uint8_t data[2];
    responseModbus(ReadInputRegisters, data);
    uint16_t value = (data[0] << 4 | data[0] >> 4) | (data[1] << 4 | data[1] >> 4);
    ESP_LOGI(TAG, "Get_EnableChargingConfig: %x", value);
    return value;
}

/*
 Configuring external release via input XR or Modbus registers
Setting status F (according to IEC 61851-1) if charging station is not available
Integer
If S1/DIP 2 = OFF:
0: Always available
(Default, if S1/DIP 2 = OFF, other
values have no effect)
If S1/DIP 2 = ON:
1: Available if XR = ON, status F
if XR = OFF
(Default, if S1/DIP 2 = ON)
2: Available if value 1 is written to
register 20001. Status F if value 0
is written to register 20001.
*/
uint16_t phoenixcontact_Set_ExternalReleaseConfig(int state)
{
    writeSingleRegister(ExternalReleaseConfig, state);
    return phoenixcontact_Get_ExternalReleaseConfig();
}

uint16_t phoenixcontact_Get_ExternalReleaseConfig()
{
    readInputRegisters(ExternalReleaseConfig, 2);
    uint8_t data[2];
    responseModbus(ReadInputRegisters, data);
    uint16_t value = (data[0] << 4 | data[0] >> 4) | (data[1] << 4 | data[1] >> 4);
    ESP_LOGI(TAG, "Get_ExternalReleaseConfig: %x", value);
    return value;
}

/*
 Activating the locking function Charging connector in the Infrastructure Socket Outlet
 Integer
If S1/DIP 3 = OFF:
0: Locking is performed automatically when the vehicle is connected (status B), unlocking is
performed if the vehicle is not detected (status A).
(Default, if S1/DIP 3 = OFF, other
values have no effect)
If S1/DIP 3 = ON:
1: Locking upon ON signal at
input ML, unlocking if signal at
input ML = OFF
(Default, if S1/DIP 3 = ON)
2: Locking in the event of a
pulsed signal at input ML, unlocking upon next pulse
3: Locking if value 1 is written to
register 20002. Unlocking if
value 0 is written.
*/
uint16_t phoenixcontact_Set_LockingConfig(int state)
{
    writeSingleRegister(ExternalReleaseConfig, state);
    return phoenixcontact_Get_LockingConfig();
}

uint16_t phoenixcontact_Get_LockingConfig()
{
    readInputRegisters(LockingConfig, 2);
    uint8_t data[2];
    responseModbus(ReadInputRegisters, data);
    uint16_t value = (data[0] << 4 | data[0] >> 4) | (data[1] << 4 | data[1] >> 4);
    ESP_LOGI(TAG, "Get_LockingConfig: %x", value);
    return value;
}

void phoenixcontact_WeldedContactDetectionConfig()
{
}

void phoenixcontact_WeldedContactDetectionDelayConfig()
{
}

void phoenixcontact_AllowStateVehiclesConfig()
{
}

void phoenixcontact_RCMConfig()
{
}

void phoenixcontact_ChangeCurrentReductionConfig()
{
}

void phoenixcontact_ChangeCurrentReductionAnalogValueForMinCurrent()
{
}

void phoenixcontact_ChangeCurrentReductionAnalogValueForMaxCurrent()
{
}

void phoenixcontact_ChangeCurrentReductionRefreshCycle()
{
}

void phoenixcontact_Input5Configuration()
{
}

void phoenixcontact_FunctionDefinitionOut0(int function)
{
    writeSingleRegister(FunctionDefinitionOut0, function);
}

void phoenixcontact_FunctionDefinitionOut1(int function)
{
    writeSingleRegister(FunctionDefinitionOut1, function);
}

void phoenixcontact_FunctionDefinitionOut2(int function)
{
    writeSingleRegister(FunctionDefinitionOut2, function);
}
void phoenixcontact_FunctionDefinitionOut3(int function)
{
    writeSingleRegister(FunctionDefinitionOut3, function);
}

/*
Behavior of digital output CON
in status ON
Only takes effect if register
5500 ≠ 0  -> 0
5501 ≠ 0  -> 15 Charging controller in status E - F
5502 ≠ 0  -> 27 Charging controller switches the charging contactor ON
5503 ≠ 0  -> 37 Vehicle connected in status B or C or D
*/
void phoenixcontact_OutputBehaviorOut0(int state)
{
    writeSingleRegister(FunctionDefinitionOut0, 1);
    writeSingleRegister(OutputBehaviorOut0, (uint16_t)state);
}
void phoenixcontact_OutputBehaviorOut1(int state)
{
    writeSingleRegister(FunctionDefinitionOut1, 1);
    writeSingleRegister(OutputBehaviorOut1, state);
}
void phoenixcontact_OutputBehaviorOut2(int state)
{
    writeSingleRegister(FunctionDefinitionOut2, 1);
    writeSingleRegister(OutputBehaviorOut2, state);
}
void phoenixcontact_OutputBehaviorOut3(int state)
{
    writeSingleRegister(FunctionDefinitionOut3, 1);
    writeSingleRegister(OutputBehaviorOut3, state);
}

void phoenixcontact_LockPulseLengthMiliseconds()
{
}
void phoenixcontact_LockCyclesWithoutConfig()
{
}
void phoenixcontact_LockPause()
{
}
void phoenixcontact_LockOnTime()
{
}
void phoenixcontact_ReferenceResistorValueLocked()
{
}
uint8_t phoenixcontact_Led1PWMFrecuency(int porcen)
{
    return writeSingleRegister(Led1PWMFrecuency, porcen);
}
uint8_t phoenixcontact_Led2PWMFrecuency(int porcen)
{
    return writeSingleRegister(Led2PWMFrecuency, porcen);
}
uint8_t phoenixcontact_Led3PWMFrecuency(int porcen)
{
    return writeSingleRegister(Led3PWMFrecuency, porcen);
}
uint8_t phoenixcontact_Led4PWMFrecuency(int porcen)
{
    return writeSingleRegister(Led4PWMFrecuency, porcen);
}

/*Only takes effect if the digital output ERR is configured with
value 0 via register 5501 

Integer
0: Output = OFF
1: Output = ON
2: Output = flashing (1 Hz) */
void phoenixcontact_Digital_OutputBehaviorOut0(int state)
{
    writeSingleRegister(FunctionDefinitionOut0, 0);
    vTaskDelay(5);
    writeSingleRegister(SettingDigitalOutputRegisterOutputOUT, (uint16_t)state);
    vTaskDelay(5);
}

void phoenixcontact_Digital_OutputBehaviorOut1(int state)
{
    writeSingleRegister(FunctionDefinitionOut1, 0);
    vTaskDelay(5);
    writeSingleRegister(SettingDigitalOutputRegisterOutputERR, (uint16_t)state);
    vTaskDelay(5);
}

void phoenixcontact_Digital_OutputBehaviorOut2(int state)
{
    writeSingleRegister(FunctionDefinitionOut2, 0);
    vTaskDelay(5);
    writeSingleRegister(SettingDigitalOutputRegisterOutputCHG, (uint16_t)state);
    vTaskDelay(5);
}

void phoenixcontact_Digital_OutputBehaviorOut3(int state)
{
    writeSingleRegister(FunctionDefinitionOut3, 0);
    vTaskDelay(5);
    writeSingleRegister(SettingDigitalOutputRegisterOutputCON, (uint16_t)state);
    vTaskDelay(5);
}

//COILS REGISTER

/*
Enabling the charging process Switching on the PWM signal
if all other conditions required are met.
Only takes effect if register 4000 is configured for this function.
1 bit
0 = charging process not enabled
1 = charging process enabled
*/
uint8_t phoenixcontact_Set_Enable_charging_process(int state)
{
    printf("WriteSingleCoil_Enable_charging_process");
    return writeSingleCoil(EnablingChargingProcess, state);
}
uint8_t phoenixcontact_Get_Enable_charging_process()
{
    printf("ReadingleCoil_Enable_charging_process");
    readCoils(EnablingChargingProcess, 1);
    uint8_t data[2];
    responseModbus(ReadCoils, data);
    uint16_t value = (data[0] << 4 | data[0] >> 4) | (data[1] << 4 | data[1] >> 4);
    ESP_LOGI(TAG, "GetEnable_charging_process: %x", value);
    return value;
}

/*
Setting the system state F (according to IEC 61851-1) if the
charging station is not available.
Only takes effect if register 4001 is configured via Modbus/RTU.
1 bit
0 = charging station not available,
status F
1 = charging station available
*/
uint8_t phoenixcontact_Set_Setting_System_StateF(int state)
{
    printf("WriteSingleCoil_Setting_System_StateF");
    return writeSingleCoil(SettingSystemState, state);
}
uint8_t phoenixcontact_Get_Setting_System_StateF()
{
    printf("ReadSingleCoil_Setting_System_StateF");
    readCoils(SettingSystemState, 1);
    uint8_t data[2];
    responseModbus(ReadCoils, data);
    uint16_t value = (data[0] << 4 | data[0] >> 4) | (data[1] << 4 | data[1] >> 4);
    ESP_LOGI(TAG, "Get_Setting_System_StateF: %x", value);
    return value;
}

/*
 Controlling the locking actuator Only takes effect if register
4002 is configured for this function.
1 bit
0 = charging connector is unlocked
1 = charging connector is locked
*/
uint8_t phoenixcontact_Set_Controlling_Locking_Actuator(int state)
{
    printf("WriteSingleCoil_Controlling_Locking_Actuator");
    return writeSingleCoil(ControllingLockingSctuator, state);
}
uint8_t phoenixcontact_Get_Controlling_Locking_Actuator()
{
    printf("ReadSingleCoil_Setting_System_StateF");
    readCoils(ControllingLockingSctuator, 1);
    uint8_t data[2];
    responseModbus(ReadCoils, data);
    uint16_t value = (data[0] << 4 | data[0] >> 4) | (data[1] << 4 | data[1] >> 4);
    ESP_LOGI(TAG, "Get_Setting_System_StateF: %x", value);
    return value;
}

/*EV-RCM function test
 1 = EV-RCM function test following the next charging process
(status A1 or B1). After the function test, the register is set to 0.
*/
uint8_t phoenixcontact_Set_EV_RCM(int state)
{
    printf("WriteSingleCoil_CEV_RCM");
    return writeSingleCoil(EVRCMFunctionTest, state);
}
uint8_t phoenixcontact_Get_EV_RCM()
{
    printf("ReadSingleCoil_Setting_System_StateF");
    readCoils(EVRCMFunctionTest, 1);
    uint8_t data[2];
    responseModbus(ReadCoils, data);
    uint16_t value = (data[0] << 4 | data[0] >> 4) | (data[1] << 4 | data[1] >> 4);
    ESP_LOGI(TAG, "Get_Setting_System_StateF: %x", value);
    return value;
}

/*
Restarting the charging controller
1 bit
Restart if value 1 is written.
*/
uint8_t phoenixcontact_Set_Reset(int state)
{
    printf("WriteSingleCoil_SetReset");
    return writeSingleCoil(RestartingChargingController, state);
}

uint8_t phoenixcontact_Get_Reset()
{
    printf("ReadCoils_GetReset");
    return readCoils(RestartingChargingController, 1);
}
