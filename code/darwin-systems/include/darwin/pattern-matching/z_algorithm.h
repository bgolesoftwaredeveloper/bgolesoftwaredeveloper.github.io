/**
 * @file        :   z_algorithm.h
 * @author      :   Braiden Gole
 * @brief       :   Header file declaring the Z-algorithm interface for string matching.
 * @details     :   Provides function prototypes and macro guards to safely expose the
 * linear-time pattern searching utility across compilation units.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#ifndef Z_ALGORITHM_H
#define Z_ALGORITHM_H

#include <stddef.h>
#include <stdbool.h>

// Function prototypes.
bool z_search(const char* text, const char* pattern, size_t* array, size_t* count);

#endif