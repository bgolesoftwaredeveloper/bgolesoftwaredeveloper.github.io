/**
 * @file        :   kmp.h
 * @author      :   Braiden Gole
 * @brief       :   Header file containing the function prototype for the Knuth-Morris-Pratt algorithm.
 * @details     :   Provides the interface for the KMP pattern-matching algorithm. 
 * The implementation handles string matching in O(n + m) time complexity
 * and includes comprehensive defensive checks for memory and bounds safety.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#ifndef KMP_H
#define KMP_H

#include <stddef.h>
#include <stdbool.h>

// Function prototypes.
bool kmp(const char* text, const char* pattern);

#endif