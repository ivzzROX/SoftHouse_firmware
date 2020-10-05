//#pragma once
#ifndef JSMN_P_H
#define JSMN_P_H

#include "engine_types.h"

int json_get_op(const char* input, struct OP* out, struct OP_ROOT* root, struct TM* time);
#endif