/**
 * @file        :   k_nearest_neighbors.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a K-Nearest Neighbors (KNN) classifier.
 * @details     :   This module implements a dynamic K-Nearest Neighbors 
 *                  classification algorithm in Standard C. Features include:
 *                  - Dynamic allocation and deep-cleanup handling for 
 *                    arbitrary feature-space datasets.
 *                  - Euclidean distance calculation optimized with inline 
 *                    looping over feature vectors.
 *                  - Bounded $O(K)$ insertion sort to track top-K nearest 
 *                    neighbors without requiring full dataset sorts.
 *                  - Dynamic vote-tallying for multi-class predictions.
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

// Include header files.
#include "../include/k_nearest_neighbors.h"

/**
 * @brief       :   Computes the Euclidean distance between two feature vectors.
 * @details     :   Calculates the straight-line distance in Euclidean space 
 *                  by taking the square root of the sum of squared differences 
 *                  across all dimensions. Marked inline for high performance.
 * @param       :   compare: The query feature vector.
 * @param       :   against: The target point's feature vector.
 * @param       :   number_of_features: The dimensionality of both vectors.
 * @return      :   double: The computed Euclidean distance score.
 */
static inline double euclidean_distance(const double* compare, const double* against, size_t number_of_features)
{
    double sum = 0.0;

    for (size_t index = 0; index < number_of_features; index++)
    {
        double difference = compare[index] - against[index];

        sum += difference * difference;
    }

    return sqrt(sum);
}

/**
 * @brief       :   Updates the top-K neighbor candidate tracking array.
 * @details     :   Inserts a new candidate distance and class label into an 
 *                  in-place bounded array. Keeps the array sorted in ascending 
 *                  order by distance so the $K$-th worst neighbor is always 
 *                  located at index `k - 1`.
 * @param       :   neighbors: The array tracking current nearest neighbors.
 * @param       :   k: The total capacity of nearest neighbors to track.
 * @param       :   label: The class label identifier of the candidate point.
 * @param       :   distance: The measured Euclidean distance to the query.
 * @return      :   void
 */
static inline void update_top_k(Neighbor* neighbors, int k, int label, double distance)
{
    // Ignore candidate if its distance is greater than or equal to current maximum distance in top-K.
    if (distance >= neighbors[k - 1].distance)
    {
        return;
    }

    // Replace the farthest neighbor at index K - 1.
    neighbors[k - 1].label = label;
    neighbors[k - 1].distance = distance;

    // Shift candidate leftwards until array order is restored by ascending distance.
    for (int index = k - 1; index > 0; index--)
    {
        if (neighbors[index].distance < neighbors[index - 1].distance)
        {
            Neighbor temp = neighbors[index];

            neighbors[index] = neighbors[index - 1];
            neighbors[index - 1] = temp;
        }
        else
        {
            break;
        }
    }
}

/**
 * @brief       :   Allocates and initializes a dataset container.
 * @details     :   Performs contiguous memory allocation for the Dataset 
 *                  struct, the array of Point instances, and feature arrays 
 *                  for each individual point. Includes deep-cleanup loops to 
 *                  prevent memory leaks if allocation fails mid-process.
 * @param       :   number_of_points: Total quantity of instances to allocate.
 * @param       :   number_of_features: Dimension size of each feature vector.
 * @return      :   Dataset*: Pointer to initialized Dataset, or NULL on failure.
 */
Dataset* initialize_dataset(size_t number_of_points, size_t number_of_features)
{   
    // Allocate the primary dataset controller structure.
    Dataset* dataset = (Dataset*)malloc(sizeof(Dataset));

    if (dataset == NULL)
    {
        return NULL;
    }

    dataset->number_of_points = number_of_points;
    dataset->number_of_features = number_of_features;

    // Allocate continuous array of point instances.
    dataset->points = (Point*)malloc(sizeof(Point) * number_of_points);

    if (dataset->points == NULL)
    {
        free(dataset);

        return NULL;
    }

    // Allocate memory for feature vectors attached to each point.
    for (size_t index = 0; index < number_of_points; index++)
    {
        dataset->points[index].features = (double*)malloc(sizeof(double) * number_of_features);

        if (dataset->points[index].features == NULL)
        {   
            // Unwind and free all previously allocated feature arrays to prevent leaks.
            for (size_t previous = 0; previous < index; previous++)
            {
                free(dataset->points[previous].features);
            }

            free(dataset->points);
            free(dataset);

            return NULL;
        }
    }

    return dataset;
}

/**
 * @brief       :   Predicts the class label for an input feature vector.
 * @details     :   Measures Euclidean distance from the query features against 
 *                  all dataset points, tracks the $K$ closest instances using 
 *                  a bounded array, performs majority-vote tallying across 
 *                  candidate classes, and returns the highest-voted class index.
 * @param       :   dataset: Pointer to the training dataset instance.
 * @param       :   features: Query feature vector array to classify.
 * @param       :   k: Number of nearest neighbors to evaluate.
 * @param       :   number_of_classes: Total number of target target classes.
 * @return      :   int: Predicted class index, or -1 on allocation failure.
 */
int predict(const Dataset* dataset, const double* features, int k, int number_of_classes)
{
    // Clamp K to the total dataset size if K exceeds total available points.
    if (k > (int)dataset->number_of_points)
    {
        k = (int)dataset->number_of_points;
    }

    // Allocate local tracking buffer for top-K candidates.
    Neighbor* k_neighbors = (Neighbor*)malloc(sizeof(Neighbor) * k);

    if (k_neighbors == NULL)
    {
        return -1;
    }

    // Initialize nearest neighbor distances to infinity.
    for (int index = 0; index < k; index++)
    {
        k_neighbors[index].label = -1;
        k_neighbors[index].distance = DBL_MAX;
    }

    // Measure distance to every point in the dataset.
    for (size_t index = 0; index < dataset->number_of_points; index++)
    {
        double distance = euclidean_distance(features, dataset->points[index].features, dataset->number_of_features);

        update_top_k(k_neighbors, k, dataset->points[index].label, distance);
    }

    // Allocate vote tallying array zero-initialized using calloc.
    int* votes = (int*)calloc(number_of_classes, sizeof(int));

    if (votes == NULL)
    {
        free(k_neighbors);

        return -1;
    }

    // Tally votes from nearest K neighbors.
    for (int index = 0; index < k; index++)
    {
        if (k_neighbors[index].label >= 0 && k_neighbors[index].label < number_of_classes)
        {
            votes[k_neighbors[index].label]++;
        }
    }

    int best_class = 0;
    int maximum_votes = -1;

    // Find class with majority vote.
    for (int class_index = 0; class_index < number_of_classes; class_index++)
    {
        if (votes[class_index] > maximum_votes)
        {
            maximum_votes = votes[class_index];
            best_class = class_index;
        }
    }

    // Free local operational buffers.
    free(k_neighbors);
    free(votes);

    return best_class;
}

/**
 * @brief       :   Frees all dynamic memory associated with a Dataset.
 * @details     :   Iterates through each allocated Point within the Dataset to 
 *                  free individual feature vector arrays before freeing the 
 *                  Point array and parent Dataset struct itself.
 * @param       :   dataset: Pointer to the Dataset instance to destroy.
 * @return      :   void
 */
void destroy_dataset(Dataset* dataset)
{
    // Guard against NULL pointer dereferences.
    if (dataset == NULL)
    {
        return;
    }

    // Free individual feature buffers allocated per point.
    for (size_t index = 0; index < dataset->number_of_points; index++)
    {
        free(dataset->points[index].features);
    }

    // Free primary point container array and parent controller structure.
    free(dataset->points);
    free(dataset);
}