/*
 * uart2.h
 *
 *  Created on: 21 сент. 2020 г.
 *      Author: D-D-E
 */

#ifndef UART_UART2_H_
#define UART_UART2_H_

#include <stdint.h>

#include "ring_buffer.h"

void UART2_Init(struct ring_buffer_struct* rx_data);

void UART2_SendData_byte(char data);
void UART2_SendData(char* data, uint32_t data_length);

#endif /* UART_UART2_H_ */
