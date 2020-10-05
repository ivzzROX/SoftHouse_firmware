#include "main.h"
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"

extern TIM_HandleTypeDef htim1;

void NMI_Handler(void)
{

}

void HardFault_Handler(void)
{
	while (1)
	{

	}
}

void MemManage_Handler(void)
{
	while (1)
	{

	}
}

void BusFault_Handler(void)
{
	while (1)
	{

	}
}

void UsageFault_Handler(void)
{
	while (1)
	{

	}
}

void DebugMon_Handler(void)
{

}

void TIM1_UP_TIM10_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim1);
}
