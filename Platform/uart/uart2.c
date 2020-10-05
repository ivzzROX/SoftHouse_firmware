/*
 * uart2.c
 *
 *  Created on: 21 сент. 2020 г.
 *      Author: D-D-E
 */

#include "uart2.h"

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_utils.h"

static struct ring_buffer_struct* UART_rx_data;

void UART2_Init(struct ring_buffer_struct* rx_data)
{
	LL_USART_InitTypeDef USART_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/**USART2 GPIO Configuration
	PA3   ------> USART2_RX
	PD5   ------> USART2_TX
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(USART2_IRQn);

	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART2, &USART_InitStruct);
	LL_USART_EnableIT_RXNE(USART2);
	LL_USART_ConfigAsyncMode(USART2);
	LL_USART_Enable(USART2);

	UART_rx_data = rx_data;
}

void UART2_SendData_byte(char data)
{
	LL_USART_TransmitData8(USART2, data);
	while (!LL_USART_IsActiveFlag_TC(USART2));
}

void UART2_SendData(char* data, uint32_t data_length)
{
	for(uint32_t i = 0; i < data_length; ++i)
	{
		UART2_SendData_byte(data[i]);
	}
}

void UART2_CharReception_Callback(void)
{
	__IO uint8_t received_char;
	received_char = LL_USART_ReceiveData8(USART2);

	RingBuffPush(UART_rx_data, (char)received_char);
}

static void Error_Callback(void)
{
	//dummy
}

void USART2_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_RXNE(USART2)) {
		UART2_CharReception_Callback();
		LL_USART_ClearFlag_RXNE(USART2);
	} else {
		Error_Callback();
	}
}


