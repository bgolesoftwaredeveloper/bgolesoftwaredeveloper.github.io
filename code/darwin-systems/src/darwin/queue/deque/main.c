/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Driver program to test a dynamic Double-Ended Queue (Deque) implementation.
 * @details     :      This file demonstrates the basic functionality of a circular-array based
 * deque. It tests front and rear insertions, handles dynamic resizing validation 
 * via boolean flag checks, displays the elements, and performs front and rear 
 * deletions before cleaning up allocated memory.
 * Compile:
 * clang ./src/darwin/queue/deque/main.c ./src/darwin/queue/deque/deque.c -I./include -o ./bin/deque_test
 * @version     :   1.0
 * @date        :   2026-05-20
 */
#include <stdio.h>

// Include header files/
#include "darwin/queue/deque.h"

int main(void)
{
    // Initialize the deque with an initial capacity of 4 elements.
    Deque* queue = initialize(4);

    if (queue == NULL)
    {
        return 1;
    }

    bool flag = true;

    // Perform a mix of front and rear insertions.
    flag &= insert_rear(queue, 10);
    flag &= insert_front(queue, 20);
    flag &= insert_front(queue, 5);
    flag &= insert_front(queue, 1);

    // Check if all insertions completed successfully
    if (!flag)
    {
        printf("%s", "All items have NOT been inserted.\n");

        return 1;
    }

    // Print the contents to the console.
    display(queue);

    int popped_value = 0;

    // Delete front.
    if (delete_front(queue, &popped_value))
    {
        printf("%s %i%c", "Delete from front:", popped_value, '\n');
    }

    // Delete rear.
    if (delete_rear(queue, &popped_value))
    {
        printf("%s %i%c", "Delete from rear:", popped_value, '\n');
    }

    // Display the final result.
    display(queue);

    // Clean up.
    clean(queue);

    return 0;
}