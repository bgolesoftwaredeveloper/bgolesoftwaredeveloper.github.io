/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Main entry point to demonstrate the Knuth-Morris-Pratt (KMP) string matching algorithm.
 * @details     :   This program validates the KMP pattern-matching implementation by searching for 
 * a specific pattern string within a target text string. The implementation 
 * utilizes size_t for index safety, includes defensive checks against NULL pointers,
 * empty strings, and memory allocation overflows, and returns a boolean result.
 * Compile:
 * clang ./src/darwin/pattern-matching/kmp/main.c ./src/darwin/pattern-matching/kmp/kmp.c -I./include -o ./bin/kmp_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/pattern-matching/kmp.h"

int main(void)
{
    // Define the text and pattern to work with.
    const char* text = "ABABDABACDABABCABAB";
    const char* pattern = "ABABCABAB";

    // Perform KMP to see if the pattern has been found.
    if (kmp(text, pattern))
    {
        printf("%s", "Pattern found.\n");
    }
    else
    {
        printf("%s", "Pattern NOT found.\n");
    }

    return 0;
}