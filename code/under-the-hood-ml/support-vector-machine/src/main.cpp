/**
 * @file        :   main.cpp
 * @author      :   Braiden Gole
 * @brief       :   Driver script demonstrating training and evaluation of a Support Vector Classifier.
 * @details     :   This program instantiates a linear Support Vector Classifier, trains it on a 2D 
 *                  toy dataset using Stochastic Gradient Descent (SGD) with hinge loss, and outputs 
 *                  the learned model parameters (weights and bias) alongside its hyperparameter summary.
 * Compile:
 * clang++ -Wall -Wextra -Wpedantic ./src/main.cpp ./src/support_vector_classifier.cpp -I./include -o ./bin/support_vector_test
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#include <iostream>

// Include header files.
#include "../include/support_vector_classifier.hpp"

int main()
{
    // Define a 2D training dataset where each observation consists of two numerical features.
    const std::vector<std::vector<double>> feature_observations =
    {
        {2.0, 3.0},
        {1.0, 1.0},
        {2.0, 1.0},
        {8.0, 9.0},
        {7.0, 8.0},
        {9.0, 7.0}  
    };

    // Define the corresponding binary target class labels (-1 for cluster A, +1 for cluster B.)
    const std::vector<int> class_labels = {-1, -1, -1, 1, 1, 1};

    // Instantiate the support vector classifier with hyperparameters.
    SupportVectorClassifier classifier(0.001, 100.0, 10000);

    std::cout << "Training the Support Vector Classifier..." << std::endl;

    // Train the classifier on the training data to find the optimal decision boundary (weight, bias).
    classifier.train(feature_observations, class_labels);

    // Display the trained model parameters (weight, vector, intercept, bias) and hyperparameters.
    print_classifier_summary(classifier);

    return 0;
}