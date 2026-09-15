/**
 * @file        :   binary_lifting.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation file for a tree structure utilizing Binary Lifting.
 * Provides rapid tree traversal tools, mapping sparse tables to compute the 
 * k-th ancestor and find the Lowest Common Ancestor (LCA) of nodes in 
 * logarithmic O(log N) runtime.
 * @version     :   1.0
 * @date        :   2026-05-23
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/binary_lifting.h"

Node* adjacency_list[MAXIMUM_NODE_COUNT] = {0};

int ancestor_table[MAXIMUM_NODE_COUNT][MAXIMUM_LOG_STEPS] = {{0}};

size_t node_depth[MAXIMUM_NODE_COUNT] = {0};

bool visited[MAXIMUM_NODE_COUNT] = {false};

/**
 * @brief       :   Initializes global data arrays and the binary lifting lookup table.
 * @details     :   Wipes depth records, resets node visitation flags, and populates 
 *                  the sparse ancestor table with missing sentinel identifier constants.
 * @param       :   number_of_nodes: The count of structural nodes being managed.
 * @return      :   void
 */
void initialize(size_t number_of_nodes)
{
    for (size_t index = 0; index < MAXIMUM_NODE_COUNT; index++)
    {
        node_depth[index] = 0;
        visited[index] = false;

        for (size_t step_index = 0; step_index < MAXIMUM_LOG_STEPS; step_index++)
        {
            ancestor_table[index][step_index] = INVALID_NODE_ID;
        }
    }
}

/**
 * @brief       :   Safely adds a directed edge to the tree's adjacency list.
 * @details     :   Allocates an edge node on the heap and adjusts pointers defensively.
 * @param       :   source: Node ID originating the link.
 * @param       :   destination: Target node ID being linked to.
 * @return      :   bool: True if allocation and bounds check succeeded, false otherwise.
 */
bool add(int source, int destination)
{
    if (source < 0 || source >= MAXIMUM_NODE_COUNT || destination < 0 || destination >= MAXIMUM_NODE_COUNT)
    {
        return false;
    }

    Node* edge = (Node*)malloc(sizeof(Node));

    if (edge == NULL)
    {
        return false;
    }

    edge->destination = destination;
    edge->next = adjacency_list[source];

    adjacency_list[source] = edge;

    return true;
}

/**
 * @brief       :   Precomputes node depths and populates the binary lifting table.
 * @details     :   Uses a depth-first traversal to fill the DP exponential steps layout.
 * @param       :   current: The node ID currently being evaluated.
 * @param       :   parent: The immediate ancestor node ID of the current node.
 * @param       :   depth: Current scalar distance away from the tree root.
 * @return      :   void
 */
void compute_depths_and_ancestors(int current, int parent, size_t depth)
{
    if (current < 0 || current >= MAXIMUM_NODE_COUNT)
    {
        return;
    }

    if (visited[current])
    {
        return;
    }

    visited[current] = true;

    node_depth[current] = depth;
    ancestor_table[current][0] = parent;

    for (size_t bit_position = 1; bit_position < MAXIMUM_LOG_STEPS; bit_position++)
    {
        int middle = ancestor_table[current][bit_position - 1];

        if (middle != INVALID_NODE_ID && middle >= 0 && middle < MAXIMUM_NODE_COUNT)
        {
            ancestor_table[current][bit_position] = ancestor_table[middle][bit_position - 1];
        }
        else
        {
            ancestor_table[current][bit_position] = INVALID_NODE_ID;
        }
    }

    Node* current_edge = adjacency_list[current];

    while (current_edge != NULL)
    {
        int child = current_edge->destination;

        if (child != parent)
        {
            compute_depths_and_ancestors(child, current, depth + 1);
        }

        current_edge = current_edge->next;
    }
}

/**
 * @brief       :   Finds the k-th (ancestor) ancestor of a given node.
 * @details     :   Decodes the bit positions of the ancestor scalar to make logarithmic jumps.
 * @param       :   current: Starting node ID.
 * @param       :   ancestor: The target distance (k) to lift up the tree structure.
 * @return      :   int: The node ID of the k-th ancestor, or INVALID_NODE_ID if out of bounds.
 */
int kth_ancestor(int current, size_t ancestor)
{
    if (current < 0 || current >= MAXIMUM_NODE_COUNT)
    {
        return INVALID_NODE_ID;
    }

    for (size_t bit_position = 0; bit_position < MAXIMUM_LOG_STEPS; bit_position++)
    {
        if ((ancestor >> bit_position) & 1)
        {
            current = ancestor_table[current][bit_position];

            if (current == INVALID_NODE_ID || current < 0 || current >= MAXIMUM_NODE_COUNT)
            {
                return INVALID_NODE_ID;
            }
        }
    }

    return current;
}

/**
 * @brief       :   Identifies the Lowest Common Ancestor (LCA) of two nodes.
 * @details     :   Equalizes node depth differences, then lifts nodes simultaneously.
 * @param       :   source: First query node ID.
 * @param       :   destination: Second query node ID.
 * @return      :   int: The shared node ID representing the LCA, or INVALID_NODE_ID.
 */
int lowest_common_ancestor(int source, int destination)
{
    if (source < 0 || source >= MAXIMUM_NODE_COUNT || destination < 0 || destination >= MAXIMUM_NODE_COUNT)
    {
        return INVALID_NODE_ID;
    }

    if (node_depth[source] < node_depth[destination])
    {
        int temp = source;
        source = destination;
        destination = temp;
    }

    size_t depth_difference = node_depth[source] - node_depth[destination];

    source = kth_ancestor(source, depth_difference);

    if (source == destination)
    {
        return source;
    }

    for (long long bit_position = MAXIMUM_LOG_STEPS - 1; bit_position >= 0; bit_position--)
    {
        int source_ancestor = ancestor_table[source][bit_position];
        int destination_ancestor = ancestor_table[destination][bit_position];

        if (source_ancestor != destination_ancestor)
        {
            source = source_ancestor;
            destination = destination_ancestor;
        }
    }

    if (source < 0 || source >= MAXIMUM_NODE_COUNT)
    {
        return INVALID_NODE_ID;
    }

    return ancestor_table[source][0];
}

void clean(size_t number_of_nodes)
{
    for (size_t id = 1; id <= number_of_nodes; id++)
    {
        Node* current = adjacency_list[id];

        while (current != NULL)
        {
            Node* edge_to_free = current;
            current = current->next;

            free(edge_to_free);
        }

        adjacency_list[id] = NULL;
    }
}