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

#define BOARD_LED	LL_GPIO_PIN_1
#define BOARD_PORT	GPIOA

#define FSTR_PORT	GPIOE

enum FSTR_LED
{
	F_LED = LL_GPIO_PIN_4,
	S_LED = LL_GPIO_PIN_5,
	T_LED = LL_GPIO_PIN_3,
	R_LED = LL_GPIO_PIN_2
};


//TODO: Refactor fst array, replace by freertos query
uint8_t FST_VALUE[3] = {0};

static void BoardLED_Init()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = BOARD_LED;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	LL_GPIO_Init(BOARD_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;//LL_GPIO_OUTPUT_OPENDRAIN;
	LL_GPIO_Init(FSTR_PORT, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(BOARD_PORT, BOARD_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, F_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, S_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, T_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, R_LED);
}

static void BoardLED_Set(uint8_t val)
{
	if(val) {
		LL_GPIO_SetOutputPin(BOARD_PORT, BOARD_LED);
	} else {
		LL_GPIO_ResetOutputPin(BOARD_PORT, BOARD_LED);
	}
}

static void FSTRLED_Set(uint32_t led, uint8_t val)
{
	if(val) {
		LL_GPIO_SetOutputPin(FSTR_PORT, led);
	} else {
		LL_GPIO_ResetOutputPin(FSTR_PORT, led);
	}
}

void Led_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    BoardLED_Init();
    while(1)
    {
    	FSTRLED_Set(F_LED, FST_VALUE[0]);
    	FSTRLED_Set(S_LED, FST_VALUE[1]);
    	FSTRLED_Set(T_LED, FST_VALUE[2]);
    	FSTRLED_Set(R_LED, 0);
    	vTaskDelay(pdMS_TO_TICKS(10));
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
