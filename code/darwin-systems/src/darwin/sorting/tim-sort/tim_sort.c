/**
 * @file        :   tim_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Timsort algorithm.
 * @details     :   Timsort is a hybrid stable sorting algorithm, derived 
 * from merge sort and insertion sort. It finds subsequences 
 * of the data that are already ordered (runs) and uses them 
 * to sort the remainder more efficiently.
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>
#include <stdlib.h>

#define RUN 32

/**
 * @brief       :   Returns the smaller of two size_t values.
 * @param       :   compare: The first value to compare.
 * @param       :   against: The second value to compare.
 * @return      :   The minimum value between compare and against.
 */
static inline size_t minimum(size_t compare, size_t against)
{
    return (compare < against) ? compare : against;
}

/**
 * @brief       :   Sorts a subarray using the Insertion Sort algorithm.
 * @details     :   Used by Timsort to sort small segments (runs) of the 
 *                  array. It is efficient for small datasets and 
 *                  partially sorted data.
 * @param       :   array: The target integer array.
 * @param       :   start: The starting index of the subarray.
 * @param       :   end:   The ending index of the subarray.
 * @return      :   void
 */
static inline void insertion_sort(int* array, size_t start, size_t end)
{
    // Iterate from the second element of the subarray to the end.
    for (size_t index = start + 1; index <= end; index++)
    {
        // Store the current element to be positioned.
        int temp = array[index];

        // Set inner index to the element immediately to the left.
        int inner = (int)index - 1;

        // Shift elements of the sorted part to the right if they are greater than temp.
        while (inner >= (int)start && array[inner] > temp)
        {
            array[inner + 1] = array[inner];
            inner--;
        }

        // Insert temp into its correct sorted position.
        array[inner + 1] = temp;
    }
}

/**
 * @brief       :   Merges two adjacent sorted subarrays into one.
 * @details     :   Allocates temporary memory to hold the two runs and 
 *                  merges them back into the original array in sorted order. 
 *                  This implementation maintains stability.
 * @param       :   array:  The target integer array.
 * @param       :   start:  The start index of the first subarray.
 * @param       :   middle: The end index of the first subarray.
 * @param       :   end:    The end index of the second subarray.
 * @return      :   void
 */
static inline void merge(int* array, size_t start, size_t middle, size_t end)
{
    // Calculate lengths of the two subarrays to be merged.
    size_t left_size = middle - start + 1;
    size_t right_size = end - middle;

    // Create temporary arrays to hold data for merging.
    int* left_part = (int*)malloc(sizeof(int) * left_size);
    int* right_part = (int*)malloc(sizeof(int) * right_size);

    // Safety check for memory allocation failure.
    if (left_part == NULL || right_part == NULL)
    {
        free(left_part);
        free(right_part);

        return;
    }

    // Copy data from the original array into the left temporary array.
    for (size_t index = 0; index < left_size; index++)
    {
        left_part[index] = array[start + index];
    }

    // Copy data from the original array into the right temporary array.
    for (size_t index = 0; index < right_size; index++)
    {
        right_part[index] = array[middle + 1 + index];
    }

    size_t left = 0;
    size_t right = 0;
    size_t merge = start;

    // Compare elements from both parts and merge them back in non-decreasing order.
    while (left < left_size && right < right_size)
    {
        if (left_part[left] <= right_part[right]) 
        {
            array[merge++] = left_part[left++];
        }
        else
        {
            array[merge++] = right_part[right++];
        }
    }

    // If any elements remain in left_part, copy them over.
    while (left < left_size)
    {
        array[merge++] = left_part[left++];
    }

    // If any elements remain in right_part, copy them over.
    while (right < right_size)
    {
        array[merge++] = right_part[right++];
    }

    // Clean up temporary memory.
    free(left_part);
    free(right_part);
}

/**
 * @brief       :   Sorts an array of integers using the Timsort algorithm.
 * @details     :   The algorithm first sorts small chunks of size RUN using 
 *                  insertion sort, then iteratively merges those chunks 
 *                  using merge sort logic. Time complexity: O(n log n).
 * @param       :   array: The target integer array to be sorted.
 * @param       :   size:  The number of elements in the array.
 * @return      :   void
 */
void tim_sort(int* array, size_t size)
{
    if (array == NULL || size < 2)
    {
        return;
    }

    // First Stage: Sort small segments of the array of length 'RUN' using Insertion Sort.
    for (size_t index = 0; index < size; index += RUN)
    {
        insertion_sort(array, index, minimum(index + RUN - 1, size - 1));
    }

    // Second Stage: Progressively merge the sorted runs.
    // Start with run size 'RUN', then double it each iteration (32, 64, 128, etc.).
    for (size_t step = RUN; step < size; step = 2 * step)
    {
        for (size_t start = 0; start < size; start += 2 * step)
        {
            // Define the midpoint and the endpoint of the two subarrays.
            size_t middle = start + step - 1;
            size_t end = minimum(start + 2 * step - 1, size - 1);

            // Merge the subarrays if a right-side counterpart exists.
            if (middle < end)
            {
                merge(array, start, middle, end);
            }
        }
    }
}