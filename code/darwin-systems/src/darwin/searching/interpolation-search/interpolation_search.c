/**
 * @file        :   interpolation_search.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the interpolation search algorithm.
 * @details     :   Interpolation search is an improvement over binary search for 
 * instances where the values in a sorted array are uniformly 
 * distributed. It parallels how humans search through a phone book,
 * estimating the position of the target value based on the 
 * values at the bounds of the search space.
 *
 * Complexity:
 * - Worst-case time: $O(n)$ (For highly non-uniform data)
 * - Best-case time:  $O(1)$
 * - Average-case:    $O(\log(\log n))$
 * - Space:           $O(1)$ (Iterative implementation)
 * - Stability:       N/A (Search algorithm)
 * @version     :   1.1
 * @date        :   2026-04-17
 */
#include <stdio.h>

/**
 * @brief       :   Searches for a key within a sorted integer array using Interpolation Search.
 * @details     :   The algorithm calculates a probe position using the formula:
 *                  pos = low + [(key - arr[low]) * (high - low) / (arr[high] - arr[low])].
 *                  It includes defensive checks for NULL pointers, empty arrays, and 
 *                  potential division by zero.
 * @param       :   array:      Pointer to the first element of the sorted integer array.
 * @param       :   size:       The total number of elements in the array.
 * @param       :   key:        The integer value to be located.
 * @param       :   out_index:  Pointer to store the resulting index if the key is found.
 * @return      :   Returns 0 on success, -1 for invalid input, and -2 if the key is not found.
 */
int interpolation_search(const int* array, size_t size, int key, size_t* out_index)
{
    if (array == NULL)
    {
        return -1;
    }

    if (size == 0)
    {
        return -2;
    }

    size_t boundary_low = 0;
    size_t boundary_high = (size - 1);

    // Ensure the key is within the range of the current boundary to prevent out-of-bounds probing.
    while (boundary_low <= boundary_high && key >= array[boundary_low] && key <= array[boundary_high])
    {
        // Guard against division by zero.
        if (array[boundary_high] == array[boundary_low])
        {
            if (array[boundary_low] == key)
            {
                if (out_index != NULL)
                {
                    *out_index = boundary_low;
                }

                return 0;
            }

            break;
        }

        // Use floating-point math for the range fraction to avoid integer overflow and maintain precision.
        double range_fraction = (double)(key - array[boundary_low]) /
            (array[boundary_high] - array[boundary_low]);

        // Calculate the probe position (projected index).
        size_t projected_index = boundary_low + (size_t)(range_fraction * (boundary_high - boundary_low));

        // Safety check to ensure the projection stayed within the current search boundaries.
        if (projected_index < boundary_low || projected_index > boundary_high)
        {
            break;
        }
        
        // Check if the key is at the projected index.
        if (array[projected_index] == key)
        {
            if (out_index != NULL)
            {
                *out_index = projected_index;
            }

            return 0;
        }

        // Adjust boundaries on the value at the projected index.
        if (array[projected_index] < key)
        {
            boundary_low = projected_index + 1;
        }
        else
        {
            // Prevent underflow if projected index is 0.
            if (projected_index == 0)
            {
                break;
            }

            boundary_high = projected_index - 1;
        }
    }

    // Key not found within the array or search boundaries.
    return -2;
}