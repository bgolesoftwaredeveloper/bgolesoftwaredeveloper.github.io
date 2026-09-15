/**
 * @file        :   merge_sort.h
 * @author      :   Braiden Gole
 * @brief       :   Interface for the Merge Sort algorithm implementation.
 * @details     :   Defines the public-facing API for a recursive, stable 
 * merge sort. This implementation utilizes a divide-and-conquer 
 * strategy and employs size_t for all indexing and boundaries 
 * to ensure 64-bit portability and prevent integer overflow.
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <stddef.h>

// Function prototypes.
void merge_sort(int array[], size_t lower_boundary, size_t upper_boundary);

#endif