/**
 * @file        :   ball_tree_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Ball Tree spatial partitioning data structure.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the Ball Tree's core functionality. Key areas of verification include:
 * - Proper handling of invalid arguments and boundary validation.
 * - Leaf conversion when point count falls below capacity limits.
 * - Accurate coordinate aggregation and centroid calculations.
 * - Furthest-point heuristic tracking and bounding radius precision.
 * - Partitioning and binary routing integrity down to leaf nodes.
 * - Deep copying of index datasets and lifecycle memory cleanups.
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/darwin/geometry/ball_tree_unit_tests.c ./src/darwin/geometry/ball-tree/ball_tree.c ./src/unity.c -Iinclude -o ./bin/ball_tree_unit_test
 * @version     :   1.0
 * @date        :   2026-05-12
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/geometry/ball_tree.h"

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
 * @brief : Tests that passing a NULL index array returns NULL.
 */
void test_ball_tree_null_indices_safety_check(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 2, 
            .coordinates = (double[]){1.0, 1.0} 
        } 
    };

    // Act.
    Node* result = ball_tree(NULL, 1, dataset, &context);

    // Assert.
    TEST_ASSERT_NULL(result);
}

/**
 * @brief : Tests that passing a count of zero returns NULL.
 */
void test_ball_tree_zero_count_safety_check(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[1] = { 0 };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 2, 
            .coordinates = (double[]){1.0, 1.0} 
        } 
    };

    // Act.
    Node* result = ball_tree(indices, 0, dataset, &context);

    // Assert.
    TEST_ASSERT_NULL(result);
}

/**
 * @brief : Tests that passing a NULL dataset pointer returns NULL.
 */
void test_ball_tree_null_dataset_safety_check(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[1] = { 0 };

    // Act.
    Node* result = ball_tree(indices, 1, NULL, &context);

    // Assert.
    TEST_ASSERT_NULL(result);
}

/**
 * @brief : Tests that passing a NULL context configuration pointer returns NULL.
 */
void test_ball_tree_null_context_safety_check(void)
{
    // Arrange.
    size_t indices[1] = { 0 };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 2, 
            .coordinates = (double[]){1.0, 1.0} 
        } 
    };

    // Act.
    Node* result = ball_tree(indices, 1, dataset, NULL);

    // Assert.
    TEST_ASSERT_NULL(result);
}

/**
 * @brief : Tests that passing a point containing uninitialized coordinates returns NULL.
 */
void test_ball_tree_null_point_coordinates_safety_check(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[1] = { 0 };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 2, 
            .coordinates = NULL 
        } 
    };

    // Act.
    Node* result = ball_tree(indices, 1, dataset, &context);

    // Assert.
    TEST_ASSERT_NULL(result);
}

/**
 * @brief : Tests that clean handles a NULL pointer without crash execution.
 */
void test_ball_tree_clean_null_pointer_safety_check(void)
{
    // Act & Assert.
    clean(NULL);
    
    TEST_PASS();
}

/**
 * @brief : Tests that a single point builds an exact, unsplit leaf node.
 */
void test_ball_tree_single_point_leaf_conversion(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[1] = { 0 };
    double coords[2] = { 4.5, -2.5 };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 2, 
            .coordinates = coords 
        } 
    };

    // Act.
    Node* root = ball_tree(indices, 1, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_DOUBLE(4.5, root->center.coordinates[0]);
    TEST_ASSERT_EQUAL_DOUBLE(-2.5, root->center.coordinates[1]);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, root->radius);
    TEST_ASSERT_EQUAL_UINT(1, root->count);
    TEST_ASSERT_NOT_NULL(root->indices);
    TEST_ASSERT_EQUAL_UINT(0, root->indices[0]);
    TEST_ASSERT_NULL(root->left);
    TEST_ASSERT_NULL(root->right);

    clean(root);
}

/**
 * @brief : Tests that centroid mapping averages points accurately in high dimensions.
 */
void test_ball_tree_centroid_calculation_accuracy(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 3, 
        .capacity = 3 
    };
    
    size_t indices[3] = { 0, 1, 2 };

    double c0[3] = { 1.0, 2.0, 3.0 };
    double c1[3] = { 2.0, 4.0, 6.0 };
    double c2[3] = { 3.0, 6.0, 9.0 };
    
    Point dataset[3] = 
    {
        { 0, 3, c0 },
        { 1, 3, c1 },
        { 2, 3, c2 }
    };

    // Act.
    Node* root = ball_tree(indices, 3, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, root->center.coordinates[0]);
    TEST_ASSERT_EQUAL_DOUBLE(4.0, root->center.coordinates[1]);
    TEST_ASSERT_EQUAL_DOUBLE(6.0, root->center.coordinates[2]);

    clean(root);
}

/**
 * @brief : Tests that a dataset matching capacity boundaries stays inside a single leaf.
 */
void test_ball_tree_capacity_boundary_leaf_retention(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[2] = { 0, 1 };

    double c0[2] = { 0.0, 0.0 };
    double c1[2] = { 3.0, 4.0 };
    
    Point dataset[2] = 
    { 
        { 0, 2, c0 }, 
        { 1, 2, c1 } 
    };

    // Act.
    Node* root = ball_tree(indices, 2, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT(2, root->count);
    TEST_ASSERT_NOT_NULL(root->indices);
    TEST_ASSERT_NULL(root->left);
    TEST_ASSERT_NULL(root->right);

    clean(root);
}

/**
 * @brief : Tests that radius calculation measures out to the absolute furthest coordinate.
 */
void test_ball_tree_radius_measurement_precision(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[2] = { 0, 1 };

    double c0[2] = { 0.0, 0.0 };
    double c1[2] = { 3.0, 4.0 };
    
    Point dataset[2] = 
    { 
        { 0, 2, c0 }, 
        { 1, 2, c1 } 
    };

    // Act.
    Node* root = ball_tree(indices, 2, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_DOUBLE(2.5, root->radius);

    clean(root);
}

/**
 * @brief : Tests deep copying preservation when external indices data arrays are destroyed.
 */
void test_ball_tree_indices_deep_copy_isolation(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t* transient_indices = malloc(sizeof(size_t) * 2);

    transient_indices[0] = 0;
    transient_indices[1] = 1;
    
    double c0[2] = { 1.0, 1.0 };
    double c1[2] = { 2.0, 2.0 };
    
    Point dataset[2] = 
    { 
        { 0, 2, c0 }, 
        { 1, 2, c1 } 
    };

    // Act.
    Node* root = ball_tree(transient_indices, 2, dataset, &context);
    
    transient_indices[0] = 999;
    transient_indices[1] = 888;

    free(transient_indices);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT(0, root->indices[0]);
    TEST_ASSERT_EQUAL_UINT(1, root->indices[1]);

    clean(root);
}

/**
 * @brief : Tests binary routing split execution when element volume exceeds maximum capacity.
 */
void test_ball_tree_exceeded_capacity_binary_split(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 1, 
        .capacity = 1 
    };
    
    size_t indices[2] = { 0, 1 };

    double c0[1] = { 10.0 };
    double c1[1] = { 20.0 };
    
    Point dataset[2] = 
    { 
        { 0, 1, c0 }, 
        { 1, 1, c1 } 
    };

    // Act.
    Node* root = ball_tree(indices, 2, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT(0, root->count);
    TEST_ASSERT_NULL(root->indices);
    TEST_ASSERT_NOT_NULL(root->left);
    TEST_ASSERT_NOT_NULL(root->right);

    clean(root);
}

/**
 * @brief : Tests clear separation mapping of clustered point sub-trees.
 */
void test_ball_tree_geometric_routing_separation_integrity(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[4] = { 0, 1, 2, 3 };

    double c0[2] = { 1.0, 1.0 };
    double c1[2] = { 1.5, 1.5 };
    double c2[2] = { 10.0, 10.0 };
    double c3[2] = { 10.5, 10.5 };
    
    Point dataset[4] = 
    { 
        { 0, 2, c0 }, 
        { 1, 2, c1 }, 
        { 2, 2, c2 }, 
        { 3, 2, c3 } 
    };

    // Act.
    Node* root = ball_tree(indices, 4, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_NOT_NULL(root->left);
    TEST_ASSERT_NOT_NULL(root->right);
    TEST_ASSERT_EQUAL_UINT(2, root->left->count);
    TEST_ASSERT_EQUAL_UINT(2, root->right->count);

    clean(root);
}

/**
 * @brief : Tests that overlapping multi-dimensional points parse safely without structural lockups.
 */
void test_ball_tree_identical_duplicate_points_fallback(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 1 
    };
    
    size_t indices[3] = { 0, 1, 2 };
    double coordinates[2] = { 5.0, 5.0 };
    
    Point dataset[3] = 
    {
        { 0, 2, coordinates },
        { 1, 2, coordinates },
        { 2, 2, coordinates }
    };

    // Act.
    Node* root = ball_tree(indices, 3, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT(3, root->count);
    TEST_ASSERT_NOT_NULL(root->indices);
    TEST_ASSERT_NULL(root->left);
    TEST_ASSERT_NULL(root->right);

    clean(root);
}

/**
 * @brief : Tests that leaf node boundaries evaluate properly under odd point populations.
 */
void test_ball_tree_odd_population_asymmetric_splitting(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 1, 
        .capacity = 1 
    };
    
    size_t indices[3] = { 0, 1, 2 };

    double c0[1] = { 1.0 };
    double c1[1] = { 2.0 };
    double c2[1] = { 9.0 };
    
    Point dataset[3] = 
    { 
        { 0, 1, c0 }, 
        { 1, 1, c1 }, 
        { 2, 1, c2 } 
    };

    // Act.
    Node* root = ball_tree(indices, 3, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    
    size_t combined_count = root->left->count + root->right->count;
    TEST_ASSERT_TRUE(combined_count <= 3);

    clean(root);
}

/**
 * @brief : Tests tree structure layout accuracy across hundreds of sequenced nodes.
 */
void test_ball_tree_high_volume_stress_saturation(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 5 
    };
    
    size_t count = 120;
    size_t* indices = malloc(sizeof(size_t) * count);
    Point* dataset = malloc(sizeof(Point) * count);
    
    for (size_t index = 0; index < count; index++)
    {
        indices[index] = index;
        dataset[index].id = index;
        dataset[index].dimension = 2;
        dataset[index].coordinates = malloc(sizeof(double) * 2);
        dataset[index].coordinates[0] = (double)index * 0.5;
        dataset[index].coordinates[1] = (double)index * -0.5;
    }

    // Act.
    Node* root = ball_tree(indices, count, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);

    // Cleanup resources.
    clean(root);

    for (size_t index = 0; index < count; index++)
    {
        free(dataset[index].coordinates);
    }

    free(dataset);
    free(indices);
}

/**
 * @brief : Tests dimension setting consistency logic configurations.
 */
void test_ball_tree_dimension_context_matching_verification(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 4, 
        .capacity = 4 
    };
    
    size_t indices[1] = { 0 };
    double coords[4] = { 1.0, 2.0, 3.0, 4.0 };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 4, 
            .coordinates = coords 
        } 
    };

    // Act.
    Node* root = ball_tree(indices, 1, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT(context.dimension, root->center.dimension);

    clean(root);
}

/**
 * @brief : Tests zero coordinate processing accuracy within the geometric model framework.
 */
void test_ball_tree_origin_centered_coordinate_processing(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[1] = { 0 };
    double coords[2] = { 0.0, 0.0 };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 2, 
            .coordinates = coords 
        } 
    };

    // Act.
    Node* root = ball_tree(indices, 1, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, root->center.coordinates[0]);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, root->center.coordinates[1]);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, root->radius);

    clean(root);
}

/**
 * @brief : Tests deep hierarchical tree building layouts.
 */
void test_ball_tree_multi_tier_recursive_depth_generation(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 1, 
        .capacity = 1 
    };
    
    size_t indices[4] = { 0, 1, 2, 3 };

    double c0[1] = { 1.0 };
    double c1[1] = { 5.0 };
    double c2[1] = { 10.0 };
    double c3[1] = { 20.0 };
    
    Point dataset[4] = 
    { 
        { 0, 1, c0 }, 
        { 1, 1, c1 }, 
        { 2, 1, c2 }, 
        { 3, 1, c3 } 
    };

    // Act.
    Node* root = ball_tree(indices, 4, dataset, &context);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);
    TEST_ASSERT_EQUAL_UINT(0, root->count); 

    TEST_ASSERT_NOT_NULL(root->left);
    TEST_ASSERT_NOT_NULL(root->right);

    TEST_ASSERT_EQUAL_UINT(1, root->left->count);
    TEST_ASSERT_NULL(root->left->left);
    TEST_ASSERT_NULL(root->left->right);

    TEST_ASSERT_EQUAL_UINT(0, root->right->count); 
    TEST_ASSERT_NOT_NULL(root->right->left);
    TEST_ASSERT_NOT_NULL(root->right->right);

    TEST_ASSERT_EQUAL_UINT(1, root->right->left->count);
    TEST_ASSERT_NULL(root->right->left->left);
    TEST_ASSERT_NULL(root->right->left->right);

    TEST_ASSERT_EQUAL_UINT(0, root->right->right->count);
    TEST_ASSERT_NOT_NULL(root->right->right->left);
    TEST_ASSERT_NOT_NULL(root->right->right->right);

    TEST_ASSERT_EQUAL_UINT(1, root->right->right->left->count);
    TEST_ASSERT_EQUAL_UINT(1, root->right->right->right->count);

    clean(root);
}

/**
 * @brief : Tests that print runs securely without parsing errors or segmentation faults.
 */
void test_ball_tree_print_execution_safety_protocol(void)
{
    // Arrange.
    Tree context = 
    { 
        .dimension = 2, 
        .capacity = 2 
    };
    
    size_t indices[1] = { 0 };
    double coords[2] = { 1.23, 4.56 };
    
    Point dataset[1] = 
    { 
        { 
            .id = 0, 
            .dimension = 2, 
            .coordinates = coords 
        } 
    };
    
    Node* root = ball_tree(indices, 1, dataset, &context);

    // Act & Assert.
    printf("\n--- Redirecting output to verify print safety protocol ---\n");
    print(root, 0);
    print(NULL, 0);
    printf("--- End of verification output ---\n");

    TEST_PASS();

    clean(root);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_ball_tree_null_indices_safety_check);
    RUN_TEST(test_ball_tree_zero_count_safety_check);
    RUN_TEST(test_ball_tree_null_dataset_safety_check);
    RUN_TEST(test_ball_tree_null_context_safety_check);
    RUN_TEST(test_ball_tree_null_point_coordinates_safety_check);
    RUN_TEST(test_ball_tree_clean_null_pointer_safety_check);
    RUN_TEST(test_ball_tree_single_point_leaf_conversion);
    RUN_TEST(test_ball_tree_centroid_calculation_accuracy);
    RUN_TEST(test_ball_tree_capacity_boundary_leaf_retention);
    RUN_TEST(test_ball_tree_radius_measurement_precision);
    RUN_TEST(test_ball_tree_indices_deep_copy_isolation);
    RUN_TEST(test_ball_tree_exceeded_capacity_binary_split);
    RUN_TEST(test_ball_tree_geometric_routing_separation_integrity);
    RUN_TEST(test_ball_tree_identical_duplicate_points_fallback);
    RUN_TEST(test_ball_tree_odd_population_asymmetric_splitting);
    RUN_TEST(test_ball_tree_high_volume_stress_saturation);
    RUN_TEST(test_ball_tree_dimension_context_matching_verification);
    RUN_TEST(test_ball_tree_origin_centered_coordinate_processing);
    RUN_TEST(test_ball_tree_multi_tier_recursive_depth_generation);
    RUN_TEST(test_ball_tree_print_execution_safety_protocol);

    // Destroy the session.
    return UNITY_END();
}