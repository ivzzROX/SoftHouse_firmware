//#pragma once
#ifndef ENG_T_H
#define ENG_T_H

#include <stdint.h>
#include <time.h>

#define BIT_CHECK_MASK 0b1000000000000000

enum LG
{
	FIRST = 1,
	OR,
	AND,
	NOR,
	NAND
	//OR_NOT,
	//AND_NOT
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
	TLGRM
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