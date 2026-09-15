/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of Generic Stack memory management.
 * @details     :   This program initializes a stack, performs operations,
 * and ensures all allocated memory is released using 
 * the clear function.
 * 
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/stack/generic-stack/main.c ./src/darwin/stack/generic-stack/generic_stack.c -I./include -o ./bin/generic_stack_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>

// Include header files.
#include "darwin/stack/generic_stack.h"

int main(void)
{
    // Define a stack to work with.
    GenericStack stack;

    // Define data to insert into the stack.
    int data[] = {10, 20, 30, 40, 50};

    // Calculate the size of the data.
    size_t size = sizeof(data) / sizeof(data[0]);

    // Initialize stack.
    if (!initialize(&stack, sizeof(int)))
    {
        fprintf(stderr, "Failed to initialize stack.\n");

        return 1;
    }

    printf("%s", "Pushing items onto the stack...\n");

    // Push items.
    for (size_t index = 0; index < size; index++)
    {
        push(&stack, &data[index]);
    }

    // Retrieve the stack size.
    printf("%s %zu%c", "Current stack size:", retrieve_stack_size(&stack), '\n');

    int top;

    // Peek of stack.
    if (peek(&stack, &top))
    {
        printf("%s %i%c", "The top item is:", top, '\n');
    }

    printf("%s", "Clearing stack and releasing memory...\n");

    // Clear heap.
    clear(&stack);

    // Check if heap is cleared.
    if (is_empty(&stack))
    {
        printf("%s %zu%c", "Stack successfully cleared. Current size:", retrieve_stack_size(&stack), '\n');
    }

    return 0;
}