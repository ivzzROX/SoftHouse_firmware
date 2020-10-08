/*
 * relay_task.c
 *
 *  Created on: 20 сент. 2020 г.
 *      Author: D-D-E
 */
#include "relay_task.h"

#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_gpio.h"

#include "bit_engine_task.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"


void SPI2_Init(void)
{
	LL_SPI_InitTypeDef SPI_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/**SPI2 GPIO Configuration
	PC2   ------> SPI2_MISO
	PC3   ------> SPI2_MOSI
	PB13  ------> SPI2_SCK
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
	SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
	SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
	SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
	SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
	SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8;
	SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
	SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	SPI_InitStruct.CRCPoly = 10;
	LL_SPI_Init(SPI2, &SPI_InitStruct);
	LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_Enable(SPI2);
}

static void SPI2_SendByte(uint8_t byte)
{
	while (!(SPI2->SR & SPI_SR_TXE));
	LL_SPI_TransmitData8(SPI2, byte);
	while (SPI2->SR & SPI_SR_BSY);
}

static void GPIO_Init()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = LL_GPIO_PIN_1; //ST_CP latchPin
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void LATCH_Set(uint8_t val)
{
	if(val) {
		LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_1);
	} else {
		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_1);
	}
}

void Relay_Task( void * pvParameters )
{
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    uint16_t out_value = 0;
    uint16_t prev_value = 0;

    SPI2_Init();
    GPIO_Init();

    while(1)
    {
    	out_value = Get_OutValue();
    	if(prev_value != out_value)
    	{
    		prev_value = out_value;
    		LATCH_Set(0);
    		SPI2_SendByte((uint8_t)(out_value >> 8));
    		SPI2_SendByte((uint8_t)(out_value & 0xFF));
    		LATCH_Set(1);
    	}

    	vTaskDelay(pdMS_TO_TICKS(70));
    }
}

void Relay_TaskInit( void )
{
	//BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

    //xReturned =
	xTaskCreate(
    				Relay_Task,      				 /* Function that implements the task. */
                    "Relay_Task",    				 /* Text name for the task. */
					128,	 						 /* Stack size in words, not bytes. */
                    ( void * ) 1,     			 	 /* Parameter passed into the task. */
                    4,				 				 /* Priority at which the task is created. */
                    &xHandle );   			   	     /* Used to pass out the created task's handle. */
}
