/*
 * sensor.h
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: D-D-E
 */

#ifndef SENSOR_TASK_SENSOR_H_
#define SENSOR_TASK_SENSOR_H_

#include <stdint.h>

#define SENSOR_MAX 		32
#define REQ_TO_DISABLE 	16

enum SENSOR_TYPE
{
	PUSH_BUTTON = 1,
	RS_BUTTON,
	LIGHT,
	TEMPERATURE
};

struct xSENSOR
{
	enum SENSOR_TYPE type;
	uint16_t addr;
	uint32_t value;
	uint8_t error_counter;
}sensor_list[SENSOR_MAX];

typedef void (*f_ptr)(const uint32_t);

void Sensor_Init(f_ptr delay_function);

void Sensor_ClearList();
void Sensor_UpdateList();
void Sensor_UpdateValue();

uint8_t Sensor_CheckValue(uint16_t sen_addr, uint32_t value);

uint8_t Sensor_GPIOUpdateList();

#endif /* SENSOR_TASK_SENSOR_H_ */
