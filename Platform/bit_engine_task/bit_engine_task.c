/*
 * bit_engine_task.c
 *
 *  Created on: 20 сент. 2020 г.
 *      Author: D-D-E
 */

#include "bit_engine_task.h"

#include "bit_engine.h"
#include "time_engine.h"
#include "json_parcer.h"
#include "ino.h"

#include "w25qxx.h"

#include <string.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

static uint16_t out_value = 0;

uint16_t Get_OutValue()
{
	return out_value;
}

void BitEngine_task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    INO_Init();

    vTaskDelay(pdMS_TO_TICKS(10));

    memset(outputs, 0, sizeof(outputs));


    while(1)
    {
    	for (uint8_t i = 0; i < OUTPUTS_N; ++i)
    	{
			if (outputs[i].branch_n == 0 || outputs[i].branch_n > 1024)
			{
				continue;
			}
			start_bit_engine(outputs[i].root_par, outputs[i].branch_n, out_value);

			uint16_t branch = outputs[i].branch_n;
			if(outputs[i].root_par[branch - 1].result) {
				out_value |= 1 << i;
			} else {
				out_value &= 0 << i;
			}
		}

    	vTaskDelay(pdMS_TO_TICKS(40));
    }
}


void BitEngine_TaskInit( void )
{
	TaskHandle_t xHandle = NULL;
	xTaskCreate(
    				BitEngine_task,      			 /* Function that implements the task. */
                    "BitEngine_task",    		     /* Text name for the task. */
					4096,	 						 /* Stack size in words, not bytes. */
                    ( void * ) 1,     			 	 /* Parameter passed into the task. */
                    4,				 				 /* Priority at which the task is created. */
                    &xHandle );   			   	     /* Used to pass out the created task's handle. */

}
