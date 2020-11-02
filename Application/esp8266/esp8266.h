/*
 * esp9266.h
 *
 *  Created on: 21 сент. 2020 г.
 *      Author: D-D-E
 */

#ifndef ESP8266_ESP8266_H_
#define ESP8266_ESP8266_H_

#include <stdint.h>

typedef void (*f_ptr)(const uint32_t);

enum ESP_MODE
{
	ESP_MODE_STATION = 1,
	ESP_MODE_SOFT_AP,
	ESP_MODE_BOTH
};

void ESP_Init(f_ptr delay_function);
void UART_Test();

uint8_t ESP_SetCipmuxZero();

uint8_t ESP_SetMode(uint8_t mode);

uint8_t ESP_SetParamsSoftAP(char * ssid, char * password);
uint8_t ESP_SetParamsStation(char * ssid, char * password);

uint8_t ESP_SendData(const char *url, uint16_t port, const char *data, uint32_t dataLength, uint8_t flagRN);

char* ESP_GetRxData();

uint8_t ESP_SoftReset();

#endif /* ESP8266_ESP8266_H_ */
