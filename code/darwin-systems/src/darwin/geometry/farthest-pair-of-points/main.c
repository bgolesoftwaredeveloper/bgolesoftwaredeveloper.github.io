/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test runner executable for the Farthest Pair of Points algorithm.
 * @details     :   Validates the performance and accuracy of the O(n log n) Convex Hull 
 * and Rotating Calipers pipeline using an arbitrary sample point cluster.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/geometry/farthest-pair-of-points/main.c ./src/darwin/geometry/farthest-pair-of-points/farthest_pair_of_points.c -I./include -o ./bin/farthest_pair_of_points_test
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#include <stdio.h>

// Include header files.
#include "darwin/geometry/farthest_pair_of_points.h"

int main(void)
{
    // Establish an arbitrary geometric point cluster C contianing sample nodes.
    Point points[] =
    {
        {0.0, 0.0}, {1.0, 2.0}, {2.0, 1.0}, {1.1, 1.1},
        {4.0, 4.0}, {0.0, 4.0}, {3.0, 0.0}, {1.5, 2.5}
    };

    // Calculate the total population count of coordinate elements inside the raw array.
    size_t number_of_points = sizeof(points) / sizeof(points[0]);

    // Compute the absolute farthest pair using the pipeline execution validity flags.
    Pair result = farthest_pair(points, number_of_points);

    if (result.is_valid)
    {
        // Summmary.
        printf("%s", "Farthest pair details:\n");
        printf("%c%s (%.2lf, %.2lf)%c", '\t', "Origin:", result.origin.x, result.origin.y, '\n');
        printf("%c%s (%.2lf, %.2lf)%c", '\t', "Termination:", result.termination.x, result.termination.y, '\n');
        printf("%c%s %.4lf%c", '\t', "Maximum absolute Euclidean distance:", result.distnace, '\n');
    }
    else
    {
        return 1;
    }

    return 0;
}