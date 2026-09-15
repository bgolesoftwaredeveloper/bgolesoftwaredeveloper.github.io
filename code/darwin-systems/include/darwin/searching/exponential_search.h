/**
 * @file        :   exponential_search.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the exponential search algorithm.
 * @details     :   Defines the interface for searching sorted arrays using 
 * exponential range detection followed by binary search.
 * @version     :   1.0
 * @date        :   2026-04-16
 */
#ifndef EXPONENTIAL_SEARCH_H
#define EXPONENTIAL_SEARCH_H

#include <stddef.h>

// Function prototypes.
int exponential_search(const int* array, size_t size, int key);

#endif