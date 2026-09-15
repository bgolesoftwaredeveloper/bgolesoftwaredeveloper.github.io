/**
 * @file        :   merge_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Recursive Merge Sort algorithm.
 * @details     :   A high-performance sorting utility using a divide-and-conquer 
 * strategy. This implementation utilizes size_t for all indexing
 * to ensure compatibility with large datasets and 64-bit architectures,
 * maintaining O(n log n) time complexity.
 * 
 * Compile:
 * clang -Wall -Wextra -Wpedantic -g ./src/darwin/sorting/merge-sort/main.c ./src/darwin/sorting/merge-sort/merge_sort.c -I./include -o ./bin/merge_sort_test
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>

// Include header files.
#include "darwin/sorting/merge_sort.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define an unsorted dataset to sort.
    int array[] = {99, 45, 12, 8, 32, 67, 1};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Print the original array.
    printf("%s", "Original: [");
    print_array(array, size);
    printf("%s", "]\n");

    // Sort the array.
    merge_sort(array, 0, size - 1);

    // Display sorted arrray.
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