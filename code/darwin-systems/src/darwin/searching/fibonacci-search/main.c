/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test harness for the Fibonacci search algorithm.
 * @details     :   This program demonstrates the usage of Fibonacci search by 
 * locating values within a sorted integer array. It validates 
 * the algorithm's return codes to handle successful searches, 
 * missing keys, and invalid input scenarios.
 *
 * Compile:
 * clang ./src/darwin/searching/fibonacci-search/main.c ./src/darwin/searching/fibonacci-search/fibonacci_search.c -I./include -o ./bin/fibonacci_search_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>

// Includce header files.
#include "darwin/searching/fibonacci_search.h"

int main(void)
{
    // Define a sorted dataset to search from.
    int array[] = {10, 22, 35, 40, 45, 50, 80, 82, 85, 90, 100};

    // Calculate the size of the array.
    size_t size = sizeof(array) / sizeof(array[0]);

    int key = 85;
    size_t index = 0;

    printf("%s %i%s", "Searching for key:", key, "...\n");

    // Execute the Fibonacci search and evaluate the boolean status.
    if (fibonacci_search(array, size, key, &index))
    {
        // Success: Value was located within the array.
        printf("%s %i %s %zu%c", "Success: key:", key, "found at index:", index, '\n');
    }
    else
    {
        // Not found or error: The key is not present or input was invalid.
        printf("%s %i %s", "Result: key:", key, "was not found in the array.\n");
    }

    printf("%c", '\n');

    int missing_key = 42;

    printf("%s %i%s", "Searching for missing key:", missing_key, "...\n");

    // Search for a missing key to demonstrate behavior using simple boolean check.
    if (!fibonacci_search(array, size, missing_key, &index))
    {
        printf("%s %i %s", "Correctly identified that", missing_key, "is not in the array.\n");
    }

    return 0;
}