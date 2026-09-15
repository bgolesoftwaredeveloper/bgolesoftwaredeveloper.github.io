/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Execution entry point to test Dijkstra's shortest path algorithm.
 * @details     :   Instantiates a 9-vertex weighted graph using a flattened row-major 
 * adjacency matrix, dynamically allocates context tracking state arrays, 
 * and executes the safe pathfinding algorithm from a source vertex.
 * clang -Wall -Wextra -Wpedantic ./src/darwin/graph/dijkstra/main.c ./src/darwin/graph/dijkstra/dijkstra.c -I./include -o ./bin/dijkstra_test
 * @version     :   1.0
 * @date        :   2026-05-24
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/graph/dijkstra.h"

int main(void)
{
     /*
     * Define the graph structure using a flattened 1D array to represent a 2D 
     * row-major adjacency matrix. A value of 0 indicates no direct edge between 
     * vertices, while any positive integer represents the weight/cost of the path.
     */
    int adjacency_matrix[NUMBER_OF_VERTICES * NUMBER_OF_VERTICES] =
    {
        0,  4,  0,  0,  0,  0,  0,  8,  0,
        4,  0,  8,  0,  0,  0,  0, 11,  0,
        0,  8,  0,  7,  0,  4,  0,  0,  2,
        0,  0,  7,  0,  9, 14,  0,  0,  0,
        0,  0,  0,  9,  0, 10,  0,  0,  0,
        0,  0,  4, 14, 10,  0,  2,  0,  0,
        0,  0,  0,  0,  0,  2,  0,  1,  6,
        8, 11,  0,  0,  0,  0,  1,  0,  7,
        0,  0,  2,  0,  0,  0,  6,  7,  0   
    };

    // Allocate dynamic heap memory to store the calculated shortest paths from source.
    int* shortest_distances = (int*)malloc(sizeof(int) * NUMBER_OF_VERTICES);

    if (shortest_distances == NULL)
    {
        return 1;
    }

    // Allocate dynamic heap memory for tracking visited/processed vertices.
    bool* visited = (bool*)malloc(sizeof(bool) * NUMBER_OF_VERTICES);

    if (visited == NULL)
    {
        return 1;
    }

    // Wrap all data arrays and structural bounds into a single execution context object.
    Graph context =
    {
        .adjacency_matrix = adjacency_matrix,
        .total_vertices = NUMBER_OF_VERTICES,
        .shortest_distances = shortest_distances,
        .visited = visited
    };

    // Set the starting point node for the algorithm.
    size_t source = 0;

    // Execute Dijkstra pathfinding optimization algorithm.
    bool success = dijkstra(&context, source);

    // Process algorithm results based on execution status.
    if (success)
    {
        printf("%s", "Dijkstra optimization completed successfully!\n\n");

        print_shortest_paths(&context);
    }
    else
    {
        fprintf(stderr, "Dijkstra execution failed. Check your graph inpnut parameters.\n");
    }

    // Release memory.
    free(shortest_distances);
    free(visited);

    return 0;
}