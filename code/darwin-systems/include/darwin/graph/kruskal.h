/**
 * @file        :   kruskal.h
 * @author      :   Braiden Gole
 * @brief       :   Header definitions for Kruskal's Minimum Spanning Tree algorithm.
 * @details     :   Declares data structures for tracking edges, graphs, and union-find 
 * subsets alongside interface prototypes required to initialize and resolve a graph.
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#ifndef KRUSKAL_H
#define KRUSKAL_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @struct      :   Edge
 * @brief       :   Represents a single weighted connection between two vertices.
 * @details     :   Tracks the directed or undirected path indices along with a signed cost weight.
 */
typedef struct
{
    size_t source;
    size_t destination;
    int weight;
} Edge;

/**
 * @struct      :   Graph
 * @brief       :   Tracks an entire network layout.
 * @details     :   Maintains tracking bounds for vertices and dynamically allocated edge collections.
 */
typedef struct
{
    size_t number_of_vertices;
    size_t number_of_edges;
    Edge* edges;
} Graph;

/**
 * @struct      :   Subset
 * @brief       :   Tracks disjoint set element attributes for union-find tracking loops.
 * @details     :   Enables flat path compression structures and optimized tree balancing using rankings.
 */
typedef struct
{
    size_t parent;
    size_t rank;
} Subset;

// Function prototypes.
Graph* initialize(size_t number_of_vertices, size_t number_of_edges);
void kruskal(const Graph* graph);

#endif