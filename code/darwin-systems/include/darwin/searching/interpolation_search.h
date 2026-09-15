/**
 * @file        :   interpolation_search.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the interpolation search algorithm.
 * @details     :   Provides the function prototype and necessary inclusions for 
 * performing interpolation search on sorted integer arrays. 
 * This implementation includes robust error handling and 
 * defensive programming checks.
 * @version     :   1.0
 * @date        :   2026-04-17
 */
#ifndef INTERPOLATION_SEARCH_H
#define INTERPOLATION_SEARCH_H

#include <stddef.h>

// Function prototypes.
int interpolation_search(const int* array, size_t size, int key, size_t* out_index);

#endif