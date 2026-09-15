/**
 * @file        :   quad_tree.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a 2D spatial-partitioning point Quadtree API.
 * @details     :   This file contains core algorithms for managing a Point Quadtree structure,
 * used for hierarchical 2D spatial representation. It exposes interfaces for 
 * node initialization, defensive point insertion with multi-subdivision control, 
 * efficient logarithmic radial range querying, and recursive memory deallocation.
 * @version     :   1.0
 * @date        :   2026-06-05
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include header files.
#include "darwin/tree/quad_tree.h"

/**
 * @brief       :   Determines whether an arbitrary 2D coordinate falls inside a regional boundary.
 * @details     :   Evaluates point membership using closed interval inequalities against the
 *                  axis-aligned bounding box boundaries extrapolated via half-dimensions.
 * @param       :   boundary The regional tracking frame defining the center and half-dimension offsets.
 * @param       :   point The exact 2D coordinates targeted for enclosure mapping validation.
 * @return      :   true if the point resides completely within the boundary borders, false otherwise.
 */
static inline bool contains(Boundary boundary, Point point)
{
    return (point.x >= boundary.x - boundary.half_width &&
            point.x <= boundary.x + boundary.half_width &&
            point.y >= boundary.y - boundary.half_height &&
            point.y <= boundary.y + boundary.half_height);
}

/**
 * @brief       :   Validates structural equality between two 2D coordinates using an absolute tolerance.
 * @details     :   Mitigates precision errors by comparing absolute coordinate differentials
 *                  against a fixed geometric configuration threshold (EPSILON).
 * @param       :   compare The source data point evaluating spatial variation.
 * @param       :   against The target data point being benchmarked against the source.
 * @return      :   true if both components converge within the epsilon tolerance, false otherwise.
 */
static inline bool equal(Point compare, Point against)
{
    return (fabs(compare.x - against.x) < EPSILON && fabs(compare.y - against.y) < EPSILON);
}

/**
 * @brief       :   Checks whether a circular target region intersects an axis-aligned bounding box.
 * @details     :   Clamps the circle center coordinates to the closest edge boundaries of the 
 *                  box to evaluate the minimal squared Euclidean distance against the squared radius.
 * @param       :   boundary The rectangular node quadrant regional boundary definition frame.
 * @param       :   center The central 2D spatial anchor of the radial search query.
 * @param       :   radius The outer reach boundary threshold of the localized radial search circle.
 * @return      :   true if the circular zone overlaps or touches the boundary region, false otherwise.
 */
static inline bool intersects_circle(Boundary boundary, Point center, double radius)
{
    // Clamp coordinates to extract the ckosest point on the bounding box to the circle center.
    double delta_x = center.x - fmax(boundary.x - boundary.half_width, fmin(center.x, boundary.x + boundary.half_width));
    double delta_y = center.y - fmax(boundary.y - boundary.half_height, fmin(center.y, boundary.y + boundary.half_height));

    // Intersection occurs if the squared distance to teh closest point is within the squared radius.
    return (delta_x * delta_x + delta_y * delta_y) <= (radius * radius);
}

/**
 * @brief       :   Splits a quadtree node into four equal sub-quadrants.
 * @details     :   Halves the geometric half-dimensions of the parent bounding frame and positions 
 *                  four newly initialized nodes centered across the cardinal sub-regions.
 * @param       :   node Pointer to the target node targeted for quadrant subdivision.
 */
static inline void subdivide(Node* node)
{
    double x = node->boundary.x;
    double y = node->boundary.y;
    double half_width = node->boundary.half_width / 2.0;
    double half_height = node->boundary.half_height / 2.0;

    // Instantiate children nodes spanning the specific sub-quadrant partitions.
    node->north_west = initialize((Boundary){x - half_width, y + half_height, half_width, half_height});
    node->north_east = initialize((Boundary){x + half_width, y + half_height, half_width, half_height});
    node->south_west = initialize((Boundary){x - half_width, y - half_height, half_width, half_height});
    node->south_east = initialize((Boundary){x + half_width, y - half_height, half_width, half_height});

    // Toggle division flag indicating parent node is now serving as an interior branch point.
    node->divided = true;
}

/**
 * @brief       :   Allocates and sets baseline variables for a new quadtree node instance.
 * @details     :   Requests heap space for structural storage, maps regional boundaries, 
 *                  and sets all point storage elements and structural child pointers to null fields.
 * @param       :   boundary The bounding box constraints governing the spatial segment of the node.
 * @return      :   Pointer to the newly instantiated Node, or NULL on memory allocation failure.
 */
Node* initialize(Boundary boundary)
{
    // Request structural slice from system memory manager.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Bind boundary layout dimensions and operational baselines.
    new_node->boundary = boundary;
    new_node->point = NULL;
    new_node->divided = false;
    
    // Default child pointer chains to empty states.
    new_node->north_west = NULL;
    new_node->north_east = NULL;
    new_node->south_west = NULL;
    new_node->south_east = NULL;

    return new_node;
}

/**
 * @brief       :   Recursively descends the tree topology to locate an optimal leaf node slot for data insertion.
 * @details     :   Handles point allocation, duplicate data screening, localized spatial subdivision, 
 *                  and existing data displacement down to appropriate sub-quadrants.
 * @param       :   node Pointer to the active tracking node processing spatial placement.
 * @param       :   point The absolute target 2D coordinates being mapped into the tree framework.
 * @param       :   depth The running recursion iteration level tracking hierarchical distribution height.
 * @return      :   true if the point is successfully sorted into a leaf node, false if rejected or depth limit exceeded.
 */
static inline bool insert_recursive(Node* node, Point point, size_t depth)
{
    // Halt insertion if the target node is null of if the point lies outside the regional parameters.
    if (node == NULL || !contains(node->boundary, point))
    {
        return false;
    }

    // Prevent call stack crashes by stopping at the structural depth threshold.
    if (depth >= (size_t)MAXIMUM_TREE_DEPTH)
    {
        return false;
    }

    // The node is an empty leaf, save data directly.
    if (node->point == NULL && !node->divided)
    {
        node->point = (Point*)malloc(sizeof(Point));

        if (node->point == NULL)
        {
            return false;
        }

        *(node->point) = point;

        return true;
    }

    // Point convergence matches an existing point, block duplicated data cyclce.
    if (node->point != NULL && equal(*(node->point), point))
    {
        return false;
    }

    // Node is crowded and undivided: Trigger spatial subdivision.
    if (!node->divided)
    {
        subdivide(node);

        // Revert insertion attemps if sub-allocations failed due to low memory states.
        if (!node->divided)
        {
            return false;
        }

        // Extract cached node  data point to push it downwards into a child frame.
        Point existing = *(node->point);

        free(node->point);

        node->point = NULL;

        // Cascade existing parent point element down into a newly created sub-quadrant.
        insert_recursive(node->north_west, existing, depth + 1);
        insert_recursive(node->north_east, existing, depth + 1);
        insert_recursive(node->south_west, existing, depth + 1);
        insert_recursive(node->south_east, existing, depth + 1);
    }

    // Route target point insertion towards matching child subdivisions.
    if (insert_recursive(node->north_west, point, depth + 1))
    {
        return true;
    }

    if (insert_recursive(node->north_east, point, depth + 1))
    {
        return true;
    }

    if (insert_recursive(node->south_west, point, depth + 1))
    {
        return true;
    }

    if (insert_recursive(node->south_east, point, depth + 1))
    {
        return true;
    }

    return false;
}

/**
 * @brief       :   Public entry point wrapper to trigger the recursive insertion mechanism.
 * @details     :   Filters out corrupted tracking point entries containing NaN coordinates 
 *                  before running the core recursive insertion function at root depth.
 * @param       :   node Pointer to the baseline root node governing the primary quadtree.
 * @param       :   point The exact target 2D coordinates being mapped into the spatial tree.
 * @return      :   true if insertion and quadrant sorting succeed, false on data error or rejection.
 */
bool insert(Node* node, Point point)
{
    // Intercept floating point undefined states to protect downstreram arithmetic.
    if (isnan(point.x) || isnan(point.y))
    {
        return false;
    }

    return insert_recursive(node, point, 0);
}

/**
 * @brief       :   Harvests all points stored within the tree that overlap a circular query range.
 * @details     :   Prunes entire tree branches early if regional boundaries do not intersect 
 *                  the query circle, optimizing the coordinate search to logarithmic time.
 * @param       :   node Pointer to the current parent or child branch node scanning local bounds.
 * @param       :   center The absolute central 2D anchor point of the search selection region.
 * @param       :   radius The linear constraint limit expanding outward from the search center.
 * @param       :   results Target result array tracking memory references allocated to receive valid points.
 * @param       :   maximum The buffer tracking ceiling index protecting array storage limits from overflow.
 * @param       :   count Active sequential match tracking counter tracking index placements across recursions.
 * @return      :   The aggregated total count of points collected inside the results array.
 */
size_t query_range_circle(Node* node, Point center, double radius, Point* results, size_t maximum, size_t count)
{
    // Halt evaluation if current scope is invalid or if target output space is saturated.
    if (node == NULL || results == NULL || count >= maximum)
    {
        return count;
    }

    // Optimization step: Ignore this entire branch if the region does not intersect the query area.
    if (!intersects_circle(node->boundary, center, radius))
    {
        return count;
    }

    // Process local node point if filled.
    if (node->point != NULL)
    {
        double delta_x = node->point->x - center.x;
        double delta_y = node->point->y - center.y;

        // Verify if point falls inside search contrainsts via standard Pythagorean evaluation.
        if ((delta_x * delta_x + delta_y * delta_y) <= (radius * radius))
        {
            results[count] = *(node->point);
            count++;

            // Early termination: Exit if the result array buffer has become fully saturated.
            if (count >= maximum)
            {
                return count;
            }
        }
    }

    // Recurse into lower sub-quadrants if teh current node is subdivided.
    if (node->divided)
    {
        count = query_range_circle(node->north_west, center, radius, results, maximum, count);
        count = query_range_circle(node->north_east, center, radius, results, maximum, count);
        count = query_range_circle(node->south_west, center, radius, results, maximum, count);
        count = query_range_circle(node->south_east, center, radius, results, maximum, count);
    }

    return count;
}

/**
 * @brief       :   Recursively descends through all sub-quadrants to deallocate node memory.
 * @details     :   Uses a post-order traversal model to free dynamic points, sub-quadrant nodes, 
 *                  and parent structures safely without leaving stranded memory allocations.
 * @param       :   node Pointer to the active target parent branch structural node slated for removal.
 */
void clean(Node* node)
{
    // Terminate traversal upon reaching the boundary coordinates of empty tree slots.
    if (node == NULL)
    {
        return;
    }

    // Deallocate local leaf point reference structure if active.
    if (node->point != NULL)
    {
        free(node->point);

        node->point = NULL;
    }

    // Cascade deletion updates down across all child quadrant branches
    clean(node->north_west);
    clean(node->north_east);
    clean(node->south_west);
    clean(node->south_east);

    // Free the local structural container allocation.
    free(node);
}