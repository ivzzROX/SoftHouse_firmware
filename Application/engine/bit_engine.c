#include "bit_engine.h"

#include "sensor.h"
#include "time_engine.h"
#include "telegram.h"
#include "ino.h"

#include <stdio.h>

//TODO: Refactor triggers and delay, common bodies and different incoming conditions

static uint8_t t_trigger_update(OP_ROOT* root, uint16_t root_n, T_TR* ttr, uint16_t state)
{
	if(ttr->root_id == -1)
	{
		if(ttr->prev_root_value == 0
		&& state == 1)
		{
			ttr->value ^= 1;
		}
		ttr->prev_root_value = state;
		return ttr->value;
	}

	for(uint16_t i = 0; i < root_n; ++i)
	{
		if(root[i].id == ttr->root_id)
		{
			if(ttr->prev_root_value == 0
			&& root[i].result == 1)
			{
				ttr->value ^= 1;
			}
			ttr->prev_root_value = root[i].result;
			return ttr->value;
		}
	}
	return 0;
}

static uint8_t rs_trigger_update(OP_ROOT* root, uint16_t root_n, RS_TR* rstr, uint16_t state)
{
	uint8_t temp = 0;
	for(uint16_t i = 0; i < root_n; ++i)
	{
		temp = rstr->value;
		if(root[i].id == rstr->root_id_s)
		{
			if(rstr->prev_root_s_value == 0
			&& root[i].result == 1)
			{
				rstr->value = 1;
			}
			rstr->prev_root_s_value = root[i].result;
			temp = rstr->value;
		}
		if(root[i].id == rstr->root_id_r)
		{
			if(rstr->prev_root_r_value == 0
			&& root[i].result == 1)
			{
				rstr->value = 0;
			}
			rstr->prev_root_r_value = root[i].result;
			temp = rstr->value;
		}
	}
	return temp;
}

static uint8_t counter_update(OP_ROOT* root, uint16_t root_n, CNTR* cntr, uint16_t value, uint16_t state)
{
	uint8_t temp = 0;

	if( cntr->root_id == -1 )
	{
		if( cntr->prev_root_value == 0
		&& state == 1 )
		{
			cntr->value++;

			if( cntr->value ==  value )
			{
				cntr->value = 0;
				temp = 1;
			}

			if( cntr->value > value )
			{
				cntr->value = 0;
				temp = 0;
			}
		}
		cntr->prev_root_value = state;
		return temp;
	}

	for(uint16_t i = 0; i < root_n; ++i)
	{
		if( cntr->root_id == root[i].id )
		{
			if( cntr->prev_root_value == 0
			&& root[i].result == 1 )
			{
				cntr->value++;

				if( cntr->value ==  value )
				{
					cntr->value = 0;
					temp = 1;
				}

				if( cntr->value > value )
				{
					cntr->value = 0;
					temp = 0;
				}
			}
			cntr->prev_root_value = root[i].result;
			return temp;
		}
	}

	return 0;
}

static uint8_t delay_update(OP_ROOT* root, uint16_t root_n, DL* delay, uint16_t state)
{
	uint8_t temp = 0;
	time_t utime = 0;
	struct tm time;

	if( delay->root_id == -1 )
	{
		get_cur_time(&time);
		utime = mktime(&time);

		if(delay->prev_root_value == 0
		&& state == 1)
		{
			delay->finish_time = utime + delay->value;
		}

		if(utime >= delay->finish_time && delay->finish_time > 0)
		{
			temp = 1;
			delay->finish_time = 0;
		}

		delay->prev_root_value = state;
		return temp;
	}

	for(uint16_t i = 0; i < root_n; ++i)
	{
		if( root[i].id == delay->root_id )
		{
			get_cur_time(&time);
			utime = mktime(&time);

			if(delay->prev_root_value == 0
			&& root[i].result == 1)
			{
				delay->finish_time = utime + delay->value;
			}

			if(utime >= delay->finish_time && delay->finish_time > 0)
			{
				temp = 1;
				delay->finish_time = 0;
			}

			delay->prev_root_value = root[i].result;
			return temp;
		}
	}
	return 0;
}

static uint8_t update_type_state(OP_ROOT* root, uint16_t root_n, uint16_t curr_root_n)
{
	for (uint16_t op = 0; op < root[curr_root_n].operation_n; ++op)
	{
		OP* op_arr = (OP*)root[curr_root_n].operation;
		if (op_arr[op].type == STATE)
		{
			for (uint16_t n = 0; n < root_n; ++n)
			{
				if (op_arr[op].root_id == root[n].id)
				{
					op_arr[op].trigger_value = root[n].result;
					break;
				}
			}
		}
	}
	return 1;
}

static uint8_t bit_proces(OP_ROOT* root, uint16_t root_n, uint16_t curr_root_n, uint16_t out)
{
	OP* op_arr = root[curr_root_n].operation;
	uint16_t op_n = root[curr_root_n].operation_n;

	if ( op_arr[0].log != FIRST) {
		return 0;
	}

	uint16_t temp = 0;
	uint16_t state = 0; //Sensor_CheckValue(op_arr[0].addr, op_arr[0].trigger_value);

	for (uint16_t i = 0; i < op_n; ++i) {

		switch(op_arr[i].type) {

		case TIME:
			temp = is_cur_time((TM*)op_arr[i].data);
			break;

		case WEEK:
			temp = is_cur_day_of_week(op_arr[i].trigger_value);
			break;

		case INO:
			temp = INO_GetValue(op_arr[i].trigger_value);
			break;

		case WEB:
			temp = Telegram_GetValue(op_arr[i].trigger_value);
			break;

		case PMO:
			temp = out & (1 << op_arr[i].trigger_value);
			break;

		case COUNTER:
			temp = counter_update(root, root_n, (CNTR*)op_arr[i].data, op_arr[i].trigger_value, state);
			break;

		case DELAY:
			temp = delay_update(root, root_n, (DL*)op_arr[i].data, state);
			break;

		case T_TRIGGER:
			temp = t_trigger_update(root, root_n, (T_TR*)op_arr[i].data, state);
			break;

		case RS_TRIGGER:
			temp = rs_trigger_update(root, root_n, (RS_TR*)op_arr[i].data, state);
			break;

		case SENSOR:
			temp = Sensor_CheckValue(op_arr[i].addr, op_arr[i].trigger_value);
			break;

		case STATE:
			update_type_state(root, root_n, curr_root_n);
			temp = op_arr[i].trigger_value;
			break;

		default:
			temp = 0;
			break;
		}

		if(i == 0) {
			state = temp;
			continue;
		}

		//OPETATIONS
		switch (op_arr[i].log)
		{
		case FIRST:
			state = temp;
			break;

		case OR:
			state |= temp;
			break;

		case AND:
			state &= temp;
			break;

		case XOR:
			state ^= temp;
			break;

		case NOR:
			state |= temp;
			state ^= 1;
			break;

		case NAND:
			state &= temp;
			state ^= 1;
			break;

		case XNOR:
			state ^= temp;
			state ^= 1;
			break;

		case NOT:
			state ^= 1;
			break;

		default:
			break;
		}

	}
	return state;
}

uint8_t start_bit_engine(OP_ROOT* root, uint16_t root_n, uint16_t out)
{
	if (root == NULL)
	{
		return 0;
	}

	for (int32_t i = 0; i < root_n; ++i)
	{
		root[i].result = bit_proces(root, root_n, i, out);
	}

	return 1;
}
