/**
 * @file        :   binary_search.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the binary search algorithm.
 * @details     :   Binary search is an efficient divide-and-conquer algorithm 
 * that finds the position of a target value within a sorted array. 
 * It repeatedly halves the search interval until the value is found 
 * or the interval is empty.
 * * Complexity:
 * - Worst-case time: $O(\log n)$
 * - Best-case time:  $O(1)$
 * - Average-case:    $O(\log n)$
 * - Space:           $O(1)$
 * - Stability:       N/A (Search algorithm)
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>

// Include header file.
#include "darwin/searching/binary_search.h"

/**
 * @brief       :   Searches for a target value within a sorted integer array.
 * @details     :   Uses a divide-and-conquer approach to locate the index of a value. 
 *                  The array must be pre-sorted in ascending order for this to function correctly.
 * @param       :   array:  Pointer to the first element of the sorted integer array.
 * @param       :   size:   The total number of elements in the array.
 * @param       :   target: The integer value to be located.
 * @return      :   The index of the target if found, otherwise returns -1.
 */
int binary_search(const int* array, size_t size, int target)
{
    if (array == NULL)
    {
        return -1;
    }

    if (size == 0)
    {
        return -1;
    }

    // Define the initial search boundaries.
    size_t low = 0;
    size_t high = size;

    // Continue searching as long as the search range is valid.
    while (low < high)
    {
        // Calculate the middle index; uses subtraction to prevent integer overflow.
        size_t middle = low + (high - low) / 2;

        // Check if the target is found at the current middle index.
        if (array[middle] == target)
        {
            return (int)middle;
        }

        // If target is larger than middle, ignore the left half.
        if (array[middle] < target)
        {
            low = middle + 1;
        }
        // If target is smaller than middle, ignore the right half.
        else
        {
            high = middle;
        }
    }

    // Return -1 if the target was not found in the array.
    return -1;
}