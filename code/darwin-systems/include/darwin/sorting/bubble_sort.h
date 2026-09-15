/**
 * @file        :   bubble_sort.h
 * @author      :   Braiden Gole
 * @brief       :   Interface for the Bubble Sort algorithm implementation.
 * @details     :   Provides function prototypes for a comparison-based bubble sort 
 * optimized with an early-exit flag. This implementation is 
 * designed for integer arrays within the Darwin Systems library.
 * @version     :   1.0
 * @date        :   2026-04-13
 */
#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

#include <stddef.h>

// Function prototypes.
void bubble_sort(int* array, size_t size);

#endif