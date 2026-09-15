/**
 * @file        :   priority_queue.h
 * @author      :   Braiden Gole
 * @brief       :   Interface for a dynamic Min-Heap Priority Queue.
 * @details     :   Provides structures and function prototypes for an automatically
 * resizing binary min-heap priority queue that tracks generic elements via void*.
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct Node
 * @brief  Represents a single element within the priority queue.
 */
typedef struct
{
    int priority;
    void* data;
} Node;

/**
 * @struct Queue
 * @brief  Represents the priority queue container state.
 */
typedef struct
{
    size_t size;
    size_t capacity;
    Node* nodes;
} Queue;

// Function prototypes.
Queue* initialize(size_t capacity);
bool enqueue(Queue* queue, int priority, void* data);
bool dequeue(Queue* queue, int* priority, void** data);
int peek(Queue* queue);
void display(const Queue* queue);
void clear(Queue* queue);

#endif