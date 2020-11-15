/*
 * perih.c
 *
 *  Created on: 15 но€б. 2020 г.
 *      Author: D-D-E
 */

#include "perih.h"

#include "stm32f4xx_ll_gpio.h"

#define PWR_CONTROL			LL_GPIO_PIN_1
#define PWR_CONTROL_PORT	GPIOE
#define BOARD_LED			LL_GPIO_PIN_1
#define BOARD_PORT			GPIOA
#define FSTR_PORT			GPIOE

enum FSTR_LED
{
	F_LED = LL_GPIO_PIN_4,
	S_LED = LL_GPIO_PIN_5,
	T_LED = LL_GPIO_PIN_3,
	R_LED = LL_GPIO_PIN_2
};

void PowerControlInit()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = PWR_CONTROL;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	LL_GPIO_Init(PWR_CONTROL_PORT, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(PWR_CONTROL_PORT, PWR_CONTROL);
}

void PowerControlPerih(uint8_t val)
{
	if(val) {
		LL_GPIO_SetOutputPin(PWR_CONTROL_PORT, PWR_CONTROL);
	} else {
		LL_GPIO_ResetOutputPin(PWR_CONTROL_PORT, PWR_CONTROL);
	}
}

void LED_Init()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = BOARD_LED;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	LL_GPIO_Init(BOARD_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = F_LED|S_LED|T_LED|R_LED;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(FSTR_PORT, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(BOARD_PORT, BOARD_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, F_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, S_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, T_LED);
	LL_GPIO_ResetOutputPin(FSTR_PORT, R_LED);
}

void BoardLED_Set(uint8_t val)
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

void FLed_Set(uint8_t val)
{
	FSTRLED_Set(F_LED, val);
}

void SLed_Set(uint8_t val)
{
	FSTRLED_Set(S_LED, val);
}

void TLed_Set(uint8_t val)
{
	FSTRLED_Set(T_LED, val);
}

void RLed_Set(uint8_t val)
{
	FSTRLED_Set(R_LED, val);
}
