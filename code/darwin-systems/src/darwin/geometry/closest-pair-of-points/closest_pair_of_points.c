/**
 * @file        :   closest_pair_of_points.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Closest Pair of Points geometric algorithm.
 * @details     :   This file implements an O(n log n) divide-and-conquer algorithm to 
 * locate the two closest points in a given multi-coordinate set.
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

// Include header files.
#include "darwin/geometry/closest_pair_of_points.h"

/**
 * @brief       :   Compares two points by their X-coordinates.
 * @details     :   Utilized as a callback sorting comparator for qsort.
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

    // Evaluate the sorting order difference mathematically.
    return (source->x > destination->x) - (source->x < destination->x);
}

/**
 * @brief       :   Compares two points by their Y-coordinates.
 * @details     :   Utilized as a callback sorting comparator for qsort.
 * @param       :   compare Pointer to the first point structure.
 * @param       :   against Pointer to the second point structure.
 * @return      :   int Result indicating relative ordering (-1, 0, or 1).
 */
static inline int compare_y(const void* compare, const void* against)
{
    // Ensure both incoming point memory addresses are valid.
    if (compare == NULL || against == NULL)
    {
        return 0;
    }

    // Cast the void references back to explicit Point types.
    const Point* source = (const Point*)compare;
    const Point* destination = (const Point*)against;

    // Evaluate the sorting order difference mathematically.
    return (source->y > destination->y) - (source->y < destination->y);
}

/**
 * @brief       :   Calculates the Euclidean distance between two points.
 * @details     :   Employs the standard Pythagorean distance formula.
 * @param       :   compare The first Point structure.
 * @param       :   against The second Point structure.
 * @return      :   double The calculated distance value.
 */
static inline double calculate_distance(Point compare, Point against)
{
    // Compute the delta horizontal offset.
    double delta_x = compare.x - against.x;

    // Compute the delta vertical offset.
    double delta_y = compare.y - against.y;

    // Calculate and return the standard square root of the summed squares.
    return sqrt(delta_x * delta_x + delta_y * delta_y);
}

/**
 * @brief       :   Determines which of two Pair structures has the smaller distance.
 * @details     :   Evaluates validity flags before performing numerical comparisons.
 * @param       :   compare The first Pair structure.
 * @param       :   against The second Pair structure.
 * @return      :   Pair The Pair struct possessing the shorter valid distance.
 */
static inline Pair minimum_pair(Pair compare, Pair against)
{
    // If the comparison pair has an invalid state, favor the second option.
    if (!compare.is_valid)
    {
        return against;
    }

    // If the secondary pair has an invalid state, favor the first option.
    if (!against.is_valid)
    {
        return compare;
    }

    // Return the specified structure that holds the shorter geometric span.
    return (compare.distance < against.distance) ? compare : against;
}

/**
 * @brief       :   Finds the closest pair of points within a narrow vertical strip.
 * @details     :   Checks neighboring coordinates sorted along the Y-axis.
 * @param       :   strip Array of points close to the dividing median line.
 * @param       :   size The quantity of points located inside the strip.
 * @param       :   closest The current historical minimum pair.
 * @return      :   Pair The absolute closest pair discovered within the strip.
 */
static inline Pair strip_closest(Point strip[], size_t size, Pair closest)
{
    // Guard against unallocated collection of sizes incapable of forming pairs.
    if (strip == NULL || size < 2)
    {
        return closest;
    }

    // Establish the current threshold baseline distance.
    double minimum_distance = closest.distance;

    // Duplicate the closest pair structure to modify safely.
    Pair minimum_pair = closest;

    // Sort the candidate strip array by Y-coordinate layout.
    qsort(strip, size, sizeof(Point), compare_y);

    // Iterate through every sorted coordinate in the vertical collection.
    for (size_t index = 0; index < size; index++)
    {
        // Check following points until their vertical distance exceeds the current minimum.
        for (size_t next = index + 1; next < size && (strip[next].y - strip[index].y) < minimum_distance; next++)
        {
            // Compute the real span separating the two active items.
            double distance = calculate_distance(strip[index], strip[next]);

            // Save the newly discovered closer position if they beat the current record.
            if (distance < minimum_distance)
            {
                minimum_distance = distance;

                minimum_pair.is_valid = true;
                minimum_pair.distance = minimum_distance;
                minimum_pair.origin = strip[index];
                minimum_pair.destination = strip[next];
            }
        }
    }

    return minimum_pair;
}

/**
 * @brief       :   Computes the closest pair using a brute-force approach.
 * @details     :   An O(n^2) method used for base cases of 3 or fewer elements.
 * @param       :   points Array of input points.
 * @param       :   number_of_points Total count of elements inside the slice.
 * @return      :   Pair The closest discovered pair.
 */
static inline Pair brute_force(const Point points[], size_t number_of_points)
{
    // Initialize a default invalid pair structure layout.
    Pair minimum_pair =
    {
        .is_valid = false,
        .distance = DBL_MAX,
    };

    // Guard against unallocated collections or sizes incapable of forming pairs.
    if (points == NULL || number_of_points < 2)
    {
        return minimum_pair;
    }

    // Establish teh default maximized bounding distance boundary.
    double minimum_distance = DBL_MAX;

    // Compare each item aginst every subsequent entry in the collection.
    for (size_t index = 0; index < number_of_points; index++)
    {
        for (size_t next = index + 1; next < number_of_points; next++)
        {
            // Compute the exact spatial distance.
            double distance = calculate_distance(points[index], points[next]);

            // Retain records of the closest match found during iteration loops.
            if (distance < minimum_distance)
            {
                minimum_distance = distance;
                
                minimum_pair.is_valid = true;
                minimum_pair.distance = minimum_distance;
                minimum_pair.origin = points[index];
                minimum_pair.destination = points[next];
            }
        }
    }

    return minimum_pair;
}

/**
 * @brief       :   Recursive divide-and-conquer sub-algorithm handler.
 * @details     :   Splits coordinate sets down their midpoint axes.
 * @param       :   points Array of sorted points by X-coordinate.
 * @param       :   size Total tracking length of the current slice.
 * @return      :   Pair The closest pair calculated from the segments.
 */
static inline Pair closest_utility(Point points[], size_t size)
{
    // Execute brute-force computation when the problem space reduces to 3 points or fewer.
    if (size <= 3)
    {
        return brute_force(points, size);
    }

    // Determine the midplane boundary index.
    size_t middle = size / 2;

    // Cahce the exact center point to reference splitting axes.
    Point middle_point = points[middle];

    // Compute closest pairing located exclusively on the left plane.
    Pair left = closest_utility(points, middle);

    // Compute closest pairings located exclusivey on the right plane.
    Pair right = closest_utility(points + middle, size - middle);

    // Select the minimum pairing context found between both operational split halves.
    Pair minimum = minimum_pair(left, right);

    // Allocate temporary local heap space to map strip components safely.
    Point* strip = (Point*)malloc(sizeof(Point) * size);

    // Fallback and return the best independent half-set data if memory allocation fails.
    if (strip == NULL)
    {
        return minimum;
    }

    // Tracking index counter for managing population nodes into the strip.
    size_t point_index = 0;

    // Collect all points whose absolute distance to the midplane is smaller than minimum.
    for (size_t index = 0; index < size; index++)
    {
        if (fabs(points[index].x - middle_point.x) < minimum.distance)
        {
            strip[point_index] = points[index];

            point_index++;
        }
    }

    // Evaluate if any corss-midplane boundary pair combinations beat the current minimum.
    Pair result = minimum_pair(minimum, strip_closest(strip, point_index, minimum));

    // Release allocated heap memory blocks immediately to avoid memory leaks.
    free(strip);

    return result;
}

/**
 * @brief       :   Public interface to find the closest pair of points.
 * @details     :   Main entryway routine handling initial sorting and execution wrappers.
 * @param       :   points Array of point structures.
 * @param       :   size Element count inside the point array structure.
 * @return      :   Pair The definitive closest pair discovered.
 */
Pair closest_pair(Point points[], size_t size)
{
    // Initialize an invalid structural pair reference.ß
    Pair invalid_pair =
    {
        .is_valid = false,
        .distance = DBL_MAX
    };

    // Avoid processing null references.
    if (points == NULL)
    {
        return invalid_pair;
    }

    // Handle insufficient data parameters safely.
    if (size < 2)
    {
        return invalid_pair;
    }

    // Pre-sort all spatial nodes by their horizontal values before computation.
    qsort(points, size, sizeof(Point), compare_x);

    // Begin executing the primary divide and conquer utility workflow.
    return closest_utility(points, size);
}