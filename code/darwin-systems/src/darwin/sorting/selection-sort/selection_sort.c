/**
 * @file        :   selection_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Selection Sort algorithm.
 * @details     :   This script provides a comparison-based sorting algorithm. 
 * It divides the input list into two parts: a sorted sublist 
 * of items built up from left to right and a sublist of the 
 * remaining unsorted items. It repeatedly finds the minimum 
 * element from the unsorted part and puts it at the beginning.
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>

/**
 * @brief       :   Swaps the values of two integers.
 * @details     :   A helper utility that exchanges the values of two memory 
 *                  locations using a temporary variable.
 * @param       :   a:  Pointer to the first integer.
 * @param       :   b:  Pointer to the second integer.
 * @return      :   void
 */
static inline void swap(int* current, int* value)
{
    int temp = *current;
    *current = *value;
    *value = temp;
}

/**
 * @brief       :   Sorts an array of integers using the Selection Sort algorithm.
 * @details     :   Maintains two subarrays in a given array: one which is already 
 *                  sorted and another which is unsorted. In every iteration, the 
 *                  minimum element from the unsorted subarray is picked and 
 *                  moved to the sorted subarray. Complexity is O(n^2).
 * @param       :   arr:  The target integer array to be sorted.
 * @param       :   len:  The number of elements in the array.
 * @return      :   void
 */
void selection_sort(int* array, size_t size)
{
    if (array == NULL || size < 2)
    {
        return;
    }

    // Position tracks the current index we are trying to fill with the correct minimum.
    for (size_t position = 0; position < size - 1; position++)
    {
        // Assume the current position holds the minimum value initially.
        size_t minimum = position;

        // Current scans the unsorterd portion (everything to the right of 'position').
        for (size_t current = position + 1; current < size; current++)
        {
            // If we find an item smaller than our current minimum...
            if (array[current] < array[minimum])
            {
                minimum = current;
            }
        }

        // If 'minimum' changed, it means we found a smaller value further down the array.
        if (minimum != position)
        {
            swap(&array[minimum], &array[position]);
        }
    }
}