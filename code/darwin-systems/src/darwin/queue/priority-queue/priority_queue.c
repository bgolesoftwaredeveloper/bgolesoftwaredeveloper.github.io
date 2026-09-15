/**
 * @file        :   priority_queue.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a dynamic Binary Min-Heap Priority Queue.
 * @details     :   This file contains the logic for managing a min-heap where
 * the smallest priority value is always at the root. It handles
 * dynamic memory management for the underlying node array.
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/queue/priority_queue.h"

/**
 * @brief Swaps two nodes in the internal array.
 */
static inline void swap(Node* source, Node* destination)
{
    Node temp = *source;
    *source = *destination;
    *destination = temp;
}

/**
 * @brief Restores min-heap property by moving a node up the tree.
 * @details Compares the node at 'index' with its parent. If smaller, it swaps 
 * and continues until the root or a smaller parent is reached.
 */
static inline void heapify_up(Queue* queue, size_t index)
{
    if (queue == NULL)
    {
        return;
    }

    // Traverse upwards from the current index to the root.
    while (index > 0)
    {
        size_t parent = (index - 1) / 2;

        // If current node priority is less than parent, it violates min-heap property.
        if (queue->nodes[index].priority < queue->nodes[parent].priority)
        {
            swap(&queue->nodes[index], &queue->nodes[parent]);

            // Move index up to the parent's position.
            index = parent;
        }
        else
        {
            // Property is satisfied; exit early.
            break;
        }
    }
}

/**
 * @brief Moves a node down the tree to maintain min-heap properties.
 */
static inline void heapify_down(Queue* queue, size_t index)
{
    size_t smallest = index;
    size_t left = 2 * index + 1;
    size_t right = 2 * index + 2;

    // Determine if the left child has higher priority (lower value).
    if (left < queue->size && queue->nodes[left].priority < queue->nodes[smallest].priority)
    {
        smallest = left;
    }

    // Determine if the right child has higher priprity (lower value).
    if (right < queue->size && queue->nodes[right].priority < queue->nodes[smallest].priority)
    {
        smallest = right;
    }

    // If a child is smaller than the parent, swap and recurse.
    if (smallest != index)
    {
        swap(&queue->nodes[index], &queue->nodes[smallest]);
        heapify_down(queue, smallest);
    }
}

/**
 * @brief Expands or shrinks the internal node buffer.
 * @return true if successful, false if memory allocation failed.
 */
static inline bool resize(Queue* queue, size_t new_capacity)
{
    // Guard against invalid resize parameters.
    if (queue == NULL || new_capacity == 0 || new_capacity < queue->size)
    {
        return false;
    }

    // Skip allocation if current capacity matches request.
    if (new_capacity == queue->capacity)
    {
        return true;
    }

    // Attempt to reallocate the memory block.
    Node* temp = (Node*)realloc(queue->nodes, sizeof(Node) * new_capacity);

    if (temp == NULL)
    {
        return false;
    }

    // Update management structure.
    queue->nodes = temp;
    queue->capacity = new_capacity;

    return true;
}

/**
 * @brief Allocates and initializes a new Priority Queue.
 * @param capacity Initial number of elements the queue can hold.
 * @return Pointer to the allocated Queue, or NULL on failure.
 */
Queue* initialize(size_t capacity)
{
    // Prevent initialzation with zero capacity.
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

    // Allocate the management structure.
    queue->nodes = (Node*)malloc(sizeof(Node) * capacity);

    // Clean up if the array allocation fails.
    if (queue->nodes == NULL)
    {
        free(queue);

        return NULL;
    }

    // Initialize state.
    queue->size = 0;
    queue->capacity = capacity;

    return queue;
}

/**
 * @brief Adds an item to the queue and restores heap order.
 * @param queue Pointer to the Queue.
 * @param priority Integer priority level (lower is higher priority).
 * @param data Pointer to generic user payload.
 * @return true if successful, false if memory allocation failed.
 */
bool enqueue(Queue* queue, int priority, void* data)
{
    // Guard against invalid inputs.
    if (queue == NULL || data == NULL)
    {
        return false;
    }

    // If the buffer is full, attempt to double the capacity.
    if (queue->size == queue->capacity)
    {
        size_t new_capacity = (queue->capacity == 0) ? 4 : queue->capacity * 2;

        if (!resize(queue, new_capacity))
        {
            return false;
        }
    }

    // Append new node to the end and bubble it up to its correct position.
    queue->nodes[queue->size].priority = priority;
    queue->nodes[queue->size].data = data;

    heapify_up(queue, queue->size);

    queue->size++;

    return true;
}

/**
 * @brief Removes and returns the highest priority element.
 * @param queue Pointer to the Queue.
 * @param priority Optional pointer to store extracted priority.
 * @param data Optional pointer to store extracted generic data.
 * @return true if an element was extracted, false if empty.
 */
bool dequeue(Queue* queue, int* priority, void** data)
{
    // Guard against empty queue or NULL management pointer.
    if (queue == NULL || queue->size == 0)
    {
        return false;
    }

    // Retrieve the root values (highest priority).
    if (priority != NULL)
    {
        *priority = queue->nodes[0].priority;
    }

    if (data != NULL)
    {
        *data = queue->nodes[0].data;
    }

    // Move the last element to the root and bubble it down.
    queue->nodes[0] = queue->nodes[queue->size - 1];
    queue->size--;

    // Rebalance only if elements remain.
    if (queue > 0)
    {
        heapify_down(queue, 0);
    }

    return true;
}

/**
 * @brief Retrieves the priority level of the top element without removal.
 */
int peek(Queue* queue)
{
    return (queue != NULL && queue->size > 0) ? queue->nodes[0].priority : -1;
}

/**
 * @brief Outputs the internal array representation for debugging.
 */
void display(const Queue* queue)
{
    // Guard against invalid queue.
    if (queue == NULL)
    {
        printf("%s", "Queue is NULL.\n");

        return;
    }

    // Guard against empty queue.
    if (queue->size == 0)
    {
        printf("%s", "Queue is empty.\n");

        return;
    }

    // Print metadata and iterate through active nodes.
    printf("%s %zu/%zu%s", "Priority Queue:", queue->size, queue->capacity, ": ");

    for (size_t index = 0; index < queue->size; index++)
    {
        printf("%i", queue->nodes[index].priority);

        if (index < queue->size - 1)
        {
            printf("%s", " -> ");
        }
    }

    printf("%c", '\n');
}

/**
 * @brief Releases all memory associated with the queue.
 */
void clear(Queue* queue)
{
    if (queue != NULL)
    {
        free(queue->nodes);
        free(queue);
    }
}