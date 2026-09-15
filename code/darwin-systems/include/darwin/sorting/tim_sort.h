/**
 * @file        :   tim_sort.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the Timsort algorithm implementation.
 * @details     :   This header defines the public interface for Timsort,
 * a stable hybrid sorting algorithm. It is designed to
 * be included in C projects requiring efficient, 
 * O(n log n) sorting of integer arrays.
 * @version     :   1.0
 * @date        :   2026-04-18
 */
#ifndef TIM_SORT_H
#define TIM_SORT_H

#include <stddef.h>

// Function prototypes.
void tim_sort(int* array, size_t size);

#endif