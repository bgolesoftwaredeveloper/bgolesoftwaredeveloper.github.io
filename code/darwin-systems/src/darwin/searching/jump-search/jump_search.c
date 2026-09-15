/**
 * @file        :   jump_search.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the jump search algorithm.
 * @details     :   Jump search (also known as block search) is an algorithm for 
 * searching a sorted array by skipping a fixed number of elements. 
 * Once an interval containing the target is identified, a linear 
 * search is performed to find the exact index.
 * * Complexity:
 * - Worst-case time: $O(\sqrt{n})$
 * - Best-case time:  $O(1)$
 * - Average-case:    $O(\sqrt{n})$
 * - Space:           $O(1)$
 * - Stability:       N/A (Search algorithm)
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#include <stdio.h>
#include <math.h>

/**
 * @brief       :   Searches for a target value within a sorted integer array using Jump Search.
 * @details     :   The algorithm determines an optimal step size (square root of n) to 
 *                  "jump" through the array, reducing the number of comparisons compared to linear search.
 * @param       :   array:  Pointer to the first element of the sorted integer array.
 * @param       :   size:   The total number of elements in the array.
 * @param       :   target: The integer value to be located.
 * @return      :   The index of the target if found, otherwise returns -1.
 */
int jump_search(const int* array, size_t size, int target)
{
    if (array == NULL || size == 0)
    {
        return -1;
    }

    // Determine the block size to jump.
    size_t step = (size_t)sqrt(size);
    size_t previous = 0;
    size_t current = step;

    // Finding the block where the itme is present.
    // Ensure we don't jump beyond the array size.
    while (array[((current < size) ? current : size) - 1] < target)
    {
        previous = current;
        current += step;

        // If the previous pointer exceeds array size, the item is not present.
        if (previous >= size)
        {
            return -1;
        }
    }

    // Perform a linear search withing the identified block.
    size_t limit = (current < size) ? step : size;
    
    while (array[previous] < target)
    {
        previous++;

        // If we reach the next block or the end of the array, the item is missing.
        if (previous == limit)
        {
            return -1;
        }
    }

    // Check if the item at the current index is the target.
    if (array[previous] == target)
    {
        return (int)previous;
    }

    return -1;
}