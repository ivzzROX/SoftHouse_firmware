#ifndef RING_BUFFER_RING_BUFFER_H_
#define RING_BUFFER_RING_BUFFER_H_
#include <stdint.h>
#include <string.h>

#define BUF_SIZE 4096
#define BUF_MASK (BUF_SIZE-1)

struct ring_buffer_struct {
	uint16_t index_in, index_out;
	char buffer[BUF_SIZE];
};

void RingBuffInit(struct ring_buffer_struct *rng);

void RingBuffPush(struct ring_buffer_struct *rng, char element);
char RingBuffPop(struct ring_buffer_struct *rng);

uint8_t RingBuffIsEmpty(struct ring_buffer_struct *rng);
void RingBuffClear(struct ring_buffer_struct *rng);

uint8_t RingBuffLength(struct ring_buffer_struct *rng);
char * RingBuffGetArray(struct ring_buffer_struct *rng);

#endif /* RING_BUFFER_RING_BUFFER_H_ */
