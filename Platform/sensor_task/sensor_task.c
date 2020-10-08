/*
 * sensor_task.c
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: D-D-E
 */

#include "sensor_task.h"

#include "sensor.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"


void Sensor_task( void * pvParameters )
{
	configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

	Sensor_Init(vTaskDelay);
	Sensor_UpdateList();

	while(1)
	{
		if(Sensor_GPIOUpdateList())
		{
			Sensor_UpdateList();
		}

		Sensor_UpdateValue();

		vTaskDelay(pdMS_TO_TICKS(70));
	}
}

void Sensor_TaskInit( void )
{
	TaskHandle_t xHandle = NULL;

	BaseType_t xReturned =
	xTaskCreate(
					Sensor_task,      				 /* Function that implements the task. */
                    "Sensor_task",    		     	 /* Text name for the task. */
					128,	 						 /* Stack size in words, not bytes. */
                    ( void * ) 1,     			 	 /* Parameter passed into the task. */
                    4,				 				 /* Priority at which the task is created. */
                    &xHandle );   			   	     /* Used to pass out the created task's handle. */

}
