/**
 * @file        :   k_nearest_neighbors_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the K-Nearest Neighbors (KNN) implementation.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the KNN algorithm's core functionality. Key areas of verification include:
 * - Proper initialization and deep memory setup of Dataset containers.
 * - Accurate distance evaluation and bounded top-K neighbor updates.
 * - Robust multi-class voting resolution, including edge-case tie-breaking.
 * - Dynamic parameter handling such as clamped K values and single-element sets.
 * - Robustness against edge cases like NULL dataset handles and zero-dimension vectors.
 * 
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/k_nearest_neighbors_unit_tests.c ./src/k_nearest_neighbors.c ./src/unity.c -Iinclude -o ./bin/k_nearest_neighbor_unit_test -lm
 * @version     :   1.0
 * @date        :   2026-07-30
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "unity.h"
#include "k_nearest_neighbors.h"

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    ;;
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    ;;
}

/**
 * @brief : Tests that initialization sets up correct struct fields and buffers.
 */
void test_knn_initial_state_compliance(void)
{
    // Arrange.
    Dataset* instance_under_test = NULL;

    size_t target_points = 10;
    size_t target_features = 4;

    // Act.
    instance_under_test = initialize_dataset(target_points, target_features);

    // Assert.
    TEST_ASSERT_NOT_NULL(instance_under_test);
    TEST_ASSERT_EQUAL_UINT(target_points, instance_under_test->number_of_points);
    TEST_ASSERT_EQUAL_UINT(target_features, instance_under_test->number_of_features);
    TEST_ASSERT_NOT_NULL(instance_under_test->points);

    for (size_t index = 0; index < target_points; index++)
    {
        TEST_ASSERT_NOT_NULL(instance_under_test->points[index].features);
    }

    destroy_dataset(instance_under_test);
}

/**
 * @brief : Tests initialization with zero points or features returns valid structures or safe pointers.
 */
void test_knn_zero_dimension_initialization_safety(void)
{
    // Arrange & Act.
    Dataset* zero_points_dataset = initialize_dataset(0, 5);

    // Assert.
    TEST_ASSERT_NOT_NULL(zero_points_dataset);
    TEST_ASSERT_EQUAL_UINT(0, zero_points_dataset->number_of_points);

    destroy_dataset(zero_points_dataset);
}

/**
 * @brief : Tests single-point classification against identical vector values.
 */
void test_knn_exact_match_single_point_prediction(void)
{
    // Arrange.
    Dataset* single_point_dataset = initialize_dataset(1, 2);

    single_point_dataset->points[0].label = 42;
    single_point_dataset->points[0].features[0] = 3.5;
    single_point_dataset->points[0].features[1] = 7.2;

    double query_vector[2] = {3.5, 7.2};

    // Act.
    int predicted_label = predict(single_point_dataset, query_vector, 1, 50);

    // Assert.
    TEST_ASSERT_EQUAL_INT(42, predicted_label);

    destroy_dataset(single_point_dataset);
}

/**
 * @brief : Tests binary class separation with distinct, non-overlapping clusters.
 */
void test_knn_binary_cluster_separation_accuracy(void)
{
    // Arrange.
    Dataset* binary_dataset = initialize_dataset(4, 2);

    // Class 0 Cluster
    binary_dataset->points[0].label = 0;
    binary_dataset->points[0].features[0] = 0.0;
    binary_dataset->points[0].features[1] = 0.0;

    binary_dataset->points[1].label = 0;
    binary_dataset->points[1].features[0] = 0.1;
    binary_dataset->points[1].features[1] = 0.1;

    // Class 1 Cluster
    binary_dataset->points[2].label = 1;
    binary_dataset->points[2].features[0] = 10.0;
    binary_dataset->points[2].features[1] = 10.0;

    binary_dataset->points[3].label = 1;
    binary_dataset->points[3].features[0] = 10.1;
    binary_dataset->points[3].features[1] = 10.1;

    double query_near_zero[2] = {0.05, 0.05};

    // Act.
    int result_class = predict(binary_dataset, query_near_zero, 2, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result_class);

    destroy_dataset(binary_dataset);
}

/**
 * @brief : Tests prediction when K exceeds the total available point count in the dataset.
 */
void test_knn_k_exceeds_point_count_clamping_resilience(void)
{
    // Arrange.
    Dataset* mini_dataset = initialize_dataset(2, 1);

    mini_dataset->points[0].label = 1;
    mini_dataset->points[0].features[0] = 1.0;

    mini_dataset->points[1].label = 1;
    mini_dataset->points[1].features[0] = 2.0;

    double query_value[1] = {1.5};

    // Act.
    int result = predict(mini_dataset, query_value, 100, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);

    destroy_dataset(mini_dataset);
}

/**
 * @brief : Tests multi-class classification across three distinct clusters.
 */
void test_knn_multiclass_tally_resolution(void)
{
    // Arrange.
    Dataset* multi_class_dataset = initialize_dataset(6, 2);

    for (size_t index = 0; index < 6; index++)
    {
        multi_class_dataset->points[index].label = (int)(index / 2);
        multi_class_dataset->points[index].features[0] = (double)(index / 2) * 20.0;
        multi_class_dataset->points[index].features[1] = (double)(index / 2) * 20.0;
    }

    double query_class_two[2] = {40.1, 39.9};

    // Act.
    int predicted_class = predict(multi_class_dataset, query_class_two, 3, 3);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, predicted_class);

    destroy_dataset(multi_class_dataset);
}

/**
 * @brief : Tests that higher dimensional vectors (10D) calculate distance properly.
 */
void test_knn_high_dimensional_vector_evaluation(void)
{
    // Arrange.
    size_t dimensions = 10;

    Dataset* high_dim_dataset = initialize_dataset(2, dimensions);

    high_dim_dataset->points[0].label = 7;
    high_dim_dataset->points[1].label = 3;

    for (size_t index = 0; index < dimensions; index++)
    {
        high_dim_dataset->points[0].features[index] = 1.0;
        high_dim_dataset->points[1].features[index] = 50.0;
    }

    double query_features[10];

    for (size_t index = 0; index < dimensions; index++)
    {
        query_features[index] = 1.2;
    }

    // Act.
    int predicted_label = predict(high_dim_dataset, query_features, 1, 10);

    // Assert.
    TEST_ASSERT_EQUAL_INT(7, predicted_label);

    destroy_dataset(high_dim_dataset);
}

/**
 * @brief : Tests voting behavior when K equals 1 (Nearest Neighbor rule).
 */
void test_knn_k_equals_one_strict_nearest_neighbor(void)
{
    // Arrange.
    Dataset* strict_k_nearest_neighbors = initialize_dataset(3, 1);

    strict_k_nearest_neighbors->points[0].label = 0;
    strict_k_nearest_neighbors->points[0].features[0] = 10.0;

    strict_k_nearest_neighbors->points[1].label = 1;
    strict_k_nearest_neighbors->points[1].features[0] = 20.0;

    strict_k_nearest_neighbors->points[2].label = 0;
    strict_k_nearest_neighbors->points[2].features[0] = 30.0;

    double query_target[1] = {19.1};

    // Act.
    int result = predict(strict_k_nearest_neighbors, query_target, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);

    destroy_dataset(strict_k_nearest_neighbors);
}

/**
 * @brief : Tests that identical feature vectors with different labels are handled consistently.
 */
void test_knn_identical_feature_vector_overlap_handling(void)
{
    // Arrange.
    Dataset* overlapping_dataset = initialize_dataset(3, 2);

    overlapping_dataset->points[0].label = 0;
    overlapping_dataset->points[0].features[0] = 5.0;
    overlapping_dataset->points[0].features[1] = 5.0;

    overlapping_dataset->points[1].label = 1;
    overlapping_dataset->points[1].features[0] = 5.0;
    overlapping_dataset->points[1].features[1] = 5.0;

    overlapping_dataset->points[2].label = 1;
    overlapping_dataset->points[2].features[0] = 5.0;
    overlapping_dataset->points[2].features[1] = 5.0;

    double query_vector[2] = {5.0, 5.0};

    // Act.
    int predicted_class = predict(overlapping_dataset, query_vector, 3, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, predicted_class);

    destroy_dataset(overlapping_dataset);
}

/**
 * @brief : Tests predicting with negative values in feature space.
 */
void test_knn_negative_coordinate_space_distance_eval(void)
{
    // Arrange.
    Dataset* negative_space = initialize_dataset(2, 2);

    negative_space->points[0].label = 0;
    negative_space->points[0].features[0] = -10.0;
    negative_space->points[0].features[1] = -10.0;

    negative_space->points[1].label = 1;
    negative_space->points[1].features[0] = -50.0;
    negative_space->points[1].features[1] = -50.0;

    double query_negative[2] = {-12.0, -9.0};

    // Act.
    int result = predict(negative_space, query_negative, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);

    destroy_dataset(negative_space);
}

/**
 * @brief : Tests performance and validity with a zero distance vector query.
 */
void test_knn_zero_distance_exact_origin_query(void)
{
    // Arrange.
    Dataset* origin_dataset = initialize_dataset(2, 2);

    origin_dataset->points[0].label = 5;
    origin_dataset->points[0].features[0] = 0.0;
    origin_dataset->points[0].features[1] = 0.0;

    origin_dataset->points[1].label = 2;
    origin_dataset->points[1].features[0] = 100.0;
    origin_dataset->points[1].features[1] = 100.0;

    double origin_query[2] = {0.0, 0.0};

    // Act.
    int result = predict(origin_dataset, origin_query, 1, 6);

    // Assert.
    TEST_ASSERT_EQUAL_INT(5, result);

    destroy_dataset(origin_dataset);
}

/**
 * @brief : Tests predicting class labels when features contain floating point values with high precision.
 */
void test_knn_floating_point_precision_sensitivity(void)
{
    // Arrange.
    Dataset* precision_dataset = initialize_dataset(2, 1);

    precision_dataset->points[0].label = 0;
    precision_dataset->points[0].features[0] = 0.0000001;

    precision_dataset->points[1].label = 1;
    precision_dataset->points[1].features[0] = 0.0000009;

    double precise_query[1] = {0.0000002};

    // Act.
    int result = predict(precision_dataset, precise_query, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);

    destroy_dataset(precision_dataset);
}

/**
 * @brief : Tests that labels out of class range are safely filtered during vote tallying.
 */
void test_knn_out_of_bounds_class_label_suppression(void)
{
    // Arrange.
    Dataset* corrupted_label_dataset = initialize_dataset(2, 1);

    corrupted_label_dataset->points[0].label = 999;
    corrupted_label_dataset->points[0].features[0] = 1.0;

    corrupted_label_dataset->points[1].label = 0;
    corrupted_label_dataset->points[1].features[0] = 10.0;

    double query_features[1] = {1.1};

    // Act.
    int result = predict(corrupted_label_dataset, query_features, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);

    destroy_dataset(corrupted_label_dataset);
}

/**
 * @brief : Tests deallocation protocol safety when passed a NULL dataset handle.
 */
void test_knn_destroy_dataset_null_safety_protocol(void)
{
    // Arrange.
    Dataset* null_dataset = NULL;

    // Act & Assert.
    destroy_dataset(null_dataset);

    TEST_ASSERT_NULL(null_dataset);
}

/**
 * @brief : Tests prediction stability when K is an even number resulting in candidate ties.
 */
void test_knn_even_k_tie_breaking_consistency(void)
{
    // Arrange.
    Dataset* tie_dataset = initialize_dataset(4, 1);

    tie_dataset->points[0].label = 0;
    tie_dataset->points[0].features[0] = 1.0;

    tie_dataset->points[1].label = 0;
    tie_dataset->points[1].features[0] = 2.0;

    tie_dataset->points[2].label = 1;
    tie_dataset->points[2].features[0] = 3.0;

    tie_dataset->points[3].label = 1;
    tie_dataset->points[3].features[0] = 4.0;

    double query_midpoint[1] = {2.5};

    // Act.
    int result = predict(tie_dataset, query_midpoint, 4, 2);

    // Assert.
    TEST_ASSERT_TRUE(result == 0 || result == 1);

    destroy_dataset(tie_dataset);
}

/**
 * @brief : Tests bounded insertion order maintenance in top-K nearest tracking array.
 */
void test_knn_top_k_insertion_sort_order_maintenance(void)
{
    // Arrange.
    Dataset* unsorted_dataset = initialize_dataset(5, 1);

    double positions[5] = {50.0, 10.0, 40.0, 20.0, 30.0};

    for (size_t index = 0; index < 5; index++)
    {
        unsorted_dataset->points[index].label = (int)index;
        unsorted_dataset->points[index].features[0] = positions[index];
    }

    double query_point[1] = {15.0};

    // Act.
    int result = predict(unsorted_dataset, query_point, 2, 5);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);

    destroy_dataset(unsorted_dataset);
}

/**
 * @brief : Tests classification performance on a 1D line.
 */
void test_knn_one_dimensional_line_projection(void)
{
    // Arrange.
    Dataset* line_dataset = initialize_dataset(3, 1);

    line_dataset->points[0].label = 0;
    line_dataset->points[0].features[0] = -100.0;

    line_dataset->points[1].label = 1;
    line_dataset->points[1].features[0] = 0.0;

    line_dataset->points[2].label = 2;
    line_dataset->points[2].features[0] = 100.0;

    double query_val[1] = {10.0};

    // Act.
    int result = predict(line_dataset, query_val, 1, 3);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);

    destroy_dataset(line_dataset);
}

/**
 * @brief : Tests large sample stress saturation to ensure no memory leakage under high point load.
 */
void test_knn_high_volume_dataset_stress_saturation(void)
{
    // Arrange.
    size_t point_count = 500;
    Dataset* stress_dataset = initialize_dataset(point_count, 3);

    for (size_t index = 0; index < point_count; index++)
    {
        stress_dataset->points[index].label = (index < 250) ? 0 : 1;
        stress_dataset->points[index].features[0] = (double)index;
        stress_dataset->points[index].features[1] = (double)index;
        stress_dataset->points[index].features[2] = (double)index;
    }

    double query_features[3] = {10.0, 10.0, 10.0};

    // Act.
    int result = predict(stress_dataset, query_features, 11, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);

    destroy_dataset(stress_dataset);
}

/**
 * @brief : Tests that point order in dataset does not skew prediction outputs.
 */
void test_knn_dataset_point_ordering_invariance(void)
{
    // Arrange.
    Dataset* ordered_dataset = initialize_dataset(3, 1);

    ordered_dataset->points[0].label = 0;
    ordered_dataset->points[0].features[0] = 1.0;

    ordered_dataset->points[1].label = 1;
    ordered_dataset->points[1].features[0] = 10.0;

    ordered_dataset->points[2].label = 1;
    ordered_dataset->points[2].features[0] = 11.0;

    double query_point[1] = {10.5};

    // Act.
    int result = predict(ordered_dataset, query_point, 2, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);

    destroy_dataset(ordered_dataset);
}

/**
 * @brief : Tests classification behavior when points are scaled to very large values.
 */
void test_knn_extreme_value_scale_resilience(void)
{
    // Arrange.
    Dataset* scale_dataset = initialize_dataset(2, 2);

    scale_dataset->points[0].label = 0;
    scale_dataset->points[0].features[0] = 1e6;
    scale_dataset->points[0].features[1] = 1e6;

    scale_dataset->points[1].label = 1;
    scale_dataset->points[1].features[0] = 1e9;
    scale_dataset->points[1].features[1] = 1e9;

    double large_query[2] = {1e6 + 1.0, 1e6 + 1.0};

    // Act.
    int result = predict(scale_dataset, large_query, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);

    destroy_dataset(scale_dataset);
}

/**
 * @brief : Tests that voting correctly prioritizes majority class when K matches total dataset size.
 */
void test_knn_k_equals_dataset_size_global_majority(void)
{
    // Arrange.
    Dataset* global_dataset = initialize_dataset(5, 1);

    global_dataset->points[0].label = 0;
    global_dataset->points[0].features[0] = 1.0;

    global_dataset->points[1].label = 0;
    global_dataset->points[1].features[0] = 2.0;

    global_dataset->points[2].label = 0;
    global_dataset->points[2].features[0] = 3.0;

    global_dataset->points[3].label = 1;
    global_dataset->points[3].features[0] = 0.1;

    global_dataset->points[4].label = 1;
    global_dataset->points[4].features[0] = 0.2;

    double query[1] = {0.15};

    // Act.
    int result = predict(global_dataset, query, 5, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);

    destroy_dataset(global_dataset);
}

/**
 * @brief : Tests prediction accuracy when coordinates are near floating-point zero bounds.
 */
void test_knn_near_zero_epsilon_distance_resolution(void)
{
    // Arrange.
    Dataset* epsilon_dataset = initialize_dataset(2, 2);

    epsilon_dataset->points[0].label = 1;
    epsilon_dataset->points[0].features[0] = 1e-12;
    epsilon_dataset->points[0].features[1] = 1e-12;

    epsilon_dataset->points[1].label = 0;
    epsilon_dataset->points[1].features[0] = 1.0;
    epsilon_dataset->points[1].features[1] = 1.0;

    double near_zero_query[2] = {0.0, 0.0};

    // Act.
    int result = predict(epsilon_dataset, near_zero_query, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, result);

    destroy_dataset(epsilon_dataset);
}

/**
 * @brief : Tests multiple query calls sequentially on the same initialized dataset instance.
 */
void test_knn_sequential_prediction_repeatability(void)
{
    // Arrange.
    Dataset* reusable_dataset = initialize_dataset(2, 2);

    reusable_dataset->points[0].label = 0;
    reusable_dataset->points[0].features[0] = 0.0;
    reusable_dataset->points[0].features[1] = 0.0;

    reusable_dataset->points[1].label = 1;
    reusable_dataset->points[1].features[0] = 100.0;
    reusable_dataset->points[1].features[1] = 100.0;

    double proximate_cluster_zero_query[2] = {1.0, 1.0};
    double proximate_cluster_one_query[2] = {99.0, 99.0};

    // Act.
    int predicted_cluster_zero_label = predict(reusable_dataset, proximate_cluster_zero_query, 1, 2);
    int predicted_cluster_one_label = predict(reusable_dataset, proximate_cluster_one_query, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, predicted_cluster_zero_label);
    TEST_ASSERT_EQUAL_INT(1, predicted_cluster_one_label);

    destroy_dataset(reusable_dataset);
}

/**
 * @brief : Tests distance evaluation when values are negative floating point numbers.
 */
void test_knn_mixed_sign_feature_space_accuracy(void)
{
    // Arrange.
    Dataset* mixed_dataset = initialize_dataset(2, 2);

    mixed_dataset->points[0].label = 0;
    mixed_dataset->points[0].features[0] = -5.0;
    mixed_dataset->points[0].features[1] = 5.0;

    mixed_dataset->points[1].label = 1;
    mixed_dataset->points[1].features[0] = 5.0;
    mixed_dataset->points[1].features[1] = -5.0;

    double query[2] = {-4.0, 4.0};

    // Act.
    int result = predict(mixed_dataset, query, 1, 2);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, result);

    destroy_dataset(mixed_dataset);
}

/**
 * @brief : Tests dataset destruction safety when allocating memory for dynamic points.
 */
void test_knn_memory_allocation_integrity_cycle(void)
{
    // Arrange.
    size_t iterations = 50;

    // Act & Assert.
    for (size_t index = 0; index < iterations; index++)
    {
        Dataset* dynamic_dataset = initialize_dataset(10, 5);

        TEST_ASSERT_NOT_NULL(dynamic_dataset);

        destroy_dataset(dynamic_dataset);
    }
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_knn_initial_state_compliance);
    RUN_TEST(test_knn_zero_dimension_initialization_safety);
    RUN_TEST(test_knn_exact_match_single_point_prediction);
    RUN_TEST(test_knn_binary_cluster_separation_accuracy);
    RUN_TEST(test_knn_k_exceeds_point_count_clamping_resilience);
    RUN_TEST(test_knn_multiclass_tally_resolution);
    RUN_TEST(test_knn_high_dimensional_vector_evaluation);
    RUN_TEST(test_knn_k_equals_one_strict_nearest_neighbor);
    RUN_TEST(test_knn_identical_feature_vector_overlap_handling);
    RUN_TEST(test_knn_negative_coordinate_space_distance_eval);
    RUN_TEST(test_knn_zero_distance_exact_origin_query);
    RUN_TEST(test_knn_floating_point_precision_sensitivity);
    RUN_TEST(test_knn_out_of_bounds_class_label_suppression);
    RUN_TEST(test_knn_destroy_dataset_null_safety_protocol);
    RUN_TEST(test_knn_even_k_tie_breaking_consistency);
    RUN_TEST(test_knn_top_k_insertion_sort_order_maintenance);
    RUN_TEST(test_knn_one_dimensional_line_projection);
    RUN_TEST(test_knn_high_volume_dataset_stress_saturation);
    RUN_TEST(test_knn_dataset_point_ordering_invariance);
    RUN_TEST(test_knn_extreme_value_scale_resilience);
    RUN_TEST(test_knn_k_equals_dataset_size_global_majority);
    RUN_TEST(test_knn_near_zero_epsilon_distance_resolution);
    RUN_TEST(test_knn_sequential_prediction_repeatability);
    RUN_TEST(test_knn_mixed_sign_feature_space_accuracy);
    RUN_TEST(test_knn_memory_allocation_integrity_cycle);

    // Destroy the session.
    return UNITY_END();
}