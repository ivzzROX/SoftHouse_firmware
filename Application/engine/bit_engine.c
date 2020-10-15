#include "bit_engine.h"

#include "sensor.h"
#include "time_engine.h"
#include "telegram.h"
#include "ino.h"

#include <stdio.h>

void t_trigger_set(uint8_t value)
{

}

uint8_t t_trigger_get()
{
	return 0;
}

uint8_t bit_proces(OP* op_arr, uint16_t op_n)
{
	if ( op_arr[0].log == FIRST) {
		return 0;
	}

	uint16_t temp = 0;
	uint16_t state = Sensor_CheckValue(op_arr[0].addr, op_arr[0].trigger_value);

	for (uint16_t i = 1; i < op_n; ++i) {

		if (op_arr[i].type == DATA) {
			temp = Sensor_CheckValue(op_arr[i].addr, op_arr[i].trigger_value);
		}

		if (op_arr[i].type == STATE) {
			temp = op_arr[i].trigger_value; //addr
		}

		switch (op_arr[i].log)
		{

		case OR: {
			state |= temp;
			break;
		}

		case AND: {
			state &= temp;
			break;
		}

		case XOR: {
			state ^= temp;
			break;
		}

		case NOR: {
			state |= temp;
			state ^= 1;
			break;
		}

		case NAND: {
			state &= temp;
			state ^= 1;
			break;
		}

		case XNOR: {
			state ^= temp;
			state ^= 1;
			break;
		}

		case NOT: {
			state ^= 1;
			break;
		}

		default:
			break;
		}
	}

	return state;
}

uint8_t map_op_on_root(OP* op, OP_ROOT* root, uint16_t root_n)
{
	if (root[0].type == PMO || root[0].type == INO || root[0].type == WEB)
	{
		return 0;
	}

	root[0].operation = op;
	uint32_t offset = root[0].operation_n;

	for (uint16_t i = 1; i < root_n; ++i)
	{
		if (root[i].type == BRCH)
		{
			root[i].operation = op + offset;
			offset += root[i].operation_n;
		}
	}
	return 1;
}

uint8_t start_bit_engine(OP_ROOT* root, uint16_t root_n, uint16_t ino)
{
	if (root == NULL)
	{
		return 0;
	}

	for (int32_t i = root_n - 1; i >= 0; --i)
	{
		switch(root[i].type)
		{
		case TIME:
		{
			root[i].result = is_cur_time((TM*)root[i].operation);
			break;
		}

		case WEEK:
		{
			root[i].result = is_cur_day_of_week(root[i].operation_n);
			break;
		}

		case INO:
		{
			uint16_t ino_pin = root[i].operation_n - 200;
			root[i].result = INO_GetValue(ino_pin);
			break;
		}

		case WEB:
		{
			uint16_t tg_key = root[i].operation_n - 600;
			root[i].result = Telegram_GetValue(tg_key);
			break;
		}

		case PMO:
		{
			uint16_t link_out = root[i].operation_n - 800;
			root[i].result = ino & (1 << link_out);
			break;
		}

		case T_TRIGGER:
		{
			root[i].result = (T_TR*)root[i].operation->value;
			break;
		}

		case RS_TRIGGER:
		{
			root[i].result = 0;
			break;
		}

		case BRCH:
			for (uint16_t o = 1; o < root[i].operation_n; ++o)
			{
				OP* op_arr = (OP*)root[i].operation;
				if (op_arr[o].type == STATE)
				{
					/*
						if operration type is state,
						engine need to find result of this state and set it
					*/
					for (uint16_t n = 1; n < root_n; ++n)
					{
						if (op_arr[o].root_id == root[n].id)
						{
							op_arr[o].trigger_value = root[n].result; //addr
							break;
						}
					}
				}
			}
			root[i].result = bit_proces((OP*)root[i].operation, root[i].operation_n);
			break;
		}
	}

	return 1;
}
