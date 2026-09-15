/**
 * @file        :   heap_sort.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Heap Sort algorithm.
 * @details     :   This script provides an in-place, comparison-based sorting 
 * algorithm using a binary max-heap. It first transforms 
 * the array into a heap and then moves the largest 
 * elements to the end of the array one by one.
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#include <stdio.h>

/**
 * @brief       :   Swaps the values of two integer elements.
 * @param       :   current: Pointer to the first integer.
 * @param       :   value:   Pointer to the second integer.
 * @return      :   void
 */
static inline void swap(int* current, int* value)
{
    int temp = *current;
    *current = *value;
    *value = temp;
}

/**
 * @brief       :   Maintains the Max-Heap property for a subtree.
 * @details     :   Ensures the subtree rooted at 'index' satisfies the 
 *                  heap property where the parent is larger than its 
 *                  children. Recursively heapifies affected subtrees.
 * @param       :   array:   The target integer array.
 * @param       :   size:    The current size of the heap.
 * @param       :   index:   The root index of the subtree to heapify.
 * @return      :   void
 */
static inline void heapify(int* array, size_t size, size_t index)
{
    size_t largest = index;
    size_t left = 2 * index + 1;
    size_t right = 2 * index + 2;

    if (left < size && array[left] > array[largest])
    {
        largest = left;
    }

    if (right < size && array[right] > array[largest])
    {
        largest = right;
    }

    if (largest != index)
    {
        swap(&array[index], &array[largest]);

        heapify(array, size, largest);
    }
}

/**
 * @brief       :   Sorts an array of integers using the Heap Sort algorithm.
 * @details     :   Constructs a max-heap and iteratively swaps the root 
 *                  with the last element, reducing heap size until sorted. 
 *                  Operates with O(n log n) time complexity.
 * @param       :   array:  The target integer array to be sorted.
 * @param       :   size:   The number of elements in the array.
 * @return      :   void
 */
void heap_sort(int* array, size_t size)
{
    if (array == NULL || size < 2)
    {
        return;
    }

    // Build the initial max heap.
    // Note: index must be signed to avoid infinite loop with size_t >= 0.
    for (long index = (long)(size / 2) - 1; index >= 0; index--)
    {
        heapify(array, size, (size_t)index);
    }

    // Extract items from the heap one by one.
    for (size_t index = size - 1; index > 0; index--)
    {
        // Move current root (maximum) to the end.
        swap(&array[0], &array[index]);

        // Re-heapify the root to maintain max-heap property.
        heapify(array, index, 0);
    }
}