/**
 * @file        :   merge_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Recursive Merge Sort algorithm.
 * @details     :   Provides a robust, divide-and-conquer sorting mechanism.
 * This implementation uses size_t for index management to 
 * ensure 64-bit safety and provides a stable sort with 
 * O(n log n) time complexity.
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief       :   Internal helper to merge two sorted sub-arrays.
 * @details     :   Allocates temporary buffers to hold left and right 
 *                  partitions, compares elements, and merges them back 
 *                  into the source array in ascending order.
 * @param       :   array:  The target integer array to modify.
 * @param       :   start:  The starting index of the first sub-array.
 * @param       :   middle: The ending index of the first sub-array.
 * @param       :   end:    The ending index of the second sub-array.
 * @return      :   void
 */
static void merge(int array[], int start, int middle, int end)
{
    // Define iterative variables. 
    size_t left_iterator = 0;
    size_t right_iterator = 0;
    size_t merge_iterator = 0;

    // Calculate the left and right sizes.
    size_t left_size = middle - start + 1;
    size_t right_size = end - middle;

    // Allocate temporary buffers.
    int* left_buffer = (int*)malloc(sizeof(int) * left_size);
    int* right_buffer = (int*)malloc(sizeof(int) * right_size);

    // Populate the temporary bufffers.
    for (left_iterator = 0; left_iterator < left_size; left_iterator++)
    {
        left_buffer[left_iterator] = array[start + left_iterator];
    }

    for (right_iterator = 0; right_iterator < right_size; right_iterator++)
    {
        right_buffer[right_iterator] = array[middle + 1 + right_iterator];
    }

    // Reset iterators for the merging process.
    left_iterator = 0;
    right_iterator = 0;
    merge_iterator = start;

    // Compare items and move the smaller one back into the array.
    while (left_iterator < left_size && right_iterator < right_size)
    {
        if (left_buffer[left_iterator] <= right_buffer[right_iterator])
        {
            array[merge_iterator] = left_buffer[left_iterator];
            left_iterator++;
        }
        else
        {
            array[merge_iterator] = right_buffer[right_iterator];
            right_iterator++;
        }

        merge_iterator++;
    }

    // Drain the remaining items from the left buffer.
    while (left_iterator < left_size)
    {
        array[merge_iterator] = left_buffer[left_iterator];
        left_iterator++;
        merge_iterator++;
    }

    // Drain the remaining items from the right buffer.
    while (right_iterator < right_size)
    {
        array[merge_iterator] = right_buffer[right_iterator];
        right_iterator++;
        merge_iterator++;
    }

    free(left_buffer);
    free(right_buffer);
}

/**
 * @brief       :   Recursively partitions and sorts an array of integers.
 * @details     :   Splits the array into smaller halves until the base case 
 *                  is reached (single element), then merges partitions 
 *                  using the merge() helper function.
 * @param       :   array:           Pointer to the integer array to be sorted.
 * @param       :   lower_boundary:  The starting index of the current range.
 * @param       :   upper_boundary:  The ending index of the current range.
 * @return      :   void
 */
void merge_sort(int array[], size_t lower_boundary, size_t upper_boundary)
{
    if (lower_boundary < upper_boundary)
    {
        // Find the split point while preventing overflow.
        size_t split = lower_boundary + (upper_boundary - lower_boundary) / 2;

        // Sort each half.
        merge_sort(array, lower_boundary, split);
        merge_sort(array, split + 1, upper_boundary);

        // Mege the halves back together.
        merge(array, lower_boundary, split, upper_boundary);
    }
}