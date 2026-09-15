/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   This is an exponential search.
 * @details     :   Compile: clang ./src/darwin/searching/exponential-search/main.c ./src/darwin/searching/exponential-search/exponential_search.c -I./include -o ./bin/exponential_search_test
 * @version     :   1.0
 * @date        :   2026-04-16
 */
#include <stdio.h>

// Include header files.
#include "darwin/searching/exponential_search.h"

int main(void)
{
    // Define a set of sorted points to search from.
    int array[] = {2, 3, 4, 10, 40, 44, 55, 70, 80, 100};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    int key = 44;

    // Run exponential search on the key.
    int result = exponential_search(array, size, key);

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