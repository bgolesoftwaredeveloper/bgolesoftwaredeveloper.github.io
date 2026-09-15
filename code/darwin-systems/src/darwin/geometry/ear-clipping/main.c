/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Program driver for testing and validating the Ear Clipping triangulation engine.
 * @details     :   Serves as the entry point to execute the Ear Clipping algorithm on a
 * pre-defined concave L-shaped polygon. It passes the vertex sequence
 * to the backend geometry engine, receives the decomposed non-overlapping 
 * triangle data buffers, and prints the individual structural node properties
 * (Origin, Pivot, Apex) directly to standard output.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/geometry/ear-clipping/main.c ./src/darwin/geometry/ear-clipping/ear_clipping.c -I./include -o ./bin/ear_clipping_test
 * @version     :   1.0
 * @date        :   2026-06-04
 */
#include <stdio.h>

// Include header files.
#include "darwin/geometry/ear_clipping.h"

int main(void)
{
    // Define a simple concave L-shaped polygon wiht 6 vertices.
    Vertex concave_polygon[] =
    {
        {0.0, 0.0},
        {5.0, 0.0},
        {5.0, 2.0},
        {2.0, 2.0},
        {2.0, 5.0},
        {0.0, 5.0}
    };

    // Calculate the number of vertices.
    size_t vertex_count = sizeof(concave_polygon) / sizeof(concave_polygon[0]);

    // Define the maximum amount of triangles.
    size_t maximum_triangles = vertex_count - 2;

    // Define the output array to display triangles attributes.
    Triangle output[maximum_triangles];

    // Keep track of how many triangles there are so we can iterate over the collection.
    size_t triangle_count = 0;

    // Perform ear clipping.
    bool success = ear_clipping(concave_polygon, vertex_count, output, &triangle_count);

    if (success)
    {
        // Summary. 
        printf("%s", "Triangulation successful!\n");

        for (size_t index = 0; index < triangle_count; index++)
        {
            printf("%s [%zu]%c", "\tTriangle", index + 1, '\n');
            printf("%s (%3.1lf, %3.1lf)%c", "\t\tOrigin:", output[index].origin.x, output[index].origin.y, '\n');
            printf("%s (%3.1lf, %3.1lf)%c", "\t\tPivot:", output[index].pivot.x, output[index].pivot.y, '\n');
            printf("%s (%3.1lf, %3.1lf)%c", "\t\tApex:", output[index].apex.x, output[index].apex.y, '\n');
            printf("%c", '\n');
        }
    }
    else
    {
        printf("%s", "Error triangulation failed.\n");
    }

    return 0;
}