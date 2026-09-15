/**
 * @file        :   ear_clipping.h
 * @author      :   Braiden Gole
 * @brief       :   Definitions and API prototypes for 2D polygon triangulation via ear clipping.
 * @details     :   Exposes the structures and functional interfaces needed to decompose
 * simple convex and concave polygons into non-overlapping triangle sets.
 * @version     :   1.0
 * @date        :   2026-06-04
 */
#ifndef EAR_CLIPPING_H
#define EAR_CLIPPING_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @struct  Vertex
 * @brief   Represents a single 2D Cartesian coordinate point.
 */
typedef struct
{
    double x;
    double y;
} Vertex;

/**
 * @struct  Triangle
 * @brief   Encapsulates three sequential vertices forming a discrete triangle slice.
 */
typedef struct
{
    Vertex origin;
    Vertex pivot;
    Vertex apex;
} Triangle;

// Function prototypes.
bool ear_clipping(const Vertex polygon[], size_t size, Triangle triangles[], size_t* count);

#endif