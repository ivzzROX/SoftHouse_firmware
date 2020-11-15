/*
 * spi3.h
 *
 *  Created on: 14 но€б. 2020 г.
 *      Author: D-D-E
 */

#ifndef SPI_SPI3_H_
#define SPI_SPI3_H_

#include <stdint.h>

#define CS 		LL_GPIO_PIN_15
#define CS_PORT GPIOA

typedef enum
{
	CS_RESET = 0,
	CS_SET
}GPIO_CsPinState;

void SPI3_Init(void);

uint8_t SPI3_TransferReceive(uint8_t data);
void SPI3_Receive(uint8_t *data, uint32_t data_size);
void SPI3_Transfer(uint8_t *data, uint32_t data_size);

void CS_Pin_Set(uint8_t val);

#endif /* SPI_SPI3_H_ */
