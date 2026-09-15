/**
 * @file        :   farthest_pair_of_points_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Farthest Pair of Points geometric algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the farthest_pair routine. Key areas of verification include:
 * - Proper handling of invalid arguments and boundary validation.
 * - Basic coordinate pairing metrics for small hull profiles.
 * - Processing accuracy of internal points vs. outer boundary extrema.
 * - Edge-case geometries including collinear networks and overlapping clusters.
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/darwin/geometry/farthest_pair_of_points_unit_tests.c ./src/darwin/geometry/farthest-pair-of-points/farthest_pair_of_points.c ./src/unity.c -Iinclude -o ./bin/farthest_pair_of_points_unit_test
 * @version     :   1.0
 * @date        :   2026-05-30
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/geometry/farthest_pair_of_points.h"

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
void test_farthest_pair_null_pointer_safety_check(void)
{
    // Act.
    Pair result = farthest_pair(NULL, 10);

    // Assert.
    TEST_ASSERT_FALSE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, result.distnace);
}

/**
 * @brief : Tests that passing a count of zero elements returns an invalid pair layout.
 */
void test_farthest_pair_zero_count_safety_check(void)
{
    // Arrange.
    Point points[1] = { {1.0, 1.0} };

    // Act.
    Pair result = farthest_pair(points, 0);

    // Assert.
    TEST_ASSERT_FALSE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, result.distnace);
}

/**
 * @brief : Tests that passing a single point returns an invalid pair layout due to size constraints.
 */
void test_farthest_pair_single_point_boundary_check(void)
{
    // Arrange.
    Point points[1] = { {5.0, 5.0} };

    // Act.
    Pair result = farthest_pair(points, 1);

    // Assert.
    TEST_ASSERT_FALSE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, result.distnace);
}

/**
 * @brief : Tests that exactly two points bypass complex loops and calculate directly.
 */
void test_farthest_pair_minimum_valid_population(void)
{
    // Arrange.
    Point points[2] = 
    {
        { 0.0, 0.0 },
        { 3.0, 4.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 2);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(5.0, result.distnace);
}

/**
 * @brief : Tests that exactly three points process correctly through the convex hull foundation.
 */
void test_farthest_pair_three_points_hull_handling(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 0.0, 0.0 },
        { 6.0, 0.0 },
        { 0.0, 8.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 3);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, result.distnace);
}

/**
 * @brief : Tests sorting tracking to ensure input points are reordered by X coordinates effectively.
 */
void test_farthest_pair_array_presorting_integrity(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 9.0, 1.0 },
        { 2.0, 5.0 },
        { 4.0, 3.0 }
    };

    // Act.
    farthest_pair(points, 3);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(2.0, points[0].x);
    TEST_ASSERT_EQUAL_DOUBLE(4.0, points[1].x);
    TEST_ASSERT_EQUAL_DOUBLE(9.0, points[2].x);
}

/**
 * @brief : Tests that interior points inside the convex hull are successfully ignored by calipers.
 */
void test_farthest_pair_interior_point_exclusion(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 0.0 }, { 10.0, 0.0 }, { 0.0, 10.0 }, { 10.0, 10.0 },
        { 5.0, 5.0 } // Central point.
    };

    // Act.
    Pair result = farthest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(sqrt(200.0), result.distnace);
}

/**
 * @brief : Tests that completely identical overlapping duplicate coordinates handle without issues.
 */
void test_farthest_pair_identical_duplicate_points_resolution(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 5.0, 5.0 },
        { 5.0, 5.0 },
        { 5.0, 5.0 },
        { 5.0, 5.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, result.distnace);
}

/**
 * @brief : Tests handling of negative coordinate layouts distributed across axes.
 */
void test_farthest_pair_negative_cartesian_quadrant_processing(void)
{
    // Arrange.
    Point points[4] = 
    {
        { -5.0, -5.0 },
        { 5.0, 5.0 },
        { -5.0, 5.0 },
        { 5.0, -5.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(sqrt(200.0), result.distnace);
}

/**
 * @brief : Tests caliper scanning behavior across perfectly horizontal collinear lines.
 */
void test_farthest_pair_horizontal_collinear_distribution(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 2.0, 3.0 },
        { 12.0, 3.0 },
        { 5.0, 3.0 },
        { 9.0, 3.0 },
        { -1.0, 3.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(13.0, result.distnace);
}

/**
 * @brief : Tests caliper scanning behavior across perfectly vertical collinear lines.
 */
void test_farthest_pair_vertical_collinear_distribution(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 4.0, 10.0 },
        { 4.0, -2.0 },
        { 4.0, 5.0 },
        { 4.0, 0.0 },
        { 4.0, 15.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(17.0, result.distnace);
}

/**
 * @brief : Tests sorting and hull updates when extreme positions share matching X values.
 */
void test_farthest_pair_tie_breaking_by_y_coordinate(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 0.0 },
        { 0.0, 10.0 },
        { 5.0, 5.0 },
        { 0.0, -5.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(15.0, result.distnace);
}

/**
 * @brief : Tests rotating calipers accuracy when the target geometry forms a regular square.
 */
void test_farthest_pair_perfect_square_perimeter(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 0.0 },
        { 4.0, 0.0 },
        { 4.0, 4.0 },
        { 0.0, 4.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(sqrt(32.0), result.distnace);
}

/**
 * @brief : Tests that multiple equivalent maximum spans return smoothly without algorithm deadlocks.
 */
void test_farthest_pair_multiple_equidistant_matches(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 5.0 },
        { 5.0, 0.0 },
        { 0.0, -5.0 },
        { -5.0, 0.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 4);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, result.distnace);
}

/**
 * @brief : Tests floating point precision stability across tightly grouped micro values.
 */
void test_farthest_pair_high_precision_micro_distance(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 1.000001, 1.0 },
        { 1.000004, 1.0 },
        { 1.000002, 1.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 3);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_FLOAT_WITHIN(1e-9, 0.000003, result.distnace);
}

/**
 * @brief : Tests that structural tracking parameters map coordinates accurately onto outputs.
 */
void test_farthest_pair_structural_field_assignment_accuracy(void)
{
    // Arrange.
    Point points[2] = 
    {
        { 10.0, 20.0 },
        { 40.0, 60.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 2);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(50.0, result.distnace);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, result.origin.x);
    TEST_ASSERT_EQUAL_DOUBLE(40.0, result.termination.x);
}

/**
 * @brief : Tests uneven odd allocations processing smoothly inside monotone chain boundaries.
 */
void test_farthest_pair_odd_population_asymmetric_hull(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 0.0 }, { 2.0, -1.0 }, { 4.0, 0.0 },
        { 3.0, 3.0 }, { 1.0, 2.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(sqrt(17.0), result.distnace);
}

/**
 * @brief : Tests calipers processing path when the farthest vertices are entirely on one extreme side.
 */
void test_farthest_pair_asymmetric_one_sided_dominance(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 0.0 },
        { 1.0, 0.5 },
        { 2.0, 0.8 },
        { 100.0, 0.0 },
        { 100.0, 1.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 5);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(sqrt(10001.0), result.distnace);
}

/**
 * @brief : Tests that overlapping points on a single shared position create an accurate zero envelope.
 */
void test_farthest_pair_all_points_at_origin_cluster(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 0.0, 0.0 },
        { 0.0, 0.0 },
        { 0.0, 0.0 }
    };

    // Act.
    Pair result = farthest_pair(points, 3);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, result.distnace);
}

/**
 * @brief : Stress tests runtime tracking capabilities under hundreds of distributed elements.
 */
void test_farthest_pair_high_volume_stress_saturation(void)
{
    // Arrange.
    size_t count = 200;
    Point* points = malloc(sizeof(Point) * count);
    
    TEST_ASSERT_NOT_NULL(points);

    for (size_t index = 0; index < count; index++)
    {
        points[index].x = (double)index * 2.0;
        points[index].y = (double)index * 3.0;
    }

    // Act.
    Pair result = farthest_pair(points, count);

    // Assert.
    TEST_ASSERT_TRUE(result.is_valid);
    
    double expected_delta_x = (double)(count - 1) * 2.0;
    double expected_delta_y = (double)(count - 1) * 3.0;

    double expected_distance = sqrt((expected_delta_x * expected_delta_x) + (expected_delta_y * expected_delta_y));
    
    TEST_ASSERT_EQUAL_DOUBLE(expected_distance, result.distnace);

    // Cleanup resources.
    free(points);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_farthest_pair_null_pointer_safety_check);
    RUN_TEST(test_farthest_pair_zero_count_safety_check);
    RUN_TEST(test_farthest_pair_single_point_boundary_check);
    RUN_TEST(test_farthest_pair_minimum_valid_population);
    RUN_TEST(test_farthest_pair_three_points_hull_handling);
    RUN_TEST(test_farthest_pair_array_presorting_integrity);
    RUN_TEST(test_farthest_pair_interior_point_exclusion);
    RUN_TEST(test_farthest_pair_identical_duplicate_points_resolution);
    RUN_TEST(test_farthest_pair_negative_cartesian_quadrant_processing);
    RUN_TEST(test_farthest_pair_horizontal_collinear_distribution);
    RUN_TEST(test_farthest_pair_vertical_collinear_distribution);
    RUN_TEST(test_farthest_pair_tie_breaking_by_y_coordinate);
    RUN_TEST(test_farthest_pair_perfect_square_perimeter);
    RUN_TEST(test_farthest_pair_multiple_equidistant_matches);
    RUN_TEST(test_farthest_pair_high_precision_micro_distance);
    RUN_TEST(test_farthest_pair_structural_field_assignment_accuracy);
    RUN_TEST(test_farthest_pair_odd_population_asymmetric_hull);
    RUN_TEST(test_farthest_pair_asymmetric_one_sided_dominance);
    RUN_TEST(test_farthest_pair_all_points_at_origin_cluster);
    RUN_TEST(test_farthest_pair_high_volume_stress_saturation);

    // Destroy the session.
    return UNITY_END();
}