/**
 * @file        :   exponential_search.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the exponential search algorithm.
 * @details     :   Provides a search mechanism for sorted arrays by doubling indices 
 * to find a range, followed by a binary search.
 * @version     :   1.0
 * @date        :   2026-04-16
 */
#include <stdio.h>

/**
 * @brief       :   Helper function to perform binary search within a specific range.
 * @param array :   Pointer to the sorted integer array.
 * @param left  :   The starting index of the range.
 * @param right :   The ending index of the range.
 * @param key   :   The integer value to search for.
 * @return      :   The index of the key if found; otherwise -1.
 */
static inline int binary_search(const int* array, size_t left, size_t right, int key)
{
    // Continue searching while the range is valid.
    while (left <= right)
    {
        // Calculate the middle point, avoiding potential overflow.
        size_t middle = left + (right - left) / 2;

        // Check if the key is present at the middle.
        if (array[middle] == key)
        {
            return (int)middle;
        }

        // If the key is greater, ignore the left half.
        if (array[middle] < key)
        {
            left = middle + 1;
        }
        // If the key is smaller, ignore the right half.
        else
        {
            // Guard against underflow when using size_t.
            if (middle == 0)
            {
                break;
            }

            right = middle - 1;
        }
    }

    // Item was not found in the array.
    return -1;
}

/**
 * @brief       :   Searches for a key in a sorted array using exponential search.
 * @param array :   Pointer to the sorted integer array.
 * @param size  :   The number of elements in the array.
 * @param key   :   The integer value to search for.
 * @return      :   The index of the key if found; otherwise -1.
 */
int exponential_search(int* array, size_t size, int key)
{
    // Validate parameters.
    if (array == NULL || size == 0)
    {
        return -1;
    }

    // Check if the item immediately.
    if (array[0] == key)
    {
        return 0;
    }

    // Initializing the index for doubling.
    size_t index = 1;
    
    // Find the range where the key may reside by repeatedly doubling the index.
    while (index < size && array[index] <= key)
    {
        index = index * 2;
    }

    // Define the lower bound from the previous doubling step.
    size_t low = index / 2;

    // Ensure the upper bound does not exceed the array size.
    size_t high = (index < size) ? index : size - 1;

    // Perform a binary search within the identified narrow range.
    return binary_search(array, low, high, key);
}