#include "software_i2c.h"
#include "Parameters.h"
#include "M90E32AS.h"
#include "SC18IS602B.h"

void app_main()
{
    //I2C config
	sw_i2c_init(PIN_SDA, PIN_SCL);
	sw_i2c_master_scan();

    //I2C-SPI
	begin_SC18IS602B();
	
    //Grid Analyzer
	LineFreq = 5255;
	PGAGain = 42; //X4
	VoltageGain = 49152;
	CurrentGain = 45200;
	begin_M90E32AS(LineFreq, PGAGain, VoltageGain, CurrentGain, CurrentGain, CurrentGain); //
	
    xTaskCreate(grid_analyzer_task, "grid_analyzer_task", 4096, NULL, 5, NULL);
	
}