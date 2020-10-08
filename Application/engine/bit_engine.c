#include "bit_engine.h"

#include "sensor.h"
#include "time_engine.h"
#include "telegram.h"
#include "ino.h"

#include <stdio.h>

uint8_t bit_proces(struct OP* op_arr, uint16_t op_n)
{
	if ( op_arr[0].log == FIRST)
	{
		uint16_t temp = 0;
		uint16_t state = Sensor_CheckValue(op_arr[0].addr, op_arr[0].trigger_value);

		for (uint16_t i = 1; i < op_n; ++i)
		{
			if (op_arr[i].type == DATA)
			{
				temp = Sensor_CheckValue(op_arr[i].addr, op_arr[i].trigger_value);
			}

			if (op_arr[i].type == STATE)
			{
				temp = op_arr[i].addr;
			}

			switch (op_arr[i].log)
			{
			case OR:
			{
				state |= temp;
				break;
			}

			case AND:
			{
				state &= temp;
				break;
			}

			case XOR:
			{
				state ^= temp;
				break;
			}

			case NOR:
			{
				state |= temp;
				state = ~state;
				break;
			}

			case NAND:
			{
				state &= temp;
				state = ~state;
				break;
			}

			case XNOR:
			{
				state ^= temp;
				state = ~state;
				break;
			}

			case NOT:
			{
				state = ~state;
				break;
			}

			default:
				break;
			}
		}

		return state;
	}
	return 0;
}

uint8_t map_op_on_root(struct OP* op, struct OP_ROOT* root, uint16_t root_n)
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

uint8_t start_bit_engine(struct OP_ROOT* root, uint16_t root_n, uint16_t ino)
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
			root[i].result = is_cur_time((struct TM*)root[i].operation);
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

		case BRCH:
			for (uint16_t o = 1; o < root[i].operation_n; ++o)
			{
				struct OP* op_arr = (struct OP*)root[i].operation;
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
							op_arr[o].addr = root[n].result;
							break;
						}
					}
				}
			}
			root[i].result = bit_proces((struct OP*)root[i].operation, root[i].operation_n);
			break;
		}
	}

	return 1;
}

// JUST CONSOLE OUTPUT FUNCTION

static const char* lg_type_to_str(enum LG log)
{
	switch (log)
	{
	case FIRST:		return " ";
	case OR:		return "OR ";
	case AND:		return "AND ";
	case NOR:		return "NOR ";
	case NAND:		return "NAND ";
	default:		return " ";
	}
}

static char days[] = "S  M  T  W  T  F  S  ";
static char* week_val_to_str(uint16_t val)
{
	for (int i = 0, n = 1; i < 7; ++i, n += 3)
	{
		if (val & (1 << i))
		{
			days[n] = 'v';
		}
		else days[n] = 'x';
	}
	return days;
}

static char* root_name_by_id(struct OP_ROOT* root, uint16_t root_n, uint8_t id)
{
	for (int i = 0; i < root_n; ++i)
	{
		if (root[i].id == id)
		{
			return root[i].name;
		}
	}
	return 0;
}

uint8_t print_roots(struct OP_ROOT* root, uint16_t root_n)
{
	if (root[0].type == PMO || root[0].type == INO)
	{
		return 0;
	}

	for (int i = 0; i < root_n; ++i)
	{
		printf("%s:\t", root[i].name);
		
		switch (root[i].type)
		{
		case BRCH:
			;
			struct OP* op_arr = (struct OP*)root[i].operation;
			for (int n = 0; n < root[i].operation_n; ++n)
			{
				if (op_arr[n].type == DATA)
				{
					printf("%s%04x ", lg_type_to_str(op_arr[n].log), op_arr[n].addr);
				}
				if (op_arr[n].type == STATE)
				{
					printf("%s%s ", lg_type_to_str(op_arr[n].log), root_name_by_id(root, root_n, op_arr[n].root_id));
				}
			}
			break;
		case TIME:
			;
			struct TM* time = (struct TM*)root[i].operation;
			printf("From:%02d:%02d to:%02d:%02d", time->from.tm_hour,
				time->from.tm_min, time->to.tm_hour, time->to.tm_min);
			break;
		case WEEK:
			printf("Days: %s", week_val_to_str(root[i].operation_n));
			break;
		case PMO:
			printf("Power multiplied output %d", root[i].operation_n);
			break;
		case INO:
			printf("Ext intput output %d", root[i].operation_n);
			break;
		case WEB:
			printf("Telegram id %d", root[i].operation_n);
			break;
		}

		printf("\tres: %d\r\n", root[i].result);
	}

	return 1;
}
