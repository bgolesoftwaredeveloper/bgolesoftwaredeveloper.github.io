/**
 * @file        :   support_vector_classifier.hpp
 * @author      :   Braiden Gole
 * @brief       :   Interface declaration for the SupportVectorClassifier class.
 * @details     :   Defines a soft-margin linear Support Vector Classifier trained using 
 *                  Stochastic Gradient Descent (SGD) with hinge loss. Provides prototypes 
 *                  for model initialization, hyperparameter accessors/mutators, single and 
 *                  batch predictions, model training, and summary visualization.
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#ifndef SUPPORT_VECTOR_CLASSIFIER_HPP
#define SUPPORT_VECTOR_CLASSIFIER_HPP

#include <vector>
#include <cstddef>

/**
 * @class       :   SupportVectorClassifier
 * @brief       :   A linear Support Vector Machine (SVM) binary classifier.
 * @details     :   This class encapsulates weight coefficients, an intercept bias, and 
 *                  optimization hyperparameters (learning rate, regularization penalty C, 
 *                  and epoch iterations). It computes linear decision hyperplanes using 
 *                  hinge loss minimization.
 */
class SupportVectorClassifier
{
    private:
        std::vector<double> weight_coefficients;
        double intercept_bias;

        double learning_rate;
        double regularization_penalty;

        std::size_t maximum_iterations;
    public:
        // Constructors.
        SupportVectorClassifier();
        SupportVectorClassifier(double learning_rate, 
            double regularization_penalty, std::size_t maximum_iterations);
        
        // Destructor.
        ~SupportVectorClassifier() = default;

        // Accessors.
        double get_learning_rate() const;
        const std::vector<double>& get_weight_coefficients() const;
        double get_intercept_bias() const;
        double get_regularization_penalty() const;
        std::size_t get_maximum_iterations() const;

        // Mutators.
        void set_intercept_bias(double bias);
        void set_learning_rate(double learning_rate);
        void set_regularization_penalty(double regularization_penalty);
        void set_maximum_iterations(std::size_t maximum_iterations);

        // Methods.
        double compute_decision_value(const std::vector<double>& sample) const;
        int predict_class_label(const std::vector<double>& sample) const;
        std::vector<int> predict_class_labels(const std::vector<std::vector<double>>& samples) const;

        void train(const std::vector<std::vector<double>>& features, const std::vector<int>& labels);
};

// Function prototypes.
void print_classifier_summary(const SupportVectorClassifier& classifier);

#endif