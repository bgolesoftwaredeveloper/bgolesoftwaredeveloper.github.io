/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Main entry point and demonstration wrapper for the Rabin-Karp string matching algorithm.
 * @details     :   This file acts as a test driver executable for the Rabin-Karp pattern-matching 
 * implementation. It instantiates a static text body and search pattern, calls the 
 * algorithm interface, and prints the operational outcomes to standard output.
 * Compile:
 * clang ./src/darwin/pattern-matching/rabin-karp/main.c ./src/darwin/pattern-matching/rabin-karp/rabin_karp.c -I./include -o ./bin/rabin_karp_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/pattern-matching/rabin_karp.h"

int main(void)
{
    // Define a source and phrase.
    const char* source = "ABCCDABRAACADABRABAFABRA";
    const char* phrase = "ABRA";

    // Show the source and the phrase.
    printf("%s %s%c", "Text:", source, '\n');
    printf("%s %s%c", "Pattern:", phrase, '\n');

    // Perform rabin karp algorithm.
    bool success = rabin_karp(source, phrase);

    if (success)
    {
        printf("%s", "Pattern found.\n");
    }
    else
    {
        printf("%s", "Not found.\n");
    }
    
    return 0;
}