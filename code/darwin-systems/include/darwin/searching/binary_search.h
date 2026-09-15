/**
 * @file        :   binary_search.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for binary search implementation.
 * @details     :   Provides the function prototype for an efficient 
 * $O(\log n)$ search algorithm. Requires the input array 
 * to be sorted in ascending order.
 * @version     :   1.0
 * @date        :   2026-04-14
 */
#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <stddef.h>

// Function prototypes.
int binary_search(const int* array, size_t size, int target);

#endif