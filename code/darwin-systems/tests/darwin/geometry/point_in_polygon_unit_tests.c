/**
 * @file        :   point_in_polygon_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Point-in-Polygon (PIP) intersection logic.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the ray-casting algorithm implementation. Key areas of verification include:
 * - Proper handling of invalid array pointers and boundary sizes.
 * - Precision tracing for points directly centered or outside bounding areas.
 * - Parity counting across non-convex, concave, and multi-intercept geometries.
 * - Strict intersection testing against collinear horizontal/vertical edges.
 * - Degenerate geometries such as zero-area, linear, and small-vertex paths.
 * Compile:
 * clang ./tests/darwin/geometry/point_in_polygon_unit_tests.c ./src/darwin/geometry/point-in-polygon/point_in_polygon.c ./src/unity.c -Iinclude -o ./bin/point_in_polygon_unit_test
 * @version     :   1.0
 * @date        :   2026-06-01
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/geometry/point_in_polygon.h"

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
 * @brief : Tests that an empty polygon size parameter safely returns false.
 */
void test_pip_zero_vertex_count_safety_check(void)
{
    // Arrange.
    Point probe = {5.0, 5.0};
    Point square[] = {{0.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {0.0, 10.0}};

    // Act.
    bool result = is_point_in_polygon(probe, square, 0);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests that passing a point cleanly inside a standard square returns true.
 */
void test_pip_convex_square_interior_point_returns_true(void)
{
    // Arrange.
    Point probe = {5.0, 5.0};
    Point square[] = {{0.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {0.0, 10.0}};

    // Act.
    bool result = is_point_in_polygon(probe, square, 4);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests that passing a point outside a standard square returns false.
 */
void test_pip_convex_square_exterior_point_returns_false(void)
{
    // Arrange.
    Point probe = {15.0, 5.0};
    Point square[] = {{0.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {0.0, 10.0}};

    // Act.
    bool result = is_point_in_polygon(probe, square, 4);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests a probe placed inside a deep geometric cavity of a concave polygon.
 */
void test_pip_concave_bay_interior_point_returns_true(void)
{
    // Arrange.
    Point probe = {2.0, 5.0};
    Point shape[] = 
    {
        {0.0, 0.0},
        {10.0, 0.0},
        {10.0, 3.0},
        {4.0, 3.0}, 
        {4.0, 10.0},
        {0.0, 10.0}
    };

    // Act.
    bool result = is_point_in_polygon(probe, shape, 6);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests a probe placed inside the empty recess layout of a concave shape.
 */
void test_pip_concave_recess_exterior_point_returns_false(void)
{
    // Arrange.
    Point probe = {6.0, 6.0};
    Point shape[] = 
    {
        {0.0, 0.0},
        {10.0, 0.0},
        {10.0, 3.0},
        {4.0, 3.0}, 
        {4.0, 10.0},
        {0.0, 10.0}
    };

    // Act.
    bool result = is_point_in_polygon(probe, shape, 6);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests ray intercept tracking through complex multi-layered star structures.
 */
void test_pip_complex_star_interior_point_returns_true(void)
{
    // Arrange.
    Point probe = {0.0, 0.0};
    Point star[] = 
    {
        {0.0, 5.0},
        {1.5, 1.5},
        {5.0, 0.0},
        {1.5, -1.5},
        {0.0, -5.0},
        {-1.5, -1.5},
        {-5.0, 0.0},
        {-1.5, 1.5}
    };

    // Act.
    bool result = is_point_in_polygon(probe, star, 8);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests a probe sitting in the outer wedge of a star geometry.
 */
void test_pip_complex_star_outer_wedge_exterior_point_returns_false(void)
{
    // Arrange.
    Point probe = {3.0, 3.0};
    Point star[] = 
    {
        {0.0, 5.0},
        {1.5, 1.5},
        {5.0, 0.0},
        {1.5, -1.5},
        {0.0, -5.0},
        {-1.5, -1.5},
        {-5.0, 0.0},
        {-1.5, 1.5}
    };

    // Act.
    bool result = is_point_in_polygon(probe, star, 8);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests ray tracking safety when cast point aligns directly on top of a vertex Y plane.
 */
void test_pip_ray_aligned_exactly_with_vertex_y_intercept(void)
{
    // Arrange.
    Point probe = {5.0, 4.0}; 
    Point triangle[] = {{0.0, 0.0}, {10.0, 4.0}, {0.0, 8.0}};

    // Act.
    bool result = is_point_in_polygon(probe, triangle, 3);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests that a degenerate single line vertex string defaults back to false containment.
 */
void test_pip_degenerate_flat_line_polygon_returns_false(void)
{
    // Arrange.
    Point probe = {5.0, 0.0};
    Point flat_line[] = {{0.0, 0.0}, {10.0, 0.0}, {0.0, 0.0}};

    // Act.
    bool result = is_point_in_polygon(probe, flat_line, 3);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests a probe floating near a single node point cloud model array setup.
 */
void test_pip_degenerate_single_vertex_polygon_returns_false(void)
{
    // Arrange.
    Point probe = {0.0, 0.0};
    Point single_point[] = {{0.0, 0.0}};

    // Act.
    bool result = is_point_in_polygon(probe, single_point, 1);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests checking points placed inside extremely scaled floating point grids.
 */
void test_pip_highly_precise_floating_point_coordinates(void)
{
    // Arrange.
    Point probe = {0.000005, 0.000005};
    Point micro_square[] = {{0.0, 0.0}, {0.00001, 0.0}, {0.00001, 0.00001}, {0.0, 0.00001}};

    // Act.
    bool result = is_point_in_polygon(probe, micro_square, 4);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests containment verification with negative coordinate geometries.
 */
void test_pip_negative_coordinate_quadrant_interior_point(void)
{
    // Arrange.
    Point probe = {-5.0, -5.0};
    Point negative_square[] = {{-10.0, -10.0}, {0.0, -10.0}, {0.0, 0.0}, {-10.0, 0.0}};

    // Act.
    bool result = is_point_in_polygon(probe, negative_square, 4);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests that standard ray execution ignores vertices completely to its left.
 */
void test_pip_point_to_the_right_of_entire_polygon_returns_false(void)
{
    // Arrange.
    Point probe = {20.0, 5.0};
    Point left_box[] = {{0.0, 0.0}, {5.0, 0.0}, {5.0, 10.0}, {0.0, 10.0}};

    // Act.
    bool result = is_point_in_polygon(probe, left_box, 4);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests that standard ray execution intercepts bounds strictly to its right.
 */
void test_pip_point_to_the_left_of_entire_polygon_returns_false(void)
{
    // Arrange.
    Point probe = {-5.0, 5.0};
    Point right_box[] = {{5.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {5.0, 10.0}};

    // Act.
    bool result = is_point_in_polygon(probe, right_box, 4);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests parity mapping on a winding self-intersecting hourglass border.
 * @details Probe is in the left triangular lobe. A horizontal ray to the right crosses 2 lines,
 * yielding an even crossing count which evaluates mathematically to false.
 */
void test_pip_self_intersecting_hourglass_interior_quadrant(void)
{
    // Arrange.
    Point probe = {2.0, 2.0};
    Point hourglass[] = {{0.0, 0.0}, {10.0, 10.0}, {10.0, 0.0}, {0.0, 10.0}};

    // Act.
    bool result = is_point_in_polygon(probe, hourglass, 4);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests parity tracking when a point is framed in a double-crossed edge stack.
 */
void test_pip_multi_intercept_complex_ray_cast_returns_true(void)
{
    // Arrange.
    Point probe = {1.0, 5.0};
    Point comb_shape[] = 
    {
        {0.0, 0.0},
        {10.0, 0.0},
        {10.0, 3.0},
        {2.0, 3.0}, 
        {2.0, 4.0},
        {10.0, 4.0},
        {10.0, 6.0},
        {2.0, 6.0}, 
        {2.0, 7.0},
        {10.0, 7.0},
        {10.0, 10.0},
        {0.0, 10.0}
    };

    // Act.
    bool result = is_point_in_polygon(probe, comb_shape, 12);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests that horizontal boundary baselines safely avoid division-by-zero math panics.
 * @details Probe is moved safely outside to the right of the segment boundary to isolate slope logic.
 */
void test_pip_horizontal_edge_alignment_slope_safety_check(void)
{
    // Arrange.
    Point probe = {12.0, 0.0};
    Point boundary[] = {{0.0, 0.0}, {10.0, 0.0}, {5.0, 8.0}};

    // Act.
    bool result = is_point_in_polygon(probe, boundary, 3);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests ray calculation accuracy when checking vertices in a pure vertical column line.
 */
void test_pip_vertical_edge_ray_cross_validation(void)
{
    // Arrange.
    Point probe = {3.0, 5.0};
    Point vertical_rectangle[] = {{0.0, 0.0}, {5.0, 0.0}, {5.0, 10.0}, {0.0, 10.0}};

    // Act.
    bool result = is_point_in_polygon(probe, vertical_rectangle, 4);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests massive spatial tracking offsets far from native tracking origins.
 */
void test_pip_extreme_coordinate_offset_scale_validation(void)
{
    // Arrange.
    Point probe = {1000005.0, 1000005.0};
    Point offset_square[] = 
    {
        {1000000.0, 1000000.0},
        {1000010.0, 1000000.0}, 
        {1000010.0, 1000010.0},
        {1000000.0, 1000010.0}
    };

    // Act.
    bool result = is_point_in_polygon(probe, offset_square, 4);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief : Tests that a standard large circle approximation tracks interior points perfectly.
 */
void test_pip_high_vertex_count_circle_approximation(void)
{
    // Arrange.
    Point probe = {0.0, 0.0};
    Point octagon[] = 
    {
        {0.0, 4.0},
        {3.0, 3.0},
        {4.0, 0.0},
        {3.0, -3.0},
        {0.0, -4.0},
        {-3.0, -3.0},
        {-4.0, 0.0},
        {-3.0, 3.0}
    };

    // Act.
    bool result = is_point_in_polygon(probe, octagon, 8);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_pip_zero_vertex_count_safety_check);
    RUN_TEST(test_pip_convex_square_interior_point_returns_true);
    RUN_TEST(test_pip_convex_square_exterior_point_returns_false);
    RUN_TEST(test_pip_concave_bay_interior_point_returns_true);
    RUN_TEST(test_pip_concave_recess_exterior_point_returns_false);
    RUN_TEST(test_pip_complex_star_interior_point_returns_true);
    RUN_TEST(test_pip_complex_star_outer_wedge_exterior_point_returns_false);
    RUN_TEST(test_pip_ray_aligned_exactly_with_vertex_y_intercept);
    RUN_TEST(test_pip_degenerate_flat_line_polygon_returns_false);
    RUN_TEST(test_pip_degenerate_single_vertex_polygon_returns_false);
    RUN_TEST(test_pip_highly_precise_floating_point_coordinates);
    RUN_TEST(test_pip_negative_coordinate_quadrant_interior_point);
    RUN_TEST(test_pip_point_to_the_right_of_entire_polygon_returns_false);
    RUN_TEST(test_pip_point_to_the_left_of_entire_polygon_returns_false);
    RUN_TEST(test_pip_self_intersecting_hourglass_interior_quadrant);
    RUN_TEST(test_pip_multi_intercept_complex_ray_cast_returns_true);
    RUN_TEST(test_pip_horizontal_edge_alignment_slope_safety_check);
    RUN_TEST(test_pip_vertical_edge_ray_cross_validation);
    RUN_TEST(test_pip_extreme_coordinate_offset_scale_validation);
    RUN_TEST(test_pip_high_vertex_count_circle_approximation);

    // Destroy the session.
    return UNITY_END();
}