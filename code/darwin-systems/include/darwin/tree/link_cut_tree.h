/**
 * @file        :   link_cut_tree.h
 * @author      :   Braiden Gole
 * @brief       :   Interface definitions for an augmented Link-Cut Tree.
 * @details     :   This header defines the core structures and API prototypes for a 
 * pointer-based Link-Cut Tree forest. It supports dynamic topological alterations 
 * (linking and cutting edges) alongside path aggregate evaluation queries (tracking 
 * path maximums) in amortized O(log n) time per operation utilizing localized 
 * auxiliary Splay Trees.
 * @version     :   1.0
 * @date        :   2026-05-22
 */
#ifndef LINK_CUT_TREE_H
#define LINK_CUT_TREE_H

#include <stddef.h>
#include <stdbool.h>

#define MAXIMUM(compare, against) ((compare) > (against) ? (compare) : (against))

/**
 * @struct      :   Node
 * @brief       :   Represents an individual node within the Link-Cut Tree forest.
 * @details     :   The Link-Cut Tree partitions a represented virtual forest into a 
 * set of disjoint paths. Each preferred path is structurally managed internally 
 * by an auxiliary Splay Tree. In this representation:
 * - Left and right child pointers track structural proximity within a specific path.
 * - The parent pointer can mean two different things: it points to either a local 
 * ancestor within the same auxiliary Splay Tree, or to the path's parent node in 
 * the represented tree via a dashed "path pointer" edge.
 */
typedef struct Node
{
    size_t id;

    int value;
    int maximum;

    bool is_reversed;

    struct Node* parent;
    struct Node* children[2];
} Node;

// Function prototypes.
Node* construct(size_t id, int value);
void splay(Node* node);
void access(Node* node);
bool assign_root(Node* node);
Node* find(Node* node);
bool link(Node* source, Node* target);
bool cut(Node* source, Node* target);
int query(Node* source, Node* target, bool* flag);
bool erase(Node* node);
void clean(Node* node);

#endif