/*
 * esp8266_task.h
 *
 *  Created on: 21 сент. 2020 г.
 *      Author: D-D-E
 */

#ifndef ESP8266_TASK_ESP8266_TASK_H_
#define ESP8266_TASK_ESP8266_TASK_H_

typedef enum {
	RST = 1,
	SET_MODE,
	CONNECT_WIFI,
	SEND_REQ
} CN_State;

void ESP_TaskInit( void );

#endif /* ESP8266_TASK_ESP8266_TASK_H_ */
