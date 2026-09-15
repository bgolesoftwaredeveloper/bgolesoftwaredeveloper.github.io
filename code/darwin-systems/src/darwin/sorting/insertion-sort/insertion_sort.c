/**
 * @file        :   insertion_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Insertion Sort algorithm.
 * @details     :   This script provides a stable, in-place implementation of 
 * Insertion Sort. It iterates through an input array, 
 * removing one element at a time and finding its correct 
 * location within the sorted portion of the array. It is 
 * highly efficient for small datasets or nearly sorted arrays.
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#include <stdio.h>

/**
 * @brief       :   Sorts an array of integers using the Insertion Sort algorithm.
 * @details     :   Builds a sorted sequence by taking one element at a time 
 *                  and inserting it into its correct position among the 
 *                  already-sorted elements. Operates with O(n^2) worst-case 
 *                  time complexity and O(1) space complexity.
 * @param       :   array:  The target integer array to be sorted.
 * @param       :   size:   The number of elements in the array.
 * @return      :   void
 */
void insertion_sort(int* array, size_t size)
{
    if (array == NULL || size < 2)
    {
        return;
    }

    // Iterate through the array starting from the second item.
    // The first item (index 0) is considered a sorted partition of length 1.
    for (size_t index = 1; index < size; index++)
    {
        // Store the value to be inserted into the sorteed sub-array.
        int key = array[index];

        // Track the potential insertion position.
        size_t insertion_index = index;

        /**
         * Compare the key with elements in the sorted sub-array (to the left).
         * If an element is greater than the key, shift it one position to the right.
         * We stop if we reach the start of the array or find a smaller/equal value.
         */
        while (insertion_index > 0 && array[insertion_index - 1] > key)
        {
            array[insertion_index] = array[insertion_index - 1];
            insertion_index--;
        }

        // Place the key into its final, correct position within the sorted sub-array.
        array[insertion_index] = key;
    }
}