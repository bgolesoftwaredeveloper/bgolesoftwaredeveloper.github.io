/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Driver program to test the brute-force string pattern matching algorithm.
 * @details     :   This file initializes a sample source text and a target search pattern, 
 * outputs them to the console, and invokes the brute-force matching 
 * function to determine if and where the pattern exists within the text.
 * Compile:
 * clang ./src/darwin/pattern-matching/boyer-moore/main.c ./src/darwin/pattern-matching/boyer-moore/boyer_moore.c -I./include -o ./bin/boyer_moore_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/pattern-matching/boyer_moore.h"

int main(void)
{
    const char* text = "ABAAABCDABCDEABCDABCDEF";
    const char* pattern = "ABCD";

    if (boyer_moore(text, pattern))
    {
        printf("%s", "Result: Pattern found successfully.\n");
    }
    else
    {
        printf("%s", "Result: Pattern NOT found.\n");
    }

    return 0;
}