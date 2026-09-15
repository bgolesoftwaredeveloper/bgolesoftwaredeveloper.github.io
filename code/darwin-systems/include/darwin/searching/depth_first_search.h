/**
 * @file        :   depth_first_search.h
 * @author      :   Braiden Gole
 * @brief       :   Function prototypes and data structures for Depth-First Search.
 * @details     :   Provides the necessary structures to represent a graph via 
 * adjacency lists and the interface for performing depth-first
 * traversals on undirected graphs.
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#ifndef DEPTH_FIRST_SEARCH_H
#define DEPTH_FIRST_SEARCH_H

#include <stdbool.h>

/**
 * @struct Node
 * @brief  A node in the adjacency list representing a connected vertex.
 */
struct Node
{
    int vertex;
    struct Node* next;
};

/**
 * @struct Graph
 * @brief  Adjacency list representation of an undirected graph.
 */
struct Graph
{
    int number_of_vertices;
    struct Node** adjacency_list;
    bool* visited;
};

// Function prototypes.
struct Graph* initialize_graph(int vertices);
void add_edge(struct Graph* graph, int source, int destination);
void depth_first_search(struct Graph* graph, int start);
void destroy_graph(struct Graph* graph);

#endif