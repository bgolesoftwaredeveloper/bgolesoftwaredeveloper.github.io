/**
 * @file        :   ear_clipping.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation implementation of the Ear Clipping algorithm for polygon triangulation.
 * @details     :   Provides a robust, dynamic triangulation subsystem designed to break down 
 * simple convex and concave polygons into collections of non-overlapping 
 * triangles. Employs cross-product winding validation, point containment 
 * checks, and ray-casting chord visibility boundary checks.
 * @version     :   1.0
 * @date        :   2026-06-04
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/geometry/ear_clipping.h"

/**
 * @brief Computes the signed 2D cross product of two sequential edge vectors.
 * @details Calculates the determinant of the 2D vectors formed by (pivot - origin) 
 *          and (apex - pivot) to evaluate relative vertex orientation.
 * @param origin The starting coordinate vertex of the primary vector.
 * @param pivot The shared intermediate corner node vertex.
 * @param apex The terminal coordinate vertex of the secondary vector.
 * @return Positive for a counter-clockwise turn, negative for clockwise, zero if collinear.
 */
static inline double cross_product(Vertex origin, Vertex pivot, Vertex apex)
{
    return (pivot.x - origin.x) * (apex.y - pivot.y) - (pivot.y - origin.y) * (apex.x - pivot.x);
}

/**
 * @brief Verifies if an arbitrary target coordinate sits inside a specific triangle.
 * @details Utilizes edge-side classification logic based on signed cross products. 
 *          If all three tracking channels share matching signs, the target point is inside.
 * @param probe The structural vertex coordinate under spatial inspection.
 * @param origin The triangle's primary anchor vertex.
 * @param pivot The triangle's secondary turning vertex.
 * @param apex The triangle's peak terminal vertex.
 * @return True if the probe point lies inside or on the boundaries, false otherwise.
 */
static inline bool in_triangle(Vertex probe, Vertex origin, Vertex pivot, Vertex apex)
{
    double crossing_origin = cross_product(origin, pivot, probe);
    double crossing_pivot = cross_product(pivot, apex, probe);
    double crossing_apex = cross_product(apex, origin, probe);

    bool has_negative = (crossing_origin < 0.0) || (crossing_pivot < 0.0) || (crossing_apex < 0.0);
    bool has_positive = (crossing_origin > 0.0) || (crossing_pivot > 0.0) || (crossing_apex > 0.0);

    return !(has_negative && has_positive);
}

/**
 * @brief Evaluates point interior assignment via Ray-Casting.
 * @details Implements a standard ray-casting algorithm across the active index ring map 
 *          to determine coordinate containment validation for the target sample.
 * @param probe The coordinate target being queried (chord midpoint).
 * @param count The total number of vertices remaining in the unclipped shape pool.
 * @param polygon The master reference coordinate buffer list.
 * @param indices The structural loop register tracking active vertex indices.
 * @return True if the probe sits strictly inside the polygon interior, false otherwise.
 */
static bool is_midpoint_interior(Vertex probe, size_t count, const Vertex polygon[], const size_t indices[])
{
    bool inside = false;

    // Track the edge segments around the perimeter using a trailing and leading loop node.
    for (size_t leading_node = 0, trailing_node = count - 1; leading_node < count; trailing_node = leading_node++)
    {
        Vertex node_lead = polygon[indices[leading_node]];
        Vertex node_trail = polygon[indices[trailing_node]];

        // Standard ray-casting crossing check for the current boundary edge segment.
        if (((node_lead.y > probe.y) != (node_trail.y > probe.y)) && 
            (probe.x < (node_trail.x - node_lead.x) * (probe.y - node_lead.y) / (node_trail.y - node_lead.y) + node_lead.x))
        {
            inside = !inside;
        }
    }

    return inside;
}

/**
 * @brief Validates if a sequential triplet of indices forms a geometrically valid ear.
 * @details Verifies that the candidate corner forms a convex interior angle, its diagonal base 
 *          chord path runs completely inside the polygon, and no other active vertices sit inside.
 * @param origin_index The tracking index of the corner's preceding base point.
 * @param pivot_index The tracking index of the active vertex tip under validation.
 * @param apex_index The tracking index of the corner's succeeding base point.
 * @param count The total remaining unclipped active vertex pool size.
 * @param polygon The master global array storing vertex locations.
 * @param indices The active lookup index map register.
 * @return True if the vertex triplet forms a valid removable ear, false otherwise.
 */
static inline bool is_ear(size_t origin_index, size_t pivot_index, size_t apex_index, size_t count, const Vertex polygon[], const size_t indices[])
{
    Vertex origin = polygon[indices[origin_index]];
    Vertex pivot = polygon[indices[pivot_index]];
    Vertex apex = polygon[indices[apex_index]];

    // Check if the ear corner turns counter-clockwise to filter out reflex corners.
    if (cross_product(origin, pivot, apex) <= 0.0)
    {
        return false;
    }
    
    // Compute the exact midpoint of the base diagonal chord line.
    Vertex chord_midpoint =
    {
        .x = (origin.x + apex.x) * 0.5,
        .y = (origin.y + apex.y) * 0.5
    };

    // Verify that the chord midpoint falls inside the polygon to protect against exterior cuts.
    if (!is_midpoint_interior(chord_midpoint, count, polygon, indices))
    {
        return false;
    }

    // Ensure no other remaining active structural nodes reside within this triangle.
    for (size_t index = 0; index < count; index++)
    {
        // Skip vertices that compose the candidate ear itself.
        if (index == origin_index || index == pivot_index || index == apex_index)
        {
            continue;
        }

        // Reject the ear if an unclipped point is caught inside the boundary.
        if (in_triangle(polygon[indices[index]], origin, pivot, apex))
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Main entry API to decompose simple polygons into explicit non-overlapping triangles.
 * @details Executes an iterative ear clipping loop execution layer. Requires an initial 
 *          Counter-Clockwise (CCW) vertex input sequence configuration layout map to execute successfully.
 * @param polygon Array containing the ordered sequence of shape perimeter coordinates.
 * @param size Total quantity of coordinate vertices constructing the input geometry.
 * @param triangles Destination allocated target array to dump output structural items.
 * @param count Pointer to save the total number of finalized compiled triangles.
 * @return True if execution finishes to completion, false if arguments or shapes are invalid.
 */
bool ear_clipping(const Vertex polygon[], size_t size, Triangle triangles[], size_t* count)
{
    // Enforce basic execution guard parameters. Shapes require a minimum of 3 vertices.
    if (polygon == NULL || triangles == NULL || count == NULL || size < 3)
    {
        return false;
    }

    // Allocate dynamic tracking map buffer memory to manage geometric collapse.
    size_t* indices = (size_t*)malloc(sizeof(size_t) * size);

    if (indices == NULL)
    {
        return false;
    }

    // Initialize our index mapping registers sequentially.
    for (size_t index = 0; index < size; index++)
    {
        indices[index] = index;
    }

    // Safety guards against complex loops, self-intersections, or incorrect winding patterns.
    size_t active_triangles = 0;
    size_t active_vertices = size;

    size_t tracking_fails = 0;
    size_t execution_limit = active_vertices * active_vertices;

    // Run iterative loops collapsing structural parameters down until only 1 final triangle is left.
    while (active_vertices > 2)
    {
        // Break out safely if execution steps exceed boundaries to prevent infinite loops.
        if (tracking_fails++ > execution_limit)
        {
            free(indices);

            return false;
        }

        bool ear_clipped = false;

        // Traverse the current structural perimeter seeking an active removable node tip.
        for (size_t index = 0; index < active_vertices; index++)
        {
            // Handle adjacent index ring configurations using cyclic boundary wrappers.
            size_t u = (index == 0) ? active_vertices - 1 : index - 1;
            size_t v = index;
            size_t w = (index + 1 == active_vertices) ? 0 : index + 1;

            // Evaluate if current sequence window forms a clean, geometric target clip layer.
            if (is_ear(u, v, w, active_vertices, polygon, indices))
            {
                // Capture the current valid ear geometry components into the destination array.
                triangles[active_triangles].origin = polygon[indices[u]];
                triangles[active_triangles].pivot = polygon[indices[v]];
                triangles[active_triangles].apex = polygon[indices[w]];

                active_triangles++;

                // Collapse the index lookup ring registry to drop the clipped vertex.
                for (size_t compression_node = v; compression_node < active_vertices - 1; compression_node++)
                {
                    indices[compression_node] = indices[compression_node + 1];
                }

                // Decrement active shape vertex parameters to reflect structural removal.
                active_vertices--;
                ear_clipped = true;

                break;
            }
        }

        // Intercept execution path if a full sweep occurs without identifying any extractable ears.
        if (!ear_clipped)
        {
            free(indices);

            return false;
        }
    }

    // Export structural counts to calling frame context parameters.
    *count = active_triangles;

    free(indices);

    return true;
}