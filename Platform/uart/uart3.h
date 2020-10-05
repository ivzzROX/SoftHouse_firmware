/*
 * uart3.h
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: D-D-E
 */

#ifndef UART_UART3_H_
#define UART_UART3_H_

#include <stdint.h>

#include "ring_buffer.h"

void UART3_Init(struct ring_buffer_struct* rx_data);

void UART3_SendData_byte(char data);
void UART3_SendData(char* data, uint32_t data_length);

void Sensor_UpdateGpioInit();
uint8_t Sensor_GpioValue();

#endif /* UART_UART3_H_ */
