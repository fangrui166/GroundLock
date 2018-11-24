/*
 * ring_buffer.h
 *
 *  Created on: Jan 9, 2017
 *      Author: Ryan Young
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "string.h"
#include "stdint.h"
#include <stddef.h>

#define MAX_BUFFER_SIZE		(130*4)

typedef struct RingBuffer
{
	uint8_t data[MAX_BUFFER_SIZE];
	volatile uint32_t head;
	volatile uint32_t tail;
	uint32_t data_size;
	uint32_t max_size;
} RingBuffer_t;

void ring_buffer_init(RingBuffer_t *rb, uint32_t data_size, uint32_t max_size);

uint8_t ring_buffer_empty(RingBuffer_t *rb);

void ring_buffer_enqueue(RingBuffer_t *rb, uint8_t *data);

uint8_t *ring_buffer_dequeue(RingBuffer_t *rb);

#endif /* RING_BUFFER_H_ */
