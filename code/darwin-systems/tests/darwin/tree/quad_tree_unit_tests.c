/**
 * @file        :   quad_tree_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a Unity validation test suite for a Point Quadtree.
 * @details     :   This suite validates spatial boundary containment, quadrant subdivision 
 * mechanics, duplicate data rejections, out-of-bounds containment defenses, 
 * and highly granular circular radial range querying execution.
 * Compile:
 * clang -DUNITY_INCLUDE_DOUBLE ./tests/darwin/tree/quad_tree_unit_tests.c ./src/darwin/tree/quad-tree/quad_tree.c ./src/unity.c -Iinclude -o ./bin/quad_tree_unit_test
 * @version     :   1.0
 * @date        :   2026-06-05
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/quad_tree.h"

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
 * @brief Verifies a fresh quadtree node initializes boundaries and null child properties perfectly.
 */
void test_initialize_should_set_correct_boundaries_and_null_properties(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 50.0, .half_height = 50.0 };

    // Act.
    Node* root = initialize(bounds);

    // Assert.
    TEST_ASSERT_NOT_NULL(root);

    TEST_ASSERT_EQUAL_DOUBLE(0.0, root->boundary.x);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, root->boundary.y);
    TEST_ASSERT_EQUAL_DOUBLE(50.0, root->boundary.half_width);
    TEST_ASSERT_EQUAL_DOUBLE(50.0, root->boundary.half_height);

    TEST_ASSERT_NULL(root->point);
    TEST_ASSERT_FALSE(root->divided);
    TEST_ASSERT_NULL(root->north_west);
    TEST_ASSERT_NULL(root->north_east);
    TEST_ASSERT_NULL(root->south_west);
    TEST_ASSERT_NULL(root->south_east);

    clean(root);
}

/**
 * @brief Verifies that inserting a point within boundaries onto an empty node maps perfectly.
 */
void test_insert_into_empty_node_should_succeed(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point point = { .x = 10.0, .y = 20.0 };

    // Act.
    bool success = insert(root, point);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_NOT_NULL(root->point);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, root->point->x);
    TEST_ASSERT_EQUAL_DOUBLE(20.0, root->point->y);
    TEST_ASSERT_FALSE(root->divided);

    clean(root);
}

/**
 * @brief Verifies inserting a point exactly on the outer boundary borders is allowed.
 */
void test_insert_exactly_on_boundary_edge_should_succeed(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point edge_point = { .x = 100.0, .y = 100.0 };

    // Act.
    bool success = insert(root, edge_point);

    // Assert.
    TEST_ASSERT_TRUE(success);

    clean(root);
}

/**
 * @brief Verifies inserting a point completely outside the geometric region is rejected.
 */
void test_insert_out_of_bounds_point_should_fail(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 50.0, .half_height = 50.0 };

    Node* root = initialize(bounds);

    Point out_point = { .x = 55.0, .y = 10.0 };

    // Act.
    bool success = insert(root, out_point);

    // Assert.
    TEST_ASSERT_FALSE(success);
    TEST_ASSERT_NULL(root->point);

    clean(root);
}

/**
 * @brief Verifies that inserting an identical duplicate point coordinates is caught and rejected.
 */
void test_insert_exact_duplicate_point_should_fail(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 50.0, .half_height = 50.0 };

    Node* root = initialize(bounds);

    Point alpha = { .x = 15.0, .y = 15.0 };
    Point beta = { .x = 15.0, .y = 15.0 };

    insert(root, alpha);

    // Act.
    bool success = insert(root, beta);

    // Assert.
    TEST_ASSERT_FALSE(success);

    clean(root);
}

/**
 * @brief Verifies that adding a second unique point triggers a quadrant subdivision split.
 */
void test_insert_second_unique_point_should_trigger_subdivision(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point alpha = { .x = 30.0, .y = 30.0 };
    Point beta = { .x = -30.0, .y = 30.0 };

    insert(root, alpha);

    // Act.
    bool success = insert(root, beta);

    // Assert.
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_TRUE(root->divided);
    TEST_ASSERT_NULL(root->point);
    TEST_ASSERT_NOT_NULL(root->north_east);
    TEST_ASSERT_NOT_NULL(root->north_west);

    clean(root);
}

/**
 * @brief Verifies that points are sorted cleanly into their specific sub-quadrant nodes.
 */
void test_subdivision_should_route_points_to_correct_quadrants(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point north_east = { .x = 50.0,  .y = 50.0 };
    Point north_west = { .x = -50.0, .y = 50.0 };
    Point south_west = { .x = -50.0, .y = -50.0 };
    Point south_east = { .x = 50.0,  .y = -50.0 };

    // Act.
    insert(root, north_east);
    insert(root, north_west);
    insert(root, south_west);
    insert(root, south_east);

    // Assert.
    TEST_ASSERT_NOT_NULL(root->north_east->point);
    TEST_ASSERT_NOT_NULL(root->north_west->point);
    TEST_ASSERT_NOT_NULL(root->south_west->point);
    TEST_ASSERT_NOT_NULL(root->south_east->point);

    TEST_ASSERT_EQUAL_DOUBLE(50.0, root->north_east->point->x);
    TEST_ASSERT_EQUAL_DOUBLE(-50.0, root->south_west->point->x);

    clean(root);
}

/**
 * @brief Verifies radial range querying returns zero matches when tree is completely empty.
 */
void test_query_range_circle_on_empty_tree_should_return_zero(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point center = { .x = 0.0, .y = 0.0 };
    Point results[5];

    // Act.
    size_t count = query_range_circle(root, center, 50.0, results, 5, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, count);

    clean(root);
}

/**
 * @brief Verifies a point located exactly within the circular bounds is successfully collected.
 */
void test_query_range_circle_should_find_point_inside(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point point = { .x = 10.0, .y = 10.0 };

    insert(root, point);

    Point center = { .x = 0.0, .y = 0.0 };
    Point results[2];

    // Act.
    size_t count = query_range_circle(root, center, 20.0, results, 2, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, count);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, results[0].x);
    TEST_ASSERT_EQUAL_DOUBLE(10.0, results[0].y);

    clean(root);
}

/**
 * @brief Verifies points outside the localized circular query target are filtered away.
 */
void test_query_range_circle_should_ignore_point_outside_radius(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point point = { .x = 40.0, .y = 40.0 };

    insert(root, point);

    Point center = { .x = 0.0, .y = 0.0 };
    Point results[2];

    // Act.
    size_t count = query_range_circle(root, center, 15.0, results, 2, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, count);

    clean(root);
}

/**
 * @brief Verifies query collections handle multiple deep points correctly.
 */
void test_query_range_circle_should_collect_multiple_matching_points(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point alpha = { .x = 5.0,  .y = 5.0  };
    Point beta = { .x = -5.0, .y = -5.0 };
    Point gamma = { .x = 80.0, .y = 80.0 };

    insert(root, alpha);
    insert(root, beta);
    insert(root, gamma);

    Point center = { .x = 0.0, .y = 0.0 };
    Point results[5];

    // Act.
    size_t count = query_range_circle(root, center, 15.0, results, 5, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, count);

    clean(root);
}

/**
 * @brief Verifies the destination buffer max capacity protection blocks overflow writes.
 */
void test_query_range_circle_should_respect_maximum_capacity_limit(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point alpha = { .x = 2.0, .y = 2.0 };
    Point beta = { .x = 3.0, .y = 3.0 };
    Point gamma = { .x = 4.0, .y = 4.0 };

    insert(root, alpha);
    insert(root, beta);
    insert(root, gamma);

    Point center = { .x = 0.0, .y = 0.0 };
    Point results[2];

    // Act.
    size_t count = query_range_circle(root, center, 50.0, results, 2, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, count);

    clean(root);
}

/**
 * @brief Verifies points sitting precisely on the query perimeter line are matched.
 */
void test_query_range_circle_point_exactly_on_perimeter_should_succeed(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point point = { .x = 0.0, .y = 10.0 };

    insert(root, point);

    Point center = { .x = 0.0, .y = 0.0 };
    Point results[2];

    // Act.
    size_t count = query_range_circle(root, center, 10.0, results, 2, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, count);

    clean(root);
}

/**
 * @brief Verifies intercepting and cleanly discarding NaN values before insertion processing.
 */
void test_insert_nan_coordinates_should_fail_gracefully(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point nan_point = { .x = NAN, .y = 5.0 };

    // Act.
    bool success = insert(root, nan_point);

    // Assert.
    TEST_ASSERT_FALSE(success);

    clean(root);
}

/**
 * @brief Verifies API function handling structures safeguard robustly against null pointer contexts.
 */
void test_api_handles_null_nodes_gracefully(void)
{
    // Arrange.
    Point point = { .x = 0.0, .y = 0.0 };
    Point results[2];

    // Act & Assert.
    TEST_ASSERT_FALSE(insert(NULL, point));
    TEST_ASSERT_EQUAL_INT(0, query_range_circle(NULL, point, 10.0, results, 2, 0));
    
    clean(NULL);
}

/**
 * @brief Verifies multi-tier layered subdivision branches continue tracking correctly under deep splits.
 */
void test_deep_hierarchical_subdivisions(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };
    
    Node* root = initialize(bounds);
    
    Point alpha = { .x = 1.0, .y = 1.0 };
    Point beta = { .x = 1.1, .y = 1.1 };
    Point gamma = { .x = 1.2, .y = 1.2 };

    // Act.
    bool result = true;

    result &= insert(root, alpha);
    result &= insert(root, beta);
    result &= insert(root, gamma);

    // Assert.
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(root->divided);

    clean(root);
}

/**
 * @brief Verifies that equal points with tiny coordinate changes within EPSILON bounds are blocked.
 */
void test_insert_point_within_epsilon_tolerance_should_fail(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);

    Point point = { .x = 10.0, .y = 10.0 };
    Point tolerance_point = { .x = 10.0 + (EPSILON / 2.0), .y = 10.0 - (EPSILON / 2.0) };

    insert(root, point);

    // Act.
    bool success = insert(root, tolerance_point);

    // Assert.
    TEST_ASSERT_FALSE(success);

    clean(root);
}

/**
 * @brief Verifies query routing excludes quadrants that do not touch the circle footprint.
 */
void test_query_skips_non_intersecting_sub_quadrants(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 100.0, .half_height = 100.0 };

    Node* root = initialize(bounds);
    
    Point point_north_east = { .x = 80.0, .y = 80.0 };
    Point point_south_west = { .x = -80.0, .y = -80.0 };
    
    insert(root, point_north_east);
    insert(root, point_south_west);

    Point center = { .x = 85.0, .y = 85.0 };
    Point results[5];

    // Act.
    size_t count = query_range_circle(root, center, 10.0, results, 5, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, count);
    TEST_ASSERT_EQUAL_DOUBLE(80.0, results[0].x);

    clean(root);
}

/**
 * @brief Verifies point insertion and query stability across negative spatial grids.
 */
void test_negative_quadrant_handling_and_query(void)
{
    // Arrange.
    Boundary bounds = { .x = -50.0, .y = -50.0, .half_width = 50.0, .half_height = 50.0 };

    Node* root = initialize(bounds);
    
    Point point = { .x = -60.0, .y = -60.0 };

    insert(root, point);

    Point center = { .x = -65.0, .y = -65.0 };
    Point results[2];

    // Act.
    size_t count = query_range_circle(root, center, 10.0, results, 2, 0);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, count);
    TEST_ASSERT_EQUAL_DOUBLE(-60.0, results[0].x);

    clean(root);
}

/**
 * @brief Verifies running clear loops releases everything and wipes pointers safely.
 */
void test_clean_invalidates_node_trees_safely(void)
{
    // Arrange.
    Boundary bounds = { .x = 0.0, .y = 0.0, .half_width = 10.0, .half_height = 10.0 };

    Node* root = initialize(bounds);

    insert(root, (Point){5.0, 5.0});
    insert(root, (Point){-5.0, -5.0});

    // Act & Assert.
    TEST_ASSERT_TRUE(root->divided);
    
    clean(root);    
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_initialize_should_set_correct_boundaries_and_null_properties);
    RUN_TEST(test_insert_into_empty_node_should_succeed);
    RUN_TEST(test_insert_exactly_on_boundary_edge_should_succeed);
    RUN_TEST(test_insert_out_of_bounds_point_should_fail);
    RUN_TEST(test_insert_exact_duplicate_point_should_fail);
    RUN_TEST(test_insert_second_unique_point_should_trigger_subdivision);
    RUN_TEST(test_subdivision_should_route_points_to_correct_quadrants);
    RUN_TEST(test_query_range_circle_on_empty_tree_should_return_zero);
    RUN_TEST(test_query_range_circle_should_find_point_inside);
    RUN_TEST(test_query_range_circle_should_ignore_point_outside_radius);
    RUN_TEST(test_query_range_circle_should_collect_multiple_matching_points);
    RUN_TEST(test_query_range_circle_should_respect_maximum_capacity_limit);
    RUN_TEST(test_query_range_circle_point_exactly_on_perimeter_should_succeed);
    RUN_TEST(test_insert_nan_coordinates_should_fail_gracefully);
    RUN_TEST(test_api_handles_null_nodes_gracefully);
    RUN_TEST(test_deep_hierarchical_subdivisions);
    RUN_TEST(test_insert_point_within_epsilon_tolerance_should_fail);
    RUN_TEST(test_query_skips_non_intersecting_sub_quadrants);
    RUN_TEST(test_negative_quadrant_handling_and_query);
    RUN_TEST(test_clean_invalidates_node_trees_safely);

    // Destroy the session.
    return UNITY_END();
}