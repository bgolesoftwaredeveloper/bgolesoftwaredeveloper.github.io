/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Entry point for demonstrating the Counting Sort algorithm.
 * @details     :   This program initializes an unsorted integer array, 
 * executes the counting_sort function, and prints the 
 * results to the console to verify sorting integrity.
 * 
 * Compile:
 * clang -Wall -Wextra -Wpedantic -g ./src/darwin/sorting/insertion-sort/main.c ./src/darwin/sorting/insertion-sort/insertion_sort.c -I./include -o ./bin/insertion_sort_test
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#include <stdio.h>

// Include header files.
#include "darwin/sorting/insertion_sort.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define an array of unsorted number to sort.
    int array[] = {12, 11, 13, 5, 6};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Print the original array.
    printf("%s", "Original array: [");
    print_array(array, size);
    printf("%s", "]\n");

    // Execute insertion sort on the array.
    insertion_sort(array, size);

    // Show the sorted array.
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