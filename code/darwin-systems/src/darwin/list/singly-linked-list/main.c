/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test suite for the Singly Linked List implementation.
 * @details     :   Demonstrates list initialization, tail-insertion, 
 * functional mapping, data updates, and node deletion.
 * Compile:
 * clang ./src/darwin/list/singly-linked-list/main.c ./src/darwin/list/singly-linked-list/linked_list.c -I./include -o ./bin/linked_list_test
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <stdio.h>

// Include header files.
#include "darwin/list/linked_list.h"

/**
 * @brief       :   Squares the value of an integer in place.
 * @param       :   number: Pointer to the integer to be squared.
 * @return      :   void
 */
static inline void square(int* number)
{
    *number *= *number;
}

/**
 * @brief       :   Prints an integer followed by a space.
 * @param       :   number: Pointer to the integer to be logged.
 * @return      :   void
 */
static inline void log(int* number)
{
    printf("%i ", *number);
}

int main(void)
{
    // Initialize the singly linked list.
    List* list = initialize();

    // Push a few items.
    append(list, 3);
    append(list, 2);
    append(list, 3);

    // View the original list.
    printf("%s", "Original: ");
    map(list, log);
    printf("%s", "NULL\n");

    map(list, square);

    // View the squared data.
    printf("%s", "Squared: ");
    map(list, log);
    printf("%s", "NULL\n");

    // We search for 4 *which was 2 squared) and upadate it.
    if (update(list, 4, 10))
    {
        printf("%s", "Update: ");
    }

    // Log after update.
    map(list, log);
    printf("%s", "NULL\n");

    if (erase(list, 9))
    {
        printf("%s", "Erased 9: ");
    }

    // Log after erase.
    map(list, log);
    printf("%s", "NULL\n");

    // Uninitialize the list.
    uninitialize(list);

    return 0;
}