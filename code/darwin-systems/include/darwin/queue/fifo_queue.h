/**
 * @file        :   fifo_queue.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for a dynamic circular FIFO queue.
 * @details     :   Defines the Queue structure and public API for managing
 * an integer-based circular buffer that supports dynamic
 * resizing and constant-time (O(1)) operations.
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#ifndef FIFO_QUEUE_H
#define FIFO_QUEUE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct Queue
 * @brief  A container for the circular FIFO queue.
 */
typedef struct
{
    int* array;
    size_t front;
    size_t rear;
    size_t size;
    size_t capacity;
} Queue;

// Function prototypes.
Queue* prepare(size_t capacity);
bool enqueue(Queue* queue, int value);
int dequeue(Queue* queue, bool* flag);
void destroy(Queue* queue);

#endif