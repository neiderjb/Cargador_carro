#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spi_flash.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "driver/i2s.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "uconsume.h"
#include "M90E32AS.h"

static const char *TAG = "Uconsume";

#define V_REF 1100
#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_7)

#define PARTITION_NAME "storage"

/*---------------------------------------------------------------
                             CONFIG
---------------------------------------------------------------*/

//i2s number
#define I2S_NUM (0)
//i2s sample rate
#define I2S_SAMPLE_RATE (200000)
//i2s data bits
#define I2S_SAMPLE_BITS (16)
//enable display buffer for debug
#define I2S_BUF_DEBUG (0)
//I2S read buffer length
#define I2S_READ_LEN (16 * 1024)
//I2S data format
#define I2S_FORMAT (I2S_CHANNEL_FMT_RIGHT_LEFT)
//I2S channel number
#define I2S_CHANNEL_NUM ((I2S_FORMAT < I2S_CHANNEL_FMT_ONLY_RIGHT) ? (2) : (1))
//I2S built-in ADC unit
#define I2S_ADC_UNIT ADC_UNIT_1
//I2S built-in ADC channel
#define I2S_ADC_CHANNEL ADC1_CHANNEL_0

/**
 * @brief I2S ADC/DAC mode init.
 */
void i2s_init()
{
	int i2s_num = I2S_NUM;
	i2s_config_t i2s_config = {
		.mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN,
		.sample_rate = I2S_SAMPLE_RATE,
		.bits_per_sample = I2S_SAMPLE_BITS,
		.communication_format = I2S_COMM_FORMAT_I2S_MSB,
		.channel_format = I2S_FORMAT,
		.intr_alloc_flags = 0,
		.dma_buf_count = 2,
		.dma_buf_len = 512,
		.use_apll = false,
		.tx_desc_auto_clear = false,
		.fixed_mclk = 0};
	//install and start i2s driver
	i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
	//init ADC pad
	i2s_set_adc_mode(I2S_ADC_UNIT, I2S_ADC_CHANNEL);
}

/**
 * @brief debug buffer data
 */
void disp_buf(uint8_t *buf, int length)
{
	printf("======\n");
	for (int i = 0; i < length; i++)
	{
		printf("%02x ", buf[i]);
		if ((i + 1) % 8 == 0)
		{
			printf("\n");
		}
	}
	printf("======\n");
}

bool analyse_window_filter_50(uint16_t *windows_in, uint16_t size_window)
{
	uint8_t bit_value = 0; //Valor del BIT adc
	uint8_t array_times[size_window];
	uint16_t _size = 0;
	uint8_t started = 0;
	uint8_t last_value = 1;
	bool result = false;
	unsigned char dat;
	//Se recorre la ventana
	//Se recorre de 2 en dos por que solo se lee un canal del ADC size = 900
	for (uint16_t x = 0; x < ((size_window)); x += 2)
	{															//Se empieza aplicar el filto 1ms despues
		volatile uint16_t adc_val = ((~windows_in[x]) & 0xFFF); //Datos vienen invertidos, solo se toma la parte inferior 12bits
		//Serial.println(adc_val);                                                  //Debug Serial ploter
		bit_value = (adc_val > 3800) ? 1 : 0; //Comparamos si el valor es mayor a 3800 de 4096 valores posibles(12bits)

		if (started == 1 || bit_value == 1)
		{
			if (last_value == bit_value)
			{
				array_times[_size]++;
			}
			else
			{
				_size++;
				array_times[_size] = 1;
			}
			last_value = bit_value;
			started = 1;
		}
	}

	uint16_t _min = 0xFFFF;
	uint16_t _max = 0;
	uint8_t _diff = 0xFFFF;
	for (uint16_t i = 0; i < _size - 11; i = i + 2)
	{ //19 maximo nuevo filtroSe recorre el nuevo buffer buscando encontrar 13 coincidencias
		uint16_t _acum = 0;
		//uint8_t _diff = 0xFFFF;                                                        //correspondientes a 8 estados en alto y 7 en bajo se quitan el
		for (uint16_t j = 0; j < 11; j++)
		{																						//primero y ultimo alto por lo cual serian 6 altos y 7 bajos
			_diff = (array_times[i + j] > 7) ? array_times[i + j] - 7 : 7 - array_times[i + j]; //5useg   7*5 = 35useg
			_acum += _diff;
		}
		_min = (_acum < _min) ? _acum : _min;
		_max = (_acum > _max) ? _acum : _max;
		if (_diff <= 10 && _min < 100)
		{
			//Serial.printf("Vals: acum:%d dif:%d min:%d max:%d \n", _acum, _diff, _min , _max);      //Debug
			result = true;
			break;
		}
	}
	dat = _min;
	//xQueueSend(_uConsume_times_queue, &_min, portMAX_DELAY);          //Descomentar para debug en uconsume_print_task                            //Debug
	memset(array_times, 0, sizeof(array_times));
	return result;
}

bool analyse_window_filter_60(uint16_t *windows_in, uint16_t size_window)
{
	uint8_t bit_value = 0; //Valor del BIT adc
	uint8_t array_times[size_window];
	uint16_t _size = 0;
	uint8_t started = 0;
	uint8_t last_value = 1;
	bool result = false;
	unsigned char dat;
	//Se recorre la ventana
	//Se recorre de 2 en dos por que solo se lee un canal del ADC size = 900
	for (uint16_t x = 0; x < ((size_window)); x += 2)
	{															//Se empieza aplicar el filto 1ms despues
		volatile uint16_t adc_val = ((~windows_in[x]) & 0xFFF); //Datos vienen invertidos, solo se toma la parte inferior 12bits
		//Serial.println(adc_val);                                                  //Debug Serial ploter
		bit_value = (adc_val > 3800) ? 1 : 0; //Comparamos si el valor es mayor a 3800 de 4096 valores posibles(12bits)
		//Debug Serial ploter
		if (started == 1 || bit_value == 1)
		{
			if (last_value == bit_value)
			{
				array_times[_size]++;
			}
			else
			{
				_size++;
				array_times[_size] = 1;
			}
			last_value = bit_value;
			started = 1;
		}
	}

	uint16_t _min = 0xFFFF;
	uint16_t _max = 0;
	uint8_t _diff = 0xFFFF;
	for (uint16_t i = 0; i < _size - 11; i = i + 2)
	{ //Se recorre el nuevo buffer buscando encontrar 13 coincidencias
		uint16_t _acum = 0;
		//uint8_t _diff = 0xFFFF;                                                            //correspondientes a 8 estados en alto y 7 en bajo se quitan el
		for (uint16_t j = 0; j < 11; j++)
		{																						//primero y ultimo alto por lo cual serian 6 altos y 7 bajos
			_diff = (array_times[i + j] > 7) ? array_times[i + j] - 7 : 7 - array_times[i + j]; //5useg
			_acum += _diff;
		}
		_min = (_acum < _min) ? _acum : _min;
		_max = (_acum > _max) ? _acum : _max;
		if (_diff <= 10 && _min < 100)
		{
			result = true;
			break;
		}
	}
	dat = _min;
	//xQueueSend(_uConsume_times_queue, &dat, portMAX_DELAY);
	memset(array_times, 0, sizeof(array_times));
	return result;
}

void uconsume_read_task(void *arg)
{
	int  freqHz = 0;
	freqHz = GetFrequency();
	if (freqHz > 49 && freqHz < 61)
	{
		ESP_LOGI(TAG, "Uconsume Frecuency OK %d \n", freqHz);
	}
	else
	{
		ESP_LOGE(TAG, "Uconsume Frecuency FAIL %d \n", freqHz);
	}

	int i2s_read_len = I2S_READ_LEN;
	int flash_wr_size = 0;
	size_t bytes_read;
	char *i2s_read_buff = (char *)calloc(i2s_read_len, sizeof(char));
	bool sempZero = true;

	uint8_t fsm_protocol_ucos = 0;
	uint8_t start_byte = 0;
	uint8_t zero_counter = 0;
	uint8_t idx_bit_uConsume = 0;
	uint8_t data_uConsume;
	uint8_t data_cont = 0;
	uint8_t data_consume[700];
	bool valid_data = false;

	uint8_t data_size = 0;
	uint8_t cont_false = 0;

	while (1)
	{
		if (sempZero)
		{
			bool bit_uconsume;
			i2s_adc_enable(I2S_NUM);
			i2s_read(I2S_NUM, (void *)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
			//disp_buf((uint8_t*) i2s_read_buff, 64);	//print data read adc
			i2s_adc_disable(I2S_NUM);
			if (freqHz == 50)
			{
				bit_uconsume = analyse_window_filter_50(i2s_read_buff, 1400);
			}
			else
			{
				bit_uconsume = analyse_window_filter_60(i2s_read_buff, 1400);
			}
			free(i2s_read_buff);
			i2s_read_buff = NULL;

			i2s_adc_enable(I2S_NUM);
			i2s_read(I2S_NUM, (void *)i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
			//disp_buf((uint8_t*) i2s_read_buff, 64);	//print data read adc
			i2s_adc_disable(I2S_NUM);
			if (freqHz == 50)
			{
				bit_uconsume = bit_uconsume || analyse_window_filter_50(i2s_read_buff, 1400);
			}
			else
			{
				bit_uconsume = bit_uconsume || analyse_window_filter_60(i2s_read_buff, 1400);
			}
			free(i2s_read_buff);
			i2s_read_buff = NULL;

			/*
			Maquina de estado reconstruccion de la trama microconsumo
			revisa trama inicial 1111 00000 - 240
			almacena los datos de microconsumos
			*/
			switch (fsm_protocol_ucos)
			{
			case 0:
				if (bit_uconsume)
				{
					start_byte++;
				}
				else
				{
					start_byte = 0;
				}
				if (start_byte > 3) //Si llega la trama inicial cambia el estado a la maquina
				{
					valid_data = true;
					fsm_protocol_ucos = 1;
					start_byte = 0;
					idx_bit_uConsume = 0;
					data_uConsume = 0;
				}
				break;
			case 1:
				if (bit_uconsume)
				{
					data_uConsume |= 128;
					zero_counter = 0;
				}
				else
				{
					if (zero_counter < 100)
					{
						zero_counter++;
					}
					else
					{
						zero_counter = 0;
						fsm_protocol_ucos = 0;
					}
				}
				idx_bit_uConsume++;

				if (idx_bit_uConsume == 8)
				{
					//Serial.printf("\ndata: %u", data_uConsume);
					data_consume[data_cont] = data_uConsume;
					data_cont++;
					data_uConsume = 0;
					idx_bit_uConsume = 0;
				}
				data_uConsume >>= 1;
				break;

			default:
				break;
			}

			if (valid_data && data_consume[0] != 0 && data_cont == 1)
			{ //Valido que llegaran una trama valida
				//data_id = data_consume[0] >> 6;						 //Se analiza  primer byte de la trama de los 4 bit de la mitad se saca el tamaño
				data_size = (data_consume[0] >> 2 & 0b00001111) + 5; //Se suma los 5 bytes estaticos del protocolo
				cont_false = 0;
			}
			/*
			Valido que llegaran una trama valida y se envia y reinicia buffer
			*/
			if (valid_data && data_cont == data_size && data_size != 0)
			{
				printf("uConsume STOP");
				for (int i = 0; i < data_size; i++)
				{
					printf("data buffer uconsume: ");
					printf(data_consume[i]);
				}

				free(data_consume);
			}

			else if (cont_false > (8 * 20))
			{
				printf("uConsume STOP-No DATA");
				free(data_consume);
			}
		}
	}
}

	void begin_ucosnume()
	{
		i2s_init();
		esp_log_level_set("I2S Uconsume", ESP_LOG_INFO);
		xTaskCreate(uconsume_read_task, "uconsume_read_task", 2048, NULL, 5, NULL);
	}