/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Entry point for demonstrating the Counting Sort algorithm.
 * @details     :   This program initializes an unsorted integer array, 
 * executes the counting_sort function, and prints the 
 * results to the console to verify sorting integrity.
 * 
 * Compile:
 * clang -Wall -Wextra -Wpedantic -g ./src/darwin/sorting/counting-sort/main.c ./src/darwin/sorting/counting-sort/counting_sort.c -I./include -o ./bin/counting_sort_test
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>

// Include header files.
#include "darwin/sorting/counting_sort.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define a unsorted dataset to sort.
    int array[] = {12, 4, 7, 12, 1, 0, 9, 4};

    // Calculate the arrays size.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Print the original array.
    printf("%s", "Original: [");
    print_array(array, size);
    printf("%s", "]\n");

    // Sort the array.
    counting_sort(array, size);

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