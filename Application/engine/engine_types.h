//#pragma once
#ifndef ENG_T_H
#define ENG_T_H

#include <stdint.h>
#include <time.h>

#define OUTPUTS_N 16

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
	WEB,
	T_TRIGGER,
	RS_TRIGGER
};

struct xTm
{
	struct tm from;
	struct tm to;
};

struct xOp
{
	uint16_t addr;
	uint32_t trigger_value;
	enum LG log;
	enum OP_TYPE type;
	uint8_t root_id;
};

struct xOpRoot
{
	enum ROOT_TYPE type;
	void* operation;
	uint16_t operation_n;
	char name[5];
	uint8_t id;
	uint16_t result;
};

struct xRsTr
{
	uint8_t root_id_s;
	uint8_t root_id_r;
	uint8_t value;
};

struct xRsTr
{
	uint8_t root_id;
	uint8_t value;
};

typedef struct xRsTr RS_TR;
typedef struct xRsTr T_TR;

typedef struct xTm TM;
typedef struct xOp OP;
typedef struct xOpRoot OP_ROOT;


struct OUTPUTS
{
	T_TR t_tr[8];
	RS_TR rs_tr[8];

	TM tim[8];

	OP par[128];
	OP_ROOT root_par[16];

	uint16_t branch_n;
}outputs[OUTPUTS_N];

#endif
