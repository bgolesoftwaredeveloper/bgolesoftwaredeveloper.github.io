/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstrates pattern matching across a Rope string data structure.
 * @details     :   Constructs a sample balanced/unbalanced binary Rope tree representing 
 * the string "Darwin_is_awesome" using disjoint leaf nodes. It then validates 
 * the defensive boundary-crossing capabilities of the pattern matching algorithm 
 * by searching for a substring that spans across separate tree leaves.
 * Compile:
 * clang ./src/darwin/pattern-matching/rope/main.c ./src/darwin/pattern-matching/rope/rope.c -I./include -o ./bin/rope_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/pattern-matching/rope.h"

int main(void)
{
    // Initialize the base leaf nodes holding the actual string pieces.
    Node suffix = { .left = NULL, .right = NULL, .weight = 7, .leaf = "awesome" };
    Node infix = { .left = NULL, .right = NULL, .weight = 4, .leaf = "_is_" };

    // From internal join nodes to link teh pieces together.
    Node join = 
    {
        .left = &infix,
        .right = &suffix,
        .weight = 4,
        .leaf = NULL
    };

    // Construct the prefix leaf and connect it to the rest of the tree.
    Node prefix = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };
    Node root_join =
    {
        .left = &prefix,
        .right = &join,
        .weight = 6,
        .leaf = NULL
    };

    // Define a target substring pattern.
    const char* boundary_search_pattern = "win_is";

    // Buffer to safely receive the exact starting index of the matched substring.
    size_t output_index = 0;

    // Execute the defensive pattern-matching routine.
    if (rope(&root_join, boundary_search_pattern, &output_index))
    {
        printf("%s", "[Success].\n");
    }
    else
    {
        printf("%s", "[Failure].\n");
    }

    return 0;
}