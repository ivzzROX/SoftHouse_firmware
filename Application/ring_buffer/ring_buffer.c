/*
 * ring_buffer.c
 *
 *  Created on: Nov 19, 2019
 *      Author: dmitr
 */

#include "ring_buffer.h"

void RingBuffInit(struct ring_buffer_struct *rng)
{
	rng->index_in = rng->index_out = 0;
}

void RingBuffPush(struct ring_buffer_struct *rng, char element)
{
	rng->buffer[rng->index_in++] = element;
	rng->index_in &= BUF_MASK;
}

char RingBuffPop(struct ring_buffer_struct *rng)
{
	char element = rng->buffer[rng->index_out++];
	rng->index_out &= BUF_MASK;
	return element;
}

uint8_t RingBuffIsEmpty(struct ring_buffer_struct *rng)
{
	if (rng->index_in == rng->index_out)
	{
		return 1;
	}
	return 0;
}

void RingBuffClear(struct ring_buffer_struct *rng)
{
	memset(rng->buffer, 0, BUF_SIZE);
	rng->buffer[0] = 0;
	rng->index_out = rng->index_in = 0;
}

uint8_t RingBuffLength(struct ring_buffer_struct *rng)
{
	return (rng->index_in - rng->index_out) & BUF_MASK;
}

char * RingBuffGetArray(struct ring_buffer_struct *rng)
{
	return ((char *)rng->buffer);
}

