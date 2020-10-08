//#pragma once
#ifndef ENG_T_H
#define ENG_T_H

#include <stdint.h>
#include <time.h>

enum LG
{
	FIRST = 1,
	OR,
	AND,
	XOR,
	NOR,
	NAND,
	XNOR,
	NOT
};

enum OP_TYPE
{
	DATA = 2,
	STATE
};

enum ROOT_TYPE
{
	BRCH = 2,
	TIME,
	WEEK,
	PMO, //power multiplied output
	INO, //ext intput output
	WEB
};

struct TM
{
	struct tm from;
	struct tm to;
};

struct OP
{
	uint16_t addr;
	uint32_t trigger_value;
	enum LG log;
	enum OP_TYPE type;
	uint8_t root_id;
};

struct OP_ROOT
{
	enum ROOT_TYPE type;
	void* operation;
	uint16_t operation_n;
	char name[5];
	uint8_t id;
	uint16_t result;
};
#endif
