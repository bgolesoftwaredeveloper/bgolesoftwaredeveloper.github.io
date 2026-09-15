/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Execution entry point for testing the Skip List data structure.
 * @details     :   Validates node construction, multi-level random lane generation, 
 * element insertion, searching, structural rendering, and memory cleanup.
 * Compile:
 * clang ./src/darwin/list/skip-list/main.c ./src/darwin/list/skip-list/skip_list.c -I./include -o ./bin/skip_list_test
 * @version     :   1.0
 * @date        :   2026-05-25
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Include header files.
#include "darwin/list/skip_list.h"

int main(void)
{
    // Initialize the random number generator.
    srand((unsigned int)time(NULL));

    // Initialize the skip list.
    SkipList* list = initialize();

    if (list == NULL)
    {
        return 1;
    }

    // Insert a few nodes to work with.
    insert(list, 5, 500);
    insert(list, 10, 1000);
    insert(list, 15, 1500);
    insert(list, 20, 2000);

    // Show the list after insertion.
    display(list);

    int key = 15;
    int value = 0;

    // Search for a key to get a value.
    if (search(list, key, &value))
    {
        printf("%s %i %s %i%c", "Successfully found key:", key, "containing value:", value, '\n');
    }
    else
    {
        printf("%s", "Not found.\n");
    }

    // Clean up.
    clean(list);

    // Avoid dangling pointers.
    list = NULL;

    return 0;
}