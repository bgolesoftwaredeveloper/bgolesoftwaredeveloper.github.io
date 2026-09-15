/**
 * @file        :   binary_lifting.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for a tree structure utilizing Binary Lifting.
 * Declares data structures, sparse tables, and lookup signatures to compute 
 * k-th ancestors and Lowest Common Ancestors (LCA) in O(log N) time.
 * @version     :   1.0
 * @date        :   2026-05-23
 */
#ifndef BINARY_LIFTING_H
#define BINARY_LIFTING_H

#include <stdbool.h>

#define MAXIMUM_NODE_COUNT 100005
#define MAXIMUM_LOG_STEPS 18
#define INVALID_NODE_ID -1

/**
 * @struct      :   Node
 * @brief       :   Represents an edge element in the tree graph structure.
 * @details     :   Constructs a single-linked list node used inside an adjacency list.
 */
typedef struct Node
{
    int destination;
    struct Node* next;
} Node;

// Global tracking structures.
struct Node* adjacency_list[MAXIMUM_NODE_COUNT];

int ancestor_table[MAXIMUM_NODE_COUNT][MAXIMUM_LOG_STEPS];

size_t node_depth[MAXIMUM_NODE_COUNT];

bool visited[MAXIMUM_NODE_COUNT];

// Function prototypes.
void initialize(size_t number_of_nodes);
bool add(int source, int destination);
void compute_depths_and_ancestors(int current, int parent, size_t depth);
int kth_ancestor(int current, size_t ancestor);
int lowest_common_ancestor(int source, int destination);
void clean(size_t number_of_nodes);

#endif