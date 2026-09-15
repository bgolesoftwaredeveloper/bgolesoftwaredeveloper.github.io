/**
 * @file        :   jump_search.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the jump search algorithm implementation.
 * @details     :   Provides the function prototype and necessary inclusions 
 * for performing a jump search on sorted integer arrays.
 * @version     :   1.0
 * @date        :   2026-04-15
 */
#ifndef JUMP_SEARCH_H
#define JUMP_SEARCH_H

#include <stddef.h>

// Function prototypes.
int jump_search(const int* array, size_t size, int target);

#endif