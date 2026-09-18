/**
 * @file        :   gaussian_mixture_model.hpp
 * @author      :   Braiden Gole
 * @brief       :   Header file declaring the GaussianMixtureModel class for unsupervised density estimation and clustering.
 * @details     :   Defines the GaussianMixtureModel class structure, including private member data, expectation-maximization (EM)
 *                  helper methods, probability density evaluation, parameter accessors/mutators, and public interface methods.
 * @version     :   1.0
 * @date        :   2026-08-04
 */
#ifndef GAUSSIAN_MIXTURE_MODEL_HPP
#define GAUSSIAN_MIXTURE_MODEL_HPP

#include <numeric>

/**
 * @class GaussianMixtureModel
 * @brief Represents a Gaussian Mixture Model (GMM) trained via Expectation-Maximization.
 */
class GaussianMixtureModel
{
    private:
        std::size_t number_of_components;
        std::size_t data_dimensionality;
        std::size_t maximum_iterations;

        double convergence_tolerance;

        std::vector<double> component_weights;
        std::vector<std::vector<double>> component_means;
        std::vector<std::vector<std::vector<double>>> component_covariances;

        std::vector<std::vector<double>> responsibilities;

        void initialize_model_parameters(const std::vector<std::vector<double>>& dataset);
        double execute_expectation_step(const std::vector<std::vector<double>>& dataset);
        void execute_maximization_step(const std::vector<std::vector<double>>& dataset);

        double calculate_gaussian_probability_density(const std::vector<double>& input,
            const std::vector<double>& mean, const std::vector<std::vector<double>>& covariance) const;

    public:
        // Constructor.
        GaussianMixtureModel(std::size_t number_of_components, std::size_t maximum_iterations = 100, double convergence_tolerance = 1e-4);

        // Destructor.
        ~GaussianMixtureModel() = default;

        // Accessors.
        const std::vector<double>& get_component_weights() const;
        const std::vector<std::vector<double>>& get_component_means() const;
        const std::vector<std::vector<std::vector<double>>>& get_component_covariances() const;

        // Mutators.
        void set_number_of_components(std::size_t count);
        void set_maximum_iterations(std::size_t iterations);
        void set_convergence_tolerance(double tolerance);

        // Methods.
        void fit(const std::vector<std::vector<double>>& dataset);
        std::vector<double> predict_component_probabilities(const std::vector<double>& dataset) const;
        void print_model_parameters() const;
};

#endif