/**
 * @file        :   boyer_moore.h
 * @author      :   Braiden Gole
 * @brief       :   Header definitions for the Boyer-Moore string pattern matching algorithm.
 * @details     :   Declares the core execution interface, operational constants, and 
 * required standard system library references utilized across the 
 * Boyer-Moore implementation footprint.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#ifndef BOYER_MOORE_H
#define BOYER_MOORE_H

#include <stddef.h>
#include <stdbool.h>

#define ASCII_SIZE 256

// Function prototypes.
bool boyer_moore(const char* text, const char* pattern);

#endif