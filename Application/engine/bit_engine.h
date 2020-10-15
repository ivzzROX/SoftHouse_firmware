//#pragma once
#ifndef BIT_E_H
#define BIT_E_H

#include "engine_types.h"


uint8_t bit_proces(OP* op_arr, uint16_t op_n);
uint8_t start_bit_engine(OP_ROOT* root, uint16_t root_n, uint16_t ino);

uint8_t map_op_on_root(OP* op, OP_ROOT* root, uint16_t root_n);

uint8_t print_roots(OP_ROOT* root, uint16_t root_n);
#endif
