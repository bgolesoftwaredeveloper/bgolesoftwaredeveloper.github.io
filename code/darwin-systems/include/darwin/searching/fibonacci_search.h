/**
 * @file        :   fibonacci_search.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the Fibonacci search algorithm.
 * @details     :   Defines the interface for searching sorted integer arrays using 
 * Fibonacci partition logic. This search is an alternative to 
 * binary search that avoids division operations.
 * @version     :   1.1
 * @date        :   2026-04-19
 */
#ifndef FIBONACCI_SEARCH_H
#define FIBONACCI_SEARCH_H

#include <stddef.h>
#include <stdbool.h>

// Function prototypes.
bool fibonacci_search(int* array, size_t size, int key, size_t* out);

#endif