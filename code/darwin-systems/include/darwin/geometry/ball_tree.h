/**
 * @file        :   ball_tree.h
 * @author      :   Braiden Gole
 * @brief       :   Interface definitions for a Ball Tree spatial partitioning hierarchy.
 * @details     :   This header defines the core structures and API required to construct,
 * traverse, and manage a multi-dimensional Ball Tree. It encapsulates
 * geometric data points, internal hierarchical nodes, and global runtime 
 * configurations to facilitate rapid nearest-neighbor queries.
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#ifndef BALL_TREE_H
#define BALL_TREE_H

#include <stddef.h>

/**
 * @struct      :   Point
 * @brief       :   Represents a single data point within a multi-dimensional metric space.
 */
typedef struct
{
    size_t id;
    size_t dimension;
    double* coordinates;   
} Point;

/**
 * @struct      :   Node
 * @brief       :   An individual bounding hypersphere (ball) wrapper tracking branch or leaf subsets.
 */
typedef struct Node
{
    Point center;
    double radius;
    size_t* indices;
    size_t count;
    struct Node* left;
    struct Node* right;
} Node;

/**
 * @struct      :   Tree
 * @brief       :   Global configuration metadata defining environmental bounds and capacity criteria.
 */
typedef struct
{
    size_t dimension;
    size_t capacity;
} Tree;

// Function prototypes.
Node* ball_tree(const size_t* indices, size_t count, const Point* dataset, const Tree* context);
void print(const Node* node, size_t depth);
void clean(Node* node);

#endif