/**
 * @file        :   breadth_first_search.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the Breadth-First Search (BFS) implementation.
 * @details     :   Defines the graph structures and function prototypes for 
 * performing level-order traversal on a graph.
 * @version     :   1.1
 * @date        :   2026-04-19
 */
#ifndef BREADTH_FIRST_SEARCH_H
#define BREADTH_FIRST_SEARCH_H

#include <stdbool.h>

#define MAXIMUM_VERTICES 100

/**
 * @struct      :   Node
 * @brief       :   Represents a single node in an adjacency list.
 * @details     :   Contains the vertex index and a pointer to the next node, 
 *                  forming a linked list for graph representation.
 */
struct Node
{
    int vertex;
    struct Node* next;
};

/**
 * @struct      :   Graph
 * @brief       :   Represents an undirected graph using adjacency lists.
 * @details     :   Contains the vertex count, an array of adjacency list pointers, 
 *                  and a boolean array to track visited status during traversal.
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
void breadth_first_search(struct Graph* graph, int start);
void destroy_graph(struct Graph* graph);

#endif