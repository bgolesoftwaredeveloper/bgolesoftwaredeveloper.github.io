/**
 * @file        :   kruskal.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of Kruskal's Minimum Spanning Tree algorithm.
 * @details     :   Provides graph initialization utilities and Kruskal's MST solver.
 * Utilizes Disjoint Set Union (DSU) with path compression and 
 * union by rank to optimize edge tracking and cycle detection.
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/graph/kruskal.h"

/**
 * @brief       :   Compares two graph edges based on their internal weights.
 * @details     :   Acts as a standard callback comparison function for qsort.
 * @param       :   source A pointer to the first edge structure.
 * @param       :   destination A pointer to the second edge structure.
 * @return      :   A negative integer if the source edge weight is smaller, 
 *                  a positive integer if greater, or zero if they match.
 */
static inline int compare(const void* source, const void* destination)
{
    // Cast input parameters back to explicit Edge structures.
    const Edge* source_edge = (const Edge*)source;
    const Edge* destination_edge = (const Edge*)destination;

    // Return the difference delta between the weights.
    return (source_edge->weight - destination_edge->weight);
}

/**
 * @brief       :   Finds the absolute root representative of a specific index.
 * @details     :   Employs path compression to flat-map parent nodes straight to the root.
 * @param       :   subsets The disjoint set structures tracking graph hierarchies.
 * @param       :   index The specific target node to search for.
 * @return      :   The size_t identifier representing the true root of the node.
 */
static inline size_t find(Subset subsets[], size_t index)
{
    // Recursively walk up to find the root parent.
    if (subsets[index].parent != index)
    {
        // Flatten hierachies using dynamic path compression.
        subsets[index].parent = find(subsets, subsets[index].parent);
    }

    return subsets[index].parent;
}

/**
 * @brief       :   Unites two distinct disjoint tree subsets together.
 * @details     :   Employs union by rank optimization to attach lower rank structures under high rank structures.
 * @param       :   subsets The disjoint set structures tracking graph hierarchies.
 * @param       :   x The first target vertex identifier.
 * @param       :   y The second target vertex identifier.
 * @return      :   True if the subsets were merged successfully, or false if they share a root.
 */
static inline bool union_sets(Subset subsets[], size_t x, size_t y)
{
    // Determine the roots of each respective subset.
    size_t x_root = find(subsets, x);
    size_t y_root = find(subsets, y);

    // If the roots are identical, a cycle is detected.
    if (x_root == y_root)
    {
        return false;
    }

    // Attach smaller tree under root of high rank tree.
    if (subsets[x_root].rank < subsets[y_root].rank)
    {
        subsets[x_root].parent = y_root;
    }
    else if (subsets[x_root].rank > subsets[y_root].rank)
    {
        subsets[y_root].parent = x_root;
    }
    else
    {
        // If ranks match, choose arbitrarily and advance the target rank boundary.
        subsets[y_root].parent = x_root;
        subsets[x_root].rank++;
    }

    return true;
}

/**
 * @brief       :   Initializes a heap-allocated Graph structure.
 * @details     :   Verifies sizing inputs and safely handles allocations for internal fields.
 * @param       :   number_of_vertices Total vertices to track within the structure.
 * @param       :   number_of_edges Total capacity limits of the inner edge tracker.
 * @return      :   A pointer to the initialized Graph structure, or NULL upon failure.
 */
Graph* initialize(size_t number_of_vertices, size_t number_of_edges)
{
    // Enforce defensive validation on input bounds parameters.
    if (number_of_vertices == 0 || number_of_edges == 0)
    {
        return NULL;
    }

    // Allocate memory for the overeaching graph metadata block.
    Graph* graph = (Graph*)malloc(sizeof(Graph));

    if (graph == NULL)
    {
        return NULL;
    }

    // Map internal limits parameters over.
    graph->number_of_vertices = number_of_vertices;
    graph->number_of_edges = number_of_edges;

    // Allocate the underlying contiguous array for explicit edge data tracking.
    graph->edges = (Edge*)malloc(sizeof(Edge) * number_of_edges);

    if (graph->edges == NULL)
    {
        free(graph);

        return NULL;
    }

    return graph;
}

/**
 * @brief       :   Computes and displays the Minimum Spanning Tree (MST) using Kruskal's algorithm.
 * @details     :   Duplicates and sorts graph edges by weight, then processes them sequentially 
 *                  using disjoint-set tracking to assemble an acyclic spanning tree layout. 
 *                  Safely handles out-of-bounds corruption and handles disconnected graph warnings.
 * @param       :   graph A pointer to the immutable input Graph structure wrapper.
 * @return      :   void
 */
void kruskal(const Graph* graph)
{
    // Halt operations immediately if the incoming data structure pointers are invalid.
    if (graph == NULL || graph->edges == NULL)
    {
        return;
    }

    size_t number_of_vertices = graph->number_of_vertices;
    size_t number_of_edges = graph->number_of_edges;

    // Allocate a localized buffer array to preserve the incoming ocnst properties.
    Edge* local_edges = (Edge*)malloc(sizeof(Edge) * number_of_edges);

    if (local_edges == NULL)
    {
        return;
    }

    // Duplicate the input properties to safely process elements.
    for (size_t index = 0; index < number_of_edges; index++)
    {
        local_edges[index] = graph->edges[index];
    }

    // Sort the duplicate edge list by weight using the standard qsort function.
    qsort(local_edges, number_of_edges, sizeof(Edge), compare);

    // Allocate the necessary elements for tracking vertex set groupings.
    Subset* subsets = (Subset*)malloc(sizeof(Subset) * number_of_vertices);

    if (subsets == NULL)
    {
        free(local_edges);

        return;
    }

    // Initialize individual isolated sets for each active vertex.
    for (size_t index = 0; index < number_of_vertices; index++)
    {
        subsets[index].parent = index;
        subsets[index].rank = 0;
    }

    size_t edge_count = 0;
    size_t edge_index = 0;

    // Allocate the output results cache tracking spanning elements.
    Edge* result = (Edge*)malloc(sizeof(Edge) * number_of_edges);

    if (result == NULL)
    {
        free(subsets);
        free(local_edges);

        return;
    }

    // Core loop continues until number_of_vertices - 1 valid tree edge are found or all edges are explored.
    while (edge_count < number_of_vertices - 1 && edge_index < number_of_edges)
    {
        // Pull the next cheapest edge in linear sequence.
        Edge next = local_edges[edge_index++];

        // Ignore corrupted items pointing past know layout dimensions.
        if (next.source >= number_of_vertices || next.destination >= number_of_vertices)
        {
            continue;
        }

        // Include the target item ifit links separate groupings securely without cyclces.
        if (union_sets(subsets, next.source, next.destination))
        {
            result[edge_count++] = next;
        }
    }

    // Flag an explicit warning notice if the graph happens to be fragmented.
    if (edge_count < number_of_vertices - 1)
    {
        printf("%s", "The graph is disconnected.\n");
    }

    printf("%s", "Edges:\n");

    long long minimum_cost = 0;

    // Output the structural metadata collection layout to stdout.
    for (size_t index = 0; index < edge_count; index++)
    {
        printf("%zu -- %zi == %i%c", result[index].source, result[index].destination, result[index].weight, '\n');

        minimum_cost += result[index].weight;
    }

    printf("%c", '\n');
    printf("%s %lld%c", "Minimum cost:", minimum_cost, '\n');

    // Free dynamic memory.
    free(subsets);
    free(local_edges);
    free(result);
}