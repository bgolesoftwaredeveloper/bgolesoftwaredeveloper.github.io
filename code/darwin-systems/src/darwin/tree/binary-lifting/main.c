/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Braiden Gole
 * @brief       :   Test runner file executing queries on a Binary Lifting tree structure.
 * Demonstrates tree initialization, undirected edge mapping, DP sparse table building,
 * and logarithmic queries for k-th ancestors and Lowest Common Ancestors (LCA).
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/binary-lifting/main.c ./src/darwin/tree/binary-lifting/binary_lifting.c -I./include -o ./bin/binary_lifting_test
 * @version     :   1.0
 * @date        :   2026-05-23
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/binary_lifting.h"

int main(void)
{
    size_t number_of_nodes = 7;

    // Wipe a reset tracking state arrays across all potential node allocations.
    initialize(number_of_nodes);

    bool success = true;

    // Add nodes to work with.
    success &= add(1, 2);
    success &= add(2, 1);
    success &= add(1, 3);
    success &= add(3, 1);
    success &= add(2, 4);
    success &= add(4, 2);
    success &= add(2, 5);
    success &= add(5, 2);
    success &= add(3, 6);
    success &= add(6, 3);
    success &= add(5, 7);
    success &= add(7, 5);

    if (!success)
    {
        fprintf(stderr, "There was an error adding edges.\n");

        return 1;
    }

    // Compute deoths and ancestors.
    compute_depths_and_ancestors(1, INVALID_NODE_ID, 0);

    int ancestor_result = kth_ancestor(7, 2);
    int result_four_seven = lowest_common_ancestor(4, 7);
    int result_four_six = lowest_common_ancestor(4, 6);

    // Summary.
    printf("%s %i%c", "2nd ancestor of node 7:", ancestor_result, '\n');
    printf("%s %i%c", "LCA of 4 and 7:", result_four_seven, '\n');
    printf("%s %i%c", "LCA of 4 and 6:", result_four_six, '\n');

    // Clean up.
    clean(number_of_nodes);

    return 0;
}