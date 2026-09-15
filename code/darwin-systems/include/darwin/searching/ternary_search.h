/**
 * @file        :   ternary_search.h
 * @author      :   Braiden Gole
 * @brief       :   Function prototypes for the ternary search algorithm.
 * @details     :   This header defines the interface for the ternary search 
 * algorithm, a divide-and-conquer method that splits a sorted 
 * array into three parts to locate a target key.
 * * Complexity:
 * - Worst-case time: $O(\log_3 n)$
 * - Space:           $O(\log_3 n)$ (Recursive stack)
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#ifndef TERNARY_SEARCH_H
#define TERNARY_SEARCH_H

#include <stddef.h>

// Function prototypes.
int ternary_search(const int* array, size_t low, size_t high, int key);

#endif