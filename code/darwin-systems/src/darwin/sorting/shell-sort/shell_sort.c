/**
 * @file        :   shell_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Shell Sort algorithm.
 * @details     :   This script provides an efficient, comparison-based sorting 
 * algorithm that generalizes insertion sort. It starts by 
 * sorting pairs of elements far apart from each other, then 
 * progressively reducing the gap between elements to be 
 * compared.
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>

/**
 * @brief       :   Sorts an array of integers using the Shell Sort algorithm.
 * @details     :   Uses the Knuth-like sequence (n/2) for gaps. By starting 
 *                  with large gaps, elements can move large distances toward 
 *                  their final positions faster than a simple insertion sort. 
 *                  Average time complexity depends on the gap sequence, 
 *                  typically around O(n log n) to O(n^1.25).
 * @param       :   array: The target integer array to be sorted.
 * @param       :   size:  The number of elements in the array.
 * @return      :   void
 */
void shell_sort(int* array, size_t size)
{
    if (array == NULL || size < 2)
    {
        return;
    }

    // Start with a large gap and reduce it by half each iteration.
    for (size_t gap = size / 2; gap > 0; gap /= 2)
    {
        // Perform a 'gapped' insertion sort for this gap size.
        for (size_t outer = gap; outer < size; outer++)
        {
            // Save this item to be inserted and make a hold at position outer.
            int temp = array[outer];

            size_t inner;

            // Shift earlier gap-sorted items up unitl the correct location for temp is found.
            for (inner = outer; inner >= gap && array[inner - gap] > temp; inner -= gap)
            {
                array[inner] = array[inner - gap];
            }

            // Put temp (the original array[outer]) in its correct location.
            array[inner] = temp;
        }
    }
}