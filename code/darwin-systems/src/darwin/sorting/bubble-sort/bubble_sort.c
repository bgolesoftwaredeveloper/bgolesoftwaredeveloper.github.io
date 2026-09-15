/**
 * @file        :   bubble_sort.c
 * @author      :   Braiden Gole
 * @brief       :   This is a bubble sort.
 * @details     :   Bubble sort is a simple comparison-based sorting algorithm. 
 * This implementation includes an optimized early-exit flag 
 * if the array becomes sorted before all passes are complete.
 * * Complexity:
 * - Worst-case time: O(n^2)
 * - Best-case time:  O(n) (with flag)
 * - Average-case:    O(n^2)
 * - Space:           O(1) (In-place)
 * - Stability:       Stable
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief       :   Swaps two integer values in memory.
 * @details     :   A static helper function used to exchange items during a pass.
 * @param       :   current:    Pointer to the first integer.
 * @param       :   leading:    Pointer to the second integer.
 */
static inline void swap(int* current, int* leading)
{
    int temp = *current;
    *current = *leading;
    *leading = temp;
}

/**
 * @brief       :   Sorts an array of integers using the bubble sort algorithm.
 * @details     :   Iteratively steps through the list, compares adjacent items and swaps them
 *                  if they are in the wrong order.
 * @param       :   array:  Pointer to the integer array to be sorted.
 * @param       :   size:   The number of items in the array.
 * @note        :   If the array is NULL or size less than 2 the function return s immediately.
 */
void bubble_sort(int* array, size_t size)
{
    if (array == NULL || size <= 1)
    {
        return;
    }

    // Iterate enough passes to sort the array.
    for (size_t pass = 0; pass < size - 1; pass++)
    {
        // Reset flag on every pass.
        bool swapped = false;

        // Compare the adjacent item and swap if the current item is larger than the adjacent.
        for (size_t index = 0; index < size - 1 - pass; index++)
        {
            if (array[index] > array[index + 1])
            {
                swap(&array[index], &array[index + 1]);

                swapped = true;
            }
        }

        // Detect if a swap occurred.
        if (!swapped)
        {
            break;
        }
    }
}