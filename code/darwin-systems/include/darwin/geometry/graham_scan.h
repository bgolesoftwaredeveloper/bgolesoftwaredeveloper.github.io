/**
 * @file        :   graham_scan.h
 * @author      :   Braiden Gole
 * @brief       :   Public interface for the Graham Scan Convex Hull algorithm.
 * @details     :   This header defines the core data structures and function prototypes 
 * required to compute the convex hull of a finite set of 2D points 
 * using the Graham Scan technique.
 * @version     :   1.0
 * @date        :   2026-06-01
 */
#ifndef GRAHAM_SCAN_H
#define GRAHAM_SCAN_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @struct  : Point
 * @brief   : Represents a coordinate node in a two-dimensional Cartesian plane.
 * @details : Uses double-precision floating-point numbers to maintain high geometric 
 * accuracy during distance and cross-product orientation evaluations.
 */
typedef struct
{
    double x;
    double y;
} Point;

// Function prototypes.
bool graham_scan(Point points[], size_t number_of_points, Point stack[], size_t* count);

#endif