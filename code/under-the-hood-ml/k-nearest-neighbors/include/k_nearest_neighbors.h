/**
 * @file        :   k_nearest_neighbors.h
 * @author      :   Braiden Gole
 * @brief       :   Header definitions for a K-Nearest Neighbors (KNN) classifier.
 * @details     :   This module provides the data structures and function 
 *                  prototypes for a K-Nearest Neighbors classification 
 *                  algorithm in C. It supports dynamic dataset memory 
 *                  allocation, bounded neighbor tracking using Euclidean 
 *                  distance, and majority-vote class predictions.
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#ifndef K_NEAREST_NEIGHBORS_H
#define K_NEAREST_NEIGHBORS_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct      :   Point
 * @brief       :   Represents a single data instance in the feature space.
 * @details     :   Holds a class label identifier alongside an allocated 
 *                  array of continuous floating-point feature values.
 */
typedef struct
{   
    int label;
    double* features;
} Point;

/**
 * @struct      :   Dataset
 * @brief       :   The primary container for training instances.
 * @details     :   Tracks the total quantity of data points, dimensionality 
 *                  of the feature space, and manages the continuous array of 
 *                  Point structures.
 */
typedef struct
{
    size_t number_of_points;
    size_t number_of_features;

    Point* points;
} Dataset;

/**
 * @struct      :   Neighbor
 * @brief       :   Auxiliary structure used to evaluate candidate neighbors.
 * @details     :   Pairs a distance metric score with a class label during 
 *                  the top-K search phase of prediction.
 */
typedef struct
{   
    int label;
    double distance;
} Neighbor;

// Function prototypes.
Dataset* initialize_dataset(size_t number_of_points, size_t number_of_features);
int predict(const Dataset* dataset, const double* features, int k, int number_of_classes);
void destroy_dataset(Dataset* dataset);

#endif