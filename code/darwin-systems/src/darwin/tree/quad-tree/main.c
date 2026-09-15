/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Algorithmic demonstration of a Point Quadtree.
 * @details     :   Provides a direct spatial execution visualization showing how
 * points are filtered, rejected, or localized inside a 2D grid.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/tree/quad-tree/main.c ./src/darwin/tree/quad-tree/quad_tree.c -I./include -o ./bin/quad_tree_test
 * @version     :   1.0
 * @date        :   2026-06-05
 */
#include <stdio.h>

// Include header files.
#include "darwin/tree/quad_tree.h"

int main(void)
{
    // Configure the spatial domain: Centered at (0, 0), expanding outward.
    Boundary root_boundary = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    // Initialize the primary root canvas.
    Node* root = initialize(root_boundary);

    if (root == NULL)
    {
        return 1;
    }

    // Dataset demonstrating a mix of valid spaces, identical coordinates.
    Point points[] =
    {
        { .x = 25.5,  .y = 30.2  },
        { .x = -45.0, .y = 60.8  },
        { .x = -10.2, .y = -12.4 },
        { .x = 85.1,  .y = -50.0 },
        { .x = 25.5,  .y = 30.2  },
        { .x = 150.0, .y = 45.0  }
    };

    // Calculate the number of points.
    size_t number_of_points = sizeof(points) / sizeof(points[0]);

    // Insert the points.
    for (size_t index = 0; index < number_of_points; index++)
    {
        bool success = insert(root, points[index]);

        printf("%s %zu (%.1lf, %.1lf) -> %s%c", "Point", index + 1, 
            points[index].x, points[index].y, success ? "SUCCESS" : "REJECTED", '\n');
    }

    // Demonstrate radial query pruning.
    Point search_center = { .x = 20.0, .y = 20.0 };

    double search_radius = 15.0;

    Point results[6];

    size_t maximum_capacity = 6;

    // Perform a query.
    size_t count = query_range_circle(root, search_center, search_radius, results, maximum_capacity, 0);

    printf("%s %.1lf %s (%.1lf, %.1lf)%c", "Searching within a radius of", search_radius, "around", search_center.x, search_center.y, '\n');

    // Print matched results from the localized query.
    for (size_t index = 0; index < count; index++)
    {
        printf("%c%s (%.1lf, %1.lf)%c", '\t', "-> Found Point:", results[index].x, results[index].y, '\n');
    }

    // Clean up.
    clean(root);

    // Avoid dangling pointers.
    root = NULL;

    return 0;
}