/**
 * @file        :   closest_pair_of_points_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Closest Pair of Points geometric algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the closest_pair routine. Key areas of verification include:
 * - Proper handling of invalid arguments and boundary validation.
 * - Basic coordinate pairing metrics for small sets.
 * - Edge-case geometries including collinear and duplicate systems.
 * - Dynamic split boundary scenarios crossing the middle strip.
 * - High-volume dataset execution stability and performance scaling.
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/darwin/geometry/closest_pair_of_points_unit_tests.c ./src/darwin/geometry/closest-pair-of-points/closest_pair_of_points.c ./src/unity.c -Iinclude -o ./bin/closest_pair_of_points_unit_test
 * @version     :   1.0
 * @date        :   2026-05-29
 */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "unity.h"
#include "darwin/geometry/closest_pair_of_points.h"

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
 * @brief : Tests that passing a NULL points array returns an invalid pair layout.
 */
void test_closest_pair_null_pointer_safety_check(void)
{
    // Act.
    Pair result = closest_pair(NULL, 10);

    // Assert.
    TEST_ASSERT_FALSE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(DBL_MAX, result.distance);
}

/**
 * @brief : Tests that passing a count of zero elements returns an invalid pair layout.
 */
void test_closest_pair_zero_count_safety_check(void)
{
    // Arrange.
    Point points[1] = { {1.0, 1.0} };

    // Act.
    Pair result = closest_pair(points, 0);

    // Assert.
    TEST_ASSERT_FALSE(result.is_valid);
}

/**
 * @brief : Tests that passing a single point returns an invalid pair layout due to size constraints.
 */
void test_closest_pair_single_point_boundary_check(void)
{
    // Arrange.
    Point points[1] = { {5.0, 5.0} };

    // Act.
    Pair result = closest_pair(points, 1);

    // Assert.
    TEST_ASSERT_FALSE(result.is_valid);
}

/**
 * @brief : Tests that exactly two points properly settle into the base case layer.
 */
void test_closest_pair_minimum_valid_population(void)
{
    // Arrange.
    Point points[2] = 
    {
        { 0.0, 0.0 },
        { 3.0, 4.0 }
    };

    // Act.
    Pair result = closest_pair(points, 2);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, result.distance);
}

/**
 * @brief : Tests that exactly three points properly trigger the brute force perimeter fallback.
 */
void test_closest_pair_three_points_brute_force_fallback(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 0.0, 0.0 },
        { 1.0, 1.0 },
        { 1.0, 0.0 }
    };

    // Act.
    Pair result = closest_pair(points, 3);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, result.distance);
}

/**
 * @brief : Tests that four points properly cross over into the recursive utility split workflow.
 */
void test_closest_pair_four_points_recursive_split(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 0.0 },
        { 10.0, 10.0 },
        { 0.1, 0.0 },
        { 10.0, 10.5 }
    };

    // Act.
    Pair result = closest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.1, result.distance);
}

/**
 * @brief : Tests sorting tracking to ensure points are reordered by X coordinates effectively.
 */
void test_closest_pair_array_presorting_integrity(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 5.0, 0.0 },
        { 1.0, 0.0 },
        { 3.0, 0.0 }
    };

    // Act.
    closest_pair(points, 3);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(1.0, points[0].x);
    TEST_ASSERT_EQUAL_DOUBLE(3.0, points[1].x);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, points[2].x);
}

/**
 * @brief : Tests that identical overlapping duplicate coordinates process cleanly without zero divisions.
 */
void test_closest_pair_identical_duplicate_points_resolution(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 1.0, 2.0 },
        { 10.0, 20.0 },
        { 1.0, 2.0 },
        { 30.0, 40.0 }
    };

    // Act.
    Pair result = closest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, result.distance);
}

/**
 * @brief : Tests that the closest pair isolated entirely on the left plane segment is found.
 */
void test_closest_pair_isolated_left_plane_dominance(void)
{
    // Arrange.
    Point points[6] = 
    {
        { 1.0, 1.0 }, { 1.2, 1.0 },
        { 10.0, 10.0 }, { 11.0, 11.0 },
        { 20.0, 20.0 }, { 21.0, 21.0 }
    };

    // Act.
    Pair result = closest_pair(points, 6);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.2, result.distance);
}

/**
 * @brief : Tests that the closest pair isolated entirely on the right plane segment is found.
 */
void test_closest_pair_isolated_right_plane_dominance(void)
{
    // Arrange.
    Point points[6] = 
    {
        { 1.0, 1.0 }, { 2.0, 2.0 },
        { 10.0, 10.0 }, { 12.0, 12.0 },
        { 30.0, 30.0 }, { 30.0, 30.1 }
    };

    // Act.
    Pair result = closest_pair(points, 6);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.1, result.distance);
}

/**
 * @brief : Tests strip scanning filters when a pair spans directly across the middle divide.
 */
void test_closest_pair_cross_midline_strip_detection(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 4.9, 5.0 },
        { 1.0, 1.0 },
        { 9.0, 9.0 },
        { 5.1, 5.0 }
    };

    // Act.
    Pair result = closest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.2, result.distance);
}

/**
 * @brief : Tests handling of negative coordinate layouts distributed across axes.
 */
void test_closest_pair_negative_cartesian_quadrant_processing(void)
{
    // Arrange.
    Point points[4] = 
    {
        { -10.0, -10.0 },
        { -10.5, -10.0 },
        { -1.0, -1.0 },
        { -2.0, -2.0 }
    };

    // Act.
    Pair result = closest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.5, result.distance);
}

/**
 * @brief : Tests linear scanning behavior across perfectly horizontal collinear lines.
 */
void test_closest_pair_horizontal_collinear_distribution(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 5.0 },
        { 10.0, 5.0 },
        { 4.0, 5.0 },
        { 7.0, 5.0 },
        { 4.3, 5.0 }
    };

    // Act.
    Pair result = closest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.3, result.distance);
}

/**
 * @brief : Tests linear scanning behavior across perfectly vertical collinear lines.
 */
void test_closest_pair_vertical_collinear_distribution(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 2.0, 20.0 },
        { 2.0, 0.0 },
        { 2.0, 8.0 },
        { 2.0, 8.4 },
        { 2.0, 15.0 }
    };

    // Act.
    Pair result = closest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.4, result.distance);
}

/**
 * @brief : Tests that multiple pair sets with equal distances return safely without sorting lockups.
 */
void test_closest_pair_multiple_equidistant_matches(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 0.0 }, { 0.0, 1.0 },
        { 10.0, 10.0 }, { 10.0, 11.0 }
    };

    // Act.
    Pair result = closest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, result.distance);
}

/**
 * @brief : Tests floating point precision handling for highly tight coordinate configurations.
 */
void test_closest_pair_high_precision_micro_distance(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 1.000001, 1.0 },
        { 1.000003, 1.0 },
        { 5.0, 5.0 },
        { 6.0, 6.0 }
    };

    // Act.
    Pair result = closest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_FLOAT_WITHIN(1e-9, 0.000002, result.distance);
}

/**
 * @brief : Tests tracking constraints inside the vertical strip window using custom offsets.
 */
void test_closest_pair_strip_y_sorting_filter_enforcement(void)
{
    // Arrange.
    Point points[6] = 
    {
        { 4.9, 1.0 }, { 5.1, 10.0 },
        { 4.9, 2.0 }, { 5.1, 20.0 },
        { 4.9, 1.5 }, { 5.1, 30.0 }
    };

    // Act.
    Pair result = closest_pair(points, 6);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.5, result.distance);
}

/**
 * @brief : Tests uneven odd allocations across internal recursive split layers.
 */
void test_closest_pair_odd_population_asymmetric_splitting(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 1.0, 1.0 },
        { 2.0, 2.0 },
        { 3.0, 3.0 },
        { 4.0, 4.0 },
        { 4.0, 4.1 }
    };

    // Act.
    Pair result = closest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.1, result.distance);
}

/**
 * @brief : Tests that structured data properties align accurately upon passing fields out.
 */
void test_closest_pair_structural_field_assignment_accuracy(void)
{
    // Arrange.
    Point points[2] = 
    {
        { 1.5, 2.5 },
        { 1.5, 3.5 }
    };

    // Act.
    Pair result = closest_pair(points, 2);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, result.distance);
    TEST_ASSERT_EQUAL_DOUBLE(1.5, result.origin.x);
    TEST_ASSERT_EQUAL_DOUBLE(1.5, result.destination.x);
}

/**
 * @brief : Stress tests stability and resolution scaling with hundreds of dynamic points.
 */
void test_closest_pair_high_volume_stress_saturation(void)
{
    // Arrange.
    size_t count = 150;
    Point* points = malloc(sizeof(Point) * count);
    
    TEST_ASSERT_NOT_NULL(points);

    for (size_t index = 0; index < count; index++)
    {
        points[index].x = (double)index * 10.0;
        points[index].y = (double)index * 10.0;
    }

    // Explicitly inject a known closest pair at the tail segment.
    points[count - 1].x = 0.0;
    points[count - 1].y = 0.5;

    // Act.
    Pair result = closest_pair(points, count);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.5, result.distance);

    // Cleanup resources.
    free(points);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_closest_pair_null_pointer_safety_check);
    RUN_TEST(test_closest_pair_zero_count_safety_check);
    RUN_TEST(test_closest_pair_single_point_boundary_check);
    RUN_TEST(test_closest_pair_minimum_valid_population);
    RUN_TEST(test_closest_pair_three_points_brute_force_fallback);
    RUN_TEST(test_closest_pair_four_points_recursive_split);
    RUN_TEST(test_closest_pair_array_presorting_integrity);
    RUN_TEST(test_closest_pair_identical_duplicate_points_resolution);
    RUN_TEST(test_closest_pair_isolated_left_plane_dominance);
    RUN_TEST(test_closest_pair_isolated_right_plane_dominance);
    RUN_TEST(test_closest_pair_cross_midline_strip_detection);
    RUN_TEST(test_closest_pair_negative_cartesian_quadrant_processing);
    RUN_TEST(test_closest_pair_horizontal_collinear_distribution);
    RUN_TEST(test_closest_pair_vertical_collinear_distribution);
    RUN_TEST(test_closest_pair_multiple_equidistant_matches);
    RUN_TEST(test_closest_pair_high_precision_micro_distance);
    RUN_TEST(test_closest_pair_strip_y_sorting_filter_enforcement);
    RUN_TEST(test_closest_pair_odd_population_asymmetric_splitting);
    RUN_TEST(test_closest_pair_structural_field_assignment_accuracy);
    RUN_TEST(test_closest_pair_high_volume_stress_saturation);

    // Destroy the session.
    return UNITY_END();
}