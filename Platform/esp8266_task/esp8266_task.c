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

	char prefix[] = "POST http://192.168.1.210:5002/sensors HTTP/1.1\r\nHost: 192.168.1.210\r\nContent-Type: application/json\r\nContent-Length:      \r\n\r\n{\"SENSOR\":[";
	char postfix[] = "]}";

	strcpy(buff, prefix);
	for(uint8_t i = 0; i < SENSOR_MAX; ++i)
	{
		char temp[64];
		if(sensor_list[i].addr == 0) {
			continue;
		}
		sprintf(temp, "{\"SN\": \"%04" PRIx16 "\", \"TYPE\": %d}, ", sensor_list[i].addr, sensor_list[i].type);
		strcat(buff, temp);
	}
	buff[strlen(buff) - 2] = 0;
	strcat(buff, postfix);

	char* ptr = strstr(buff, "Content-Length:");
	char* ptr_stt = strstr(buff, "\r\n\r\n");
	char* ptr_end = strstr(buff, "]}");
	char content_length[5];
	sprintf(content_length, "%d", ptr_end - ptr_stt - 2);
	memcpy(ptr + 16, content_length, strlen(content_length));

	ESP_SendData(URL, PORT, buff, sizeof(buff) - 1, 1);
}

typedef enum CONNECNION_STATE {
	RST = 1,
	SET_MODE,
	CONNECT_WIFI,
	SEND_REQ
}connection_state;

void ESP_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    ESP_Init(vTaskDelay);
    connection_state = 1;

    while(1)
    {
    	switch(connection_state)
    	{
    	case RST:
    		ESP_SoftReset();
    		connection_state = SET_MODE;
    		break;

    	case SET_MODE:
    		if(ESP_SetMode(ESP_MODE_BOTH) == 1)
    		{
    			connection_state = CONNECT_WIFI;
    		} else {
    			connection_state = RST;
    		}
    		break;

    	case CONNECT_WIFI:
    		if( ESP_SetParamsSoftAP("DontTouchAnything", "save_809") == 1) {
    			ESP_UpdateTime();
    			connection_state = SEND_REQ;
    		}
    		break;

    	case SEND_REQ:
    		ESP_GetJson();
    		break;

    	default:
    		break;

    	}

    	//ESP_SendSensorList();
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


