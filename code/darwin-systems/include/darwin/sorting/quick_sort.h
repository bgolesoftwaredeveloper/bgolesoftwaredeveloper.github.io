/**
 * @file        :   quick_sort.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the iterative Quick Sort algorithm.
 * @details     :   This module provides the interface for an iterative 
 * implementation of Quick Sort, utilizing a manual stack 
 * to manage sub-array boundaries.
 * @version     :   1.1
 * @date        :   2026-04-13
 */
#ifndef QUICK_SORT_H
#define QUICK_SORT_H

#include <stddef.h>

// Function prototypes.
void quick_sort(int array[], size_t low, size_t high);

#endif