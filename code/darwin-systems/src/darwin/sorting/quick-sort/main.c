/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of an iterative Quick Sort algorithm.
 * @details     :   Compile:
 *                  Compile:
 *                  clang -Wall -Wextra -Wpedantic -g ./src/darwin/sorting/quick-sort/main.c ./src/darwin/sorting/quick-sort/quick_sort.c -I./include -o ./bin/quick_sort_test
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <stdio.h>

// Include header files.
#include "darwin/sorting/quick_sort.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define an unsorted array to work with.
    int array[] = {10, 7, 8, 9, 1, 5};

    // Calculate the arrays size.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Print the original array.
    printf("%s", "Original array: [");
    print_array(array, size);
    printf("%s", "]\n");

    // Sort the array.
    quick_sort(array, 0, size - 1);

    // Display the sorted array.
    printf("%s", "Sorted array: [");
    print_array(array, size);
    printf("%s", "]\n");
    
    return 0;
}

/**
 * @brief       :   Prints the array before and after sorting.
 * @details     :   A helper function to display information to the conosle.
 * @param       :   array:    Pointer to the integer array.
 * @param       :   size:     The number of items in the array.
 */
void print_array(int* array, size_t size)
{
    for (size_t index = 0; index < size; index++)
    {
        (index < size - 1) ? printf("%i, ", array[index]) : printf("%i", array[index]);
    }
}