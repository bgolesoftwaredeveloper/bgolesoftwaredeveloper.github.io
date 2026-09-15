/**
 * @file        :   rabin_karp.c
 * @author      :   Braiden Gole
 * @brief       :   Implements the Rabin-Karp string matching algorithm.
 * @details     :   Uses a rolling hash system to find substring matches in linear time. Includes 
 * defensive boundaries to prevent signed integer overflows during numeric calculation.
 * Compile:
 * clang ./src/darwin/pattern-matching/rabin-karp/main.c ./src/darwin/pattern-matching/rabin-karp/rabin_karp.c -I./include -o ./bin/rabin_karp_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>

// Include header files.
#include "darwin/pattern-matching/rabin_karp.h"

/**
 * @brief       :   Performs an optimized search for a substring pattern within a body of text.
 * @details     :   Utilizes a mathematical rolling hash to scan windows of text in constant time,
 *                  falling back to an explicit character verification loop only when a hash collision occurs.
 * @param       :   haystack: The null-terminated source string container to scan.
 * @param       :   needle: The null-terminated substring target to discover.
 * @return      :   bool: true if the absolute pattern match is found, false if missing completely.
 */ 
bool rabin_karp(const char* haystack, const char* needle)
{
    // Validate inputs are not NULL pointers.
    if (haystack == NULL || needle == NULL)
    {
        return false;
    }

    // Calculate the size of the haystack and neeedle.
    size_t text_length = strlen(haystack);
    size_t pattern_length = strlen(needle);

    // Ensure pattern and text lengths are valid for matching.
    if (pattern_length == 0)
    {
        return false;
    }

    if (pattern_length > text_length)
    {
        return false;
    }

    long long leading_digit_multiplier = 1;

    // Calculate the multiplier for the highest-order digit of the window.
    for (size_t iterator = 0; iterator < pattern_length - 1; iterator++)
    {
        // Protect against integer overflow prior to multiplication.
        if (leading_digit_multiplier > (LLONG_MAX / ALPHABET_BASE))
        {
            return false;
        }

        leading_digit_multiplier = (leading_digit_multiplier * ALPHABET_BASE) % PRIME_MODULUS;
    }

    long long current_pattern_hash = 0;
    long long current_window_hash = 0;
    
    bool discovered = false;

    // Construct the initial hash states for the pattern and first text window.
    for (size_t index = 0; index < pattern_length; index++)
    {
        // Protect against hash accumulation overflow boundaries.
        if (current_pattern_hash > (LLONG_MAX - ALPHABET_BASE) / ALPHABET_BASE || current_window_hash > (LLONG_MAX - ALPHABET_BASE) / ALPHABET_BASE)
        {
            return false;
        }

        current_pattern_hash = (ALPHABET_BASE * current_pattern_hash + (unsigned char)needle[index]) % PRIME_MODULUS;
        current_window_hash = (ALPHABET_BASE * current_window_hash + (unsigned char)haystack[index]) % PRIME_MODULUS;
    }

    size_t maximum_index = text_length - pattern_length;

    // Slide the winodw across the body of the text sequence.
    for (size_t window_index = 0; window_index <= maximum_index; window_index++)
    {
        // If hash codes match, perform a deep verification check to rule out false positives.
        if (current_pattern_hash == current_window_hash)
        {
            size_t match_index = 0;

            for (match_index = 0; match_index < pattern_length; match_index++)
            {
                if (haystack[window_index + match_index] != needle[match_index])
                {
                    break;
                }
            }

            // Entire loop cleared without a mismatch: Exact match unconvered.
            if (match_index == pattern_length)
            {
                discovered = true;
            }
        }

        // Shift window forward by computing the rolling hash of the subsequent black.
        if (window_index < maximum_index)
        {
            long long leading_code = (unsigned char)haystack[window_index];
            long long trailing_code = (unsigned char)haystack[window_index + pattern_length];

            long long high_order_term = leading_code * leading_digit_multiplier;

            // Protect against arithmetic overflow during term multiplication.
            if (leading_code != 0 && high_order_term / leading_code != leading_digit_multiplier)
            {
                return false;
            }

            // Drop the old leading element and add the new trailing element.
            current_window_hash = (ALPHABET_BASE * (current_window_hash - high_order_term) + trailing_code) % PRIME_MODULUS;

            // Normalize the hash signature back to a positive spectrum if subtraction dipped below zero.
            if (current_window_hash < 0)
            {
                current_window_hash = current_window_hash + PRIME_MODULUS;
            }
        }
    }

    return discovered;
}