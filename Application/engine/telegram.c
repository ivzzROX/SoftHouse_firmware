/*
 * telegram.c
 *
 *  Created on: 1 окт. 2020 г.
 *      Author: D-D-E
 */

#include "telegram.h"

#include <string.h>

#define TG_MAX 256

static struct TLGRM
{
	uint16_t key;
	uint8_t value;
}tg[TG_MAX];

void Telegram_ClearValue()
{
	memset(tg, 0, TG_MAX);
}

void Telegram_UpdateValue(char* arr)
{

}

uint8_t Telegram_GetValue(uint16_t key)
{
	for(uint16_t i = 0; i < TG_MAX; ++i) {
		if (tg[i].key == key) {
			return tg[i].value;
		}
	}
	return 0;
}
