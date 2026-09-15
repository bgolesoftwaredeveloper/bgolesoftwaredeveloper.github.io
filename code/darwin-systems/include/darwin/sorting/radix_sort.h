/**
 * @file        :   radix_sort.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the Radix Sort algorithm implementation.
 * @details     :   This header provides the public interface for Radix Sort, 
 * a non-comparative sorting algorithm that sorts integers 
 * by processing individual digits.
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#include <stddef.h>

// Function prototypes.
void radix_sort(int* array, size_t size);

#endif