/**
 * @file        :   insertion_sort.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the Insertion Sort algorithm implementation.
 * @details     :   Defines the interface for a stable, in-place insertion sort. 
 * This algorithm is efficient for small datasets and 
 * maintains a time complexity of O(n) in the best-case 
 * scenario (already sorted data).
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#ifndef INSERTION_SORT_H
#define INSERTION_SORT_H

#include <stddef.h>

// Function prototypes.
void insertion_sort(int* array, size_t size);

#endif