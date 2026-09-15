/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of the ternary search algorithm.
 * @details     :   Ternary search is a divide-and-conquer algorithm for searching 
 * a sorted array by dividing the search space into three equal 
 * parts using two pivot points. This recursive implementation 
 * efficiently narrows down the target location.
 *
 * Complexity:
 * - Worst-case time: $O(\log_3 n)$
 * - Best-case time:  $O(1)$
 * - Average-case:    $O(\log_3 n)$
 * - Space:           $O(\log_3 n)$ (Due to recursion stack)
 * - Stability:       N/A (Search algorithm)
 * Compile
 * clang ./src/darwin/searching/ternary-search/main.c ./src/darwin/searching/ternary-search/ternary_search.c -I./include -o ./bin/ternary_search_test
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#include <stdio.h>

/**
 * @brief       :   Searches for a key within a sorted integer array using Ternary Search.
 * @details     :   The algorithm calculates two pivots (left and right) to split the 
 *                  array into thirds. It sequentially checks the first, middle, and 
 *                  last thirds to locate the key.
 * @param       :   array:  Pointer to the first element of the sorted integer array.
 * @param       :   low:    The starting index of the search range.
 * @param       :   high:   The ending index of the search range.
 * @param       :   key:    The integer value to be located.
 * @return      :   The index of the key if found, otherwise returns -1.
 */
int ternary_search(const int* array, size_t low, size_t high, int key)
{
    if (array == NULL)
    {
        return -1;
    }

    if (high >= low)
    {
        // Distinguish the two midpoints clearly.
        size_t left_pivot = low + (high - low) / 3;
        size_t right_pivot = high - (high - low) / 3;

        // Check if the key is at either pivot.
        if (array[left_pivot] == key)
        {
            return (int)left_pivot;
        }
        
        if (array[right_pivot] == key)
        {
            return (int)right_pivot;
        }

        // 1. Search the first third.
        if (key < array[left_pivot])
        {
            if (left_pivot == 0)
            {
                return -1;
            }

            return ternary_search(array, low, left_pivot - 1, key);
        }
        // 2. Search the middle third.
        else if (key > array[left_pivot] && key < array[right_pivot])
        {
            return ternary_search(array, left_pivot + 1, right_pivot - 1, key);
        }
        // 3. Search the last third.
        else
        {
            return ternary_search(array, right_pivot + 1, high, key);
        }
    }

    // Key not present in the current search boundaries.
    return -1;
}