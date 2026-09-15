/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Main entry point to demonstrate Prim's Minimum Spanning Tree (MST) algorithm.
 * @details     :      Initializes a weighted, connected, undirected graph represented as an adjacency 
 * matrix and executes Prim's algorithm to find and display its Minimum Spanning Tree.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/prim/main.c ./src/darwin/graph/prim/prim.c -I./include -o ./bin/prim_test
 * @version     :   1.0
 * @date        :   2026-05-28
 */
#include <stdio.h>

// Include header files.
#include "darwin/graph/prim.h"

int main(void)
{
    // Define a graph to work with.
    int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES] =
    {
        {0, 2, 0, 6, 0},
        {2, 0, 3, 8, 5},
        {0, 3, 0, 0, 7},
        {6, 8, 0, 0, 9},
        {0, 5, 7, 9, 0}
    };

    // Execute the algorithm.
    prim(graph);

    return 0;
}