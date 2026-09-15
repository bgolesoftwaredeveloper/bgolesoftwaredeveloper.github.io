/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test harness for the interpolation search algorithm.
 * @details     :   This program demonstrates the efficiency of interpolation search 
 * by searching for values in a linearly increasing array. It 
 * validates the result using the function's return codes and 
 * outputs the found index.
 *
 * Compile:
 * clang ./src/darwin/searching/interpolation-search/main.c ./src/darwin/searching/interpolation-search/interpolation_search.c -I./include -o ./bin/interpolation_search_test
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#include <stdio.h>

// Include header files
#include "darwin/searching/interpolation_search.h"

int main(void)
{
    // Define a sorted dataset to search from.
    int array[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    int key = 70;
    size_t index = 0;

    printf("%s %i%s", "Searching for key:", key, "...\n");

    // Execute the interpolation search.
    int status = interpolation_search(array, size, key, &index);

    // Evaluate the return status codes defined in the algorithm implementation.
    switch (status)
    {
        case 0:
            // Succcess: Value was located with the array.
            printf("%s %i %s %zu%c", "Success key:", key, "found at index:", index, '\n');
            break;
        case -1:
            // Error: The function received a NULL pointer.
            fprintf(stderr, "Error: Invalid input provided (NULL array).");
            break;
        case -2:
            // Not found: The key is either outside the range or not present in the values.
            printf("%s %i %s", "Result: key:", key, "was not found in the array.\n");
            break;
        default:
            // Uknown status code:
            fprintf(stderr, "Error: An unexpected error occurred.\n");
            break;
    }

    printf("%c", '\n');

    int missing_key = 25;

    printf("%s %i%s", "Searching for missing key:", missing_key, "...\n");

    // Search for a missing key to demonstrate behavior.
    if (interpolation_search(array, size, missing_key, &index) == -2)
    {
        printf("%s %i %s", "Correctly identified that", missing_key, "is not in the array.\n");
    }

    return 0;
}