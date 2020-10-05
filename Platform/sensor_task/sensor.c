/*
 * sensore.c
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: D-D-E
 */

#include "sensor.h"
#include "uart3.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

struct ring_buffer_struct UART3_rx_data;
static uint8_t sensor_counter = 0;
static char buff[16] = { 0 };
static f_ptr delay;

void Sensor_ClearList()
{
	sensor_counter = 0;
	memset(sensor_list, 0, sizeof(struct SENSOR) * SENSOR_MAX);
}

void Sensor_Init(f_ptr delay_function)
{
	UART3_Init(&UART3_rx_data);
	Sensor_UpdateGpioInit();
	Sensor_ClearList();
	delay = delay_function;
}

uint8_t Sensor_GPIOUpdateList()
{
	return Sensor_GpioValue();
}

void Sensor_UpdateList()
{
	Sensor_ClearList();

	for(uint32_t addr = 0; addr <= 0xFFFF; ++addr)
	{
		RingBuffClear(&UART3_rx_data);

		sprintf(buff, "%04" PRIx16 "t", (uint16_t)addr);
		UART3_SendData(buff, 5);

		delay(1);
		char* ptr = NULL;
		if( (ptr = strstr(RingBuffGetArray(&UART3_rx_data), "/")) != NULL )
		{
			sensor_list[sensor_counter].addr = addr;
			sensor_list[sensor_counter].type = (enum SENSOR_TYPE) *(ptr + 1);
			sensor_counter++;
			if(sensor_counter >= 16)
			{
				UART3_SendData_byte('/');
				return;
			}
		}
		UART3_SendData_byte('/');
	}
}

void Sensor_UpdateValue()
{
	for(uint8_t i = 0; i < SENSOR_MAX; ++i)
	{
		if(sensor_list[i].addr == 0) {
			continue;
		}

		RingBuffClear(&UART3_rx_data);

		sprintf(buff, "%04" PRIx16 "v", sensor_list[i].addr);
		UART3_SendData(buff, 5);

		delay(5);
		char* ptr = NULL;
		if( (ptr = strstr(RingBuffGetArray(&UART3_rx_data), "/")) != NULL )
		{
			sensor_list[i].value = *(ptr + 1) << 24 | *(ptr + 2) << 16 | *(ptr + 3) << 8 | *(ptr + 4);
		} else {
			if(sensor_list[i].error_counter > REQ_TO_DISABLE)
			{
				sensor_list[i].addr = 0;
			} else {
				sensor_list[i].value = 0;
				sensor_list[i].error_counter++;
			}
		}

		UART3_SendData_byte('/');
	}
}

uint8_t Sensor_CheckValue(uint16_t sen_addr, uint32_t value)
{
	for(uint8_t i = 0; i < SENSOR_MAX; ++i)
	{
		if(sensor_list[i].addr == sen_addr)
		{
			return sensor_list[i].value >= value ? 1 : 0;
		}
	}
	return 0;
}
