/*
 * init_task.c
 *
 *  Created on: 15 но€б. 2020 г.
 *      Author: D-D-E
 */

#include "init_task.h"

#include "engine_types.h"
#include "perih.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

void Init_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    LED_Init();
    PowerControlInit();

    PowerControlPerih(ON);

    BoardLED_Set(OFF);
    FLed_Set(OFF);
    SLed_Set(OFF);
    TLed_Set(OFF);
    RLed_Set(OFF);

    outputs_updated_flag = 0;

    vTaskDelay(pdMS_TO_TICKS(10));
    vTaskDelete(NULL);
}

void Init_TaskInit( void )
{
	TaskHandle_t xHandle = NULL;
    xTaskCreate(
    				Init_Task,      			 /* Function that implements the task. */
                    "Init_Task",    			 /* Text name for the task. */
					2048,	 					 /* Stack size in words, not bytes. */
                    ( void * ) 1,     		 	 /* Parameter passed into the task. */
                    4,				 			 /* Priority at which the task is created. */
                    &xHandle );   		   	     /* Used to pass out the created task's handle. */
}
