/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of the Heap Sort algorithm.
 * @details     :   This program initializes an unsorted integer array, 
 * displays its initial state, sorts it using the 
 * Heap Sort algorithm (an in-place, non-stable sort), 
 * and prints the final sorted result.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/sorting/heap-sort/main.c ./src/darwin/sorting/heap-sort/heap_sort.c -I./include -o ./bin/heap_sort_test
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>

// Include header files.
#include "darwin/sorting/heap_sort.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define an array of unsorted numbers.
    int array[] = {12, 11, 13, 5, 6, 7};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Print the original unsorted array.
    printf("%s", "Original array: [");
    print_array(array, size);
    printf("%s", "]\n");

    /**
     * Perform the Heap Sort.
     * The algorithm builds a max-heap from the input data, then
     * repeatedly swaps the root (maximum element) with the last 
     * element and restores the heap property until sorted.
     */
    heap_sort(array, size);

    // Show the final sorted array result.
    printf("%s", "Sorted array: [");
    print_array(array, size);
    printf("%s", "]\n");

    return 0;
}

/**
 * @brief       :   Prints the array elements to the standard output.
 * @details     :   Iterates through the array and formats the output with 
 *                  commas for readability.
 * @param       :   array:    Pointer to the integer array.
 * @param       :   size:     The number of items in the array to print.
 * @return      :   void
 */
void print_array(int* array, size_t size)
{
    for (size_t index = 0; index < size; index++)
    {
        (index < size - 1) ? printf("%i, ", array[index]) : printf("%i", array[index]);
    }
}