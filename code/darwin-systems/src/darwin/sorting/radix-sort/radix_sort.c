/**
 * @file        :   radix_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Radix Sort algorithm.
 * @details     :   This script provides a non-comparative integer sorting 
 * algorithm. It sorts data by processing individual digits 
 * sharing the same significant position and value. This 
 * implementation uses the Least Significant Digit (LSD) approach.
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief       :   Finds the maximum value in an integer array.
 * @details     :   Iterates through the array to identify the largest element, 
 *                  which determines the number of digits to process.
 * @param       :   array:  The target integer array.
 * @param       :   size:   The number of elements in the array.
 * @return      :   int:    The maximum value found in the array.
 */
static inline int get_maximum(const int* array, size_t size)
{
    int maximum = array[0];

    for (size_t index = 1; index < size; index++)
    {
        if (array[index] > maximum)
        {
            maximum = array[index];
        }
    }

    return maximum;
}

/**
 * @brief       :   Performs a stable counting sort based on a specific digit.
 * @details     :   A helper subroutine that sorts the array based on the digit 
 *                  represented by the current exponent (1, 10, 100, etc.). 
 *                  Stability is maintained to ensure the correctness of the 
 *                  overall Radix Sort.
 * @param       :   array:    The target integer array to be partially sorted.
 * @param       :   size:     The number of elements in the array.
 * @param       :   exponent: The current digit position (10^i) being processed.
 * @return      :   void
 */
static inline void counting_sort(int* array, size_t size, int exponent)
{
    int* output = (int*)malloc(sizeof(int) * size);
    
    if (output == NULL)
    {
        return;
    }

    int count[10] = {0};

    // Store count of occurrences of each digit in count[].
    for (size_t index = 0; index < size; index++)
    {
        count[(array[index] / exponent) % 10]++;
    }

    // Update count[i] to store the actual position of this digit in output[].
    for (int index = 1; index < 10; index++)
    {
        count[index] += count[index - 1];
    }

    // Build the output array using the count positions.
    for (int index = (int)size - 1; index >= 0; index--)
    {
        output[count[(array[index] / exponent) % 10] - 1] = array[index];
        count[(array[index] / exponent) % 10]--;
    }

    // Transfer the sorted elements back into the original array.
    for (size_t index = 0; index < size; index++)
    {
        array[index] = output[index];
    }

    free(output);
}

/**
 * @brief       :   Sorts an array of integers using the Radix Sort algorithm.
 * @details     :   Processes each digit position starting from the least 
 *                  significant digit (LSD) up to the most significant digit. 
 *                  Complexity is O(nk) where k is the number of digits.
 * @param       :   array:  The target integer array to be sorted.
 * @param       :   size:   The number of elements in the array.
 * @return      :   void
 */
void radix_sort(int* array, size_t size)
{
    if (array == NULL || size < 2)
    {
        return;
    }

    // Identify the maximum number to determine the number of digits.
    int maximum = get_maximum(array, size);

    // Apply counting sort to every digit position.
    for (int exponent = 1; maximum / exponent > 0; exponent *= 10)
    {
        counting_sort(array, size, exponent);
    }
}