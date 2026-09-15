/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of an advanced circular FIFO queue demonstration.
 * @details     :   This program showcases the lifecycle of a dynamic circular queue, 
 * including initialization, automatic memory resizing, circular 
 * index wrapping, and safe memory deallocation.
 * Compile:
 * clang ./src/darwin/queue/fifo-queue/main.c ./src/darwin/queue/fifo-queue/fifo_queue.c -I./include -o ./bin/fifo_queue_test
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>

// Include header files.
#include "darwin/queue/fifo_queue.h"

int main(void)
{
    // Initial capacity set low to demonstrate teh dynamic resize.
    size_t capacity = 3;

    // Allocate and initialize the queue structure and its internal buffer.
    Queue* queue = prepare(capacity);

    if (queue == NULL)
    {
        return 1;
    }

    printf("%s %zu%c", "Queue prepared with capacity:", queue->capacity, '\n');

    // Fill the queue to its initial capacity limit.
    enqueue(queue, 10);
    enqueue(queue, 20);
    enqueue(queue, 30);

    printf("%s", "Enqueue 40 (This should trigger a resize)...\n");

    // Adding this 4th element forces the queue to double its capacity and re-align its internal pointers.
    if (enqueue(queue, 40))
    {
        printf("%s %zu%c", "New capacity after resize: ", queue->capacity, '\n');
    }

    bool success = false;

    printf("%c", '\n');

    printf("%s", "Dequeue process:\n");

    // Process the queue until it is empty.
    // This demonstrate the First-In-First-Out (FIFO) nature of the structure.
    while (queue->size > 0)
    {
        int value = dequeue(queue, &success);

        if (success)
        {
            printf("%s %i %s %zu%c", "Dequeued:", value, "Remaining size:", queue->size, '\n');
        }
    }

    printf("%c", '\n');

    // Attempt to dequeue from an empty queue to verify error handling.
    int empty_value = dequeue(queue, &success);

    if (!success)
    {
        printf("%s", "Successfully caught empty queue dequeue attempt.\n");
    }

    // Free the internal array and the queue structure to prevent memory leaks.
    destroy(queue);

    printf("%s", "Queue destroyed.\n");

    return 0;
}