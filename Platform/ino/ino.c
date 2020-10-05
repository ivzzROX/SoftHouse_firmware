/*
 * gpio.c
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: D-D-E
 */

#include "ino.h"

#include "stm32f4xx_ll_gpio.h"

/*
 * INO - PE8-PE15 inputs pins
 */

void INO_Init()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = LL_GPIO_PIN_8 || LL_GPIO_PIN_9 || LL_GPIO_PIN_10 || LL_GPIO_PIN_11
					 || LL_GPIO_PIN_12 || LL_GPIO_PIN_13 || LL_GPIO_PIN_14 || LL_GPIO_PIN_15;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	LL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

uint8_t INO_GetValue(uint8_t pin)
{
	return LL_GPIO_ReadInputPort(GPIOE) & (1 << pin);
}
