/**
 * @file        :   fifo_queue.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a dynamic circular FIFO queue.
 * @details     :   Provides a high-performance queue using a circular buffer 
 * strategy. Includes automatic geometric resizing to 
 * prevent overflow while maintaining O(1) average time 
 * complexity for enqueue and dequeue operations.
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/queue/fifo_queue.h"

// Function prototypes.
static bool resize(Queue* queue);

/**
 * @brief Checks if the queue contains no elements.
 */
static inline bool is_empty(Queue* queue)
{
    return queue == NULL || queue->size == 0;
}

/**
 * @brief Checks if the queue has reached its current allocation limit.
 */
static inline bool is_full(Queue* queue)
{
    return queue != NULL && queue->size == queue->capacity;
}

/**
 * @brief Allocates and initializes a new FIFO queue.
 * @param capacity Initial number of elements the queue can hold.
 * @return Pointer to the allocated Queue, or NULL on failure.
 */
Queue* prepare(size_t capacity)
{
    // Prevent initialization with zero capacity.
    if (capacity == 0)
    {
        return NULL;
    }

    // Allocate the management structure.
    Queue* queue = (Queue*)malloc(sizeof(Queue));

    if (queue == NULL)
    {
        return NULL;
    }

    // Allocate the internal data buffer.
    queue->capacity = capacity;
    queue->array = (int*)malloc(sizeof(int) * queue->capacity);

    // Clean up if the array allocation fails.
    if (queue->array == NULL)
    {
        free(queue);

        return NULL;
    }

    // Initialize state pointers.
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;

    return queue;
}

/**
 * @brief Adds an integer to the back of the queue.
 * @param queue Pointer to the Queue.
 * @param value Integer value to store.
 * @return true if successful, false if memory allocation failed.
 */
bool enqueue(Queue* queue, int value)
{
    // Ensure we have a valid queue.
    if (queue == NULL)
    {
        return false;
    }

    // If the buffer is full, attempt to grow it.
    if (is_full(queue))
    {
        if (!resize(queue))
        {
            return false;
        }
    }

    // Store value at the current rear and increment rear circularity.
    queue->array[queue->rear] = value;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;

    return true;
}

/**
 * @brief Expands the internal buffer capacity.
 * @return true if expansion was successful, false otherwise.
 */
static bool resize(Queue* queue)
{
    // Ensure we have a valid queue.
    if (queue == NULL)
    {
        return false;
    }

    // Make a reference to the old capacity and perform arithmetic to get new capacity.
    size_t old_capacity = queue->capacity;
    size_t new_capacity = old_capacity * 2;

    // Allocate a larger memory block.
    int* array = (int*)malloc(sizeof(int) * new_capacity);

    if (array == NULL)
    {
        return false;
    }

    // Re-align elements: Copy from 'front' to the end, then wrap around.
    for (size_t index = 0; index < queue->size; index++)
    {
        array[index] = queue->array[(queue->front + index) % old_capacity];
    }

    // Release old memory.
    free(queue->array);

    // Update queue metadata to reflect the new contiguous layout.
    queue->array = array;
    queue->front = 0;
    queue->rear = queue->size;
    queue->capacity = new_capacity;

    return true;
}

/**
 * @brief Removes and returns the element at the front of the queue.
 * @param queue Pointer to the Queue.
 * @param flag Pointer to a boolean to indicate success/failure.
 * @return The dequeued value, or 0 if empty (check flag for validity).
 */
int dequeue(Queue* queue, bool* flag)
{
    // Guard against empty queue or NULL flag pointers.
    if (is_empty(queue))
    {
        if (flag != NULL)
        {
            *flag = false;
        }

        return 0;
    }

    // Retrieve the front value.
    int value = queue->array[queue->front];

    // Increment front circularly and decrement the size.
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    // Set success flag if the pointer is valid.
    if (flag != NULL)
    {
        *flag = true;
    }

    return value;
}

/**
 * @brief Releases all memory associated with the queue.
 * @param queue Pointer to the Queue to destroy.
 */
void destroy(Queue* queue)
{
    if (queue != NULL)
    {
        free(queue->array);
        free(queue);
    }
}