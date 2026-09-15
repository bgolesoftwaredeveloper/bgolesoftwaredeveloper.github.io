/**
 * @file        :   ear_clipping_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Ear Clipping polygon triangulation logic.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the ear-clipping algorithm implementation. Key areas of verification include:
 * - Proper handling of invalid array pointers and minimum buffer sizes.
 * - Triangulation of simple convex and highly concave geometries.
 * - Spatial tracking across negative and floating point coordinates.
 * - Enforcement of safety limits on self-intersecting or invalid winding patterns.
 * - Graceful rejection of degenerate paths such as lines, duplicates, and empty rings.
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/darwin/geometry/ear_clipping_unit_tests.c ./src/darwin/geometry/ear-clipping/ear_clipping.c ./src/unity.c -Iinclude -o ./bin/ear_clipping_unit_test
 * @version     :   1.0
 * @date        :   2026-06-01
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/geometry/ear_clipping.h"

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
 * @brief : Tests that an empty polygon size parameter safely triggers a validation failure.
 */
void test_ear_clipping_zero_vertex_count_safety_check(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}, {5.0, 5.0}};

    Triangle output[1];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 0, output, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_INT(0, count);
}

/**
 * @brief : Tests that passing a NULL pointer for the polygon array returns false.
 */
void test_ear_clipping_null_polygon_pointer_safety_check(void)
{
    // Arrange.
    Triangle output[1];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(NULL, 3, output, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests that passing a NULL pointer for the output triangle array returns false.
 */
void test_ear_clipping_null_output_buffer_safety_check(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}, {5.0, 5.0}};

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 3, NULL, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests that an invalid size count below the structural threshold of 3 returns false.
 */
void test_ear_clipping_insufficient_vertices_safety_check(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}};

    Triangle output[1];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 2, output, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests triangulation of a basic convex triangle (base case).
 */
void test_ear_clipping_simple_convex_triangle_returns_one_triangle(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {4.0, 0.0}, {0.0, 4.0}};

    Triangle output[1];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 3, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, count);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, output[0].origin.x);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, output[0].origin.y);
}

/**
 * @brief : Tests a perfect 10x10 square to verify standard convex decomposition.
 */
void test_ear_clipping_convex_square_decomposes_into_two_triangles(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {0.0, 10.0}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, count);
}

/**
 * @brief : Tests the baseline concave L-shaped layout to verify internal visibility validation.
 */
void test_ear_clipping_concave_l_shape_triangulates_perfectly(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}, {5.0, 2.0}, {2.0, 2.0}, {2.0, 5.0}, {0.0, 5.0}};

    Triangle output[4];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 6, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(4, count);
}

/**
 * @brief : Tests a concave chevron (V-shape) to ensure the engine avoids cutting external space.
 */
void test_ear_clipping_concave_chevron_prevents_exterior_slices(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {4.0, 2.0}, {8.0, 0.0}, {4.0, 6.0}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, count);
}

/**
 * @brief : Tests validation logic against bad clockwise (CW) winding directions.
 */
void test_ear_clipping_clockwise_winding_fails_convexity_checks(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {0.0, 5.0}, {5.0, 5.0}, {5.0, 0.0}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests a degenerate perfectly flat horizontal line segment path.
 */
void test_ear_clipping_degenerate_flat_line_returns_false(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}, {10.0, 0.0}, {0.0, 0.0}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests a shape containing consecutive duplicate vertices.
 */
void test_ear_clipping_collinear_duplicate_vertices_rejection(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}, {5.0, 0.0}, {0.0, 5.0}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests containment verification with negative coordinate quadrant domains.
 */
void test_ear_clipping_negative_coordinate_quadrant_processing(void)
{
    // Arrange.
    Vertex polygon[] = {{-10.0, -10.0}, {-2.0, -10.0}, {-2.0, -2.0}, {-10.0, -2.0}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, count);
}

/**
 * @brief : Tests calculations on micro-scaled highly precise floating point coordinates.
 */
void test_ear_clipping_highly_precise_floating_point_precision(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {0.00002, 0.0}, {0.00002, 0.00002}, {0.0, 0.00002}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, count);
}

/**
 * @brief : Tests that massive structural scales do not cause precision overruns.
 */
void test_ear_clipping_extreme_coordinate_offset_scale_validation(void)
{
    // Arrange.
    Vertex polygon[] = 
    {
        {1000000.0, 1000000.0},
        {1000010.0, 1000000.0},
        {1000010.0, 1000010.0},
        {1000000.0, 1000010.0}
    };

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(2, count);
}

/**
 * @brief : Tests an invalid self-intersecting hourglass profile to verify execution limits.
 */
void test_ear_clipping_self_intersecting_hourglass_fails_gracefully(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {10.0, 10.0}, {10.0, 0.0}, {0.0, 10.0}};

    Triangle output[2];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 4, output, &count);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief : Tests an octagon to confirm smooth slicing execution on larger convex paths.
 */
void test_ear_clipping_convex_octagon_generates_six_triangles(void)
{
    // Arrange.
    Vertex polygon[] = 
    {
        {1.0, 0.0},
        {3.0, 0.0},
        {4.0, 1.0},
        {4.0, 3.0},
        {3.0, 4.0},
        {1.0, 4.0},
        {0.0, 3.0},
        {0.0, 1.0}
    };

    Triangle output[6];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 8, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(6, count);
}

/**
 * @brief : Tests a comb structure containing a reflex vertex sitting on a shared horizontal baseline.
 */
void test_ear_clipping_collinear_reflex_edge_handling(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}, {5.0, 4.0}, {3.0, 4.0}, {3.0, 2.0}, {1.0, 2.0}, {1.0, 4.0}, {0.0, 4.0}};

    Triangle output[6];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 8, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(6, count);
}

/**
 * @brief : Tests a large open U-shaped room corridor structure.
 */
void test_ear_clipping_deep_concave_u_channel_perimeter(void)
{
    // Arrange.
    Vertex polygon[] = 
    {
        {0.0, 0.0},
        {6.0, 0.0},
        {6.0, 6.0},
        {4.0, 6.0},
        {4.0, 2.0},
        {2.0, 2.0},
        {2.0, 6.0},
        {0.0, 6.0}
    };

    Triangle output[6];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 8, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(6, count);
}

/**
 * @brief : Tests a simple convex polygon layout containing three perfectly collinear nodes on an edge.
 */
void test_ear_clipping_three_point_collinear_edge_ignoring(void)
{
    // Arrange.
    Vertex polygon[] = {{0.0, 0.0}, {5.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {0.0, 10.0}};

    Triangle output[3];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 5, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(3, count);
}

/**
 * @brief : Tests a highly complex symmetrical spike shape (3D engine element).
 */
void test_ear_clipping_complex_quad_star_polygon(void)
{
    // Arrange.
    Vertex polygon[] = 
    {
        {0.0, 5.0},
        {-1.0, 1.0},
        {-5.0, 0.0},
        {-1.0, -1.0},
        {0.0, -5.0},
        {1.0, -1.0}, 
        {5.0, 0.0},
        {1.0, 1.0}
    };

    Triangle output[6];

    size_t count = 0;

    // Act.
    bool result = ear_clipping(polygon, 8, output, &count);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(6, count);
}

/**
 * @brief : Main runtime setup driver executing registration mappings across all tests.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_ear_clipping_zero_vertex_count_safety_check);
    RUN_TEST(test_ear_clipping_null_polygon_pointer_safety_check);
    RUN_TEST(test_ear_clipping_null_output_buffer_safety_check);
    RUN_TEST(test_ear_clipping_insufficient_vertices_safety_check);
    RUN_TEST(test_ear_clipping_simple_convex_triangle_returns_one_triangle);
    RUN_TEST(test_ear_clipping_convex_square_decomposes_into_two_triangles);
    RUN_TEST(test_ear_clipping_concave_l_shape_triangulates_perfectly);
    RUN_TEST(test_ear_clipping_concave_chevron_prevents_exterior_slices);
    RUN_TEST(test_ear_clipping_clockwise_winding_fails_convexity_checks);
    RUN_TEST(test_ear_clipping_degenerate_flat_line_returns_false);
    RUN_TEST(test_ear_clipping_collinear_duplicate_vertices_rejection);
    RUN_TEST(test_ear_clipping_negative_coordinate_quadrant_processing);
    RUN_TEST(test_ear_clipping_highly_precise_floating_point_precision);
    RUN_TEST(test_ear_clipping_extreme_coordinate_offset_scale_validation);
    RUN_TEST(test_ear_clipping_self_intersecting_hourglass_fails_gracefully);
    RUN_TEST(test_ear_clipping_convex_octagon_generates_six_triangles);
    RUN_TEST(test_ear_clipping_collinear_reflex_edge_handling);
    RUN_TEST(test_ear_clipping_deep_concave_u_channel_perimeter);
    RUN_TEST(test_ear_clipping_three_point_collinear_edge_ignoring);
    RUN_TEST(test_ear_clipping_complex_quad_star_polygon);

    // Destroy the session.
    return UNITY_END();
}