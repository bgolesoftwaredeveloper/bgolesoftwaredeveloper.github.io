/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test runner implementation for the Closest Pair of Points algorithm.
 * @details     :   This program instantiates a static array of 2D coordinates and invokes 
 * the divide-and-conquer 'closest_pair' utility to determine and print 
 * the two nearest points and their geometric distance.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/geometry/closest-pair-of-points/main.c ./src/darwin/geometry/closest-pair-of-points/closest_pair_of_points.c -I./include -o ./bin/closest_pair_of_points_test
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#include <stdio.h>

// Include header files.
#include "darwin/geometry/closest_pair_of_points.h"

int main(void)
{
    // Define a set of points to work with.
    Point points[] =
    {
        {2.0, 3.0}, {12.0, 30.0}, 
        {40.0, 50.0}, {5.0, 1.0},
        {12.0, 10.0}, {3.0, 4.0}
    };

    // Calculate the number of points.
    size_t number_of_points = sizeof(points) / sizeof(points[0]);

    // Perform closest-pair-of-points algorithm.
    Pair result = closest_pair(points, number_of_points);

    if (result.is_valid)
    {
        // Summary.
        printf("%s (%.2lf, %.2lf)%c", "Origin:", result.origin.x, result.origin.y, '\n');
        printf("%s (%.2lf, %.2lf)%c", "Destination:", result.destination.x, result.destination.y, '\n');
    }
    else
    {
        printf("%s", "Could not find a valid pair.\n");
    }

    return 0;
}