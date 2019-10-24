#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 
#include <math.h>

#include "cJSON.h"


#include "Function.h"
#include "Parameters.h"

/*! \brief createjson.
 *
 *  This function create a json object to send to MQTT broker
 *  
 *  \param volA       Votage phase A.
 *  \param curr       Current phase A.
 *  \param potenA     Active Power phase A.
 *  \param potenA     Active Power phase A.
 *  \param potenR     Reactive Power phase A.
 *  \param potenApp   Apparent Power phase A.
 *  \param potenApp   Math Power phase A.
 *  \param temp       temperture.
 */

char * createjson(double volA, double currA, double factor, double potenA, double potenR, double potenApp, double potenMat, double temp)
{

	cJSON *root;
	char *datatoreturn;

	
	root = cJSON_CreateObject();

    double _volA = round(volA*100)/100.0;
    double _currA = round(currA*100)/100.0;
    double _factor = round(factor*100)/100.0;
    double _potenA = round(potenA*100)/100.0;
    double _potenR = round(potenR*100)/100.0;
    double _potenApp = round(potenApp*100)/100.0;
    double _potenMat = round(potenMat*100)/100.0;
    double _temp = round(temp*100)/100.0;

	cJSON_AddItemToObject(root, "V1", cJSON_CreateNumber(_volA));
	cJSON_AddItemToObject(root, "C1", cJSON_CreateNumber(_currA));
	cJSON_AddItemToObject(root, "P1", cJSON_CreateNumber(_potenA));
	cJSON_AddItemToObject(root, "PR1", cJSON_CreateNumber(_potenR));
	cJSON_AddItemToObject(root, "PA1", cJSON_CreateNumber(_potenApp));
	cJSON_AddItemToObject(root, "F1", cJSON_CreateNumber(_factor));
	cJSON_AddItemToObject(root, "PM1", cJSON_CreateNumber(_potenMat));
	cJSON_AddItemToObject(root, "TEM", cJSON_CreateNumber(_temp));

	datatoreturn = cJSON_PrintUnformatted(root);
    return datatoreturn;
	//sendMessage(datatoreturn, "airis/1155/power_analizer");
}


char *substring(char *string, int position, int length)
{
	char *pointer;
	int c;

	pointer = malloc(length + 1);

	if (pointer == NULL)
	{
		printf("Unable to allocate memory.\n");
		exit(1);
	}

	for (c = 0; c < length; c++)
	{
		*(pointer + c) = *(string + position - 1);
		string++;
	}

	*(pointer + c) = '\0';

	return pointer;
}



/*! \brief decToBinary.
 *
 *  This function convert decimal number to binary, return int array with the number in binary.
 * 	
 *  \param n number.
 */
int *decToBinary(int n)
{
	static int ret[8];
	for (int i = 7; i >= 0; i--)
	{
		int k = n >> i;
		if (k & 1)
			ret[i] = 1;
		else
			ret[i] = 0;
	}
	return ret;
}

/** @ingroup util_word
    Return low word of a 32-bit integer.
    @param uint32_t ww (0x00000000..0xFFFFFFFF)
    @return low word of input (0x0000..0xFFFF)
*/
uint16_t lowWord(uint32_t ww)
{
  return (uint16_t) ((ww) & 0xFFFF);
}

uint8_t lowByte(uint16_t ww)
{
  return (uint8_t) ((ww) & 0xFFFF);
}

uint8_t highByte(uint16_t ww)
{
  return (uint8_t) ((ww) >> 8);
}


/** @ingroup util_word
    Return high word of a 32-bit integer.
    @param uint32_t ww (0x00000000..0xFFFFFFFF)
    @return high word of input (0x0000..0xFFFF)
*/
uint16_t highWord(uint32_t ww)
{
  return (uint16_t) ((ww) >> 16);
}