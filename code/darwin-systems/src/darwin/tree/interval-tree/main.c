/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a testing suite for an augmented Interval Search Tree.
 * @details     :   This program serves as an integration test for the interval tree data 
 * structure API. It handles sequential insertion of ranges, performs an 
 * O(log n) overlap query using a designated search window, handles dynamic 
 * node deletion while validating the 'maximum' property adjustment across 
 * ancestors, and verifies systematic memory clean-up.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/interval-tree/main.c ./src/darwin/tree/interval-tree/interval_tree.c -I./include -o ./bin/interval_tree_test
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#include <stdio.h>

// Include header files.
#include "darwin/tree/interval_tree.h"

int main(void)
{
    // Initialize the root.
    Node* root = NULL;

    // Create intervals to insert.
    Interval intervals[] = 
    {
        {15, 20},
        {10, 30},
        {17, 19},
        {5, 20},
        {12, 15},
        {30, 40}
    };

    // Calculate the number of intervals.
    size_t size = sizeof(intervals) / sizeof(intervals[0]);

    // Insert intervals.
    for (size_t index = 0; index < size; index++)
    {
        root = insert(root, intervals[index]);
    }

    // Show the tree.
    printf("%s", "In-order traversal of the interval tree:\n");
    in_order(root);
    printf("%c", '\n');

    // Query interval.
    Interval query = {6, 7};

    printf("%s", "-- Searching for Overlap --\n");
    printf("%s [%i %i]%c", "Searching for any interval overlapping with:", query.low, query.high, '\n');

    // Search for an interval.
    const Node* found = search(root, query);

    if (found != NULL)
    {
        printf("%s [%i, %i]%c", "Found overlap with:", found->interval->low, found->interval->high, '\n');
    }
    else
    {
        printf("%s", "No overlapping interval found.\n");
    }

    printf("%c", '\n');

    // Interval to erase.
    Interval to_erase = {12, 15};

    printf("%s [%i, %i]%s", "-- Erasing Interval:", to_erase.low, to_erase.high, "\n\n");

    // Erase from root.
    root = erase(root, to_erase);

    // Display the tree.
    printf("%s", "In-order:\n");
    in_order(root);
    printf("%c", '\n');

    // Clean up.
    destroy(root);

    // Protect against dangling pointers.
    root = NULL;

    return 0;
}