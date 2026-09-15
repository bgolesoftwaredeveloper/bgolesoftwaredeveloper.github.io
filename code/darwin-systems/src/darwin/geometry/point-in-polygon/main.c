/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Program driver for testing point-in-polygon geometry logic.
 * @details     :   Serves as the entry point to execute and validate the 
 * ray-casting algorithm implementation. It defines a static 
 * polygon tracking zone and passes explicit test probes to 
 * verify inside versus outside boundary logic.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/geometry/point-in-polygon/main.c ./src/darwin/geometry/point-in-polygon/point_in_polygon.c -I./include -o ./bin/point_in_polygon_test
 * @version     :   1.0
 * @date        :   2026-06-04
 */
#include <stdio.h>

// Include header files.
#include "darwin/geometry/point_in_polygon.h"

int main(void)
{
    // Define a 10x10 square polygon tracking zone starting at the origin (0, 0).
    Point tracking_zone[] =
    {
        {0.0, 0.0},
        {10.0, 0.0},
        {10.0, 10.0},
        {0.0, 10.0}
    };

    // Calculate the number of vertices in the tracking zone array.
    size_t zone_vertex_count = sizeof(tracking_zone) / sizeof(tracking_zone[0]);

    // Setup coordinate targets for boundary validation.
    Point interior_probe = {5.6, 5.6};
    Point exterior_probe = {12.0, 5.0};

    // Evaluate the interior probe target.
    if (is_point_in_polygon(interior_probe, tracking_zone, zone_vertex_count))
    {
        printf("%s", "Status: INSIDE (Target detected within bounds).\n");
    }
    else
    {
        printf("%s", "Status: OUTSIDE (Target out of bounds).\n");
    }

    printf("%c", '\n');

    printf("%s", "Probe location (12.0, 5.0):\n");

    // Evaluate the exterior orobe target.
    if (is_point_in_polygon(exterior_probe, tracking_zone, zone_vertex_count))
    {
        printf("%s", "Status: INSIDE (Target detected within bounds).\n");
    }
    else
    {
        printf("%s", "Status: OUTSIDE (Target out of bounds).\n");
    }

    return 0;
}