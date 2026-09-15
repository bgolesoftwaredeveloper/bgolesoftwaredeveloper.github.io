/**
 * @file        :   quick_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of an iterative Quick Sort algorithm.
 * @details     :   This module provides a non-recursive version of Quick Sort 
 * using a manual stack to manage sub-array boundaries.
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief       :   Swaps the values of two integer variables.
 * @param       :   current: Pointer to the first integer.
 * @param       :   value: Pointer to the second integer.
 */
static inline void swap(int* current, int* value)
{
    int temp = *current;
    *current = *value;
    *value = temp;
}

/**
 * @brief       :   Partitions the array around a pivot element.
 * @details     :   The last element is chosen as the pivot. Elements smaller 
 *                  than the pivot are moved to the left, and larger to the right.
 * @param       :   array:  The integer array to partition.
 * @param       :   low:    The starting index of the segment.
 * @param       :   high:   The ending index of the segment.
 * @return      :   int:    The final index position of the pivot.
 */
static size_t partition(int array[], size_t low, size_t high)
{
    int pivot = array[high];
    size_t index = low;

    for (size_t pass = low; pass < high; pass++)
    {
        if (array[pass] <= pivot)
        {
            swap(&array[index], &array[pass]);
            index++;
        }
    }

    swap(&array[index], &array[high]);

    return index;
}

/**
 * @brief       :   Sorts an array of integers using the Quick Sort algorithm.
 * @details     :   An iterative approach using a stack to store the low and 
 * high indices of sub-arrays yet to be processed.
 * @param       :   array:   The integer array to be sorted.
 * @param       :   low:     The starting index of the array.
 * @param       :   high:    The ending index of the array.
 */
void quick_sort(int array[], size_t low, size_t high)
{
    if (array == NULL || low >= high)
    {
        return;
    }

    // Create an auxiliary stack.
    size_t size = (high - low + 1) * 2;

    int* stack = (int*)malloc(sizeof(int) * size);

    if (stack == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");

        return;
    }

    // Initialize top of stack.
    int top = -1;

    // Push initial values of low and high to stack.
    stack[++top] = (int)low;
    stack[++top] = (int)high;

    // Keep popping from stack while is not empty.
    while (top >= 0)
    {
        high = (size_t)stack[top--];
        low = (size_t)stack[top--];

        size_t pivot = partition(array, low, high);

        // If there are elements on the left side of the pivot,
        // then push left side to stack.
        if (pivot > low + 1)
        {
            stack[++top] = (int)low;
            stack[++top] = (int)pivot - 1;
        }

        if (pivot + 1 < high)
        {
            stack[++top] = (int)pivot + 1;
            stack[++top] = (int)high;
        }
    }

    free(stack);
}