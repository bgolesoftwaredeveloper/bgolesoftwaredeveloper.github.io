/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of the ternary search algorithm.
 * @details     :   Compile: clang ./src/darwin/searching/ternary-search/main.c ./src/darwin/searching/ternary-search/ternary_search.c -I./include -o ./bin/ternary_search_test
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#include <stdio.h>

// Include header files.
#include "darwin/searching/ternary_search.h"

// Function prototypes.
void print_array(int* array, size_t size);

int main(void)
{
    // Define a sorted dataset to search.
    int array[] = {10, 22, 35, 40, 45, 50, 80, 82, 90, 100};

    // Calculate the array size.
    size_t size = sizeof(array) / sizeof(array[0]);

    // Define search parameters.
    int target = 80;

    // Display the array being searched.
    printf("%s", "Search space: [");
    print_array(array, size);
    printf("%s", "]\n");

    // Perform the ternary search.
    // Range is from index 0 to size - 1.
    int result = ternary_search(array, 0, size - 1, target);

    // Display the search result.
    if (result != -1)
    {
        printf("Target %i found at index: %d\n", target, result);
    }
    else
    {
        printf("Target %i not found in the array.\n", target);
    }
    
    return 0;
}

/**
 * @brief       :   Prints the array elements to the console.
 * @details     :   A helper function to visualize the dataset used for the search.
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