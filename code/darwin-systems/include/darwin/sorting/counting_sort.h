/**
 * @file        :   counting_sort.h
 * @author      :   Braiden Gole
 * @brief       :   Interface for the Counting Sort algorithm implementation.
 * @details     :   Provides the function prototype for counting_sort, an 
 * O(n + k) non-comparative sorting algorithm. This 
 * implementation is designed for non-negative integers 
 * and maintains stability by preserving the relative 
 * order of equal elements.
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#ifndef COUNTING_SORT_H
#define COUNTING_SORT_H

#include <stddef.h>

// Function prototypes.
void counting_sort(int* array, size_t size);

#endif