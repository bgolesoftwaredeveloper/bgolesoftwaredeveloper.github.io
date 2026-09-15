/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of the Selection Sort algorithm.
 * @details     :   This program initializes an unsorted integer array, 
 * displays its initial state, sorts it using Selection Sort, 
 * and prints the final sorted result.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/sorting/selection-sort/main.c ./src/darwin/sorting/selection-sort/selection_sort.c -I./include -o ./bin/selection_sort_test
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>

// Include header files.
#include "darwin/sorting/selection_sort.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define a test dataset of unsorted integers.
    int array[] = {64, 25, 12, 22, 11};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Print the original array.
    printf("Original array: [");
    print_array(array, size);
    printf("]\n");

    /**
     * Perform the Selection Sort.
     * This processes the array by repeatedly finding the minimum element
     * from the unsorted part and putting it at the beginning.
     */
    selection_sort(array, size);

    // Show the sorted array.
    printf("Sorted array: [");
    print_array(array, size);
    printf("]\n");

    return 0;
}

/**
 * @brief       :   Prints the array before and after sorting.
 * @details     :   A helper function to display information to the console.
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