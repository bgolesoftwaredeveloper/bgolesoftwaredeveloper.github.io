/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Driver program to test the Graham Scan convex hull algorithm.
 * @details     :   This program initializes a set of 2D points (including internal,
 * boundary, and collinear points), allocates memory for a stack
 * buffer, calls the graham_scan utility, and prints the resulting
 * convex hull coordinates in counter-clockwise order.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/geometry/graham-scan/main.c ./src/darwin/geometry/graham-scan/graham_scan.c -I./include -o ./bin/graham_scan_test
 * @version     :   1.0
 * @date        :   2026-06-01
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/geometry/graham_scan.h"

int main(void)
{
    // Define an un-ordered sample array of 2D coordinate points.
    Point points[] =
    {
        {0.0, 3.0},
        {1.0, 1.0},
        {2.0, 2.0},
        {4.0, 4.0},
        {0.0, 0.0},
        {1.0, 2.0},
        {3.0, 1.0},
        {3.0, 3.0},
        {2.0, 0.0},
        {4.0, 0.0}
    };

    // Calculate the number of points.
    size_t number_of_points = sizeof(points) / sizeof(points[0]);

    // Initialize an output stack array capable of holding the worst-case scenario hull.
    Point* stack = (Point*)malloc(sizeof(Point) * number_of_points);

    if (stack == NULL)
    {
        return 1;
    }

    // Keep traack of the final number of points composing the convex hull parameter.
    size_t count = 0;

    // Run the Graham Scan geometric calculation routine.
    bool success = graham_scan(points, number_of_points, stack, &count);

    // Catch processing failures such as degenerate geography or empty pointer sets.
    if (!success)
    {
        fprintf(stderr, "Convex hull computation failed.\n");

        return 1;
    }

    // Sumnary.
    printf("%s", "The counter-clockwise boundary points are:\n");
    printf("%s", "---------------------------------------------\n");

    for (size_t index = 0; index < count; index++)
    {
        printf("%s %zu%c (%.1lf, %.1lf)%c", "\tPoint", index + 1, ':', stack[index].x, stack[index].y, '\n');
    }

    printf("%s", "---------------------------------------------\n");
    
    // Clean up the stack.
    free(stack);

    // Avoid dangling pointers.
    stack = NULL;

    return 0;
}