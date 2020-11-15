/*
 * led_task.c
 *
 *  Created on: 20 сент. 2020 г.
 *      Author: D-D-E
 */
#include "led_task.h"

#include "perih.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

void Led_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    vTaskDelete(NULL);
    //while(1)
    //{
    //	vTaskDelay(pdMS_TO_TICKS(100));
    //}
}

void Led_TaskInit( void )
{
	TaskHandle_t xHandle = NULL;

	xTaskCreate(
    				Led_Task,      				 /* Function that implements the task. */
                    "Led_Task",    				 /* Text name for the task. */
					128,	 					 /* Stack size in words, not bytes. */
                    ( void * ) 1,     		 	 /* Parameter passed into the task. */
                    4,				 			 /* Priority at which the task is created. */
                    &xHandle );   		   	     /* Used to pass out the created task's handle. */
}
