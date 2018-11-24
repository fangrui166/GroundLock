/*
 * ring_buffer.c
 *
 *  Created on: Jan 9, 2017
 *      Author: Ryan Young
 */

#include "ring_buffer.h"

void static inc_tail(RingBuffer_t *rb)
{
    rb->tail += rb->data_size;

    /* wrap around */
    if (rb->tail >= rb->max_size)
    {
        rb->tail = 0;
    }
}

void static inc_head(RingBuffer_t *rb)
{
    uint32_t tail = rb->tail;
    rb->head += rb->data_size;

    /* wrap around */
    if (rb->head >= rb->max_size)
    {
        rb->head = 0;
    }

    /* overflow */
    if (rb->head == tail)
    {
        inc_tail(rb);
    }
}

void ring_buffer_init(RingBuffer_t *rb, uint32_t data_size, uint32_t max_length)
{
    rb->head = 0;
    rb->tail = 0;
    rb->data_size = data_size;
    rb->max_size = data_size * max_length;

    /* limit max number of elements */
    if (rb->max_size > MAX_BUFFER_SIZE)
    {
        rb->max_size = (MAX_BUFFER_SIZE / data_size) * data_size;
    }
}

uint8_t ring_buffer_empty(RingBuffer_t *rb)
{
    uint32_t tail = rb->tail;
    return (rb->head == tail);
}

void ring_buffer_enqueue(RingBuffer_t *rb, uint8_t *data)
{
    memcpy(rb->data + rb->head, data, rb->data_size);
    inc_head(rb);
}

uint8_t *ring_buffer_dequeue(RingBuffer_t *rb)
{
    uint32_t tail = rb->tail;
    if (rb->head != tail)
    {
        uint8_t *data = rb->data + rb->tail;
        inc_tail(rb);

        return data;
    }
    else
    {
        return NULL;
    }
}

