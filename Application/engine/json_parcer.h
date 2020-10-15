//#pragma once
#ifndef JSMN_P_H
#define JSMN_P_H

#include "engine_types.h"

int json_get_op(const char* input, OP* out, OP_ROOT* root, TM* time, T_TR* t_trigger);
#endif
