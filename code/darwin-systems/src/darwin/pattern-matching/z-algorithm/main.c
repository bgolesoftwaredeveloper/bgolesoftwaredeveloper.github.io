/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Main application entry point executing the Z-algorithm.
 * @details     :   Calculates search pattern matching frequencies and absolute positions 
 * within a target text string using an optimal linear-time scan.
 * Compile:
 * clang ./src/darwin/pattern-matching/z-algorithm/main.c ./src/darwin/pattern-matching/z-algorithm/z_algorithm.c -I./include -o ./bin/z_algorithm_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include header files.
#include "darwin/pattern-matching/z_algorithm.h"

int main(void)
{
    // Define the source text string and the target pattern to search for.
    const char* text = "aabaacaadaabaaba";
    const char* pattern = "aaba";

    // Allocate memory for the output buffer.
    size_t maximum_possible_matches = strlen(text);
    size_t* results = (size_t*)malloc(sizeof(size_t) * maximum_possible_matches);
    size_t count = 0;

    if (results == NULL)
    {
        return 1;
    }

    // Summary.
    printf("%s %s%c", "Text:", text, '\n');
    printf("%s %s%c", "Pattern:", pattern, '\n');

    // Execute the Z-algorithm.
    if (z_search(text, pattern, results, &count))
    {
        // Show the matching indices.
        printf("%s %zu %s", "Pattern found", count, "time(s) at indices: ");

        for (size_t index = 0; index < count; index++)
        {
            printf("%zu ", results[index]);
        }

        printf("%c", '\n');
    }
    else
    {
        printf("%s", "Pattern not found.\n");
    }

    // Clean up.
    free(results);

    return 0;
}