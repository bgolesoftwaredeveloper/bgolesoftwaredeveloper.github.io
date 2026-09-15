/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of the jump search algorithm.
 * @details     :   This program initializes a sorted integer array and 
 * utilizes the jump_search function to locate a target 
 * value, printing the resulting index to the console.
 * Compile:
 * clang ./src/darwin/searching/jump-search/main.c ./src/darwin/searching/jump-search/jump_search.c -I./include -o ./bin/jump_search_test
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#include <stdio.h>

// Include header files.
#include "darwin/searching/jump_search.h"

int main(void)
{   
    // Define a sorted array to search from.
    int array[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610};

    // Calculate the array size.
    size_t size = sizeof(array) / sizeof(array[0]);

    // The number to search for.
    int target = 55;

    // Attemp to look for a target.
    int index = jump_search(array, size, target);

    // Was the target found?
    if (index != -1)
    {
        printf("%s %i%c", "Item found at index:", index, '\n');
    }
    else
    {
        printf("%s %i%c", "Item found at index:", index, '\n');
    }

    return 0;
}