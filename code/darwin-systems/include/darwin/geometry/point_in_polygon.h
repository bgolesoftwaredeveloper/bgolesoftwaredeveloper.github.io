/**
 * @file        :   point_in_polygon.h
 * @author      :   Braiden Gole
 * @brief       :   Interface definitions for 2D Point-in-Polygon (PIP) operations.
 * @details     :   Provides the fundamental Point structure definition and the
 * function prototype required to execute ray-casting boundary 
 * checks within 2D coordinate spaces.
 * @version     :   1.0
 * @date        :   2026-06-04
 */
#ifndef POINT_IN_POLYGON_H
#define POINT_IN_POLYGON_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @struct Point
 * @brief  Represents a single position coordinate within a 2D Cartesian plane.
 */
typedef struct
{
    double x;
    double y;
} Point;

// Function prototypes.
bool is_point_in_polygon(Point point, Point polygon[], size_t size);

#endif