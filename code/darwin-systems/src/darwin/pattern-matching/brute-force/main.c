/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Driver program to test the brute-force string pattern matching algorithm.
 * @details     :   This file initializes a sample source text and a target search pattern, 
 * outputs them to the console, and invokes the brute-force matching 
 * function to determine if and where the pattern exists within the text.
 * Compile:
 * clang ./src/darwin/pattern-matching/brute-force/main.c ./src/darwin/pattern-matching/brute-force/brute_force.c -I./include -o ./bin/brute_force_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/pattern-matching/brute_force.h"

int main(void)
{
    // Define the hardcoded text string and the pattern we want to search for.
    const char* text = "ABAAABCDABCDE";
    const char* pattern = "ABCD";

    // Summary.
    printf("%s %s%c", "Text:", text, '\n');
    printf("%s %s%c", "Pattern:", pattern, '\n');

    // Execute the brute-force search algorithm.
    if (!brute_force(text, pattern))
    {
        printf("%s", "Pattern not found in the provided text.\n");
        
        return 1;
    }
    else
    {
        printf("%s", "Pattern found in the provided text.\n");
    }

    return 0;
}