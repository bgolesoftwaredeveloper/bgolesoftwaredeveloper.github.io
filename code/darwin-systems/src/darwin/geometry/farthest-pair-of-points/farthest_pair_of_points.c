/**
 * @file        :   farthest_pair_of_points.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Farthest Pair of Points geometric algorithm.
 * @details     :   This file implements an O(n log n) algorithm utilizing Andrew's 
 * Monotone Chain approach to construct a Convex Hull, followed by a linear 
 * Rotating Calipers scan to locate the two furthest separated coordinates.
 * @version     :   1.0
 * @date        :   2026-05-30
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include header files.
#include "darwin/geometry/farthest_pair_of_points.h"

/**
 * @brief       :   Calculates the squared Euclidean distance between two points.
 * @details     :   Omits the standard square root operation to conserve processing cycles.
 * @param       :   source The starting Point structure.
 * @param       :   destination The target Point structure.
 * @return      :   double The calculated squared distance value.
 */
static inline double calculate_squared_distance(Point source, Point destination)
{
    // Compute the delta horizontal offset.
    double delta_x = source.x - destination.x;

    // Compute the delta vertical offfset.
    double delta_y = source.y - destination.y;

    // Evaluate and return the summed squares of both offsets.
    return (delta_x * delta_x) + (delta_y * delta_y);
}

/**
 * @brief       :   Computes the cross product of two vectors sharing an origin.
 * @details     :   Determines directional turns: > 0 for counter-clockwise, < 0 for clockwise, 0 if collinear.
 * @param       :   origin The base intersection coordinate node.
 * @param       :   left The endpoint of the left-hand tracking vector.
 * @param       :   right The endpoint of the right-hand tracking vector.
 * @return      :   double The calculated vector cross product scalar value.
 */
static inline double cross_product(Point origin, Point left, Point right)
{
    // Establish independent offset vector structures relative to the origin.
    double left_x = left.x - origin.x;
    double left_y = left.y - origin.y;
    double right_x = right.x - origin.x;
    double right_y = right.y - origin.y;

    // Apply the standard cross-matrix determinant calculation layout.
    return (left_x * right_y) - (left_y * right_x);
}

/**
 * @brief       :   Compares two points primarily by X-coordinates, secondarily by Y-coordinates.
 * @details     :   Utilized as a callback sorting comparator for qsort lexicographical ordering.
 * @param       :   compare Pointer to the first point structure.
 * @param       :   against Pointer to the second point structure.
 * @return      :   int Result indicating relative ordering (-1, 0, or 1).
 */
static inline int compare_x(const void* compare, const void* against)
{
    // Ensure both incoming point memory addresses are valid.
    if (compare == NULL || against == NULL)
    {
        return 0;
    }

    // Cast the void references back to explicit Point types.
    const Point* source = (const Point*)compare;
    const Point* destination = (const Point*)against;

    // Evaluate the primary sorting order difference along the horizontal plane.
    if (source->x != destination->x)
    {
        return (source->x > destination->x) - (source->x < destination->x);
    }

    // Evaluate the secondary fallback sorting order difference along the vertical plane.
    return (source->y > destination->y) - (source->y < destination->y);
}

/**
 * @brief       :   Constructs the outer convex hull perimeter around a spatial layout.
 * @details     :   Implements Andrew's Monotone Chain algorithm sweeping in two passes.
 * @param       :   points Array of structural input coordinate markers.
 * @param       :   size Total population tracking length of the points array.
 * @param       :   hull Allocation pointer destination buffer to hold outer hull nodes.
 * @param       :   hull_size Return tracking address mapping the finalized hull element count.
 * @return      :   bool True if the construction pipeline succeeded, false otherwise.
 */
static inline bool convex_hull(Point points[], size_t size, Point hull[], size_t* hull_size)
{
    // Guard against null addresses, unallocated parameters, or empty collection sizes.
    if (points == NULL || hull == NULL || hull_size == 0 || size == 0)
    {
        return false;
    }

    // Handlde trivial baseline data loops that cannot form functional closed bounding polygons.
    if (size <= 3)
    {
        for (size_t index = 0; index < size; index++)
        {
            hull[index] = points[index];
        }

        *hull_size = size;

        return true;
    }

    // Arrange all data points lexicographically by horizontal layout properties.
    qsort(points, size, sizeof(Point), compare_x);

    // Track active insertions inside the termination hull buffer.
    size_t hull_index = 0;

    // Scan forward from left to right across the sorted array to construc the lower hull.
    for (size_t index = 0; index < size; index++)
    {
        // Filter out clockwise turns and collinear intersections on the boundary.
        while (hull_index >= 2 && cross_product(hull[hull_index - 2], hull[hull_index - 1], points[index]) <= 0)
        {
            hull_index--;
        }

        hull[hull_index++] = points[index];
    }

    // Retain a baseline tracking threshold separating lower and upper construction curves.
    size_t lower_limit = hull_index + 1;

    // Scan backward from right to left across the sorted array to construct the upper hull.
    for (size_t index = size - 1; index > 0; index--)
    {
        size_t adjusted_index = index - 1;

        // Filter out clockwise turns and collinear entries relative to upper boundary profiles.
        while (hull_index >= lower_limit && cross_product(hull[hull_index - 2], hull[hull_index - 1], points[index]) <= 0)
        {
            hull_index--;
        }

        hull[hull_index++] = points[adjusted_index];
    }

    // Drop the redundant loop starting node duplicated during construction to close the chain.
    *hull_size = hull_index - 1;

    return true;
}

/**
 * @brief       :   Finds the farthest pair of points on a verified convex polygon hull boundary.
 * @details     :   Implements an O(n) Rotating Calipers linear scan tracking antipodal points.
 * @param       :   hull Read-only array housing computed convex boundary coordinates.
 * @param       :   size Total active element size of the convex hull structure.
 * @return      :   Pair The Pair structure containing the maximum distance properties.
 */
static inline Pair evaluate_rotating_calipers(const Point hull[], size_t size)
{
    // Initialize a default invalid structural tracking pair template layout.
    Pair minimum_pair =
    {
        .is_valid = false,
        .distnace = 0.0
    };

    // Avoid validating empty sets of missing memory regions.
    if (hull == NULL || size == 0)
    {
        return minimum_pair;
    }

    // Process isolated single element boundaries.
    if (size == 1)
    {
        minimum_pair.is_valid = true;
        minimum_pair.origin = hull[0];
        minimum_pair.termination = hull[0];

        return minimum_pair;
    }

    // Process basic dual element linear lines.
    if (size == 2)
    {
        minimum_pair.is_valid = true;
        minimum_pair.distnace = sqrt(calculate_squared_distance(hull[0], hull[1]));
        minimum_pair.origin = hull[0];
        minimum_pair.termination = hull[1];

        return minimum_pair;
    }

    // Estab,ish the initial tracking maximum bounds reference.
    double maximum_squared_distance = -1.0;

    // Index tracking the structural vertex located directly across from the active edge.
    size_t antipodal_index = 1;

    // Step through every sequential perimeter edge line of the convex envelope.
    for (size_t edge_start = 0; edge_start < size; edge_start++)
    {
        size_t edge_end = (edge_start + 1) % size;

        while (true)
        {
            size_t next_antipodal = (antipodal_index + 1) % size;

            // Compute relative spatial triangle area heights to detect maximum distance shifts.
            double current_area = fabs(cross_product(hull[edge_start], hull[antipodal_index], hull[next_antipodal]));
            double next_area = fabs(cross_product(hull[edge_start], hull[edge_end], hull[next_antipodal]));

            // Advance the parallel pointer context if the upcoming vertex offers a tailer projection.
            if (next_area > current_area)
            {
                antipodal_index = next_antipodal;
            }
            else
            {
                break;
            }
        }

        // Measure spatial profile separation between the edge origin and the antipodal vertex.
        double distance_from_start = calculate_squared_distance(hull[edge_start], hull[antipodal_index]);

        if (distance_from_start > maximum_squared_distance)
        {
            maximum_squared_distance = distance_from_start;

            minimum_pair.is_valid = true;
            minimum_pair.origin = hull[edge_start];
            minimum_pair.termination = hull[antipodal_index];
        }

        // Measure spatial prfile separation between the edge termination and the antipodal vertex.
        double distance_from_end = calculate_squared_distance(hull[edge_end], hull[antipodal_index]);

        if (distance_from_end > maximum_squared_distance)
        {
            maximum_squared_distance = distance_from_end;

            minimum_pair.is_valid = true;
            minimum_pair.origin = hull[edge_end];
            minimum_pair.termination = hull[antipodal_index];
        }
    }

    // Extract the precise linear Euclidean distance value exactly once upon pipeline exit.
    minimum_pair.distnace = sqrt(maximum_squared_distance);

    return minimum_pair;
}

/**
 * @brief       :   Public interface to find the farthest pair of points.
 * @details     :   Main gateway routing logic managing allocations, hull sweeps, and cleanup wrappers.
 * @param       :   points Array of coordinates to evaluate.
 * @param       :   size The element count parameter of the input array.
 * @return      :   Pair The definitive furthest pair object discovered.
 */
Pair farthest_pair(Point points[], size_t size)
{
    // Setup a default invalid fallback pair profile.
    Pair invalid_pair =
    {
        .is_valid = false,
        .distnace = -1.0
    };

    // Avoid passing operations into unasigned structures.
    if (points == NULL)
    {
        return invalid_pair;
    }

    // Dismiss collections lacking sufficient entities to formulat epair arrangements.
    if (size < 2)
    {
        return invalid_pair;
    }

    // Allocate safe heap memory workspace to handle maximum theoretical hull elements (2 * size).  
    Point* hull = (Point*)malloc(sizeof(Point) * 2 * size);

    if (hull == NULL)
    {
        return invalid_pair;
    }

    size_t active_hull_size = 0;

    // Execute the primary monotone sorting and filter envelope stage.
    if (!convex_hull(points, size, hull, &active_hull_size))
    {
        free(hull);

        return invalid_pair;
    }

    // Compute the ultimiate caliper boundaies on the isolated hull perimeter.
    Pair result = evaluate_rotating_calipers(hull, active_hull_size);

    // Free buffer.
    free(hull);

    return result;
}