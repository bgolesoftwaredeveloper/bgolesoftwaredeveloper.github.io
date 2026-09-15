/**
 * @file        :   graham_scan_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Graham Scan Convex Hull geometric algorithm.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the graham_scan routine. Key areas of verification include:
 * - Proper handling of invalid arguments and null boundary validation.
 * - Minimum required input point populations and geometry constraints.
 * - Removal of interior coordinates vs. correct retention of boundary extrema.
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/darwin/geometry/graham_scan_unit_tests.c ./src/darwin/geometry/graham-scan/graham_scan.c ./src/unity.c -Iinclude -o ./bin/graham_scan_unit_test
 * @version     :   1.0
 * @date        :   2026-06-01
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/geometry/graham_scan.h"

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
 * @brief : Tests that passing a NULL points array pointer safely returns false.
 */
void test_graham_scan_null_points_safety_check(void)
{
    // Arrange.
    Point stack[5];

    size_t count = 0;

    // Act.
    bool success = graham_scan(NULL, 5, stack, &count);

    // Assert.
    TEST_ASSERT_FALSE(success);
    TEST_ASSERT_EQUAL_INT(0, count);
}

/**
 * @brief : Tests that passing a NULL destination stack pointer safely returns false.
 */
void test_graham_scan_null_stack_safety_check(void)
{
    // Arrange.
    Point points[3] = { {0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0} };

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 3, NULL, &count);

    // Assert.
    TEST_ASSERT_FALSE(success);
}

/**
 * @brief : Tests that passing a NULL output counter reference safely returns false.
 */
void test_graham_scan_null_count_safety_check(void)
{
    // Arrange.
    Point points[3] = { {0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0} };
    Point stack[3];

    // Act.
    bool success = graham_scan(points, 3, stack, NULL);

    // Assert.
    TEST_ASSERT_FALSE(success);
}

/**
 * @brief : Tests that a point dataset with fewer than 3 coordinates safely returns false.
 */
void test_graham_scan_insufficient_population_boundary_check(void)
{
    // Arrange.
    Point points[2] = { {0.0, 0.0}, {5.0, 5.0} };

    Point stack[2];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 2, stack, &count);

    // Assert.
    TEST_ASSERT_FALSE(success);
    TEST_ASSERT_EQUAL_INT(0, count);
}

/**
 * @brief : Tests that exactly three non-collinear points successfully yield a 3-point hull.
 */
void test_graham_scan_minimum_valid_population(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 0.0, 0.0 },
        { 4.0, 0.0 },
        { 0.0, 3.0 }
    };

    Point stack[3];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 3, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(3, count);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, stack[0].x);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, stack[0].y);
}

/**
 * @brief : Tests that perfectly horizontal collinear elements fail to create a valid hull polygon.
 */
void test_graham_scan_completely_collinear_horizontal_fail(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 2.0 },
        { 5.0, 2.0 },
        { 2.0, 2.0 },
        { 9.0, 2.0 }
    };

    Point stack[4];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_FALSE(success);
}

/**
 * @brief : Tests that perfectly vertical collinear elements fail to create a valid hull polygon.
 */
void test_graham_scan_completely_collinear_vertical_fail(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 3.0, -1.0 },
        { 3.0, 10.0 },
        { 3.0, 4.0 },
        { 3.0, 0.0 }
    };

    Point stack[4];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_FALSE(success);
}

/**
 * @brief : Tests that internal coordinates nested inside a square perimeter are successfully excluded.
 */
void test_graham_scan_interior_point_exclusion(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 0.0 },
        { 4.0, 0.0 },
        { 4.0, 4.0 },
        { 0.0, 4.0 },
        { 2.0, 2.0 }
    };

    Point stack[5];
    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 5, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(4, count);
    
    for (size_t index = 0; index < count; index++)
    {
        bool is_interior = (stack[index].x == 2.0 && stack[index].y == 2.0);

        TEST_ASSERT_FALSE(is_interior);
    }
}

/**
 * @brief : Tests sorting execution to ensure the baseline pivot point lands cleanly at index zero.
 */
void test_graham_scan_pivot_selection_integrity(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 5.0, 5.0 },
        { 1.0, 10.0 },
        { -2.0, -3.0 },
        { 4.0, -3.0 }
    };

    Point stack[4];
    size_t count = 0;

    // Act.
    graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(-2.0, points[0].x);
    TEST_ASSERT_EQUAL_DOUBLE(-3.0, points[0].y);
}

/**
 * @brief : Tests sorting resolution when multiple coordinates share identical lowest Y attributes.
 */
void test_graham_scan_lowest_y_tie_broken_by_leftmost_x(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 3.0, -5.0 },
        { -1.0, -5.0 },
        { 8.0, -5.0 },
        { 2.0, 10.0 }
    };

    Point stack[4];
    size_t count = 0;

    // Act.
    graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, points[0].x);
    TEST_ASSERT_EQUAL_DOUBLE(-5.0, points[0].y);
}

/**
 * @brief : Tests that completely overlapping identical coordinates compress safely during filtering.
 */
void test_graham_scan_identical_duplicate_points_resolution(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 0.0 },
        { 0.0, 0.0 },
        { 4.0, 0.0 },
        { 0.0, 4.0 },
        { 0.0, 4.0 }
    };

    Point stack[5];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 5, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(3, count);
}

/**
 * @brief : Tests geometry calculations distributed symmetrically across negative Cartesian quadrants.
 */
void test_graham_scan_negative_cartesian_quadrant_processing(void)
{
    // Arrange.
    Point points[4] = 
    {
        { -1.0, -1.0 },
        { -5.0, -1.0 },
        { -5.0, -5.0 },
        { -1.0, -5.0 }
    };

    Point stack[4];
    
    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(4, count);
}

/**
 * @brief : Tests intermediate filtering when a collinear line fragment lies directly on an outer boundary edge.
 */
void test_graham_scan_boundary_collinear_points_reduction(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 0.0 },
        { 2.0, 0.0 },
        { 4.0, 0.0 },
        { 4.0, 4.0 },
        { 0.0, 4.0 }
    };

    Point stack[5];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 5, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(4, count);

    for (size_t index = 0; index < count; index++)
    {
        bool is_midpoint = (stack[index].x == 2.0 && stack[index].y == 0.0);

        TEST_ASSERT_FALSE(is_midpoint);
    }
}

/**
 * @brief : Tests polar angle ordering sorting constraints when multiple targets share a matching vector angle.
 */
void test_graham_scan_radial_sorting_collinear_distance_ordering(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 0.0 },
        { 2.0, 2.0 },
        { 4.0, 4.0 },
        { 0.0, 5.0 }
    };

    Point stack[4];

    size_t count = 0;

    // Act.
    graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_EQUAL_INT(3, count);
    TEST_ASSERT_EQUAL_DOUBLE(4.0, stack[1].x);
    TEST_ASSERT_EQUAL_DOUBLE(4.0, stack[1].y);
}

/**
 * @brief : Tests backtracking capability when a highly concave inner notch forces stack unrolling operations.
 */
void test_graham_scan_concave_notch_backtracking_unroll(void)
{
    // Arrange.
    Point points[5] = 
    {
        { 0.0, 0.0 },
        { 4.0, 0.0 },
        { 2.0, 1.0 },
        { 4.0, 4.0 },
        { 0.0, 4.0 }
    };

    Point stack[5];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 5, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(4, count);
}

/**
 * @brief : Tests processing execution stability across micro distances with high-precision floating points.
 */
void test_graham_scan_high_precision_micro_distance(void)
{
    // Arrange.
    Point points[3] = 
    {
        { 0.000000001, 0.000000001 },
        { 0.000000005, 0.000000002 },
        { 0.000000002, 0.000000006 }
    };

    Point stack[3];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 3, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(3, count);
}

/**
 * @brief : Tests structural perimeter calculation when inputs form an asymmetric triangle outline.
 */
void test_graham_scan_asymmetric_triangle_perimeter(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 0.0 },
        { 10.0, 2.0 },
        { 3.0, 8.0 },
        { 4.0, 3.0 }
    };

    Point stack[4];
    
    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(3, count);
}

/**
 * @brief : Tests validation logic paths when every coordinate matches exactly at the zero-origin vector.
 */
void test_graham_scan_all_points_at_origin_cluster_fail(void)
{
    // Arrange.
    Point points[4] = 
    {
        { 0.0, 0.0 },
        { 0.0, 0.0 },
        { 0.0, 0.0 },
        { 0.0, 0.0 }
    };

    Point stack[4];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 4, stack, &count);

    // Assert.
    TEST_ASSERT_FALSE(success);
}

/**
 * @brief : Tests that a complex, multi-point polygon envelope generates the correct counter-clockwise vertex count.
 */
void test_graham_scan_complex_irregular_polygon_envelope(void)
{
    // Arrange.
    Point points[8] = 
    {
        { 2.0, 1.0 }, { 4.0, 1.0 }, { 5.0, 3.0 }, { 4.0, 5.0 },
        { 2.0, 5.0 }, { 1.0, 3.0 }, { 3.0, 3.0 }, { 2.5, 4.0 }
    };

    Point stack[8];

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, 8, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_INT(6, count);
}

/**
 * @brief : Stress tests system behavior and structural integrity under hundreds of elements.
 */
void test_graham_scan_high_volume_stress_saturation(void)
{
    // Arrange.
    size_t total_elements = 150;

    Point* points = malloc(sizeof(Point) * total_elements);
    Point* stack = malloc(sizeof(Point) * total_elements);
    
    TEST_ASSERT_NOT_NULL(points);
    TEST_ASSERT_NOT_NULL(stack);

    for (size_t index = 0; index < total_elements; index++)
    {
        points[index].x = (double)(index % 10) * 10.0;
        points[index].y = (double)(index / 10) * 5.0;
    }

    size_t count = 0;

    // Act.
    bool success = graham_scan(points, total_elements, stack, &count);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_TRUE(count >= 4);

    // Free resources.
    free(points);
    free(stack);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_graham_scan_null_points_safety_check);
    RUN_TEST(test_graham_scan_null_stack_safety_check);
    RUN_TEST(test_graham_scan_null_count_safety_check);
    RUN_TEST(test_graham_scan_insufficient_population_boundary_check);
    RUN_TEST(test_graham_scan_minimum_valid_population);
    RUN_TEST(test_graham_scan_completely_collinear_horizontal_fail);
    RUN_TEST(test_graham_scan_completely_collinear_vertical_fail);
    RUN_TEST(test_graham_scan_interior_point_exclusion);
    RUN_TEST(test_graham_scan_pivot_selection_integrity);
    RUN_TEST(test_graham_scan_lowest_y_tie_broken_by_leftmost_x);
    RUN_TEST(test_graham_scan_identical_duplicate_points_resolution);
    RUN_TEST(test_graham_scan_negative_cartesian_quadrant_processing);
    RUN_TEST(test_graham_scan_boundary_collinear_points_reduction);
    RUN_TEST(test_graham_scan_radial_sorting_collinear_distance_ordering);
    RUN_TEST(test_graham_scan_concave_notch_backtracking_unroll);
    RUN_TEST(test_graham_scan_high_precision_micro_distance);
    RUN_TEST(test_graham_scan_asymmetric_triangle_perimeter);
    RUN_TEST(test_graham_scan_all_points_at_origin_cluster_fail);
    RUN_TEST(test_graham_scan_complex_irregular_polygon_envelope);
    RUN_TEST(test_graham_scan_high_volume_stress_saturation);

    // Destroy the session.
    return UNITY_END();
}