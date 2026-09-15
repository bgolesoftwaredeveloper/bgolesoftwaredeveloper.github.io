/**
 * @file        :   brute_force.h
 * @author      :   Braiden Gole
 * @brief       :   Function prototypes and definitions for brute-force pattern matching.
 * @details     :   Exposes the interface for the naive string matching algorithm,
 * allowing client applications to scan a block of text for a 
 * specific substring pattern.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <stdbool.h>
#include <stddef.h>

// Function prototypes.
bool brute_force(const char* text, const char* pattern);

#endif