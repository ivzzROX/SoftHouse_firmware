/*
 * esp8266_task.c
 *
 *  Created on: 21 сент. 2020 г.
 *      Author: D-D-E
 */

#include "esp8266_task.h"

#include "esp8266.h"
#include "requests.h"
#include "perih.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

void ESP_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    ESP_Init(vTaskDelay);
    CN_State connection_state = RST;

    while(1)
    {
    	switch(connection_state)
    	{
    	case RST:
    		FLed_Set(OFF);
    		ESP_SoftReset();

    		connection_state = SET_MODE;
    		break;

    	case SET_MODE:
    		ESP_SetMode(ESP_MODE_BOTH);

    		connection_state = CONNECT_WIFI;
    		break;

    	case CONNECT_WIFI:
    		ESP_SetParamsSoftAP("DontTouchAnything", "save_809");
    		ESP_UpdateTime();
    		FLed_Set(ON);

    		connection_state = SEND_REQ;
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


