/*
 * perih.h
 *
 *  Created on: 15 но€б. 2020 г.
 *      Author: D-D-E
 */

#ifndef PERIH_PERIH_H_
#define PERIH_PERIH_H_

#include <stdint.h>

typedef enum
{
	OFF = 0,
	ON
} LED_Status;

void LED_Init();
void BoardLED_Set(uint8_t val);
void FLed_Set(uint8_t val);
void SLed_Set(uint8_t val);
void TLed_Set(uint8_t val);
void RLed_Set(uint8_t val);

void PowerControlInit();
void PowerControlPerih(uint8_t val);

#endif /* PERIH_PERIH_H_ */
