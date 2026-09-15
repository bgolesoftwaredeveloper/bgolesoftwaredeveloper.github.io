/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of the Timsort algorithm.
 * @details     :   This program initializes an unsorted integer array, 
 * displays its initial state, sorts it using Timsort 
 * (a hybrid stable sorting algorithm), and prints the 
 * final sorted result.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/sorting/tim-sort/main.c ./src/darwin/sorting/tim-sort/tim_sort.c -I./include -o ./bin/tim_sort_test
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>

// Include header files.
#include "darwin/sorting/tim_sort.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define a test dataset of unsorted integers.
    int array[] = {45, -2, 11, 0, 64, 25, 12, 22, 11, 8, 30, 15, 99, -10, 4};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Print the original array state.
    printf("Original array: [");
    print_array(array, size);
    printf("]\n");

    /**
     * Perform the Timsort.
     * The algorithm will sort the array by dividing it into small runs,
     * sorting those runs with insertion sort, and then merging them 
     * using the merge sort logic.
     */
    tim_sort(array, size);

    // Show the final sorted array.
    printf("Sorted array: [");
    print_array(array, size);
    printf("]\n");

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