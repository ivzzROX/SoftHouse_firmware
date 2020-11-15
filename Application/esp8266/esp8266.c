/*
 * esp8266.c
 *
 *  Created on: 21 сент. 2020 г.
 *      Author: D-D-E
 */

#include "esp8266.h"

#include "uart2.h"
#include "ring_buffer.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>

struct ring_buffer_struct UART2_rx_data;
char request[1024];
static f_ptr delay;

void ESP_Init(f_ptr delay_function)
{
	RingBuffInit(&UART2_rx_data);
	UART2_Init(&UART2_rx_data);
	UART2_SendData("\r\n", 2);
	delay = delay_function;
}

void UART_Test()
{
	char test_str[] = "uart2 work\r\n";
	UART2_SendData(test_str, sizeof(test_str));
}

static void requestFlush()
{
	memset(request, 0, 1024);
}

static uint8_t charCallBack(char * key, uint16_t bias)
{
	char * temp = RingBuffGetArray(&UART2_rx_data) + bias;
	if(key != NULL && strlen(temp) >= strlen(key))
	{
		if(strstr(temp, key) != NULL)
		{
			return 1;
		}
	}

	return 0;
}

static uint8_t waitCallBack(char * aim, uint16_t ms)
{
	for( ; ms > 0; --ms)
	{
		if(charCallBack(aim, 0))
		{
			RingBuffClear(&UART2_rx_data);
			return 1;
		}
		delay(1);
	}
	RingBuffClear(&UART2_rx_data);
	return 0;
}

char* ESP_GetRxData()
{
	return RingBuffGetArray(&UART2_rx_data);
}

uint8_t ESP_SoftReset()
{
	UART2_SendData("AT+RST\r\n", 8);
	return 1;
}

uint8_t ESP_SetMode(uint8_t mode)
{
	requestFlush();
	sprintf(request, "AT+CWMODE=%d\r\n", mode);
	RingBuffClear(&UART2_rx_data);
	UART2_SendData(request, strlen(request));

	if(waitCallBack("OK", 2000))
	{
		UART2_SendData("AT+RST\r\n", 8);
		if(waitCallBack("ready", 2000))
		{
			return 1;
		}
		else asm("NOP");
	}

	return 0;
}

uint8_t ESP_SetCipmuxZero()
{
	requestFlush();
	sprintf(request, "AT+CIPMUX=0\r\n");
	UART2_SendData(request, strlen(request));
	RingBuffClear(&UART2_rx_data);

	if(waitCallBack("OK", 500))
	{
		return 1;
	}
	return 0;
}

uint8_t ESP_SetParamsSoftAP(char* ssid, char* password)
{
	if(ssid != NULL && password != NULL)
	{
		requestFlush();
		sprintf(request, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, password);
		RingBuffClear(&UART2_rx_data);
		UART2_SendData(request, strlen(request));

		if(waitCallBack("OK", 10000))
		{
			return 1;
		}
	}
	return 0;
}

uint8_t ESP_SetParamsStation(char * ssid, char * password)
{
	if(ssid != NULL && password != NULL)
	{
		requestFlush();
		sprintf(request, "AT+CWSAP=\"%s\",\"%s\",5,3\r\n", ssid, password);
		RingBuffClear(&UART2_rx_data);
		UART2_SendData(request, strlen(request));

		if(waitCallBack("OK", 500))
		{
			UART2_SendData("AT+RST\r\n", 8);
			if(waitCallBack("ready", 2000))
			{
				return 1;
			}
			else asm("NOP");
		}
	}

	return 0;
}

uint8_t ESP_SendData(const char *url, uint16_t port, const char *data, uint32_t dataLength, uint8_t flagRN)
{
	requestFlush();
	sprintf(request, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", url, port);
	RingBuffClear(&UART2_rx_data);
	UART2_SendData(request, strlen(request));

	if(waitCallBack("OK", 2000))
	{
		requestFlush();
		sprintf(request, "AT+CIPSEND=%lu\r\n", dataLength);
		RingBuffClear(&UART2_rx_data);
		UART2_SendData(request, strlen(request));
		delay(10);

		if(waitCallBack(">", 1000) && data != NULL)
		{
			for(int i = 0; i < dataLength; i++)
			{
				UART2_SendData_byte(data[i]);
			}
			if(flagRN)
			{
				UART2_SendData("\r\n", 2);
			}
		}

	} else {
		requestFlush();
		sprintf(request, "AT+CIPCLOSE\r\n");
		UART2_SendData(request, strlen(request));
	}

	return 1;
}
