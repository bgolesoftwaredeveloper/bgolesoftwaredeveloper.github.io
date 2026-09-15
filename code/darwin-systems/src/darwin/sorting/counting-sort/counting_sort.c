/**
 * @file        :   counting_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Counting Sort algorithm for non-negative integers.
 * @details     :   This script provides a stable counting sort implementation. It uses 
 * auxiliary memory to count occurrences of each value and determine 
 * their final positions. Note that this algorithm is most efficient 
 * when the range of input data (k) is not significantly larger than 
 * the number of elements (n).
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief       :   Sorts an array of integers using the Counting Sort algorithm.
 * @details     :   Identifies the maximum value to create a frequency map, 
 *                  calculates cumulative positions, and reconstructs the 
 *                  array in a stable, ascending order.
 * @param       :   array:  The target integer array to be sorted.
 * @param       :   size:   The number of elements in the array.
 * @return      :   void
 */
void counting_sort(int* array, size_t size)
{
    if (array == NULL || size <= 1)
    {
        return;
    }

    // Find the maximum item.
    int maximum = array[0];

    for (size_t index = 1; index < size; index++)
    {
        if (array[index] > maximum)
        {
            maximum = array[index];
        }
    }

    // Memory allocation.
    int* count = (int*)calloc(maximum + 1, sizeof(int));
    int* output = (int*)malloc(sizeof(int) * size);

    // Memory check.
    if (count == NULL || output == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.");

        free(count);
        free(output);

        return;
    }

    // Store the frequency of each item.
    for (size_t index = 0; index < size; index++)
    {
        count[array[index]]++;
    }

    // Cumulative count for positioning.
    for (int index = 1; index <= maximum; index++)
    {
        count[index] += count[index - 1];
    }

    // Build the output array.
    for (size_t backwards = size; backwards > 0; backwards--)
    {
        size_t index = (backwards - 1);
        output[count[array[index]] - 1] = array[index];
        count[array[index]]--;
    }

    // Copy back to the original.
    for (size_t index = 0; index < size; index++)
    {
        array[index] = output[index];
    }

    free(count);
    free(output);
}