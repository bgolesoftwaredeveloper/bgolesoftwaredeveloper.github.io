/**
 * @file        :   closest_pair_of_points.h
 * @author      :   Braiden Gole
 * @brief       :   Declarations and structural definitions for the Closest Pair of Points algorithm.
 * @details     :   This header exposes the structures and public interface needed to calculate
 * the minimum Euclidean distance between coordinates in a 2D plane.
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#ifndef CLOSEST_PAIR_OF_POINTS_H
#define CLOSEST_PAIR_OF_POINTS_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct      :   Point
 * @brief       :   Represents a single location in a two-dimensional coordinate space.
 */
typedef struct
{
    double x;
    double y;
} Point;

/**
 * @struct      :   Pair
 * @brief       :   Encapsulates a matched set of two spatial points and the distance between them.
 */
typedef struct
{
    bool is_valid;
    double distance;
    Point origin;
    Point destination;
} Pair;

// Function prototypes.
Pair closest_pair(Point points[], size_t size);

#endif