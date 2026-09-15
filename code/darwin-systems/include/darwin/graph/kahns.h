/**
 * @file        :   kahns.h
 * @author      :   Braiden Gole
 * @brief       :   Declarations, data structures, and API for Kahn's topological sort.
 * @details     :   Defines the core structural elements for a directed graph 
 * represented via adjacency lists along with in-degree lookup tracking.
 * Provides memory-safe prototyping interfaces for graph operations.
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#ifndef KAHNS_H
#define KAHNS_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @struct Node
 * @brief  A singly-linked list node representing an edge destination.
 */
typedef struct Node
{
    size_t destination;
    struct Node* next;
} Node;

/**
 * @struct Graph
 * @brief  Container managing adjacency lists and dependency tracking arrays.
 */
typedef struct
{
    size_t number_of_vertices;
    size_t* in_degree;
    Node** adjacency_lists;
} Graph;

// Function prototypes.
Graph* initialize(size_t vertices);
bool add_edge(Graph* graph, size_t source, size_t destination);
bool kahns(const Graph* graph);
void clean(Graph* graph);

#endif