#include "software_i2c.h"
#include "Parameters.h"
#include "M90E32AS.h"
#include "SC18IS602B.h"

#include "ZDU0210RJX.h"
#include "modbusMaster.h"
#include "phoenixcontact.h"

static const char *TAG = "main";


void app_main()
{
    //I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();

    //I2C-SPI
	begin_SC18IS602B();

	//I2C-UART
	begin_ZDU0210RJX(0xFF, 0xFF);

	//I2C Modbus Phoenic Contact
	begin_modbusMaster(3);
	begin_phoenixcontact();
	ESP_LOGI(TAG, "Phoenix Output Out Err Chg CON\n");
	phoenixcontact_Digital_OutputBehaviorOut0(2);
	phoenixcontact_Digital_OutputBehaviorOut1(2); 
	phoenixcontact_Digital_OutputBehaviorOut2(2);
	phoenixcontact_Digital_OutputBehaviorOut3(2);
	ESP_LOGI(TAG, "Phoenix Write Coil\n");
	phoenixcontact_Set_Controlling_Locking_Actuator(0);
	vTaskDelay(500);
	phoenixcontact_Set_Controlling_Locking_Actuator(1);
	phoenixcontact_Get_Controlling_Locking_Actuator(0);
	ESP_LOGI(TAG, "Phoenix Read Coil\n");
	phoenixcontact_Get_Controlling_Locking_Actuator();
	ESP_LOGI(TAG, "Phoenix Read Holding S1 y S2\n");
	switch_S1();
	switch_S2(); 
	phoenixcontact_Get_EnableChargingConfig();
	ESP_LOGI(TAG, "Phoenix Read Input\n");
	Year_manufacture();
	
    //Grid Analyzer
	LineFreq = 5255;
	PGAGain = 42; //X4
	VoltageGain = 49152;
	CurrentGain = 45200;
	begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //
	
    xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 4096, NULL, 5, NULL);
	
}