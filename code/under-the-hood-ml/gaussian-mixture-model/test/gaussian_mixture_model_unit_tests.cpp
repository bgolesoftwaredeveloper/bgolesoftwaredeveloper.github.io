/**
 * @file        :   gaussian_mixture_model_unit_tests.cpp
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for GaussianMixtureModel implementation.
 * @details     :   Provides 30 comprehensive unit tests using the Unity Test Framework
 *                  to validate constructor initialization, getter/setter mutations, 1D/2D probability
 *                  density calculations, expectation-maximization model fitting, posterior membership
 *                  predictions, exception handlings, convergence behavior, and summary outputs.
 * Compile:
 * clang -c -DUNITY_INCLUDE_DOUBLE ./src/unity.c -Iinclude -o ./bin/unity.o
 * clang++ -std=c++17 -DUNITY_INCLUDE_DOUBLE ./test/gaussian_mixture_model_unit_tests.cpp ./src/gaussian_mixture_model.cpp ./bin/unity.o -Iinclude -o ./bin/gmm_unit_test
 * @version     :   1.0
 * @date        :   2026-08-04
 */
#include <vector>
#include <stdexcept>
#include <cmath>

extern "C" {
    #include "unity.h"
}

#include "gaussian_mixture_model.hpp"

/**
 * @brief Sets up resources before each test execution.
 */
void setUp(void)
{
    ;;
}

/**
 * @brief Cleans up resources after each test execution.
 */
void tearDown(void)
{
    ;;
}

/**
 * @brief 1. Tests default parameter initialization via constructor.
 */
void test_gmm_constructor_defaults(void)
{
    // Arrange & Act.
    GaussianMixtureModel gaussian_mixture_model(3);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, gaussian_mixture_model.get_component_weights().size());
    TEST_ASSERT_EQUAL_UINT(0, gaussian_mixture_model.get_component_means().size());
    TEST_ASSERT_EQUAL_UINT(0, gaussian_mixture_model.get_component_covariances().size());
}

/**
 * @brief 2. Tests parameterized constructor state assignment.
 */
void test_gmm_parameterized_constructor_assignment(void)
{
    // Arrange & Act.
    GaussianMixtureModel gaussian_mixture_model(2, 50, 1e-4);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 3. Tests set_number_of_components mutator and valid update.
 */
void test_gmm_set_number_of_components_valid(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);

    // Act & Assert.
    gaussian_mixture_model.set_number_of_components(5);
}

/**
 * @brief 4. Tests set_number_of_components zero count exception.
 */
void test_gmm_set_number_of_components_zero_exception(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);

    // Act & Assert.
    try
    {
        gaussian_mixture_model.set_number_of_components(0);

        TEST_FAIL_MESSAGE("Expected std::invalid_argument exception was not thrown.");
    }
    catch (const std::invalid_argument& exception)
    {
        TEST_ASSERT_NOT_NULL(exception.what());
    }
}

/**
 * @brief 5. Tests set_maximum_iterations mutator and valid update.
 */
void test_gmm_set_maximum_iterations_valid(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);

    // Act & Assert.
    gaussian_mixture_model.set_maximum_iterations(200);
}

/**
 * @brief 6. Tests set_maximum_iterations zero value exception.
 */
void test_gmm_set_maximum_iterations_zero_exception(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);

    // Act & Assert.
    try
    {
        gaussian_mixture_model.set_maximum_iterations(0);

        TEST_FAIL_MESSAGE("Expected std::invalid_argument exception was not thrown.");
    }
    catch (const std::invalid_argument& e)
    {
        TEST_ASSERT_NOT_NULL(e.what());
    }
}

/**
 * @brief 7. Tests set_convergence_tolerance mutator and valid update.
 */
void test_gmm_set_convergence_tolerance_valid(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);

    // Act & Assert.
    gaussian_mixture_model.set_convergence_tolerance(1e-6);
}

/**
 * @brief 8. Tests set_convergence_tolerance non-positive exception.
 */
void test_gmm_set_convergence_tolerance_invalid_exception(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);

    // Act & Assert.
    try
    {
        gaussian_mixture_model.set_convergence_tolerance(0.0);

        TEST_FAIL_MESSAGE("Expected std::invalid_argument exception was not thrown.");
    }
    catch (const std::invalid_argument& e)
    {
        TEST_ASSERT_NOT_NULL(e.what());
    }
}

/**
 * @brief 9. Tests fitting model on empty dataset (graceful early return).
 */
void test_gmm_fit_empty_dataset_handling(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);
    std::vector<std::vector<double>> empty_dataset;

    // Act.
    gaussian_mixture_model.fit(empty_dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 10. Tests parameter container sizing after fitting a 1D dataset.
 */
void test_gmm_fit_1d_dataset_parameter_dimensions(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{-1.0}, {0.0}, {1.0}, {10.0}, {11.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_weights().size());
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_means().size());
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_covariances().size());
    TEST_ASSERT_EQUAL_UINT(1, gaussian_mixture_model.get_component_means()[0].size());
    TEST_ASSERT_EQUAL_UINT(1, gaussian_mixture_model.get_component_covariances()[0].size());
}

/**
 * @brief 11. Tests parameter container sizing after fitting a 2D dataset.
 */
void test_gmm_fit_2d_dataset_parameter_dimensions(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{1.0, 2.0}, {2.0, 3.0}, {8.0, 9.0}, {9.0, 10.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_weights().size());
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_means().size());
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_means()[0].size());
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_covariances()[0].size());
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_covariances()[0][0].size());
}

/**
 * @brief 12. Tests prior component weight summation equals 1.0 after fitting.
 */
void test_gmm_fit_component_weights_sum_to_one(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(3, 20, 1e-4);
    std::vector<std::vector<double>> dataset = {{1.0, 1.0}, {5.0, 5.0}, {10.0, 10.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    double sum = 0.0;

    for (double weight : gaussian_mixture_model.get_component_weights())
    {
        sum += weight;
    }

    TEST_ASSERT_EQUAL_DOUBLE(1.0, sum);
}

/**
 * @brief 13. Tests posterior membership probability vector size.
 */
void test_gmm_predict_component_probabilities_size(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(3, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}};

    gaussian_mixture_model.fit(dataset);

    std::vector<double> test_point = {1.5, 1.5};

    // Act.
    std::vector<double> probabilities = gaussian_mixture_model.predict_component_probabilities(test_point);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(3, probabilities.size());
}

/**
 * @brief 14. Tests normalized posterior probabilities summation equals 1.0.
 */
void test_gmm_predict_component_probabilities_sum_to_one(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 20, 1e-6);
    std::vector<std::vector<double>> dataset = {{2.0, 3.0}, {7.0, 8.0}};

    gaussian_mixture_model.fit(dataset);

    std::vector<double> test_point = {2.1, 2.9};

    // Act.
    std::vector<double> probabilities = gaussian_mixture_model.predict_component_probabilities(test_point);

    // Assert.
    double sum = probabilities[0] + probabilities[1];

    TEST_ASSERT_DOUBLE_WITHIN(1e-5, 1.0, sum);
}

/**
 * @brief 15. Tests convergence on well-separated 1D clusters.
 */
void test_gmm_1d_well_separated_clusters(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 100, 1e-6);
    std::vector<std::vector<double>> dataset;

    for (int index = 0; index < 20; index++)
    {
        dataset.push_back({-1.0 + (index * 0.1)});
    }

    for (int index = 0; index < 20; ++index)
    {
        dataset.push_back({99.0 + (index * 0.1)});
    }

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    std::vector<double> prob_near_zero = gaussian_mixture_model.predict_component_probabilities({0.05});
    std::vector<double> prob_near_hundred = gaussian_mixture_model.predict_component_probabilities({100.5});

    TEST_ASSERT_TRUE(prob_near_zero[0] > 0.90 || prob_near_zero[1] > 0.90);
    TEST_ASSERT_TRUE(prob_near_hundred[0] > 0.90 || prob_near_hundred[1] > 0.90);
}

/**
 * @brief 16. Tests convergence on well-separated 2D clusters.
 */
void test_gmm_2d_well_separated_clusters(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 50, 1e-6);
    std::vector<std::vector<double>> dataset;

    for (int index = 0; index < 30; index++)
    {
        dataset.push_back({2.0 + (index * 0.01), 3.0 + (index * 0.01)});
    }

    for (int index = 0; index < 30; index++)
    {
        dataset.push_back({7.0 + (index * 0.01), 8.0 + (index * 0.01)});
    }

    // Act.
    gaussian_mixture_model.fit(dataset);

    std::vector<double> prob_c1 = gaussian_mixture_model.predict_component_probabilities({2.0, 3.0});
    std::vector<double> prob_c2 = gaussian_mixture_model.predict_component_probabilities({7.0, 8.0});

    // Assert.
    TEST_ASSERT_TRUE((prob_c1[0] > 0.90 && prob_c2[1] > 0.90) || (prob_c1[1] > 0.90 && prob_c2[0] > 0.90));
}

/**
 * @brief 17. Tests model stability when fitted for a single EM iteration.
 */
void test_gmm_single_iteration_execution(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 1, 1e-4);
    std::vector<std::vector<double>> dataset = {{1.0, 2.0}, {3.0, 4.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 18. Tests sequential retraining resets component parameters correctly.
 */
void test_gmm_sequential_retraining_resets_parameters(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 20, 1e-4);
    std::vector<std::vector<double>> dataset_1d = {{1.0}, {2.0}, {3.0}};

    gaussian_mixture_model.fit(dataset_1d);

    // Act.
    std::vector<std::vector<double>> dataset_2d = {{1.0, 2.0}, {3.0, 4.0}};

    gaussian_mixture_model.fit(dataset_2d);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_means()[0].size());
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_covariances()[0].size());
}

/**
 * @brief 19. Tests model fitting execution on a single observation dataset.
 */
void test_gmm_single_observation_dataset_training(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(1, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{5.0, 10.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(1.0, gaussian_mixture_model.get_component_weights()[0]);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, gaussian_mixture_model.get_component_means()[0][0]);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, gaussian_mixture_model.get_component_means()[0][1]);
}

/**
 * @brief 20. Tests posterior probability prediction idempotency across calls.
 */
void test_gmm_prediction_idempotency(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 20, 1e-4);
    std::vector<std::vector<double>> dataset = {{1.0, 1.0}, {5.0, 5.0}};

    gaussian_mixture_model.fit(dataset);

    std::vector<double> test_point = {2.0, 2.0};

    // Act.
    std::vector<double> pred1 = gaussian_mixture_model.predict_component_probabilities(test_point);
    std::vector<double> pred2 = gaussian_mixture_model.predict_component_probabilities(test_point);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(pred1[0], pred2[0]);
    TEST_ASSERT_EQUAL_DOUBLE(pred1[1], pred2[1]);
}

/**
 * @brief 21. Tests print_model_parameters execution without crashing.
 */
void test_gmm_print_model_parameters_execution(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 5, 1e-4);
    std::vector<std::vector<double>> dataset = {{1.0, 2.0}, {3.0, 4.0}};

    gaussian_mixture_model.fit(dataset);

    // Act & Assert.
    gaussian_mixture_model.print_model_parameters();
}

/**
 * @brief 22. Tests covariance matrix positivity / regularization floor.
 */
void test_gmm_covariance_regularization_positivity(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(1, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{1.0, 1.0}, {1.0, 1.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    const auto& covariances = gaussian_mixture_model.get_component_covariances()[0];

    TEST_ASSERT_TRUE(covariances[0][0] > 0.0);
    TEST_ASSERT_TRUE(covariances[1][1] > 0.0);
}

/**
 * @brief 23. Tests model training with negative feature coordinates.
 */
void test_gmm_negative_feature_values_training(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 20, 1e-4);
    std::vector<std::vector<double>> dataset = {{-10.0, -10.0}, {-9.0, -9.0}, {-1.0, -1.0}, {-2.0, -2.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 24. Tests training stability with large magnitude inputs.
 */
void test_gmm_large_magnitude_floating_point_inputs(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{1e4, 1e4}, {1e4 + 1.0, 1e4 + 1.0}, {-1e4, -1e4}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 25. Tests training stability with tiny epsilon inputs.
 */
void test_gmm_small_magnitude_epsilon_inputs(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{1e-5, 1e-5}, {2e-5, 2e-5}, {-1e-5, -1e-5}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 26. Tests posterior calculation on origin test point (0, 0).
 */
void test_gmm_zero_vector_prediction(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{0.0, 0.0}, {5.0, 5.0}};

    gaussian_mixture_model.fit(dataset);

    std::vector<double> origin = {0.0, 0.0};

    // Act.
    std::vector<double> probabilities = gaussian_mixture_model.predict_component_probabilities(origin);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, probabilities.size());
    TEST_ASSERT_EQUAL_DOUBLE(1.0, probabilities[0] + probabilities[1]);
}

/**
 * @brief 27. Tests 1D probability density integration around single component center.
 */
void test_gmm_1d_probability_density_peak(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(1, 10, 1e-4);
    std::vector<std::vector<double>> dataset = {{0.0}, {0.1}, {-0.1}};

    gaussian_mixture_model.fit(dataset);

    // Act.
    std::vector<double> center_prob = gaussian_mixture_model.predict_component_probabilities({0.0});

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(1.0, center_prob[0]);
}

/**
 * @brief 28. Tests constructor parameter mutation propagation before fitting.
 */
void test_gmm_mutator_propagation_before_fit(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2);

    // Act.
    gaussian_mixture_model.set_number_of_components(4);
    gaussian_mixture_model.set_maximum_iterations(15);
    gaussian_mixture_model.set_convergence_tolerance(1e-3);

    std::vector<std::vector<double>> dataset = {{1.0}, {2.0}, {3.0}, {4.0}};

    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(4, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 29. Tests convergence threshold exit when tolerance is high.
 */
void test_gmm_early_convergence_exit(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 1000, 1e2);
    std::vector<std::vector<double>> dataset = {{1.0, 1.0}, {1.1, 1.1}, {5.0, 5.0}};

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, gaussian_mixture_model.get_component_weights().size());
}

/**
 * @brief 30. Tests equal cluster weight allocation for symmetric datasets.
 */
void test_gmm_symmetric_dataset_equal_weights(void)
{
    // Arrange.
    GaussianMixtureModel gaussian_mixture_model(2, 50, 1e-4);
    std::vector<std::vector<double>> dataset = {
        {-5.0, -5.0}, {-4.9, -4.9},
        { 5.0,  5.0}, { 4.9,  4.9}
    };

    // Act.
    gaussian_mixture_model.fit(dataset);

    // Assert.
    TEST_ASSERT_DOUBLE_WITHIN(0.05, 0.5, gaussian_mixture_model.get_component_weights()[0]);
    TEST_ASSERT_DOUBLE_WITHIN(0.05, 0.5, gaussian_mixture_model.get_component_weights()[1]);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_gmm_constructor_defaults);
    RUN_TEST(test_gmm_parameterized_constructor_assignment);
    RUN_TEST(test_gmm_set_number_of_components_valid);
    RUN_TEST(test_gmm_set_number_of_components_zero_exception);
    RUN_TEST(test_gmm_set_maximum_iterations_valid);
    RUN_TEST(test_gmm_set_maximum_iterations_zero_exception);
    RUN_TEST(test_gmm_set_convergence_tolerance_valid);
    RUN_TEST(test_gmm_set_convergence_tolerance_invalid_exception);
    RUN_TEST(test_gmm_fit_empty_dataset_handling);
    RUN_TEST(test_gmm_fit_1d_dataset_parameter_dimensions);
    RUN_TEST(test_gmm_fit_2d_dataset_parameter_dimensions);
    RUN_TEST(test_gmm_fit_component_weights_sum_to_one);
    RUN_TEST(test_gmm_predict_component_probabilities_size);
    RUN_TEST(test_gmm_predict_component_probabilities_sum_to_one);
    RUN_TEST(test_gmm_1d_well_separated_clusters);
    RUN_TEST(test_gmm_2d_well_separated_clusters);
    RUN_TEST(test_gmm_single_iteration_execution);
    RUN_TEST(test_gmm_sequential_retraining_resets_parameters);
    RUN_TEST(test_gmm_single_observation_dataset_training);
    RUN_TEST(test_gmm_prediction_idempotency);
    RUN_TEST(test_gmm_print_model_parameters_execution);
    RUN_TEST(test_gmm_covariance_regularization_positivity);
    RUN_TEST(test_gmm_negative_feature_values_training);
    RUN_TEST(test_gmm_large_magnitude_floating_point_inputs);
    RUN_TEST(test_gmm_small_magnitude_epsilon_inputs);
    RUN_TEST(test_gmm_zero_vector_prediction);
    RUN_TEST(test_gmm_1d_probability_density_peak);
    RUN_TEST(test_gmm_mutator_propagation_before_fit);
    RUN_TEST(test_gmm_early_convergence_exit);
    RUN_TEST(test_gmm_symmetric_dataset_equal_weights);

    // Destroy the session.
    return UNITY_END();
}