/*
 * led_task.c
 *
 *  Created on: 20 сент. 2020 г.
 *      Author: D-D-E
 */
#include "led_task.h"

#include "stm32f4xx_ll_gpio.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

static void BoardLED_Init()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void BoardLED_Set(uint8_t val)
{
	if(val) {
		LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
	} else {
		LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);
	}
}

void Led_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    BoardLED_Init();
    while(1)
    {
    	BoardLED_Set(1);
    	vTaskDelay(pdMS_TO_TICKS(500));
    	BoardLED_Set(0);
    	vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Led_TaskInit( void )
{
	//BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

    //xReturned =
	xTaskCreate(
    				Led_Task,      				 /* Function that implements the task. */
                    "Led_Task",    				 /* Text name for the task. */
					128,	 					 /* Stack size in words, not bytes. */
                    ( void * ) 1,     		 	 /* Parameter passed into the task. */
                    4,				 			 /* Priority at which the task is created. */
                    &xHandle );   		   	     /* Used to pass out the created task's handle. */
}
