/**
 * @file        :   deque.h
 * @author      :   Braiden Gole
 * @brief       :   Header interface for a dynamic circular Double-Ended Queue (Deque).
 * @details     :   Defines the core structural layout and external API prototypes
 * for an efficient, dynamic circular-buffer deque.
 * @version     :   1.0
 * @date        :   2026-05-20
 */
#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct Deque
 * @brief  Structure representing a dynamic circular double-ended queue.
 */
typedef struct
{
    int* data;
    size_t front;
    size_t rear;
    size_t size;
    size_t capacity;
} Deque;

// Function prototypes.
Deque* initialize(size_t capacity);
bool insert_front(Deque* queue, int value);
bool insert_rear(Deque* queue, int value);
bool delete_front(Deque* queue, int* value);
bool delete_rear(Deque* queue, int* value);
void display(const Deque* queue);
void clean(Deque* queue);

#endif