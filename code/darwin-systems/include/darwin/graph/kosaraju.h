/**
 * @file        :   kosaraju.h
 * @author      :   Braiden Gole
 * @brief       :   Header definition for Kosaraju's SCC algorithm.
 * @details     :   Provides the data structures and function prototypes 
 * necessary for representing a directed graph using an 
 * adjacency list and identifying its Strongly Connected 
 * Components (SCCs).
 * @version     :   1.0
 * @date        :   2026-05-08
 */
#ifndef KOSARAJU_H
#define KOSARAJU_H

#include <stddef.h>

/**
 * @struct      :   Node
 * @brief       :   A single element in the adjacency list.
 * @var id      :   The unique identifier (index) of the destination vertex.
 * @var next    :   Pointer to the next neighbor in the linked list.
 */
typedef struct Node
{
    size_t id;
    struct Node* next;
} Node;

/**
 * @struct      :   Graph
 * @brief       :   Representation of a directed graph.
 * @var total_vertices : The number of nodes in the graph.
 * @var adjacency_list : Array of pointers, where each pointer is the 
 * head of a linked list of neighboring nodes.
 */
typedef struct
{
    size_t total_vertices;
    Node** adjacency_list;
} Graph;

// Function prototypes.
Graph* initialize_graph(size_t vertices);
void add_edge(Graph* graph, size_t source, size_t destination);
void destroy_graph(Graph* graph);

// Strongly Connected Components.
Graph* generate_transposed(Graph* graph);
void strongly_connected_components(Graph* graph);

#endif