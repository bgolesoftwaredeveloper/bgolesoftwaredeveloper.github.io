/**
 * @file        :   support_vector_classifier_unit_tests.cpp
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for SupportVectorClassifier implementation.
 * @details     :   Provides 30 comprehensive unit tests using the Unity Test Framework
 *                  to validate constructor initialization, getter/setter mutations, linear
 *                  margin boundaries, decision score calculation, label predictions,
 *                  training error handlings, convergence behavior, and summary outputs.
 * Compile:
 * clang -c -DUNITY_INCLUDE_DOUBLE ./src/unity.c -Iinclude -o ./bin/unity.o
 * clang++ -std=c++17 -DUNITY_INCLUDE_DOUBLE ./test/support_vector_classifier_unit_tests.cpp ./src/support_vector_classifier.cpp ./bin/unity.o -Iinclude -o ./bin/svc_unit_test
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#include <vector>
#include <stdexcept>
#include <cmath>

extern "C" {
    #include "unity.h"
}

#include "support_vector_classifier.hpp"

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
 * @brief 1. Tests default constructor initialization values.
 */
void test_svc_default_constructor_defaults(void)
{
    // Arrange & Act.
    SupportVectorClassifier svc;

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(0.001, svc.get_learning_rate());
    TEST_ASSERT_EQUAL_DOUBLE(100.0, svc.get_regularization_penalty());
    TEST_ASSERT_EQUAL_UINT(10000, svc.get_maximum_iterations());
    TEST_ASSERT_EQUAL_DOUBLE(0.0, svc.get_intercept_bias());
    TEST_ASSERT_EQUAL_UINT(0, svc.get_weight_coefficients().size());
}

/**
 * @brief 2. Tests parameterized constructor state assignment.
 */
void test_svc_parameterized_constructor_assignment(void)
{
    // Arrange & Act.
    SupportVectorClassifier svc(0.05, 50.0, 500);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(0.05, svc.get_learning_rate());
    TEST_ASSERT_EQUAL_DOUBLE(50.0, svc.get_regularization_penalty());
    TEST_ASSERT_EQUAL_UINT(500, svc.get_maximum_iterations());
    TEST_ASSERT_EQUAL_DOUBLE(0.0, svc.get_intercept_bias());
}

/**
 * @brief 3. Tests learning rate accessor and mutator.
 */
void test_svc_set_and_get_learning_rate(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    // Act.
    svc.set_learning_rate(0.025);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(0.025, svc.get_learning_rate());
}

/**
 * @brief 4. Tests regularization penalty accessor and mutator.
 */
void test_svc_set_and_get_regularization_penalty(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    // Act.
    svc.set_regularization_penalty(250.0);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(250.0, svc.get_regularization_penalty());
}

/**
 * @brief 5. Tests maximum iterations accessor and mutator.
 */
void test_svc_set_and_get_maximum_iterations(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    // Act.
    svc.set_maximum_iterations(2500);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2500, svc.get_maximum_iterations());
}

/**
 * @brief 6. Tests intercept bias accessor and mutator.
 */
void test_svc_set_and_get_intercept_bias(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    // Act.
    svc.set_intercept_bias(-1.25);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(-1.25, svc.get_intercept_bias());
}

/**
 * @brief 7. Tests decision score computation for a trained 2D point.
 */
void test_svc_compute_decision_value_calculation(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> x_axis = {{0.0, 0.0}};
    std::vector<int> y = {1};
    
    svc.set_maximum_iterations(1);
    svc.train(x_axis, y);
    svc.set_intercept_bias(0.5);

    std::vector<double> sample = {2.0, 3.0};

    // Act.
    double decision_score = svc.compute_decision_value(sample);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(0.5, decision_score);
}

/**
 * @brief 8. Tests exception throwing on decision value dimension mismatch.
 */
void test_svc_compute_decision_value_dimension_mismatch_exception(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> x_axis = {{1.0, 2.0}};
    std::vector<int> y = {1};

    svc.set_maximum_iterations(1);
    svc.train(x_axis, y);

    std::vector<double> mismatched_sample = {1.0, 2.0, 3.0};

    // Act & Assert.
    try
    {
        svc.compute_decision_value(mismatched_sample);

        TEST_FAIL_MESSAGE("Expected std::invalid_argument exception was not thrown.");
    }
    catch (const std::invalid_argument& e)
    {
        TEST_ASSERT_NOT_NULL(e.what());
    }
}

/**
 * @brief 9. Tests binary class label prediction mapping for positive score.
 */
void test_svc_predict_class_label_positive_boundary(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> x_axis = {{1.0}};
    std::vector<int> y = {1};

    svc.set_maximum_iterations(1);
    svc.train(x_axis, y);
    svc.set_intercept_bias(0.1);

    std::vector<double> sample = {0.0};

    // Act.
    int label = svc.predict_class_label(sample);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, label);
}

/**
 * @brief 10. Tests binary class label prediction mapping for negative score.
 */
void test_svc_predict_class_label_negative_boundary(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> x_axis = {{1.0}};
    std::vector<int> y = {1};

    svc.set_maximum_iterations(1);
    svc.train(x_axis, y);
    svc.set_intercept_bias(-0.1);

    std::vector<double> sample = {0.0};

    // Act.
    int label = svc.predict_class_label(sample);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, label);
}

/**
 * @brief 11. Tests decision boundary exact zero tie breaking to +1.
 */
void test_svc_predict_class_label_zero_boundary_tie_break(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> x_axis = {{1.0}};
    std::vector<int> y = {1};

    svc.set_maximum_iterations(1);
    svc.train(x_axis, y);
    svc.set_intercept_bias(0.0);

    std::vector<double> sample = {0.0};

    // Act.
    int label = svc.predict_class_label(sample);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, label);
}

/**
 * @brief 12. Tests batch sample prediction array length and order matching.
 */
void test_svc_predict_class_labels_batch_vector(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> x_axis = {{1.0}};
    std::vector<int> y = {1};

    svc.set_maximum_iterations(1);
    svc.train(x_axis, y);
    svc.set_intercept_bias(1.0);

    std::vector<std::vector<double>> samples = {{1.0}, {-2.0}, {0.0}};

    // Act.
    std::vector<int> predictions = svc.predict_class_labels(samples);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(3, predictions.size());
    TEST_ASSERT_EQUAL_INT(1, predictions[0]);
    TEST_ASSERT_EQUAL_INT(1, predictions[1]);
    TEST_ASSERT_EQUAL_INT(1, predictions[2]);
}

/**
 * @brief 13. Tests training failure exception when feature matrix is empty.
 */
void test_svc_train_empty_features_exception(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> empty_features;
    std::vector<int> labels = {1, -1};

    // Act & Assert.
    try
    {
        svc.train(empty_features, labels);

        TEST_FAIL_MESSAGE("Expected exception on empty features vector.");
    }
    catch (const std::invalid_argument& e)
    {
        TEST_ASSERT_NOT_NULL(e.what());
    }
}

/**
 * @brief 14. Tests training failure exception when target labels are empty.
 */
void test_svc_train_empty_labels_exception(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> features = {{1.0, 2.0}};
    std::vector<int> empty_labels;

    // Act & Assert.
    try
    {
        svc.train(features, empty_labels);

        TEST_FAIL_MESSAGE("Expected exception on empty labels vector.");
    }
    catch (const std::invalid_argument& e)
    {
        TEST_ASSERT_NOT_NULL(e.what());
    }
}

/**
 * @brief 15. Tests training failure exception when dataset feature and label counts mismatch.
 */
void test_svc_train_dimension_mismatch_exception(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> features = {{1.0, 2.0}, {3.0, 4.0}};
    std::vector<int> labels = {1};

    // Act & Assert.
    try
    {
        svc.train(features, labels);

        TEST_FAIL_MESSAGE("Expected exception on sample/label length mismatch.");
    }
    catch (const std::invalid_argument& e)
    {
        TEST_ASSERT_NOT_NULL(e.what());
    }
}

/**
 * @brief 16. Tests weight initialization clearing during fit process.
 */
void test_svc_train_weight_vector_initialization_dimension(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> features = {{1.0, 2.0, 3.0, 4.0}};
    std::vector<int> labels = {1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(4, svc.get_weight_coefficients().size());
}

/**
 * @brief 17. Tests linear separation convergence on a simple 1D dataset.
 */
void test_svc_train_linearly_separable_1d(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.01, 10.0, 1000);

    std::vector<std::vector<double>> features = {{-2.0}, {-1.0}, {1.0}, {2.0}};
    std::vector<int> labels = {-1, -1, 1, 1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, svc.predict_class_label({-1.5}));
    TEST_ASSERT_EQUAL_INT(1, svc.predict_class_label({1.5}));
}

/**
 * @brief 18. Tests linear separation convergence on 2D cluster points.
 */
void test_svc_train_linearly_separable_2d_clusters(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.01, 100.0, 2000);

    std::vector<std::vector<double>> features = {
        {1.0, 1.0}, {1.5, 1.5},
        {5.0, 5.0}, {6.0, 6.0}
    };

    std::vector<int> labels = {-1, -1, 1, 1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, svc.predict_class_label({1.2, 1.2}));
    TEST_ASSERT_EQUAL_INT(1, svc.predict_class_label({5.5, 5.5}));
}

/**
 * @brief 19. Tests model behavior when learning rate is set to zero.
 */
void test_svc_train_zero_learning_rate_stability(void)
{
    // Arrange.
    try
    {
        SupportVectorClassifier svc(0.0, 10.0, 100);

        std::vector<std::vector<double>> features = {{1.0, 2.0}};
        std::vector<int> labels = {1};

        // Act.
        svc.train(features, labels);

        // Assert.
        TEST_ASSERT_EQUAL_DOUBLE(0.0, svc.get_weight_coefficients()[0]);
        TEST_ASSERT_EQUAL_DOUBLE(0.0, svc.get_weight_coefficients()[1]);
        TEST_ASSERT_EQUAL_DOUBLE(0.0, svc.get_intercept_bias());
    }
    catch(const std::invalid_argument& exception)
    {
        TEST_ASSERT_NOT_NULL(exception.what());
    }
}

/**
 * @brief 20. Tests model fit consistency over a single iteration epoch.
 */
void test_svc_train_single_iteration_execution(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.01, 100.0, 1);

    std::vector<std::vector<double>> features = {{2.0, -1.0}};
    std::vector<int> labels = {1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, svc.get_weight_coefficients().size());
}

/**
 * @brief 21. Tests high feature space scaling up to 20 dimensions.
 */
void test_svc_high_dimensional_feature_training(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.001, 10.0, 100);

    std::size_t dimension = 20;

    std::vector<double> sample_pos(dimension, 2.0);
    std::vector<double> sample_neg(dimension, -2.0);

    std::vector<std::vector<double>> features = {sample_pos, sample_neg};
    std::vector<int> labels = {1, -1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(dimension, svc.get_weight_coefficients().size());
}

/**
 * @brief 22. Tests prediction stability when evaluating origin point.
 */
void test_svc_zero_vector_prediction(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> features = {{1.0, 1.0}};
    std::vector<int> labels = {1};

    svc.set_maximum_iterations(1);
    svc.train(features, labels);
    svc.set_intercept_bias(2.5);

    std::vector<double> origin = {0.0, 0.0};

    // Act.
    double score = svc.compute_decision_value(origin);
    int label = svc.predict_class_label(origin);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(2.5, score);
    TEST_ASSERT_EQUAL_INT(1, label);
}

/**
 * @brief 23. Tests training with negative feature coordinates.
 */
void test_svc_negative_feature_values_training(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.01, 50.0, 500);

    std::vector<std::vector<double>> features = {
        {-10.0, -10.0}, {-8.0, -9.0},
        {-1.0, -1.0}, {-2.0, -1.0}
    };

    std::vector<int> labels = {-1, -1, 1, 1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_INT(-1, svc.predict_class_label({-9.0, -9.0}));
    TEST_ASSERT_EQUAL_INT(1, svc.predict_class_label({-1.5, -1.0}));
}

/**
 * @brief 24. Tests training stability with high magnitude floating point values.
 */
void test_svc_large_magnitude_floating_point_inputs(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.00001, 1.0, 100);

    std::vector<std::vector<double>> features = {
        {1e5, 1e5},
        {-1e5, -1e5}
    };

    std::vector<int> labels = {1, -1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, svc.get_weight_coefficients().size());
}

/**
 * @brief 25. Tests training stability with microscopic epsilon values.
 */
void test_svc_small_magnitude_epsilon_inputs(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.001, 100.0, 100);

    std::vector<std::vector<double>> features = {
        {1e-6, 1e-6},
        {-1e-6, -1e-6}
    };

    std::vector<int> labels = {1, -1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, svc.get_weight_coefficients().size());
}

/**
 * @brief 26. Tests re-running train() sequentially overwrites model weights properly.
 */
void test_svc_sequential_retraining_resets_weights(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.01, 10.0, 100);

    std::vector<std::vector<double>> features_3d = {{1.0, 1.0, 1.0}};
    std::vector<int> labels_3d = {1};

    // Act (First Train).
    svc.train(features_3d, labels_3d);

    // Assert (First Train).
    TEST_ASSERT_EQUAL_UINT(3, svc.get_weight_coefficients().size());

    // Arrange (Second Train).
    std::vector<std::vector<double>> features_2d = {{2.0, 2.0}};
    std::vector<int> labels_2d = {-1};

    // Act (Second Train).
    svc.train(features_2d, labels_2d);

    // Assert (Second Train).
    TEST_ASSERT_EQUAL_UINT(2, svc.get_weight_coefficients().size());
}

/**
 * @brief 27. Tests identical features with opposite labels convergence resilience.
 */
void test_svc_overlapping_identical_features_opposite_labels(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.001, 10.0, 200);

    std::vector<std::vector<double>> features = {
        {1.0, 1.0},
        {1.0, 1.0}
    };

    std::vector<int> labels = {1, -1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, svc.get_weight_coefficients().size());
}

/**
 * @brief 28. Tests print_classifier_summary output execution without crashing.
 */
void test_svc_print_classifier_summary_execution(void)
{
    // Arrange.
    SupportVectorClassifier svc;

    std::vector<std::vector<double>> features = {{1.0, 2.0}};
    std::vector<int> labels = {1};

    svc.train(features, labels);

    // Act.
    print_classifier_summary(svc);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, svc.get_weight_coefficients().size());
}

/**
 * @brief 29. Tests consistency of predictions across repeated query evaluations.
 */
void test_svc_prediction_idempotency(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.01, 50.0, 500);

    std::vector<std::vector<double>> features = {{2.0}, {-2.0}};
    std::vector<int> labels = {1, -1};

    svc.train(features, labels);

    std::vector<double> sample = {1.5};

    // Act.
    int pred1 = svc.predict_class_label(sample);
    int pred2 = svc.predict_class_label(sample);

    // Assert.
    TEST_ASSERT_EQUAL_INT(pred1, pred2);
}

/**
 * @brief 30. Tests single-sample dataset training execution.
 */
void test_svc_single_observation_dataset_training(void)
{
    // Arrange.
    SupportVectorClassifier svc(0.01, 100.0, 100);

    std::vector<std::vector<double>> features = {{5.0, 10.0}};
    std::vector<int> labels = {1};

    // Act.
    svc.train(features, labels);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, svc.get_weight_coefficients().size());
    TEST_ASSERT_EQUAL_INT(1, svc.predict_class_label({5.0, 10.0}));
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_svc_default_constructor_defaults);
    RUN_TEST(test_svc_parameterized_constructor_assignment);
    RUN_TEST(test_svc_set_and_get_learning_rate);
    RUN_TEST(test_svc_set_and_get_regularization_penalty);
    RUN_TEST(test_svc_set_and_get_maximum_iterations);
    RUN_TEST(test_svc_set_and_get_intercept_bias);
    RUN_TEST(test_svc_compute_decision_value_calculation);
    RUN_TEST(test_svc_compute_decision_value_dimension_mismatch_exception);
    RUN_TEST(test_svc_predict_class_label_positive_boundary);
    RUN_TEST(test_svc_predict_class_label_negative_boundary);
    RUN_TEST(test_svc_predict_class_label_zero_boundary_tie_break);
    RUN_TEST(test_svc_predict_class_labels_batch_vector);
    RUN_TEST(test_svc_train_empty_features_exception);
    RUN_TEST(test_svc_train_empty_labels_exception);
    RUN_TEST(test_svc_train_dimension_mismatch_exception);
    RUN_TEST(test_svc_train_weight_vector_initialization_dimension);
    RUN_TEST(test_svc_train_linearly_separable_1d);
    RUN_TEST(test_svc_train_linearly_separable_2d_clusters);
    RUN_TEST(test_svc_train_zero_learning_rate_stability);
    RUN_TEST(test_svc_train_single_iteration_execution);
    RUN_TEST(test_svc_high_dimensional_feature_training);
    RUN_TEST(test_svc_zero_vector_prediction);
    RUN_TEST(test_svc_negative_feature_values_training);
    RUN_TEST(test_svc_large_magnitude_floating_point_inputs);
    RUN_TEST(test_svc_small_magnitude_epsilon_inputs);
    RUN_TEST(test_svc_sequential_retraining_resets_weights);
    RUN_TEST(test_svc_overlapping_identical_features_opposite_labels);
    RUN_TEST(test_svc_print_classifier_summary_execution);
    RUN_TEST(test_svc_prediction_idempotency);
    RUN_TEST(test_svc_single_observation_dataset_training);

    // Destory the sesssion.
    return UNITY_END();
}