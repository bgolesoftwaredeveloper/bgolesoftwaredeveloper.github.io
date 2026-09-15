/**
 * @file        :   farthest_pair_of_points.h
 * @author      :   Braiden Gole
 * @brief       :   Definitions and architectural structures for the Farthest Pair of Points algorithm.
 * @details     :   Provides the public data definitions, operational return models, 
 * and external entry interface required to execute the O(n log n) Convex Hull 
 * and Rotating Calipers pipeline.
 * @version     :   1.0
 * @date        :   2026-05-30
 */
#ifndef FARTHEST_PAIR_OF_POINTS_H
#define FARTHEST_PAIR_OF_POINTS_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief       :   Structure representing a geometric coordinate node.
 * @details     :   Defines an explicit spatial position located on a 2D Cartesian plane.
 */
typedef struct
{
    double x;
    double y;
} Point;

/**
 * @brief       :   Structure tracking a calculated pair of points and their distance profile.
 * @details     :   Capsules the definitive spatial extraction records returned by the execution pipeline.
 */
typedef struct
{
    bool is_valid;
    double distnace;
    Point origin;
    Point termination;
} Pair;

// Function prototypes.
Pair farthest_pair(Point points[], size_t size);

#endif