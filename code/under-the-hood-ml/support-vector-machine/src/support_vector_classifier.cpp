/**
 * @file        :   support_vector_classifier.cpp
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the SupportVectorClassifier class and associated utility functions.
 * @details     :   Provides member definitions for training a soft-margin linear Support Vector Classifier
 *                  using Stochastic Gradient Descent (SGD) with hinge loss, computing raw decision boundary
 *                  scores, predicting discrete binary class labels, and querying classifier parameters.
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <cmath>

// Include header files.
#include "support_vector_classifier.hpp"

/**
 * @brief Default constructor initializing hyperparameter defaults.
 */
SupportVectorClassifier::SupportVectorClassifier()
{   
    set_intercept_bias(0.0);
    set_learning_rate(0.001);
    set_regularization_penalty(100.0);
    set_maximum_iterations(10000);
}

/**
 * @brief Parameterized constructor setting initial learning rate, regularization parameter, and max iterations.
 * @param learning_rate Initial learning rate step size.
 * @param regularization_penalty Regularization parameter C scaling hinge loss penalty vs margin size.
 * @param maximum_iterations Maximum number of optimization epochs.
 */
SupportVectorClassifier::SupportVectorClassifier(double learning_rate, double regularization_penalty, std::size_t maximum_iterations)
{
    set_intercept_bias(0.0);
    set_learning_rate(learning_rate);
    set_regularization_penalty(regularization_penalty);
    set_maximum_iterations(maximum_iterations);
}

// Accessors.

/**
 * @brief Retrieves the current learning rate step size.
 * @return Learning rate value as a double.
 */
double SupportVectorClassifier::get_learning_rate() const
{
    return this->learning_rate;
}

/**
 * @brief Retrieves the learned feature weight vector.
 * @return Constant reference to vector of weight coefficients.
 */
const std::vector<double>& SupportVectorClassifier::get_weight_coefficients() const
{
    return this->weight_coefficients;
}

/**
 * @brief Retrieves the learned intercept bias parameter.
 * @return Intercept bias value as a double.
 */
double SupportVectorClassifier::get_intercept_bias() const
{
    return this->intercept_bias;
}

/**
 * @brief Retrieves the regularization parameter C.
 * @return Regularization penalty weight as a double.
 */
double SupportVectorClassifier::get_regularization_penalty() const
{
    return this->regularization_penalty;
}

/**
 * @brief Retrieves the maximum number of optimization epoch iterations.
 * @return Maximum iteration count.
 */
std::size_t SupportVectorClassifier::get_maximum_iterations() const
{
    return this->maximum_iterations;
}

// Mutators.

/**
 * @brief Sets the intercept bias offset parameter.
 * @param bias New bias value.
 */
void SupportVectorClassifier::set_intercept_bias(double bias)
{
    if (!std::isfinite(bias))
    {
        throw std::invalid_argument("Intercept bias must be a finite numerical value.");
    }

    this->intercept_bias = bias;
}

/**
 * @brief Sets the learning rate step size for gradient updates.
 * @param learning_rate New learning rate double value.
 */
void SupportVectorClassifier::set_learning_rate(double learning_rate)
{
    if (!std::isfinite(learning_rate) || learning_rate <= 0.0)
    {
        throw std::invalid_argument("Learning rate must be a finite positive double value strictly greater than 0.0.");
    }

    this->learning_rate = learning_rate;
}

/**
 * @brief Sets the regularization penalty hyperparameter C.
 * @param regularization_penalty Regularization magnitude.
 */
void SupportVectorClassifier::set_regularization_penalty(double regularization_penalty)
{
    if (!std::isfinite(regularization_penalty) || regularization_penalty < 0.0)
    {
        throw std::invalid_argument("Regularization penalty must be a finite non-negative double value (>= 0.0).");
    }

    this->regularization_penalty = regularization_penalty;
}

/**
 * @brief Sets the maximum iteration steps for optimization loops.
 * @param maximum_iterations Epoch count upper limit.
 */
void SupportVectorClassifier::set_maximum_iterations(std::size_t maximum_iterations)
{
    if (maximum_iterations <= 0)
    {
        throw std::invalid_argument("Maximum iterations must be greater than zero.");
    }
    
    this->maximum_iterations = maximum_iterations;
}

// Methods.

/**
 * @brief Computes the functional margin / signed decision distance for a single sample.
 * @param sample Feature vector for query observation.
 * @return Continuous raw decision score (dot product plus bias).
 * @throws std::invalid_argument If sample dimension does not match weight coefficient vector dimension.
 */
double SupportVectorClassifier::compute_decision_value(const std::vector<double>& sample) const
{
    // Ensure the input sample dimension matches trained weight vector dimesion.
    if (sample.size() != weight_coefficients.size())
    {
        throw std::invalid_argument("Input feature vector dimension does not align with trained weight coefficients.");
    }

    // Compute dot product between weight vector and input sample feature vector.
    const double inner_feature_product = std::inner_product(sample.begin(), sample.end(), weight_coefficients.begin(), 0.0);

    // Calculate signed decision score.
    return inner_feature_product + this->intercept_bias;
}

/**
 * @brief Predicts discrete binary target class label (-1 or +1) for a single sample.
 * @param sample Feature vector for query observation.
 * @return Integer target class prediction (-1 or +1).
 */
int SupportVectorClassifier::predict_class_label(const std::vector<double>& sample) const
{
    // Calculate raw continuous decision distance from separating hyperpalne.
    const double raw_decision_score = compute_decision_value(sample);

    // Map positive decision distance to class +1, otherwise map to class -1.
    return raw_decision_score >= 0.0 ? 1 : -1;
}

/**
 * @brief Predicts discrete binary target class labels for a dataset matrix of samples.
 * @param samples Matrix of input observations (rows as samples, columns as features).
 * @return Vector of integer predicted class labels (-1 or +1).
 */
std::vector<int> SupportVectorClassifier::predict_class_labels(const std::vector<std::vector<double>>& samples) const
{
    std::vector<int> predicted_class_labels;

    // Pre-allocated vector memory to prevent dynamic re-allocations during iteration.
    predicted_class_labels.reserve(samples.size());

    // Iterate over each observation in dataset and predict its class label.
    for (const auto& individual : samples)
    {
        predicted_class_labels.push_back(predict_class_label(individual));
    }

    return predicted_class_labels;
}

/**
 * @brief Fits the linear decision boundary to training data via SGD with Hinge Loss.
 * @param features 2D matrix of training observations.
 * @param labels Vector of target binary class labels (-1 or +1).
 * @throws std::invalid_argument If feature dataset or labels are empty, or if sample/label lengths mismatch.
 */
void SupportVectorClassifier::train(const std::vector<std::vector<double>>& features, const std::vector<int>& labels)
{
    // Guard against empty datasets.
    if (features.empty() || labels.empty())
    {
        throw std::invalid_argument("Training feature dataset and target labels must not be empty.");
    }

    // Ensure sample count matches target label count.
    if (features.size() != labels.size())
    {
        throw std::invalid_argument("Dimension mismatch between feature observations and target labels.");
    }

    const std::size_t total_observations = features.size();
    const std::size_t feature_dimension_count = features[0].size();

    // Initialize all model weight coefficients to zero.
    this->weight_coefficients.assign(feature_dimension_count, 0.0);
    this->intercept_bias = 0.0;

    // Main optimization loop running for specified epoch count.
    for (std::size_t step = 0; step < this->maximum_iterations; step++)
    {
        // Stochastic gradient descent: Iterate through each sample sequentially.
        for (std::size_t observation_index = 0; observation_index < total_observations; observation_index++)
        {
            const std::vector<double>& current_feature = features[observation_index];
            const int current_target_label = labels[observation_index];

            // Compute hyperplane dot product.
            const double hyperplane_dot_product = std::inner_product(current_feature.begin(), 
                current_feature.end(), weight_coefficients.begin(), 0.0);

            // Compute functional margin conditions.
            const double margin_value = current_target_label * (hyperplane_dot_product + this->intercept_bias);

            if (margin_value >= 1.0)
            {
                // Correctly classified outside margin: Apply standard L2 regularization weight decay.
                for (std::size_t feature_index = 0; feature_index < feature_dimension_count; feature_index++)
                {
                    const double regularization_gradient = this->weight_coefficients[feature_index] / static_cast<double>(this->maximum_iterations);

                    this->weight_coefficients[feature_index] -= this->learning_rate * regularization_gradient;
                }
            }
            else
            {
                // Misclassified or inside margin: Update weights using hinge loss penalty gradient.
                for (std::size_t feature_index = 0; feature_index < feature_dimension_count; feature_index++)
                {
                    const double regularization_gradient = this->weight_coefficients[feature_index] / static_cast<double>(this->maximum_iterations);
                    const double hinge_loss_gradient = this->regularization_penalty * current_target_label * current_feature[feature_index];

                    this->weight_coefficients[feature_index] -= this->learning_rate * (regularization_gradient - hinge_loss_gradient);
                }

                // Update intercept bias using hinge loss.
                this->intercept_bias += this->learning_rate * this->regularization_penalty * current_target_label;
            }
        }
    }
}

// Utilities.

/**
 * @brief Prints formatted parameters, weights, bias, and hyperparameters of a classifier to std::cout.
 * @param classifier Reference to SupportVectorClassifier instance to print.
 */
void print_classifier_summary(const SupportVectorClassifier& classifier)
{
    std::cout << "-- Support Vector Classifier --" << std::endl;
    std::cout << "Weight Coefficients: [";

    std::size_t size = classifier.get_weight_coefficients().size();

    std::size_t count = 0;
    
    // Print weight array values with proper spacing and formatting.
    for (double weight : classifier.get_weight_coefficients())
    {   
        if (count < size - 1)
        {
            std::cout << weight << " ";
        }
        else
        {
            std::cout << weight;
        }

        count++;
    }

    std::cout << "]" << std::endl;

    std::cout << "Intercept Bias: " << classifier.get_intercept_bias() << std::endl;
    std::cout << "Learning Rate: " << classifier.get_learning_rate() << std::endl;
    std::cout << "Regularization penalty: " << classifier.get_regularization_penalty() << std::endl;
    std::cout << "Maximum iterations: " << classifier.get_maximum_iterations() << std::endl;
    std::cout << "-------------------------------" << std::endl;
}