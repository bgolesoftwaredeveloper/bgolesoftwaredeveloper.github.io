/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of a dynamic Binary Min-Heap Priority Queue.
 * @details     :      This program initializes a priority queue, demonstrates the
 * use of "sticky" error handling with bitwise AND, and processes 
 * tasks based on their priority values rather than arrival order.
 * Compile:
 * clang ./src/darwin/queue/priority-queue/main.c ./src/darwin/queue/priority-queue/priority_queue.c -I./include -o ./bin/priority_queue_test
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#include <stdio.h>

// Include header files.
#include "darwin/queue/priority_queue.h"

int main(void)
{
    size_t capacity = 10;

    // Initialize the queue with an initial capacity.
    Queue* queue = initialize(capacity);

    if (queue == NULL)
    {
        return 1;
    }

    printf("%s", "-- Enqueuing Tasks --\n");

    /**
     * We initialize 'flag' to true (1). By using the bitwise AND assignment (&=_
     * if any single enqueue returns false (0), the flag becomes 0 and "sticks" there.
     * 
     * Even if later enqueues succeed (returning 1), 0 & 1 is still 0.
     * 
     * This allows us to check once at the end if the entire batch was successful.
     */
    bool flag = true;

    flag &= enqueue(queue, 30, "Low Priority Task");
    flag &= enqueue(queue, 5, "Urgent System Fix");
    flag &= enqueue(queue, 15, "Medium Priority Task");
    flag &= enqueue(queue, 1, "Critical Security Patch");
    flag &= enqueue(queue, 50, "Background Log Cleanup");

    // If the flag is false, at least one operation failed.
    if (!flag)
    {
        fprintf(stderr, "Enqueue failed.\n");

        return 1;
    }

    // Visualize the internal heap array structure.
    display(queue);

    // Look at the root of the heap (the minimum priority value) without removing it.
    int top = peek(queue);

    printf("%s %i%c", "Peek:", top, '\n');

    int priority = 0;
    char* task_name = NULL;

    // dequeue() returns true as long as there are items in the queue.
    while (dequeue(queue, &priority, (void**)&task_name))
    {
        printf("%s [%s] with priority score: %i%c", "Executing:", task_name, priority, '\n');
    }

    // Clean up allocated memory.
    clear(queue);

    return 0;
}