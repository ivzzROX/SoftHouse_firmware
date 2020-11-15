/*
 * spi3.c
 *
 *  Created on: 14 но€б. 2020 г.
 *      Author: D-D-E
 */

#include "spi3.h"

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_utils.h"

void SPI3_Init(void)
{
	LL_SPI_InitTypeDef SPI_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/**SPI3 GPIO Configuration
	PB3   ------> SPI3_SCK
	PB4   ------> SPI3_MISO
	PB5   ------> SPI3_MOSI
	*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_3|LL_GPIO_PIN_4|LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
	SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
	SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
	SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
	SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
	SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
	SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
	SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	SPI_InitStruct.CRCPoly = 10;
	LL_SPI_Init(SPI3, &SPI_InitStruct);
	LL_SPI_SetStandard(SPI3, LL_SPI_PROTOCOL_MOTOROLA);

	GPIO_InitStruct.Pin = CS;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(CS_PORT, &GPIO_InitStruct);

	LL_SPI_Enable(SPI3);
}

void CS_Pin_Set(uint8_t val)
{
	if(val) {
		LL_GPIO_SetOutputPin(CS_PORT, CS);
	} else {
		LL_GPIO_ResetOutputPin(CS_PORT, CS);
	}
}

uint8_t SPI3_TransferReceive(uint8_t data)
{
	while(LL_SPI_IsActiveFlag_BSY(SPI3));
	while(!LL_SPI_IsActiveFlag_TXE(SPI3));

	if(LL_SPI_IsActiveFlag_RXNE(SPI3))
	{
		LL_SPI_ReceiveData8(SPI3);
	}

	LL_SPI_TransmitData8(SPI3, data);
	while(!LL_SPI_IsActiveFlag_TXE(SPI3));
	while(!LL_SPI_IsActiveFlag_RXNE(SPI3));

	return LL_SPI_ReceiveData8(SPI3);
}

void SPI3_Receive(uint8_t *data, uint32_t data_size)
{
	while(LL_SPI_IsActiveFlag_BSY(SPI3));

	uint32_t i = 0;

	LL_SPI_TransmitData8(SPI3, 0);
	while(!LL_SPI_IsActiveFlag_TXE(SPI3));

	while(LL_SPI_IsActiveFlag_RXNE(SPI3))
	{
		data[i++] = LL_SPI_ReceiveData8(SPI3);
		if(i >= data_size)
		{
			return;
		}
		LL_SPI_TransmitData8(SPI3, 0);
		while(!LL_SPI_IsActiveFlag_TXE(SPI3));
	}

	return;
}

void SPI3_Transfer(uint8_t *data, uint32_t data_size)
{
	while(LL_SPI_IsActiveFlag_BSY(SPI3));

	for(uint32_t i = 0; i < data_size; ++i)
	{
		LL_SPI_TransmitData8(SPI3, data[i]);
		while(!LL_SPI_IsActiveFlag_TXE(SPI3));
		//while(!LL_SPI_IsActiveFlag_RXNE(SPI3));
	}

	return;
}
