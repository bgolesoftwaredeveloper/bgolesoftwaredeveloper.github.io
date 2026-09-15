/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   This is a binary search.
 * @details     :   Compile:
 *                  clang ./src/darwin/searching/binary-search/main.c ./src/darwin/searching/binary-search/binary_search.c -I./include -o ./bin/binary_search_test
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <stdio.h>

// Include header files.
#include "darwin/searching/binary_search.h"

int main(void)
{
    // Define a sorted array to search from.
    int array[] = {10, 22, 35, 40, 50, 80, 99};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Run binary search on the target.
    int result = binary_search(array, size, 80);

    // Was the item found?
    if (result != -1)
    {
        printf("%s %i%c", "Item found at index:", result, '\n');
    }
    else
    {
        printf("%s", "Item not found.\n");
    }

    return 0;
}