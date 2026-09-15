/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Execution entry point to test the Floyd-Warshall algorithm.
 * @details     :   This program constructs a directed, weighted graph represented 
 * as an adjacency matrix and executes the Floyd-Warshall algorithm 
 * to calculate the all-pairs shortest paths while checking for 
 * negative weight cycles.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/floyd-warshall/main.c ./src/darwin/graph/floyd-warshall/floyd_warshall.c -I./include -o ./bin/floyd_warshall_test
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#include <stdio.h>

// Include header files.
#include "darwin/graph/floyd_warshall.h"

int main(void)
{
    // Initialize a graph to work with.
    int graph[VERTICES][VERTICES] =
    {
        {0, 5, INFINITE, 10},
        {INFINITE, 0, 3, INFINITE},
        {INFINITE, INFINITE, 0, 1},
        {INFINITE, INFINITE, INFINITE, 0}
    };

    // Execute the Floyd-Warshall algorithm on the graph.
    bool success = floyd_warshall(graph);

    if (!success)
    {
        fprintf(stderr, "Graph contains a negative weight cycle!");

        return 1;
    }

    return 0;
}