/**
 * @file        :   boyer_moore.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Boyer-Moore string pattern matching algorithm.
 * @details     :   Provides the underlying logic for the Boyer-Moore search. It processes 
 * and utilizes both the Bad Character and Good Suffix heuristics to skip 
 * large portions of text, optimizing search efficiency by scanning 
 * characters from right to left.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/pattern-matching/boyer_moore.h"

/**
 * @brief       :   Preprocesses the pattern to populate the Bad Character shift table.
 * @details     :   Maps out the rightmost occurrences of characters within the search pattern. 
 *                  This allows the algorithm to safely align matching characters when a mismatch 
 *                  occurs or skip ahead if the mismatched character does not exist in the pattern.
 * @param       :   pattern: The substring query being preprocessed.
 * @param       :   length: The designated character length of the target pattern.
 * @param       :   table: The array lookup structure matching the maximum ASCII size to populate.
 * @return      :   void
 */
static inline void compute_bad_character_shifts(const char* pattern, size_t length, int table[ASCII_SIZE])
{
    // Initialize all entry indices within the table to -1, assuming non-existence.
    for (size_t ascii_index = 0; ascii_index < ASCII_SIZE; ascii_index++)
    {
        table[ascii_index] = -1;
    }

    // Assign the pattern's character indexes to track their last structural occurrences.
    for (size_t pattern_index = 0; pattern_index < length; pattern_index++)
    {
        table[(unsigned char)pattern[pattern_index]] = (int)pattern_index;
    }
}

/**
 * @brief       :   Preprocesses the pattern to populate the Good Suffix shift table.
 * @details     :   Analyzes inner sub-patterns, borders, and structural suffixes to calculate 
 *                  how far the string pattern can skip ahead safely when a structural suffix mismatch 
 *                  occurs after a partial match.
 * @param       :   pattern: The substring query being preprocessed.
 * @param       :   length: The designated character length of the target pattern.
 * @param       :   borders: Temporary allocation tracker mapping explicit sub-pattern borders.
 * @param       :   table: The target shift table initialized to record valid safety leaps.
 * @return      :   void
 */
static inline void compute_good_suffix_shifts(const char* pattern, size_t length, size_t borders[], size_t table[])
{
    size_t left_cursor = length + 1;
    size_t right_cursor = length;

    // Define the base terminal boundary condition.
    borders[right_cursor] = left_cursor;

    // Phase 1: Track internal borders and record shifts for complete suffixes.
    while (right_cursor > 0)
    {
        // Walk backwards to resolve repeating patterns or misaligned inner sequences.
        while (left_cursor <= length && pattern[right_cursor - 1] != pattern[left_cursor - 1])
        {
            if (table[left_cursor] == 0)
            {
                table[left_cursor] = left_cursor - right_cursor;
            }

            left_cursor = borders[left_cursor];
        }

        right_cursor--;
        left_cursor--;

        borders[right_cursor] = left_cursor;
    }

    // Phase 2: Handle partial suffix loops and track overall prefix-border links.
    size_t border_position = borders[0];

    for (size_t pattern_index = 0; pattern_index <= length; pattern_index++)
    {
        // Fill unmapped positions using the primary border boundaries.
        if (table[pattern_index] == 0)
        {
            table[pattern_index] = border_position;
        }
        
        // Advance structural positions if the outer bounds drop below the current tracker index.
        if (pattern_index == border_position)
        {
            border_position = borders[border_position];
        }
    }
}

/**
 * @brief       :   Executes the optimized Boyer-Moore pattern matching routine.
 * @details     :   Coordinates allocations, handles structural data checks, and runs the scanning loop. 
 *                  It parses backward through the pattern and evaluates the maximum possible shift 
 *                  from the bad-character and good-suffix tables upon encountering a character clash.
 * @param       :   text: The null-terminated source string container to scan.
 * @param       :   pattern: The null-terminated substring target to discover.
 * @return      :   bool: true if the sequence is successfully found at least once, false if missing.
 */
bool boyer_moore(const char* text, const char* pattern)
{
    // Return early if pointer arguments are structurally invalid.
    if (text == NULL || pattern == NULL)
    {
        return false;
    }

    size_t text_length = strlen(text);
    size_t pattern_length = strlen(pattern);

    // Ensure lengths are logical and fitting for sub-pattern isolation tasks.
    if (pattern_length == 0 || pattern_length > text_length)
    {
        return false;
    }

    int table[ASCII_SIZE];

    // Allocate dynamic memory tracks for the good suffix lookups.
    size_t* good_suffix_table = (size_t*)calloc(pattern_length + 1, sizeof(size_t));
    size_t* suffix_borders = (size_t*)malloc(sizeof(size_t) * (pattern_length + 1));

    // Safety fallback block handling catastrophic heap allocation failures.
    if (good_suffix_table == NULL || suffix_borders == NULL)
    {
        free(good_suffix_table);
        free(suffix_borders);

        return false;
    }

    // Perform preprocessing passes for both core shift heuristics.
    compute_bad_character_shifts(pattern, pattern_length, table);
    compute_good_suffix_shifts(pattern, pattern_length, suffix_borders, good_suffix_table);

    size_t text_shift = 0;

    bool discovered = false;

    // Scan text footprint until the right boundary constraints hit text limits.
    while (text_shift <= (text_length - pattern_length))
    {
        // Start right-to-left evaluation at the final character index of the pattern.
        long long scan_index = (long long)pattern_length - 1;

        // Decrease indices as long as characters under evaluation continue matching.
        while (scan_index >= 0 && pattern[scan_index] == text[text_shift + (size_t)scan_index])
        {
            scan_index--;
        }

        // If the scan index underflows zero, a total pattern match has been hit.
        if (scan_index < 0)
        {
            discovered = true;

            // Use the base good suffix table metric to shift the pattern forward safely.
            text_shift += good_suffix_table[0];
        }
        else
        {
            // Resolve character mismatch information.
            char mismatched = text[text_shift + (size_t)scan_index];

            // Evaluate shift bounds calculated by the bad character heuristic.
            long long bad_calculated_shift = scan_index - table[(unsigned char)mismatched];

            // Evaluate shift bounds calculated by the good suffix heuristic.
            size_t good_suffix_shift = good_suffix_table[(size_t)scan_index + 1];

            // Ensure the bad character heuristic never results in an illogical zero or backward shift.
            size_t final = (bad_calculated_shift > 0) ? (size_t)bad_calculated_shift : 1;

            // Advance the text pointer alignment by selecting the max shift value between both rules.
            text_shift += (final > good_suffix_shift) ? final : good_suffix_shift;
        }
    }

    // Clean up heap space allocations to prevent memory leaks.
    free(good_suffix_table);
    free(suffix_borders);

    return discovered;
}