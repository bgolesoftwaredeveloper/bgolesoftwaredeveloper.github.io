/**
 * @file        :   ball_tree.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a Ball Tree spatial partitioning data structure.
 * @details     :   This source file provides the primary operations required to construct, 
 * print, and clean up a Ball Tree hierarchy. Ball Trees partition data 
 * points into bounding hyperspheres (balls) to facilitate highly 
 * efficient multi-dimensional nearest-neighbor searches and spatial queries.
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Include header files.
#include "darwin/geometry/ball_tree.h"

/**
 * @brief       :   Computes the Euclidean distance between two multi-dimensional points.
 * @details     :   Iterates across every dimension defined in the context to accumulate the
 *                  squared differences between coordinates, returning the final square root.
 * @param       :   reference: Const pointer to the first point structure.
 * @param       :   candidate: Const pointer to the second point structure.
 * @param       :   context: Const pointer to the global configuration containing dimension details.
 * @return      :   double: The computed Euclidean distance between the two points.
 */
static inline double calculate_distance(const Point* reference, const Point* candidate, const Tree* context)
{
    double sum_of_squares = 0.0;

    // Accumulate squared coordinate difference across all available dimensions.
    for (size_t index = 0; index < context->dimension; index++)
    {
        double difference = reference->coordinates[index] - candidate->coordinates[index];

        sum_of_squares += difference * difference;
    }

    return sqrt(sum_of_squares);
}

/**
 * @brief       :   Finds the index of the point furthest from a specified target point.
 * @details     :   Performs a linear scan over a subset of point indices to locate the dataset
 *                  element yielding the maximum Euclidean distance from the target reference.
 * @param       :   indices: Array of index values referencing target data subset elements.
 * @param       :   count: Total count of items present in the indices array.
 * @param       :   dataset: Const pointer to the base continuous array containing all points.
 * @param       :   target: Const pointer to the reference point from which distance is measured.
 * @param       :   context: Const pointer to the global tree structure context details.
 * @return      :   size_t: The relative index location within the 'indices' array of the furthest point.
 */
static inline size_t find_furthest_point(const size_t* indices, size_t count, const Point* dataset, const Point* target, const Tree* context)
{
    // Validate bounds and reference pointers to prevent segmentation faults.
    if (indices == NULL || count == 0 || dataset == NULL || target == NULL || context == NULL)
    {
        return 0;
    }

    size_t furthest_index = indices[0];

    double maximum_distance = -1.0;

    // Linearly process the selection set to isolate the maximum metric distance.
    for (size_t index = 0; index < count; index++)
    {
        size_t current_index = indices[index];

        double current_distance = calculate_distance(&dataset[current_index], target, context);

        if (current_distance > maximum_distance)
        {
            maximum_distance = current_distance;
            furthest_index = index;
        }
    }

    return furthest_index;
}

/**
 * @brief       :   Recursively constructs a hierarchical Ball Tree node from a dataset subset.
 * @details     :   Calculates the centroid and bounding radius of the target dataset subset. If the 
 *                  element count exceeds the maximum leaf capacity, it splits the data by selecting 
 *                  two distance poles and routing elements into child sub-trees.
 * @param       :   indices: Array of dataset index values to be encapsulated by this node.
 * @param       :   count: Total number of indexes inside the tracking tracking collection.
 * @param       :   dataset: Const pointer referencing the underlying structural data matrix points.
 * @param       :   context: Const pointer holding tree capacity metrics and spatial dimensions.
 * @return      :   Tree*: Pointer to the allocated node, or NULL on failure or validation exit.
 */
Node* ball_tree(const size_t* indices, size_t count, const Point* dataset, const Tree* context)
{
    // Validate core configuration layouts before beginning allocation steps.
    if (indices == NULL || count == 0 || dataset == NULL || context == NULL)
    {
        return NULL;
    }

    // Allocate space for the node container.
    Node* node = (Node*)malloc(sizeof(Node));

    if (node == NULL)
    {
        return NULL;
    }

    // Assign default safety configurations to structural elements.    
    node->indices = NULL;
    node->radius = 0.0;
    node->count = 0;
    node->left = NULL;
    node->right = NULL;

    // Allocate memory tracking the center coordinate values of this cluster.
    node->center.coordinates = (double*)calloc(context->dimension, sizeof(double));

    if (node->center.coordinates == NULL)
    {
        free(node);

        return NULL;
    }

    node->center.id = 0;
    node->center.dimension = context->dimension;

    // Phase 1: Accumulate coordinate metrics across every item to deteermine the centroid vector.
    for (size_t index = 0; index < count; index++)
    {
        size_t current_index = indices[index];

        // Ensure target data points are intact and populated.
        if (dataset[current_index].coordinates == NULL)
        {
            free(node->center.coordinates);
            free(node);

            return NULL;
        }

        for (size_t dimension_index = 0; dimension_index < context->dimension; dimension_index++)
        {
            node->center.coordinates[dimension_index] += dataset[current_index].coordinates[dimension_index];
        }
    }

    // Divide structural sums by point volume to yield the precise average centroid values.
    for (size_t dimension_index = 0; dimension_index < context->dimension; dimension_index++)
    {
        node->center.coordinates[dimension_index] /= (double)count;
    }

    // Derive the bounding sphere radius by checking the furthest element distance from the center.
    for (size_t index = 0; index < count; index++)
    {
        size_t current_index = indices[index];
        
        double current_distance = calculate_distance(&dataset[current_index], &node->center, context);

        if (current_distance > node->radius)
        {
            node->radius = current_distance;
        }
    }

    // Convert this into a leaf if count meets capacity limits.
    if (count <= context->capacity)
    {
        node->indices = (size_t*)malloc(sizeof(size_t) * count);

        if (node->indices == NULL)
        {
            free(node->center.coordinates);
            free(node);

            return NULL;
        }

        // Deep-copy indices directly into the tracking payload space.
        memcpy(node->indices, indices, count * sizeof(size_t));

        node->count = count;

        return node;
    }

    // Select split pivots using an approximation of the furthest-point heuristic.
    size_t arbitrary_index = indices[0];

    // Identify two polar opposite points within the current subset boundary.
    size_t north_pole_pivot = indices[find_furthest_point(indices, count, dataset, &dataset[arbitrary_index], context)];
    size_t south_pole_pivot = indices[find_furthest_point(indices, count, dataset, &dataset[north_pole_pivot], context)];

    // Allocate partitioning containers for left and right binary routing splits.
    size_t* left_indices = (size_t*)malloc(sizeof(size_t) * count);
    size_t* right_indices = (size_t*)malloc(sizeof(size_t) * count);

    if (left_indices == NULL || right_indices == NULL)
    {
        free(left_indices);
        free(right_indices);
        free(node->center.coordinates);
        free(node);

        return NULL;
    }

    size_t left_count = 0;
    size_t right_count = 0;

    // Distribute remaining dataset coordinates based on closest geometric proximity to either pole.
    for (size_t index = 0; index < count; index++)
    {
        size_t current_point_index = indices[index];

        double distance_to_left_boundary = calculate_distance(&dataset[current_point_index], &dataset[north_pole_pivot], context);
        double distance_to_right_boundary = calculate_distance(&dataset[current_point_index], &dataset[south_pole_pivot], context);

        if (distance_to_left_boundary < distance_to_right_boundary)
        {
            left_indices[left_count++] = current_point_index;
        }
        else
        {
            right_indices[right_count++] = current_point_index;
        }
    }

    // If a split produces empty clusters, fallback to creating an unsplit leaf.
    if (left_count == 0 || right_count == 0)
    {
        node->indices = (size_t*)malloc(sizeof(size_t) * count);

        if (node->indices == NULL)
        {
            free(left_indices);
            free(right_indices);
            free(node->center.coordinates);
            free(node);

            return NULL;
        }
        else
        {
            memcpy(node->indices, indices, count * sizeof(size_t));

            node->count = count;
        }

        free(left_indices);
        free(right_indices);

        return node;
    }

    // Recursively build down the hierarchical tree structure.
    node->left = ball_tree(left_indices, left_count, dataset, context);
    node->right = ball_tree(right_indices, right_count, dataset, context);

    // Free local transient tracking matrices used during split.
    free(left_indices);
    free(right_indices);

    return node;
}

/**
 * @brief       :   Displays the structural details of a Ball Tree node recursively.
 * @details     :   Traverses the tree structure in-order, applying tab structures relative 
 *                  to tree depth to visualize the nested cluster structure in the terminal.
 * @param       :   node: Const pointer tracking the target tree branch node.
 * @param       :   depth: Current layer depth indexing level used to align output text tabs.
 */
void print(const Node* node, size_t depth)
{
    if (node == NULL)
    {
        return;
    }

    // Indent tracking elements using layout depth values.
    for (size_t index = 0; index < depth; index++)
    {
        printf("%c", '\t');
    }

    // Output target boundary configurations cleanly.
    printf("%s%.2lf%s", "Radius:[", node->radius, "][");

    // Render multi-dimensional vector elements sequentially.
    for (size_t dimension_index = 0; dimension_index < node->center.dimension; dimension_index++)
    {
        (dimension_index < node->center.dimension - 1) ? printf("%.2lf, ", node->center.coordinates[dimension_index]) :
            printf("%.2lf", node->center.coordinates[dimension_index]);
    }

    printf("%s", "]\n");

    // Recursively step into nested child node elements.
    print(node->left, depth + 1);
    print(node->right, depth + 1);
}

void clean(Node* node)
{
    if (node == NULL)
    {
        return;
    }

    // Recurse into child layers first to clear allocations from the bottom up.
    clean(node->left);
    clean(node->right);

    // Free the tracking array allocated for center dimension vectors.
    if (node->center.coordinates != NULL)
    {
        free(node->center.coordinates);
    }

    // Free structural index allocations on leaf data boundaries.
    if (node->indices != NULL)
    {
        free(node->indices);
    }

    // Deallocate the actual parent structural shell layer wrapper space.
    free(node);
}