/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a testing suite for a Link-Cut Tree.
 * @details     :   This program serves as an integration test for the dynamic Link-Cut 
 * Tree data structure API. It handles sequential node instantiation inside a 
 * loop, links individual structures to compose a consolidated path topology, 
 * performs an amortized path aggregate query validating maximum property parsing, 
 * alters system orientation via absolute root assignments, and handles comprehensive 
 * memory cleanup.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/link-cut-tree/main.c ./src/darwin/tree/link-cut-tree/link_cut_tree.c -I./include -o ./bin/link_cut_tree_test
 * @version     :   1.0
 * @date        :   2026-05-22
 */
#include <stdio.h>

// Include header files.
#include "darwin/tree/link_cut_tree.h"

enum
{
    NUMBER_OF_NODES = 5
};

int main(void)
{
    Node* nodes[NUMBER_OF_NODES + 1] = {NULL};

    int values[NUMBER_OF_NODES + 1] = {0, 10, 20, 50, 30, 40};

    // Construct nodes to work with.
    for (size_t index = 1; index <= NUMBER_OF_NODES; index++)
    {
        nodes[index] = construct(index, values[index]);

        if (nodes[index] == NULL)
        {
            fprintf(stderr, "Error: Out of memory.\n");

            return 1;
        }
    }

    // Link the nodes.
    for (size_t index = 1; index < NUMBER_OF_NODES; index++)
    {
        link(nodes[index], nodes[index + 1]);
    }

    // Assign a new root.
    assign_root(nodes[5]);

    bool flag = false;

    // Query.
    int maximum_path_value = query(nodes[1], nodes[5], &flag);

    if (flag)
    {
        printf("%s %i%c", "Maximum value:", maximum_path_value, '\n');
    }

    Node* current = find(nodes[5]);

    // Is found?
    (current != NULL) ? printf("%s %zu%c", "Query:", current->id, '\n') : printf("%c", '\n');

    // Clean up.
    clean(current);
    
    return 0;
}