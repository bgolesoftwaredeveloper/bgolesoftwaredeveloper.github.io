/**
 * @file        :   main.cpp
 * @author      :   Braiden Gole
 * @brief       :   Demonstrates and verifies the Gaussian Mixture Model (GMM) class using synthetic 2D data.
 * @details     :   Generates a dataset consisting of two distinct 2D Gaussian clusters, fits a 2-component
 *                  Gaussian Mixture Model using the Expectation-Maximization (EM) algorithm, prints the fitted
 *                  parameters (weights, means, and covariance matrices), and calculates posterior membership
 *                  probabilities for test points.
 * Compile:
 * clang++ -Wall -Wextra -Wpedantic ./src/main.cpp ./src/gaussian_mixture_model.cpp -I./include -o ./bin/gaussian_mixture_model_test
 * @version     :   1.0
 * @date        :   2026-08-04
 */
#include <iostream>
#include <vector>
#include <random>

// Include header files.
#include "../include/gaussian_mixture_model.hpp"

int main()
{
    // Container to hold generated 2D data points.
    std::vector<std::vector<double>> dataset;

    // Seed Mersenne Twister engine for reproducible synthetic data generation.
    std::mt19937 engine(12345);

    // Define Gaussian distributions for Cluster 1 centered at (2.0, 3.0) with std dev 0.5.
    std::normal_distribution<double> cluster_one_x(2.0, 0.5);
    std::normal_distribution<double> cluster_one_y(3.0, 0.5);

    // Define Gaussian distributions for Cluster 2 centered at (7.0, 8.0) with std dev 0.8.
    std::normal_distribution<double> cluster_two_x(7.0, 0.8);
    std::normal_distribution<double> cluster_two_y(8.0, 0.8);

    // Generate 150 samples belonging to Cluster 1.
    for (int point_index = 0; point_index < 150; point_index++)
    {
        dataset.push_back({cluster_one_x(engine), cluster_one_y(engine)});
    }

    // Generate 150 samples belonging to Cluster 2.
    for (int point_index = 0; point_index < 150; point_index++)
    {
        dataset.push_back({cluster_two_x(engine), cluster_two_y(engine)});
    }

    // Instantiate GMM configured for 2 components, max 50 EM iterations, and 1e-4 convergence tolerance.
    GaussianMixtureModel gaussian_mixture_model(2, 50, 1e-4);

    // Run the Expectation-Maximization algorithm to fit the dataset.
    gaussian_mixture_model.fit(dataset);

    // Display converged parameters (mixing weights, mean vectors, covariance matrices).
    gaussian_mixture_model.print_model_parameters();

    // Define test points near known cluster centers to test posterior estimation.
    std::vector<double> test_point_near_cluster_one = {2.1, 2.9};
    std::vector<double> test_point_near_cluster_two = {7.2, 7.8};

    // Calculate component probability distributions for test points.
    std::vector<double> probabilities_one = gaussian_mixture_model.predict_component_probabilities(test_point_near_cluster_one);
    std::vector<double> probabilities_two = gaussian_mixture_model.predict_component_probabilities(test_point_near_cluster_two);

    // Print calculated posterior probabilities.
    std::cout << std::endl;
    std::cout << "================ Posterior Predictions ================" << std::endl;
    std::cout << "Test Point [2.1, 2.9] probabilities:" << std::endl;

    for (std::size_t component_index = 0; component_index < probabilities_one.size(); ++component_index)
    {
        std::cout << "\tComponent " << component_index + 1 << ": " << probabilities_one[component_index] << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Test Point [7.2, 7.8] probabilities:" << std::endl;

    for (std::size_t component_index = 0; component_index < probabilities_two.size(); ++component_index)
    {
        std::cout << "\tComponent " << component_index + 1 << ": " << probabilities_two[component_index] << std::endl;
    }

    return 0;
}