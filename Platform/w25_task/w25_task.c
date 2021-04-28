/*
 * w25_save_task.c
 *
 *  Created on: 15 но€б. 2020 г.
 *      Author: D-D-E
 */

#include "w25_task.h"

#include "w25qxx.h"
#include "engine_types.h"
#include "perih.h"

#include <stdint.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#define UPDATE_TASK_TIME 1000 * 60 * 5 // 5 minutes

void write_outputs()
{
	W25qxx_WriteBlock((uint8_t *)outputs, 1, 0, sizeof(W25qxx_WriteBlock));
}

void read_outputs()
{
	W25qxx_ReadBlock((uint8_t *)outputs, 1, 0, sizeof(W25qxx_WriteBlock));
}


void W25_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    TLed_Set(OFF);

    if(W25qxx_Init() == 1)
    {
    	TLed_Set(ON);
    } else {
    	RLed_Set(ON);
    	vTaskDelete(NULL);
	}

    while(1)
    {
    	vTaskDelay(pdMS_TO_TICKS(UPDATE_TASK_TIME));
    	if(outputs_updated_flag == 1)
    	{
    		write_outputs();
    	}
    }
}

void W25_TaskInit( void )
{
	TaskHandle_t xHandle = NULL;
    xTaskCreate(
    				W25_Task,      			 	 /* Function that implements the task. */
                    "W25_Task",    			 	 /* Text name for the task. */
					256,	 					 /* Stack size in words, not bytes. */
                    ( void * ) 1,     		 	 /* Parameter passed into the task. */
                    4,				 			 /* Priority at which the task is created. */
                    &xHandle );   		   	     /* Used to pass out the created task's handle. */
}
