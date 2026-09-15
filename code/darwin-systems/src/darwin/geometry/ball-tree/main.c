/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test runner implementation for the Ball Tree geometric structure.
 * @details     :   This program instantiates a multi-dimensional coordinate dataset, 
 * constructs a spatial partitioning Ball Tree via recursive binary 
 * routing, and prints the hierarchical bounding regions to stdout.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/geometry/ball-tree/main.c ./src/darwin/geometry/ball-tree/ball_tree.c -I./include -o ./bin/ball_tree_test
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/geometry/ball_tree.h"

int main(void)
{
    // Initialize the context.
    Tree context;

    // Assign the fields values.
    context.dimension = 2;
    context.capacity = 2;

    size_t count = 5;

    Point dataset[5];

    // Define the coordinates to work with.
    double coordinates[5][2] =
    {
        {1.0, 2.0},
        {1.5, 1.8},
        {5.0, 8.0},
        {6.0, 7.0},
        {2.0, 3.0}
    };

    // Add coordinates.
    for (size_t index = 0; index < count; index++)
    {
        dataset[index].id = index;
        dataset[index].dimension = context.dimension;
        dataset[index].coordinates = coordinates[index];
    }

    // Allocate  the indices.
    size_t* indices = (size_t*)malloc(sizeof(size_t) * count);

    if (indices == NULL)
    {
        return 1;
    }

    for (size_t index = 0; index < count; index++)
    {
        indices[index] = index;
    }

    printf("%s", "Ball Tree:\n");

    // Execute the ball-tree algorithm
    Node* root = ball_tree(indices, count, dataset, &context);

    if (root != NULL)
    {
        // Summary.
        print(root, 0);

        // Clean up.
        clean(root);
    }

    free(indices);

    return 0;
}