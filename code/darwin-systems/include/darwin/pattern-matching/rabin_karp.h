/**
 * @file        :   rabin_karp.h
 * @author      :   Braiden Gole
 * @brief       :   Interface definitions and configuration constants for the Rabin-Karp algorithm.
 * @details     :   Exposes the public header interface API along with fixed numeric defaults for 
 * radix alphabet scaling metrics and safe modular arithmetic distributions.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#ifndef RABIN_KARP_H
#define RABIN_KARP_H

#include <stdbool.h>
#include <stddef.h>

#define ALPHABET_BASE 256
#define PRIME_MODULUS 101

// Function prototypes.
bool rabin_karp(const char* haystack, const char* needle);

#endif