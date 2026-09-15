/**
 * @file        :   graham_scan.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Graham Scan algorithm for Convex Hull extraction.
 * @details     :   This file provides utilities to compute the convex hull of a given 
 * finite set of 2D coordinates. It handles sorting vertices by polar 
 * angle using a stateful global context and filters duplicate collinear points.
 * @version     :   1.0
 * @date        :   2026-06-01
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/geometry/graham_scan.h"

/**
 * @brief Global file-scope context tracking point.
 * @details Stores the reference origin (lowest, left-most point) utilized by the qsort 
 * callback to compute and compare relative polar angles between vectors.
 */
static Point context;

/**
 * @brief Calculates the squared Euclidean distance between two 2D coordinates.
 * @details Avoiding the square root function improves computation speed when comparing relative lengths.
 * @param compare The source point.
 * @param against The target point to measure distance against.
 * @return The square of the absolute distance between the two coordinates as a double.
 */
static inline double calculate_squared_distance(Point compare, Point against)
{
    // Calculate the differences along both the X and Y axies.
    double delta_x = compare.x - against.x;
    double delta_y = compare.y - against.y;

    // Return standard squared distance equation value.
    return (delta_x * delta_x) + (delta_y * delta_y);
}

/**
 * @brief Determines the directional orientation of three ordered points.
 * @details Calculates the cross product of the vectors created by the point segments.
 * @param anchor The common start point or vertex of the comparison angle.
 * @param current The endpoint of the first vector segment acting as a turning node.
 * @param target The candidate point creating the second vector segment.
 * @return An integer representing geometric orientation:
 * - 0 : Points are perfectly collinear.
 * - 1 : Sequence forms a clockwise orientation.
 * - 2 : Sequence forms a counter-clockwise orientation.
 */
static inline int determine_orientation(Point anchor, Point current, Point target)
{
    // Compute the cross product of vectors (current - anchor) and (target - current).
    double cross_product = (current.y - anchor.y) * (target.x - current.x) -
        (current.x - anchor.x) * (target.y - current.y);
    
    // Cross product equals zero means the three points lie on the exact same line segment.
    if (cross_product == 0.0)
    {
        return 0;
    }

    // Positive values imply a right/clockwise turn, negative values mean a left/counter-clockwise turn.
    return (cross_product > 0.0) ? 1 : 2;
}

/**
 * @brief Callback sorting routine used by standard library qsort.
 * @details Arranges two given points based on their counter-clockwise polar angle relative to 
 * the static reference context point. Ties break by preserving the furthest point.
 * @param compare A void pointer referencing the primary point structure.
 * @param against A void pointer referencing the secondary point structure to compare against.
 * @return An integer control sequence indicating precedence:
 * - Negative (-1) : 'compare' precedes 'against'.
 * - Positive (1)  : 'against' precedes 'compare'.
 * - Zero (0)      : Pointers are invalid or structural matching matches.
 */
static inline int compare_by_polar_angle(const void* compare, const void* against)
{
    // Safety check against uninitialized or NULL pointer parameters passed by qsort.
    if (compare == NULL || against == NULL)
    {
        return 0;
    }

    // Case the generic void pointers back to their native Point structure types.
    const Point* compare_point = (const Point*)compare;
    const Point* against_point = (const Point*)against;

    // Evaluate the winding order of the two target points releative to our global reference context point.
    int orientation = determine_orientation(context, *compare_point, *against_point);

    // If the points are collinear with the reference point, order them by distance.
    if (orientation == 0)
    {
        double distance_compare = calculate_squared_distance(context, *compare_point);
        double distance_against = calculate_squared_distance(context, *against_point);

        if (distance_compare < distance_against)
        {
            return -1;
        }

        if (distance_compare > distance_against)
        {
            return 1;
        }

        return 0;
    }

    // If the winding orientation is counter-clockwise, the first point is already in corrrect relative order.
    return (orientation == 2) ? -1 : 1;
}

/**
 * @brief Main routine executing the Graham Scan algorithm to find the Convex Hull.
 * @details mutates the tracking array to place elements in order, drops redundant collinear 
 * components, and saves the wrapped boundary values sequentially inside the output stack.
 * @param points The initial unsorted array of point structures.
 * @param number_of_points The overall size or count of elements stored within the input array.
 * @param stack An allocated destination buffer used to track and store the active convex hull nodes.
 * @param count An output parameter updated with the final vertex count of the convex hull.
 * @return True if the convex hull was calculated successfully; false if inputs are invalid or degenerate.
 */
bool graham_scan(Point points[], size_t number_of_points, Point stack[], size_t* count)
{
    // Validate parameters.
    if (points == NULL || stack == NULL || count == NULL)
    {
        return false;
    }

    // A convex boundary mathematically requires at least 3 distinct non-collinear vertices.
    if (number_of_points < 3)
    {
        return false;
    }

    size_t lowest_index = 0;

    // Linear search loop to determine the bottom-most coordinate (breaking ties via the left-most X value).
    for (size_t index = 1; index < number_of_points; index++)
    {
        double current_y = points[index].y;
        double lowest_y = points[lowest_index].y;

        // Update tracking index if the target Y is lower, or if it matches but has a smaller X component.
        if ((current_y < lowest_y) || (current_y == lowest_y && points[index].x < points[lowest_index].x))
        {
            lowest_index = index;
        }
    }

    // Swap the chosen baseline anchor pivot point into index 0 of the point working array.
    Point temporary = points[0];
    points[0] = points[lowest_index];
    points[lowest_index] = temporary;

    // Cahce this pviot point globally so that the quicksort callback routine can access it as an origin.
    context = points[0];

    // Sort the remaining points array subset radially from index 1 to teh end using polar angles.
    qsort(&points[1], number_of_points - 1, sizeof(Point), compare_by_polar_angle);

    size_t modified_count = 1;

    // Scan through the sorted coordinates to compact the array and drop intermediate collinear entries.
    for (size_t index = 1; index < number_of_points; index++)
    {
        // Skip over sequential points that share an identical polar slope angle relative to our context pivot.
        while (index < number_of_points - 1 && determine_orientation(context, points[index], points[index + 1]) == 0)
        {
            index++;
        }

        // Shift the remaining valid outer point downward into the compacted tracking array layout.
        points[modified_count] = points[index];
        modified_count++;
    }

    // Cancel execution if the reduction operation stripped away too many points to create a geometric polygon.
    if (modified_count < 3)
    {
        return false;
    }

    size_t size = 0;

    // Seed the output stack array with the baseline context pivot and the first two sorted angles.
    stack[size++] = points[0];
    stack[size++] = points[1];
    stack[size++] = points[2];

    // Traverse the compressed array structure to evaluate the boundary path trajectory.
    for (size_t index = 3; index < modified_count; index++)
    {
        // If the path makes a clockwise or collinear turn, the current hull corner point is invalid.
        while (size > 1 && determine_orientation(stack[size - 2], stack[size - 1], points[index]) != 2)
        {
            size--;
        }

        // Push the valid candidate coordinate onto the top of the processing hull stack tracking sequence.
        stack[size++] = points[index];
    }

    // Passs back the final size tally of the computed hull coordinates array layer to the caller.
    *count = size;

    return true;
}