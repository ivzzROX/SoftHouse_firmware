/*
 * uart3.c
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: D-D-E
 */

#include "uart3.h"

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_utils.h"

static struct ring_buffer_struct* UART_rx_data;

void UART3_Init(struct ring_buffer_struct* rx_data)
{
	LL_USART_InitTypeDef USART_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */

	/**USART3 GPIO Configuration
	PD8   ------> USART3_TX
	PD9   ------> USART3_RX
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_8|LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(USART3_IRQn);

	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART3, &USART_InitStruct);
	LL_USART_EnableIT_RXNE(USART3);
	LL_USART_ConfigAsyncMode(USART3);
	LL_USART_Enable(USART3);

	UART_rx_data = rx_data;
}

void Sensor_UpdateGpioInit()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t Sensor_GpioValue()
{
	return LL_GPIO_ReadInputPort(GPIOA) & LL_GPIO_PIN_0;
}

void UART3_SendData_byte(char data)
{
	LL_USART_TransmitData8(USART3, data);
	while (!LL_USART_IsActiveFlag_TC(USART3));
}

void UART3_SendData(char* data, uint32_t data_length)
{
	for(uint32_t i = 0; i < data_length; ++i)
	{
		UART3_SendData_byte(data[i]);
	}
}

void UART3_CharReception_Callback(void)
{
	__IO uint8_t received_char;
	received_char = LL_USART_ReceiveData8(USART3);

	RingBuffPush(UART_rx_data, (char)received_char);
}

static void Error_Callback(void)
{
	//dummy
}

void USART3_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_RXNE(USART3)) {
		UART3_CharReception_Callback();
		LL_USART_ClearFlag_RXNE(USART3);
	} else {
		Error_Callback();
	}
}
