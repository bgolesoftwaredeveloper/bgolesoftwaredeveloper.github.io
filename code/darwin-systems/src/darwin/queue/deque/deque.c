/**
 * @file        :   deque.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a dynamic circular Double-Ended Queue (Deque).
 * @details     :   Provides $O(1)$ front/rear insertions and deletions using a 
 * dynamically resizing circular array structure. Modulo arithmetic 
 * safely wraps unsigned size_t indicators around buffer boundaries.
 * @version     :   1.0
 * @date        :   2026-05-20
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/queue/deque.h"

/**
 * @brief Checks if the deque has zero elements.
 * @param queue Pointer to the target Deque structure.
 * @return true if empty or NULL, false otherwise.
 */
static inline bool is_empty(const Deque* queue)
{
    return queue == NULL || queue->size == 0;
}

/**
 * @brief Checks if the deque has hit its maximum available element capacity.
 * @param queue Pointer to the target Deque structure.
 * @return true if full, false if space is available or NULL.
 */
static inline bool is_full(const Deque* queue)
{
    return queue != NULL && queue->size == queue->capacity;
}

/**
 * @brief Doubles the underlying buffer capacity when the array fills up.
 * @param queue Pointer to the target Deque structure.
 * @return true on successful memory reallocation, false on failure.
 */
static inline bool resize(Deque* queue)
{
    if (queue == NULL)
    {
        return false;
    }

    size_t old_capacity = queue->capacity;
    size_t new_capacity = old_capacity * 2;

    // Allocate a brand new buffer with double the storage capacity.
    int* data = (int*)malloc(sizeof(int) * new_capacity);

    if (data == NULL)
    {
        return false;
    }

    // Unroll the circular elements into sequential indexes in the new buffer.
    for (size_t index = 0; index < queue->size; index++)
    {
        data[index] = queue->data[(queue->front + index) % old_capacity];
    }

    // Safely discard old underlying array structure.
    free(queue->data);

    // Reset layput mappings: Front becomes 0, rear aligns with old element capacity.
    queue->front = 0;
    queue->rear = queue->size - 1;
    queue->data = data;
    queue->capacity = new_capacity;

    return true;
}

/**
 * @brief Allocates and creates a fresh deque instance.
 * @param capacity Initial underlying storage size limits.
 * @return Pointer to new Deque struct instance, or NULL on error.
 */
Deque* initialize(size_t capacity)
{
    if (capacity == 0)
    {
        return NULL;
    }

    // Allocate outer structure.
    Deque* queue = (Deque*)calloc(1, sizeof(Deque));

    if (queue == NULL)
    {
        return NULL;
    }

    // Allocate the inner dynamic array.
    queue->data = (int*)malloc(sizeof(int) * capacity);

    if (queue->data == NULL)
    {
        free(queue);

        return NULL;
    }

    // set structure state variables.
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    queue->capacity = capacity;

    return queue;
}

/**
 * @brief Pushes an item directly onto the head of the deque.
 * @param queue Pointer to the deque instance.
 * @param value Integer item to store.
 * @return true on successful injection, false on error.
 */
bool insert_front(Deque* queue, int value)
{
    if (queue == NULL)
    {
        return false;
    }

    // If current capacity limits are crossed, double total buffer scope.
    if (is_full(queue))
    {
        if (!resize(queue))
        {
            return false;
        }
    }

    if (is_empty(queue))
    {
        // First value sets identical indices points.
        queue->front = 0;
        queue->rear = 0;
    }
    else
    {
        // Decremented front index circularly. Adding capacity prevents unsigned underflow.
        queue->front = (queue->front - 1 + queue->capacity) % queue->capacity;
    }

    // Write data value down and grow overall tracker size.
    queue->data[queue->front] = value;
    queue->size++;

    return true;
}

/**
 * @brief Pushes an item directly onto the tail of the deque.
 * @param queue Pointer to the deque instance.
 * @param value Integer item to store.
 * @return true on successful injection, false on error.
 */
bool insert_rear(Deque* queue, int value)
{
    if (queue == NULL)
    {
        return false;
    }

    // Check capacity thresholds before pushing new content down.
    if (is_full(queue))
    {
        if (!resize(queue))
        {
            return false;
        }
    }

    if (is_empty(queue))
    {
        // Initial element setup.
        queue->front = 0;
        queue->rear = 0;
    }
    else
    {
        // Increment rear tracker step circularly over layout boundary.
        queue->rear = (queue->rear + 1) % queue->capacity;
    }

    // Place contents into slot and increases counter size.
    queue->data[queue->rear] = value;
    queue->size++;

    return true;
}

/**
 * @brief Pops and extracts an item out from the head of the deque.
 * @param queue Pointer to the deque instance.
 * @param value Target address where the popped element will be stored.
 * @return true on successful removal, false if the queue is empty.
 */
bool delete_front(Deque* queue, int* value)
{
    if (is_empty(queue))
    {
        return false;
    }

    // Safely output front data payload back out via pass-by-pointer.
    if (value != NULL)
    {
        *value = queue->data[queue->front];
    }

    if (queue->front == queue->rear)
    {
        // If it was the last element, reset trackers back cleanly.
        queue->front = 0;
        queue->rear = 0;
    }
    else
    {
        // Step front target pointer up circularly.
        queue->front = (queue->front + 1) % queue->capacity;
    }

    queue->size--;

    return true;
}

/**
 * @brief Pops and extracts an item out from the tail of the deque.
 * @param queue Pointer to the deque instance.
 * @param value Target address where the popped element will be stored.
 * @return true on successful removal, false if the queue is empty.
 */
bool delete_rear(Deque* queue, int* value)
{
    if (is_empty(queue))
    {
        return false;
    }

    // Safely capture target data from current tail index location.
    if (value != NULL)
    {
        *value = queue->data[queue->rear];
    }

    if (queue->front == queue->rear)
    {
        // Structure is now empty; restore indexes to 0.
        queue->front = 0;
        queue->rear = 0;
    }
    else
    {
        // Safely slide rear backwards using modular arithmetic.
        queue->rear = (queue->rear - 1 + queue->capacity) % queue->capacity;
    }

    queue->size--;

    return true;
}

/**
 * @brief Traverses and prints the active contents of the deque.
 * @param queue Const read-only pointer to the target Deque.
 */
void display(const Deque* queue)
{
    if (is_empty(queue))
    {
        return;
    }

    // Walk over total current footprint linearly, offsetting by front index position.
    for (size_t index = 0; index < queue->size; index++)
    {
        printf("%i ", queue->data[(queue->front + index) % queue->capacity]);
    }

    printf("%c", '\n');
}

/**
 * @brief Destroys and cleans up internal and external allocated memories.
 * @param queue Pointer to the target deque instance.
 */
void clean(Deque* queue)
{
    if (queue == NULL)
    {
        return;
    }

    // Free child properties first before sweeping parent metadata container out.
    free(queue->data);
    free(queue);
}
