/**
 * @file        :   brute_force.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the brute-force string pattern matching algorithm.
 * @details     :   Provides the underlying logic for the brute-force search. It character-by-character
 * checks for the pattern sequence at each viable shifting index of the target text.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>
#include <string.h>

// Include header files.
#include "darwin/pattern-matching/brute_force.h"

/**
 * @brief       :   Performs a naive search for a substring pattern within a body of text.
 * @details     :   Shifts the pattern over the text sequence, utilizing nested iteration loops 
 *                  to compare characters. Breaks execution loops early upon discovering mismatches.
 * @param       :   text: The null-terminated source string container to scan.
 * @param       :   pattern: The null-terminated substring target to discover.
 * @return      :   bool: true if the absolute pattern match is found, false if missing completely.
 */
bool brute_force(const char* text, const char* pattern)
{
    if (text == NULL || pattern == NULL)
    {
        return false;
    }
    
    size_t text_length = strlen(text);
    size_t pattern_length = strlen(pattern);

    if (text_length < pattern_length)
    {
        return false;
    }

    // Calculate the maximum index where the pattern can realistically start.
    size_t maximum = text_length - pattern_length;

    bool flag = false;

    // Shift the pattern over the text one position at a time.
    for (size_t text_index = 0; text_index <= maximum; text_index++)
    {
        size_t pattern_index;

        // Check for a pattern match at the current text alignment position.
        for (pattern_index = 0; pattern_index < pattern_length; pattern_index++)
        {
            if (text[text_index + pattern_index] != pattern[pattern_index])
            {
                // Mismatch detected!
                break;
            }
        }

        // If the inner loop completed without breaking, a full match was found.
        if (pattern_index == pattern_length)
        {
            flag = true;

            break;
        }
    }

    return flag;
}