/**
 * @file        :   z_algorithm.c
 * @author      :   Braiden Gole
 * @brief       :   Linear-time string pattern matching utility implementing the Z-algorithm.
 * @details     :   Provides functions to construct a foundational preprocessing Z-array and
 * leverage it to extract all exact occurrences of a pattern substring inside
 * a body of text in $O(n + m)$ time and space complexity.
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/pattern-matching/z_algorithm.h"

/**
 * @brief       :   Populates the Z-array for a given concatenated string sequence.
 * @details     :   Maintains a dynamic text boundary segment called the "Z-box" (defined by 
 *                  box_left and box_right) which tracks the furthest matching prefix substring.
 *                  Uses previously computed prefix metrics to skip redundant character assessments.
 * @param       :   combined: Constant pointer to the concatenated string (pattern + '$' + text).
 * @param       :   z_array: Pointer to the pre-allocated tracking buffer of matching lengths.
 * @param       :   length: The total size_t capacity of the combined string.
 * @return      :   void
 */
static inline void compute_z_array(const char* combined, size_t* z_array, size_t length)
{
    // Left and right index boundaries of the current prefix-matching window (Z-box).
    size_t box_left = 0;
    size_t box_right = 0;

    // The first item is trivially set to 0 as the string cannot match a proper substring of itself.
    z_array[0] = 0;

    // Process the string sequentially starting immediately after the first character.
    for (size_t current = 1; current < length; current++)
    {
        // Case 1: The current index falls entirely outside the current Z-box boundary window.
        if (current > box_right)
        {
            box_left = current;
            box_right = current;

            // Explicitly match characters sequentially starting back from the prefix origin.
            while (box_right < length && combined[box_right - box_left] == combined[box_right])
            {
                box_right++;
            }

            // Record the total consecutive matches found and shift right back to the terminal index.
            z_array[current] = box_right - box_left;
            box_right--;
        }
        else
        {
            // Case 2: The current index lies safely inside the active Z-box segment window.
            size_t prefix_counterpart = current - box_left;
            size_t remaining_box_capacity = box_right - current + 1;

            // Subcase A: Cached prefix value fits completely inside the remaining Z-box capacity boundaries.
            if (z_array[prefix_counterpart] < remaining_box_capacity)
            {
                z_array[current] = z_array[prefix_counterpart];
            }
            else
            {
                // Subcase B: Matches touch or extend past the Z-box; reset the window and continue scanning.
                box_left = current;

                while (box_right < length && combined[box_right - box_left] == combined[box_right])
                {
                    box_right++;
                }

                // Update the array with the new structural match offset count and shift back the boundary.
                z_array[current] = box_right - box_left;
                box_right--;
            }
        }
    }
}

/**
 * @brief       :   Scans text to find all absolute starting position indices matching a pattern.
 * @details     :   Constructs an isolated structural runtime string block using a '$' separator,
 *                  invokes the array calculation machine, and isolates the matching text offsets.
 * @param       :   text: Constant pointer to the target body text string to scan.
 * @param       :   pattern: Constant pointer to the specific target match sequence.
 * @param       :   array: Pointer to an output buffer array capturing successful match points.
 * @param       :   count: Pointer to a size_t tracker documenting total matches found.
 * @return      :   bool: true if at least one match was successfully registered, otherwise false.
 */
bool z_search(const char* text, const char* pattern, size_t* array, size_t* count)
{
    // Perform safety validations across all inbound procedural operational parameters.
    if (text == NULL || pattern == NULL || array == NULL || count == NULL)
    {
        return false;
    }

    // Reset the outer reference count tracker before initiating tracking loops.
    *count = 0;

    // Cache the text and pattern string lengths.
    size_t text_length = strlen(text);
    size_t pattern_length = strlen(pattern);

    if (text_length == 0 || pattern_length == 0)
    {
        return false;
    }

    // Calculate the combined length: pattern length + 1 (for separator) + text length.
    size_t combined_length = pattern_length + 1 + text_length;

    // Allocate memory for the temporary concatenated working text array layout buffer (+1 for null terminator).
    char* combined = (char*)malloc(sizeof(char) * (combined_length + 1));

    if (combined == NULL)
    {
        return false;
    }

    // Build the structural format: Copy pattern, inject unique separator character, then append text.
    strcpy(combined, pattern);
    combined[pattern_length] = '$';
    strcpy(combined + pattern_length + 1, text);

    // Dynamically allocate memory to hold the processed Z-array results.
    size_t* z_array = (size_t*)malloc(sizeof(size_t) * combined_length);

    if (z_array == NULL)
    {
        free(combined);

        return false;
    }

    // Execute the linear preprocessing scan algorithm to construct the tracking metadata values.
    compute_z_array(combined, z_array, combined_length);

    // Filter through the computed array results to locate matches equal to the pattern size.
    for (size_t index = 0; index < combined_length; index++)
    {
        if (z_array[index] == pattern_length)
        {
            // If the buffer pointer is valid, calculate and assign the converted text index point.
            if (array != NULL)
            {
                array[*count] = index - pattern_length - 1;
            }

            // Increment the match value stored at the referenced memory address pointer safely.
            (*count)++;
        }
    }

    // Reclaim dynamically allocated heap allocations to secure memory footprint blocks.
    free(combined);
    free(z_array);

    // Return status flag true if matches were populated inside the output collection.
    return (*count > 0);
}