/*
 * esp8266_task.c
 *
 *  Created on: 21 сент. 2020 г.
 *      Author: D-D-E
 */

#include "esp8266_task.h"

#include "esp8266.h"
#include "bit_engine_task.h"
#include "rtc.h"
#include "sensor.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

const char URL[] = "192.168.1.210";
const uint16_t PORT = 5002;

void ESP_GetJson()
{
	const char get_t[] = "GET http://192.168.1.210/test HTTP/1.1\r\nHost: 192.168.1.210\r\n\r\n";
	ESP_SendData(URL, PORT, get_t, sizeof(get_t) - 1, 1);
	vTaskDelay(pdMS_TO_TICKS(1000));

	char* ptr = NULL;
	if( (ptr = strstr(ESP_GetRxData(), "{")) != NULL )
	{
		clear_info(outputs, 0);
		update_info_from_json(ptr, outputs, 0);
	}
}

void ESP_UpdateTime()
{
	const char get_t[] = "GET http://192.168.1.210/timestamp HTTP/1.1\r\nHost: 192.168.1.210\r\n\r\n";
	ESP_SendData(URL, PORT, get_t, sizeof(get_t) - 1, 1);
	vTaskDelay(pdMS_TO_TICKS(1000));

	char* ptr_tm = NULL;
	char* ptr_tz = NULL;
	if( (ptr_tm = strstr(ESP_GetRxData(), "\"tm:")) != NULL
	 && (ptr_tz = strstr(ESP_GetRxData(),  ",tz:")) != NULL )
	{
		struct tm tmp;
		long tm = strtol(ptr_tm + 4, NULL, 10);
		int8_t tz = strtol(ptr_tz + 4, NULL, 10);
		(void) localtime_r((time_t *)&tm, &tmp);
		tmp.tm_hour += tz;
		RTC_SetTime(&tmp);
	}
}

void ESP_SendSensorList()
{
	char buff[512] = { 0 };

	char prefix[] = "POST http://192.168.1.210:5002/register HTTP/1.1\r\nHost: 192.168.1.210\r\n\r\n{\"SENSOR\":[";
	char postfix[] = "]}";

	strcpy(buff, prefix);
	for(uint8_t i = 0; i < SENSOR_MAX; ++i)
	{
		char temp[64];
		if(sensor_list[i].addr == 0) {
			continue;
		}
		sprintf(temp, "{\"SN\": %04" PRIx16 "\"TYPE\": %d},", sensor_list[i].addr, sensor_list[i].type);
		strcat(buff, temp);
	}
	buff[strlen(buff) - 1] = 0;
	strcpy(buff, postfix);

	ESP_SendData(URL, PORT, buff, sizeof(buff) - 1, 1);
}

void ESP_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    ESP_Init(vTaskDelay);

    ESP_SetMode(ESP_MODE_BOTH);
    ESP_SetCipmuxZero();
    ESP_SetParamsSoftAP("DontTouchAnything", "save_809");

    ESP_UpdateTime();

    while(1)
    {
    	ESP_GetJson();
    	vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void ESP_TaskInit( void )
{
	TaskHandle_t xHandle = NULL;
    xTaskCreate(
    				ESP_Task,      				 /* Function that implements the task. */
                    "ESP_Task",    				 /* Text name for the task. */
					2048,	 					 /* Stack size in words, not bytes. */
                    ( void * ) 1,     		 	 /* Parameter passed into the task. */
                    4,				 			 /* Priority at which the task is created. */
                    &xHandle );   		   	     /* Used to pass out the created task's handle. */
}


