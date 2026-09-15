/**
 * @file        :   quad_tree.h
 * @author      :   Braiden Gole
 * @brief       :   Structure definitions and core API for a Point Quadtree.
 * @details     :   Provides the types and function prototypes required to construct,
 * populate, and query a 2D spatial-partitioning tree structure.
 * @version     :   1.0
 * @date        :   2026-06-05
 */
#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include <stdbool.h>
#include <stddef.h>

#define MAXIMUM_TREE_DEPTH 32
#define EPSILON 1e-9

/**
 * @brief Represents a single coordinate point in a 2D geometric space.
 */
typedef struct
{
    double x;
    double y;
} Point;

/**
 * @brief Defines an Axis-Aligned Bounding Box (AABB) regional boundary.
 */
typedef struct
{
    double x;
    double y;
    double half_width;
    double half_height;
} Boundary;

/**
 * @brief A spatial-partitioning node representing a region within the quadtree grid.
 */
typedef struct Node
{
    Boundary boundary;
    Point* point;

    bool divided;

    struct Node* north_west;
    struct Node* north_east;
    struct Node* south_west;
    struct Node* south_east;
} Node;

// Function prototypes.
Node* initialize(Boundary boundary);
bool insert(Node* node, Point point);
size_t query_range_circle(Node* node, Point center, double radius, Point* results, size_t maximum, size_t count);
void clean(Node* node);

#endif