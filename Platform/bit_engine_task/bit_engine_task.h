/*
 * bit_engine_task.h
 *
 *  Created on: 20 сент. 2020 г.
 *      Author: D-D-E
 */

#ifndef BIT_ENGINE_TASK_BIT_ENGINE_TASK_H_
#define BIT_ENGINE_TASK_BIT_ENGINE_TASK_H_

#include "engine_types.h"

void BitEngine_TaskInit( void );

struct OUTPUTS
{
	struct TM tim[8];
	struct OP par[128];
	struct OP_ROOT root_par[16];
	uint16_t branch_n;
}outputs[16];

void update_info_from_json(char* buff, struct OUTPUTS* out, uint16_t out_n);
void clear_info(struct OUTPUTS* out, uint16_t out_n);

uint16_t Get_OutValue();

#endif /* BIT_ENGINE_TASK_BIT_ENGINE_TASK_H_ */
