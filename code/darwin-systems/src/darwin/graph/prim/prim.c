/**
 * @file        :   prim.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of Prim's Minimum Spanning Tree (MST) algorithm.
 * @details     :   Contains the core logic for selecting minimum weight edges to construct 
 * a spanning tree of an undirected, weighted graph from an adjacency matrix.
 * @version     :   1.0
 * @date        :   2026-05-28
 */
#include <stdio.h>

// Include header files.
#include "darwin/graph/prim.h"

/**
 * @brief Finds the vertex with the minimum key value from the set of vertices not yet included in the MST.
 * @param key Array containing the minimum edge weights connecting each vertex to the MST.
 * @param set Boolean array tracking whether a vertex is included in the MST.
 * @return The index of the vertex with the minimum key value, or (size_t)-1 if pointers are invalid.
 */
static inline size_t minimum_key(const int key[], const bool set[])
{
    // Guard against NULL context evaluation.
    if (key == NULL || set == NULL)
    {
        return (size_t)-1;
    }

    int minimum = INT_MAX;

    size_t minimum_index = (size_t)-1;

    // Iterate through all vertices to locate the one with the smallest weight outside the current minimum spanning tree.
    for (size_t index = 0; index < NUMBER_OF_VERTICES; index++)
    {
        if (set[index] == false && key[index] < minimum)
        {
            minimum = key[index];
            minimum_index = index;
        }
    }

    return minimum_index;
}

/**
 * @brief Prints the constructed Minimum Spanning Tree to the standard output.
 * @param parent Array storing the parent vertex for each node in the MST.
 * @param graph Adjacency matrix representing the edge weights of the graph.
 */
static inline void print(const size_t parent[], const int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES])
{
    // Guard against printing a noo-existent or unallocated context.
    if (parent == NULL || graph == NULL)
    {
        return;
    }

    printf("%s", "Edge:\n");

    // Skip root node index 0 and print the parent-child linkages forming the tree.
    for (size_t index = 1; index < NUMBER_OF_VERTICES; index++)
    {
        if (parent[index] < NUMBER_OF_VERTICES)
        {
            printf("%zu - %zu%c%i%c", parent[index], index, '\t', graph[index][parent[index]], '\n');
        }
    }
}

/**
 * @brief Computes and prints the Minimum Spanning Tree (MST) of a graph using Prim's algorithm.
 * @param graph Adjacency matrix representing the edge weights of the graph.
 */
void prim(const int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES])
{
    // Guard against processing a non-existent context matrix.
    if (graph == NULL)
    {
        return;
    }

    // Allocate stack tracking space for parents, minimum edge weights, and MST inclusionn status.
    size_t parent[NUMBER_OF_VERTICES];

    int key[NUMBER_OF_VERTICES];

    bool set[NUMBER_OF_VERTICES];

    // Phase 1: Initialize all tracking buffers to default unvisited status.
    for (size_t index = 0; index < NUMBER_OF_VERTICES; index++)
    {
        key[index] = INT_MAX;
        set[index] = false;
        parent[index] = (size_t)-1;
    }

    // Seed the starting vertex context to kick off the evaluation loop.
    key[0] = 0;
    parent[0] = (size_t)-1;

    // Phase 2: Core processing evaluation loop.
    for (size_t count = 0; count < NUMBER_OF_VERTICES; count++)
    {
        // Extract the next closest, independent vertex out of the remaining set pool.
        size_t minimum = minimum_key(key, set);

        // Verify index fit bounds or if a disonnnected graph component halts the execution time. 
        if (minimum == (size_t)-1 || key[minimum] == INT_MAX)
        {
            return;
        }

        // Record the extracted vertex into the stable MST path context.
        set[minimum] = true;

        // Traverse all target adjacent connections of the extracted node to update tracking metrics.
        for (size_t index = 0; index < NUMBER_OF_VERTICES; index++)
        {
            // Update the key and parent index only if an edge exists, the node is unvisited, and its weight is smaller than current tracking context.
            if (graph[minimum][index] > 0 && set[index] == false && graph[minimum][index] < key[index])
            {
                parent[index] = minimum;
                key[index] = graph[minimum][index];
            }
        }
    }

    // Phase 3: Display the final edge structure path results.
    print(parent, graph);
}