/**
 * @file        :   point_in_polygon.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of 2D Point-in-Polygon (PIP) intersection checking.
 * @details     :   Contains the functional logic for the ray-casting algorithm.
 * By processing sequential vertex segments, this routine counts
 * horizontal boundary crossings to determine geometric containment.
 * @version     :   1.0
 * @date        :   2026-06-04
 */
#include <stdio.h>

// Include header files.
#include "darwin/geometry/point_in_polygon.h"

/**
 * @brief Evaluates if a 2D coordinate target resides inside a closed polygon boundary.
 * @details Casts an infinite ray horizontally to the right from the target point.
 *          Each time the ray crosses a polygon edge, the containment state flips.
 *          An odd number of total intersections confirms the point is inside.
 * @param point The target coordinate profile to test.
 * @param polygon Array of vertices forming the closed perimeter.
 * @param size The total vertex count of the polygon perimeter array.
 * @return True if the point resides inside the polygon bounds, false otherwise.
 */
bool is_point_in_polygon(Point point, Point polygon[], size_t size)
{
    // Track the odd/even intersection state parity (false = even/outside, true = odd/inside).
    bool inside = false;

    // Loop through each edge segment.
    for (size_t index = 0, previous = size - 1; index < size; previous = index++)
    {
        // Check if the test point's Y-level falls stictly between the vertical span of the current edge.
        bool intersects_y = ((polygon[index].y > point.y) != (polygon[previous].y > point.y));
        
        if (intersects_y)
        {
            // Use linear interpolation to find the exact X-coordinate where the edge crosses point.y.
            double intersect_x =  (polygon[previous].x - polygon[index].x) * (point.y - polygon[index].y) / 
                                 (polygon[previous].y - polygon[index].y) + 
                                 polygon[index].x;
            
            // If the calculated intersection point happens to the righ tof the test point, the ray has crossed.
            if (point.x < intersect_x)
            {
                // Flip the tracking boolean to invert containment status (Event -> Odd -> Even).
                inside = !inside;
            }
        }
    }

    return inside;
}