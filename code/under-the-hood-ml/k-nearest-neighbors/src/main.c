/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test driver for the K-Nearest Neighbors (KNN) classifier.
 * @details     :   This program demonstrates initialization, population, query 
 *                  classification, and cleanup for a 2D feature-space dataset 
 *                  using the KNN algorithm. It populates two distinct clusters 
 *                  representing Class 0 and Class 1, evaluates query vectors 
 *                  against them, and outputs predicted labels.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/main.c ./src/k_nearest_neighbors.c -I./include -o ./bin/k_nearest_neighbor_test
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#include <stdio.h>

// Include hearder files.
#include "../include/k_nearest_neighbors.h"

int main(void)
{
    // Define parameters: 6 data points, 2D space (x, y), 2 classes.
    size_t number_of_points = 6;
    size_t number_of_features = 2;
    
    int number_of_classes = 2;
    int k = 3;

    // Allocate memory for the training dataset container and internal point buffers.
    Dataset* dataset = initialize_dataset(number_of_points, number_of_features);

    if (dataset == NULL)
    {
        return 1;
    }

    // Populate class 0 training points (clusted around (1.0, 1.0)).
    dataset->points[0].label = 0;

    dataset->points[0].features[0] = 1.0;
    dataset->points[0].features[1] = 1.1;

    dataset->points[1].label = 0;
    
    dataset->points[1].features[0] = 1.2; 
    dataset->points[1].features[1] = 0.9;

    dataset->points[2].label = 0;

    dataset->points[2].features[0] = 0.8; 
    dataset->points[2].features[1] = 1.0;

    // Populate class 1 training points (Clusted around (5.0, 5.0)).
    dataset->points[3].label = 1;

    dataset->points[3].features[0] = 5.0; 
    dataset->points[3].features[1] = 5.2;

    dataset->points[4].label = 1;

    dataset->points[4].features[0] = 4.8; 
    dataset->points[4].features[1] = 4.9;

    dataset->points[5].label = 1;

    dataset->points[5].features[0] = 5.3;
    dataset->points[5].features[1] = 5.1; 

    // Define test query feature vectors to classify.
    double class_zero[2] = {1.1, 1.0};
    double class_one[2] = {4.9, 5.1};

    // Predict class for query 1 (Expected: class 0).
    int result = predict(dataset, class_zero, k, number_of_classes);

    if (result != -1)
    {
        printf("%s %d%c", "Query point (1.1, 1.0) -> Predicted Class:", result, '\n');
    }
    else
    {
        fprintf(stderr, "Prediction failed due to memory allocation error.\n");
    }

    // Predict class for query 2 (Expected: Class 1).
    result = predict(dataset, class_one, k, number_of_classes);

    if (result != -1)
    {
        printf("%s %d%c", "Query point (4.9, 5.1) -> Predicted Class:", result, '\n');
    }
    else
    {
        fprintf(stderr, "Prediction failed due to memory allocation error.\n");
    }

    // Free all dynamic memory associated with the training dataset.
    destroy_dataset(dataset);

    return 0;
}