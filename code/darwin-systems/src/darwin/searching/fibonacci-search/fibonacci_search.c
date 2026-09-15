/**
 * @file        :   fibonacci_search.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Fibonacci search algorithm.
 * @details     :   Uses Fibonacci numbers to divide the array into segments. This 
 * approach is often faster than binary search on systems where 
 * division is expensive, as it only uses addition and subtraction.
 * @version     :   1.1
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdbool.h>

/**
 * @brief       :   Helper function to find the minimum of two size_t values.
 * @param       :   compare:   First value.
 * @param       :   against:   Second value.
 * @return      :   The smaller of the two values.
 */
static inline size_t minimum(size_t compare, size_t against)
{
    return (compare <= against) ? compare : against;
}

/**
 * @brief       :   Searches for a key within a sorted integer array using Fibonacci Search.
 * @details     :   The algorithm initializes three Fibonacci numbers (grandparent, 
 *                  parent, current) to narrow the search range. It utilizes an 
 *                  offset to track the eliminated portion of the array from the left.
 * @param       :   array:      Pointer to the first element of the sorted integer array.
 * @param       :   size:       The total number of elements in the array.
 * @param       :   key:        The integer value to be located.
 * @param       :   out:        Pointer to store the resulting index if the key is found.
 * @return      :   Returns true on success, and false for failure.
 */
bool fibonacci_search(int* array, size_t size, int key, size_t* out)
{
    if (array == NULL || size == 0)
    {
        return false;
    }

    // Initialize fibonacci numbers.
    size_t grandparent = 0;
    size_t parent = 1;
    size_t current = grandparent + parent;

    // Find the smallest fibonacci number greater than or equal to size.
    while (current < size)
    {
        grandparent = parent;
        parent = current;
        current = grandparent + parent;
    }

    // Offset marks the eliminated range from the front.
    int offset = -1;

    // While there are items to inspect.
    while (current > 1)
    {
        // Calculate the index to check.
        size_t index = minimum((size_t)(offset + grandparent), size - 1);

        // If the key is greater, move the Fibonacci sequence down two steps.
        if (array[index] < key)
        {
            current = parent;
            parent = grandparent;
            grandparent = current - parent;
            
            offset = (int)index;
        }
        // If the key is smaller, move the Fibonacci sequence down one step.
        else if (array[index] > key)
        {
            current = grandparent;
            parent = parent - grandparent;
            grandparent = current - parent;
        }
        else
        {
            if (out != NULL)
            {
                *out = index;
            }

            return true;
        }
    }

    if (parent == 1 && (size_t)(offset + 1) < size && array[offset + 1] == key)
    {
        if (out != NULL)
        {
            *out = (size_t)(offset + 1);
        }

        return true;
    }

    return false;
}