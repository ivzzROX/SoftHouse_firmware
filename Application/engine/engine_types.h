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
	SENSOR = 1,
	TIME,
	WEEK,
	PMO,
	INO,
	WEB,
	T_TRIGGER,
	RS_TRIGGER,
	COUNTER,
	DELAY,
	STATE
};

struct xTm
{
	struct tm from;
	struct tm to;
};

struct xOp
{
	uint8_t root_id;
	uint32_t addr;
	void* data;
	uint32_t trigger_value;
	enum LG log;
	enum OP_TYPE type;
};

struct xOpRoot
{
	void* operation;
	uint16_t operation_n;
	char name[5];
	uint8_t id;
	uint8_t result;
};

struct xRsTr
{
	int8_t root_id_s;
	int8_t root_id_r;
	uint8_t value;
	uint8_t prev_root_s_value;
	uint8_t prev_root_r_value;
};

struct xTTr
{
	int8_t root_id;
	uint8_t value;
	uint8_t prev_root_value;
};

struct xCounter
{
	int8_t root_id;
	uint16_t value;
	uint8_t prev_root_value;
};

struct xDelay
{
	int8_t root_id;
	uint16_t value;
	time_t finish_time;
	uint8_t prev_root_value;
};

typedef struct xRsTr RS_TR;
typedef struct xTTr T_TR;

typedef struct xCounter CNTR;
typedef struct xDelay DL;

typedef struct xTm TM;
typedef struct xOp OP;
typedef struct xOpRoot OP_ROOT;

struct OUTPUTS
{
	T_TR t_tr[8];
	RS_TR rs_tr[8];
	CNTR cntr[8];
	DL del[8];
	TM tim[8];
	OP par[128];
	OP_ROOT root_par[16];

	uint16_t branch_n;
}outputs[OUTPUTS_N];

#endif
