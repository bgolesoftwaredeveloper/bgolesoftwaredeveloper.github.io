/**
 * @file        :   gaussian_mixture_model.cpp
 * @author      :   Braiden Gole
 * @brief       :   Implementation file for the Gaussian Mixture Model (GMM) class using Expectation-Maximization.
 * @details     :   Provides implementation for training a Gaussian Mixture Model (GMM) via the 
 *                  Expectation-Maximization (EM) algorithm, calculating multivariate Gaussian probability 
 *                  densities, predicting posterior probabilities, and validating model parameters.
 * @version     :   1.0
 * @date        :   2026-08-04
 */
#include <iostream>
#include <cmath>
#include <random>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <stdexcept>

// Include header files.
#include "../include/gaussian_mixture_model.hpp"

/**
 * @brief Constructs a Gaussian Mixture Model instance and validates parameters via mutators.
 * @param number_of_components The number of Gaussian mixture components (clusters).
 * @param maximum_iterations Maximum number of EM algorithm steps to perform.
 * @param convergence_tolerance Stopping threshold based on log-likelihood difference between iterations.
 */
GaussianMixtureModel::GaussianMixtureModel(std::size_t number_of_components,
    std::size_t maximum_iterations, double convergence_tolerance)
    : data_dimensionality(0)
{
    set_number_of_components(number_of_components);
    set_maximum_iterations(maximum_iterations);
    set_convergence_tolerance(convergence_tolerance);
}

// Accessors

/**
 * @brief Returns a constant reference to the mixing weights vector.
 * @return const std::vector<double>& Component prior probabilities.
 */
const std::vector<double>& GaussianMixtureModel::get_component_weights() const
{
    return this->component_weights;
}

/**
 * @brief Returns a constant reference to the component mean vectors.
 * @return const std::vector<std::vector<double>>& Component means matrix [K x D].
 */
const std::vector<std::vector<double>>& GaussianMixtureModel::get_component_means() const
{
    return this->component_means;
}

/**
 * @brief Returns a constant reference to the component covariance matrices.
 * @return const std::vector<std::vector<std::vector<double>>>& Component covariance tensors [K x D x D].
 */
const std::vector<std::vector<std::vector<double>>>& GaussianMixtureModel::get_component_covariances() const
{
    return this->component_covariances;
}

// Mutators

/**
 * @brief Sets the number of mixture components with validation.
 * @param count The desired number of mixture components (must be > 0).
 * @throws std::invalid_argument If count is zero.
 */
void GaussianMixtureModel::set_number_of_components(std::size_t count)
{
    if (count <= 0)
    {
        throw std::invalid_argument("Number of components must be greater than zero.");
    }

    this->number_of_components = count;
}

/**
 * @brief Sets the maximum number of EM iterations with validation.
 * @param iterations The maximum allowed iterations (must be > 0).
 * @throws std::invalid_argument If iterations is zero.
 */
void GaussianMixtureModel::set_maximum_iterations(std::size_t iterations)
{
    if (iterations <= 0)
    {
        throw std::invalid_argument("Maximum iterations must be at least 1.");
    }

    this->maximum_iterations = iterations;
}

/**
 * @brief Sets the convergence threshold for log-likelihood changes with validation.
 * @param tolerance The convergence tolerance value (must be > 0.0).
 * @throws std::invalid_argument If tolerance is less than or equal to zero.
 */
void GaussianMixtureModel::set_convergence_tolerance(double tolerance)
{
    if (tolerance <= 0.0)
    {
        throw std::invalid_argument("Convergence tolerance must be strictly positive.");
    }

    this->convergence_tolerance = tolerance;
}

// Methods

/**
 * @brief Computes the multivariate Gaussian probability density for a given data point.
 * @param input The data point vector.
 * @param mean The component mean vector.
 * @param covariance The component covariance matrix.
 * @return double Calculated probability density value.
 */
double GaussianMixtureModel::calculate_gaussian_probability_density(const std::vector<double>& input,
    const std::vector<double>& mean, const std::vector<std::vector<double>>& covariance) const
{
    // Handle 1D Gaussian distribution.
    if (data_dimensionality == 1)
    {
        double variance = covariance[0][0];
        double difference = input[0] - mean[0];
        double exponent_term = -0.5 * (difference * difference) / variance;
        double normalization_constant = 1.0 / (std::sqrt(2.0 * M_PI * variance));
        
        return normalization_constant * std::exp(exponent_term);
    }
    // Handle 2D Gaussian distribution using explicit 2x2 matrix inversion Mahalanbis distance.
    else if (data_dimensionality == 2)
    {
        // Compute 2x2 determinant.
        double determinant = covariance[0][0] * covariance[1][1] - covariance[0][1] * covariance[1][0];

        // Apply regularization threshold to prevent division by zero or singular matrix issue.
        if (determinant <= 0.0)
        {
            determinant = 1e-6;
        }

        // Calculate inverse of 2x2 covariance matrix.
        double inverse_covariance[2][2];

        inverse_covariance[0][0] = covariance[1][1] / determinant;
        inverse_covariance[0][1] = -covariance[0][1] / determinant;
        inverse_covariance[1][0] = -covariance[1][0] / determinant;
        inverse_covariance[1][1] = covariance[0][0] / determinant;

        // Vector displacement from mean.
        double horizontal_difference = input[0] - mean[0];
        double vertical_difference = input[1] - mean[1];

        // Compute Mahalanbis distance term: (x - mu)^T * Sigma^-1 * (x - mu).
        double mahalanobis_squared_distance = horizontal_difference * (horizontal_difference * inverse_covariance[0][0] + vertical_difference * inverse_covariance[1][0]) +
            vertical_difference * (horizontal_difference * inverse_covariance[0][1] + vertical_difference * inverse_covariance[1][1]);

        double normalization_constant = 1.0 / (2.0 * M_PI * std::sqrt(determinant));
        
        return normalization_constant * std::exp(-0.5 * mahalanobis_squared_distance);
    }

    return 0.0;
}

/**
 * @brief Initializes model weights, means, and covariance matrices prior to EM execution.
 * @param dataset The input dataset used to sample initial mean positions.
 */
void GaussianMixtureModel::initialize_model_parameters(const std::vector<std::vector<double>>& dataset)
{
    std::size_t total_points = dataset.size();

    // Assign uniform prior weights to each component.
    component_weights.assign(number_of_components, 1.0 / static_cast<double>(number_of_components));

    // Space initial means evenly across the dataset points to guarantee spread.
    component_means.assign(number_of_components, std::vector<double>(data_dimensionality, 0.0));

    for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
    {
        std::size_t point_index = (component_index * total_points) / number_of_components;

        component_means[component_index] = dataset[point_index];
    }

    // Compute the global empirical variance of the dataset across each dimension.
    std::vector<double> empirical_variance(data_dimensionality, 1.0);

    for (std::size_t dimension_index = 0; dimension_index < data_dimensionality; dimension_index++)
    {
        double dimension_mean = 0.0;

        for (std::size_t point_index = 0; point_index < total_points; point_index++)
        {
            dimension_mean += dataset[point_index][dimension_index];
        }

        dimension_mean /= static_cast<double>(total_points);

        double accumulated_variance = 0.0;

        for (std::size_t point_index = 0; point_index < total_points; point_index++)
        {
            double difference = dataset[point_index][dimension_index] - dimension_mean;

            accumulated_variance += difference * difference;
        }

        // Use global variance, ensuring a minimum threshold of 1.0.
        empirical_variance[dimension_index] = std::max(accumulated_variance / static_cast<double>(total_points), 1.0);
    }

    // Initialize covariance matrices scaled by empirical variance.
    component_covariances.assign(number_of_components, 
        std::vector<std::vector<double>>(data_dimensionality, std::vector<double>(data_dimensionality, 0.0)));

    for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
    {
        for (std::size_t dimension_index = 0; dimension_index < data_dimensionality; dimension_index++)
        {
            component_covariances[component_index][dimension_index][dimension_index] = empirical_variance[dimension_index];
        }
    }
}

/**
 * @brief Executes the Expectation (E) step of the EM algorithm to compute soft responsibilities.
 * @param dataset The input dataset being fitted.
 * @return double The current total log-likelihood over the dataset.
 */
double GaussianMixtureModel::execute_expectation_step(const std::vector<std::vector<double>>& dataset)
{
    std::size_t total_points = dataset.size();
    
    double current_log_likelihood = 0.0;

    // Calculate unnormalized responsibilities for each point across all mixture components.
    for (std::size_t point_index = 0; point_index < total_points; point_index++)
    {
        double total_weighted_probability_density = 0.0;

        for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
        {
            double probability_density_value = calculate_gaussian_probability_density(
                dataset[point_index],
                component_means[component_index],
                component_covariances[component_index]
            );

            responsibilities[point_index][component_index] = component_weights[component_index] * probability_density_value;
            total_weighted_probability_density += responsibilities[point_index][component_index];
        }

        // Normalize responsibilities so that sum over components for point_index equals 1.0.
        for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
        {
            if (total_weighted_probability_density > 0.0)
            {
                responsibilities[point_index][component_index] /= total_weighted_probability_density;
            }
            else
            {
                responsibilities[point_index][component_index] = 1.0 / static_cast<double>(number_of_components);
            }
        }

        // Accumulate log-likelihood with a floor value to prevent log(0).
        current_log_likelihood += std::log(std::max(total_weighted_probability_density, 1e-10));
    }

    return current_log_likelihood;
}

/**
 * @brief Executes the Maximization (M) step of the EM algorithm to update model parameters.
 * @param dataset The input dataset used for parameter re-estimation.
 */
void GaussianMixtureModel::execute_maximization_step(const std::vector<std::vector<double>>& dataset)
{
    std::size_t total_points = dataset.size();

    for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
    {
        // Calculate total effective responsibility assigned to this component.
        double total_effective_responsibility = 0.0;

        for (std::size_t point_index = 0; point_index < total_points; point_index++)
        {
            total_effective_responsibility += responsibilities[point_index][component_index];
        }

        // Update mixing weight for the component.
        component_weights[component_index] = total_effective_responsibility / static_cast<double>(total_points);

        // Compute updated mean vector into a temporary variable.
        std::vector<double> updated_mean(data_dimensionality, 0.0);

        for (std::size_t point_index = 0; point_index < total_points; point_index++)
        {
            for (std::size_t dimension_index = 0; dimension_index < data_dimensionality; dimension_index++)
            {
                updated_mean[dimension_index] += responsibilities[point_index][component_index] * dataset[point_index][dimension_index];
            }
        }

        for (std::size_t dimension_index = 0; dimension_index < data_dimensionality; dimension_index++)
        {
            updated_mean[dimension_index] /= total_effective_responsibility;
        }

        // Update covariance matrix using updated_mean (without mutating component_means prematurely).
        std::vector<std::vector<double>> updated_covariance(data_dimensionality, std::vector<double>(data_dimensionality, 0.0));

        for (std::size_t point_index = 0; point_index < total_points; point_index++)
        {
            for (std::size_t row_dimension = 0; row_dimension < data_dimensionality; row_dimension++)
            {
                for (std::size_t column_dimension = 0; column_dimension < data_dimensionality; column_dimension++)
                {
                    double primary_difference = dataset[point_index][row_dimension] - updated_mean[row_dimension];
                    double secondary_difference = dataset[point_index][column_dimension] - updated_mean[column_dimension];
                    
                    updated_covariance[row_dimension][column_dimension] += 
                        responsibilities[point_index][component_index] * primary_difference * secondary_difference;
                }
            }
        }

        // Commit updated mean vector to the class member AFTER covariance accumulation finishes.
        component_means[component_index] = updated_mean;

        // Normalize covariance matrices and add diagonal regularization to ensure invertibility.
        for (std::size_t row_dimension = 0; row_dimension < data_dimensionality; row_dimension++)
        {
            for (std::size_t column_dimension = 0; column_dimension < data_dimensionality; column_dimension++)
            {
                component_covariances[component_index][row_dimension][column_dimension] = updated_covariance[row_dimension][column_dimension] / total_effective_responsibility;
            }

            component_covariances[component_index][row_dimension][row_dimension] += 1e-6;
        }
    }
}

/**
 * @brief Fits the Gaussian Mixture Model to an input dataset using the EM algorithm.
 * @param dataset Matrix where each row represents a data point vector [N x D].
 */
void GaussianMixtureModel::fit(const std::vector<std::vector<double>>& dataset)
{
    if (dataset.empty())
    {
        return;
    }

    // Infer data dimensionality and total data points from input vector.
    data_dimensionality = dataset[0].size();

    std::size_t total_points = dataset.size();

    // Allocate responsibility matrix memory.
    responsibilities.assign(total_points, std::vector<double>(number_of_components, 0.0));

    // Initialize means, weights, and covariances.
    initialize_model_parameters(dataset);

    double previous_log_likelihood = -std::numeric_limits<double>::infinity();

    // Run iterative EM process until max iterations reached or likelihood converges.
    for (std::size_t iteration = 0; iteration < maximum_iterations; iteration++)
    {
        double current_log_likelihood = execute_expectation_step(dataset);

        execute_maximization_step(dataset);

        double absolute_likelihood_change = std::abs(current_log_likelihood - previous_log_likelihood);

        // Check convergence criterion.
        if (absolute_likelihood_change < convergence_tolerance)
        {
            break;
        }

        previous_log_likelihood = current_log_likelihood;
    }
}

/**
 * @brief Predicts normalized posterior probabilities (responsibilities) for a given data point.
 * @param point Data point vector [D].
 * @return std::vector<double> Posterior probability distribution across mixture components.
 */
std::vector<double> GaussianMixtureModel::predict_component_probabilities(const std::vector<double>& point) const
{
    if (data_dimensionality == 0 && !point.empty())
    {
        const_cast<GaussianMixtureModel*>(this)->data_dimensionality = point.size();
    }

    std::vector<double> posterior_probabilities(number_of_components, 0.0);
    
    double sum_weighted_densities = 0.0;

    bool weights_valid = (component_weights.size() == number_of_components);

    // Calculate component densities scaled by prior weights.
    for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
    {   
        double weight = weights_valid ? component_weights[component_index] : (1.0 / number_of_components);
        double density_value = calculate_gaussian_probability_density(point,
            component_means[component_index], component_covariances[component_index]);

        posterior_probabilities[component_index] = weight * density_value;
        sum_weighted_densities += posterior_probabilities[component_index];
    }

    // Normalize posterior probabilities across components.
    if (sum_weighted_densities > 0.0 && !std::isnan(sum_weighted_densities))
    {
        for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
        {
            posterior_probabilities[component_index] /= sum_weighted_densities;
        }
    }
    else
    {
        for (std::size_t component_index = 0; component_index < number_of_components; component_index++)
        {
            posterior_probabilities[component_index] = 1.0 / static_cast<double>(number_of_components);
        }
    }

    return posterior_probabilities;
}

/**
 * @brief Prints formatted model parameters (weights, mean vectors, covariance matrices) to stdout.
 */
void GaussianMixtureModel::print_model_parameters() const
{
    std::ios_base::fmtflags original_flags = std::cout.flags();

    std::cout << std::endl;
    std::cout << "================ ESTIMATED MODEL PARAMETERS ================" << std::endl;
    std::cout << std::fixed << std::setprecision(4);

    for (std::size_t component_index = 0; component_index < number_of_components; ++component_index)
    {
        std::cout << std::endl;
        std::cout << "--- Gaussian Component " << (component_index + 1) << " ---" << std::endl;
        std::cout << "  Weight: " << component_weights[component_index] << std::endl;

        std::cout << "  Mean Vector: [ ";

        for (double mean_element : component_means[component_index])
        {
            std::cout << mean_element << " ";
        }
        std::cout << "]" << std::endl;

        std::cout << "  Covariance Matrix:" << std::endl;

        for (const auto& covariance_row : component_covariances[component_index])
        {
            std::cout << "\t[ ";
            
            for (double matrix_element : covariance_row)
            {
                std::cout << matrix_element << " ";
            }
            std::cout << "]" << std::endl;
        }
    }

    std::cout << "============================================================\n\n";
    std::cout.flags(original_flags);
}