
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/pattern-matching/kmp.h"

/**
 * @brief       :   Computes the Longest Prefix Suffix (LPS) table for the given pattern.
 * @details     :   Tracks the lengths of the longest proper prefix that matches a proper suffix 
 *                  for each sub-pattern window. Used to determine how many positions to skip shifts 
 *                  on a text alignment mismatch.
 * @param       :   pattern: The null-terminated substring target being preprocessed.
 * @param       :   pattern_length: The length of the pattern sequence.
 * @param       :   lps: The pre-allocated table buffer to populate with prefix lengths.
 * @return      :   void
 */
static inline void compute_longest_prefix_suffix(const char* pattern, size_t pattern_length, size_t* lps)
{
    size_t length_of_prefix = 0;

    // A single character has no proper prefix/suffix.
    lps[0] = 0;

    size_t pattern_index = 1;

    // Iterate through the pattern to fill out the remaining indices of the table.
    while (pattern_index < pattern_length)
    {
        if (pattern[pattern_index] == pattern[length_of_prefix])
        {
            length_of_prefix++;

            lps[pattern_index] = length_of_prefix;

            pattern_index++;
        }
        else
        {
            // Mimatch scenario within the prefix evaluation.
            if (length_of_prefix != 0)
            {
                length_of_prefix = lps[length_of_prefix - 1];
            }
            else
            {
                // No prefix match available: Reset weight at this slot to zero.
                lps[pattern_index] = 0;

                pattern_index++;
            }
        }
    }
}

/**
 * @brief       :   Performs an optimized search for a substring pattern within a body of text.
 * @details     :   Utilizes a computed fallback state machine lookup table to skip redundant character 
 *                  comparisons when mismatches occur, preventing the text pointer from backtracking.
 * @param       :   text: The null-terminated source string container to scan.
 * @param       :   pattern: The null-terminated substring target to discover.
 * @return      :   bool: true if the absolute pattern match is found, false if missing completely.
 */
bool kmp(const char* text, const char* pattern)
{
    // Validate inputs are not NULL pointers.
    if (text == NULL || pattern == NULL)
    {
        return false;
    }

    size_t text_length = strlen(text);
    size_t pattern_length = strlen(pattern);

    // Ensure pattern and text lengths are valid for matching.
    if (pattern_length == 0)
    {
        return false;
    }

    if (pattern_length > text_length)
    {
        return false;
    }

    // Protect against integer mutliplication overflow during allocation calculations.
    if (pattern_length > SIZE_MAX / sizeof(size_t))
    {
        return false;
    }

    // Allocate memory for the fallback state table.
    size_t* lsp_array = (size_t*)malloc(sizeof(size_t) * pattern_length);

    if (lsp_array == NULL)
    {
        return false;
    }

    // Construct the fallback lookup index table.
    compute_longest_prefix_suffix(pattern, pattern_length, lsp_array);

    size_t text_index = 0;
    size_t pattern_index = 0;

    bool found = false;

    // Loop through the text while remaining characters exceed remaining pattern requirements.
    while ((text_length - text_index) >= (pattern_length - pattern_index))
    {
        if (pattern[pattern_index] == text[text_index])
        {
            pattern_index++;
            text_index++;
        }

        if (pattern_index == pattern_length)
        {
            // Full match uncovered.
            found = true;

            break;
        }
        else if (text_index < text_length && pattern[pattern_index] != text[text_index])
        {
            if (pattern_index != 0)
            {
                // Shif the pattern alignment back based on known prefix redundancies.
                pattern_index = lsp_array[pattern_index - 1];
            }
            else
            {
                // Fall back completely to advance tracking evaluation.
                text_index++;
            }
        }
    }

    // Release allocated lookup array memory prior to exit franes,
    free(lsp_array);

    return found;
}