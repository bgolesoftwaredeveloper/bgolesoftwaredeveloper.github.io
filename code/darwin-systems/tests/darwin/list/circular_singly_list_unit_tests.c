/**
 * @file        :   circular_singly_list_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Radial (Circular) Singly Linked List system.
 * @details     :   This test suite utilizes the Unity Test Framework to rigorously
 * validate the behavior and structural integrity of a circular
 * singly linked list (RadialSystem). 
 * * The suite focuses on five primary domains:
 * 1. Lifecycle: Ensures proper memory allocation during ignition 
 * and deep-cleaning/nullification during shutdown.
 * 2. Expansion: Validates O(1) insertion at the cursor and the 
 * maintenance of the circular link property.
 * 3. Rotational Logic: Confirms that cursor movement correctly 
 * cycles through nodes and handles large step counts via 
 * modulo arithmetic.
 * 4. Collapse: Verifies that node removal correctly bridges the 
 * remaining nodes and handles the "empty system" state.
 * 5. Defensive Programming: Confirms that functions fail 
 * gracefully when passed NULL pointers or empty structures.
 * Compile:
 * clang ./tests/darwin/list/circular_singly_list_unit_tests.c ./src/darwin/list/circular-singly-list/circular_singly_linked_list.c ./src/unity.c -Iinclude -o ./bin/circular_singly_list_unit_test
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/list/circular_singly_linked_list.h"

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
 * @brief Ensures ignite creates a non-null management structure.
 */
void test_ignite_returns_non_null_system(void)
{
    // Arrange.
    RadialSystem* genesis_vortex = NULL;

    // Act.
    genesis_vortex = ignite();

    // Assert.
    TEST_ASSERT_NOT_NULL(genesis_vortex);

    shutdown(&genesis_vortex);
}

/**
 * @brief Ensures ignition correctly initializes count to zero.
 */
void test_ignite_initializes_zero_count(void)
{
    // Arrange.
    RadialSystem* void_system = ignite();

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(0, void_system->count);

    shutdown(&void_system);
}

/**
 * @brief Ensures ignition sets the cursor to null.
 */
void test_ignite_initializes_null_cursor(void)
{
    // Arrange.
    RadialSystem* empty_orbit = ignite();

    // Act & Assert.
    TEST_ASSERT_NULL(empty_orbit->cursor);

    shutdown(&empty_orbit);
}

/**
 * @brief Ensures first expansion sets the cursor.
 */
void test_expand_orbit_sets_cursor_on_first_node(void)
{
    // Arrange.
    RadialSystem* solo_system = ignite();

    int energy_level = 50;

    // Act.
    expand_orbit(solo_system, energy_level);

    // Assert.
    TEST_ASSERT_NOT_NULL(solo_system->cursor);
    TEST_ASSERT_EQUAL_INT(energy_level, solo_system->cursor->energy);

    shutdown(&solo_system);
}

/**
 * @brief Ensures first node points to itself to maintain circularity.
 */
void test_expand_orbit_first_node_is_circular(void)
{
    // Arrange.
    RadialSystem* single_node_ring = ignite();

    // Act.
    expand_orbit(single_node_ring, 100);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(single_node_ring->cursor, single_node_ring->cursor->next_orbit);

    shutdown(&single_node_ring);
}

/**
 * @brief Ensures count increments correctly across multiple expansions.
 */
void test_expand_orbit_increments_count_correctly(void)
{
    // Arrange.
    RadialSystem* growing_system = ignite();

    // Act.
    expand_orbit(growing_system, 10);
    expand_orbit(growing_system, 20);
    expand_orbit(growing_system, 30);

    // Assert.
    TEST_ASSERT_EQUAL_INT(3, growing_system->count);

    shutdown(&growing_system);
}

/**
 * @brief Ensures rotation of one step updates cursor to the next node.
 */
void test_rotate_one_step_moves_cursor_forward(void)
{
    // Arrange.
    RadialSystem* binary_system = ignite();

    expand_orbit(binary_system, 1);
    expand_orbit(binary_system, 2);

    SolarNode* head_reference = binary_system->cursor->next_orbit;

    // Act.
    rotate(binary_system, 1);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(head_reference, binary_system->cursor);

    shutdown(&binary_system);
}

/**
 * @brief Ensures rotating by the total count returns cursor to the same spot.
 */
void test_rotate_full_cycle_returns_to_origin(void)
{
    // Arrange.
    RadialSystem* loop_system = ignite();

    expand_orbit(loop_system, 100);
    expand_orbit(loop_system, 200);

    SolarNode* origin_node = loop_system->cursor;

    // Act.
    rotate(loop_system, 2);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(origin_node, loop_system->cursor);

    shutdown(&loop_system);
}

/**
 * @brief Ensures modulo logic handles large step counts efficiently.
 */
void test_rotate_modulo_logic_handles_large_values(void)
{
    // Arrange.
    RadialSystem* modulo_vessel = ignite();

    expand_orbit(modulo_vessel, 10);
    expand_orbit(modulo_vessel, 20);

    SolarNode* head_node = modulo_vessel->cursor->next_orbit;

    // Act.
    rotate(modulo_vessel, 21);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(head_node, modulo_vessel->cursor);

    shutdown(&modulo_vessel);
}

/**
 * @brief Ensures collapsing the only node nullifies the system.
 */
void test_collapse_next_on_single_node_clears_cursor(void)
{
    // Arrange.
    RadialSystem* lonely_system = ignite();

    expand_orbit(lonely_system, 99);

    // Act.
    collapse_next(lonely_system);

    // Assert.
    TEST_ASSERT_NULL(lonely_system->cursor);
    TEST_ASSERT_EQUAL_INT(0, lonely_system->count);

    shutdown(&lonely_system);
}

/**
 * @brief Ensures collapsing middle node bridges the gap correctly.
 */
void test_collapse_next_removes_correct_node_and_relinks(void)
{
    // Arrange.
    RadialSystem* triple_ring = ignite();

    expand_orbit(triple_ring, 1);
    expand_orbit(triple_ring, 2);
    expand_orbit(triple_ring, 3);

    // Act.
    collapse_next(triple_ring);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, triple_ring->count);
    TEST_ASSERT_EQUAL_INT(2, triple_ring->cursor->next_orbit->energy);

    shutdown(&triple_ring);
}

/**
 * @brief Ensures rotate handles zero steps safely.
 */
void test_rotate_zero_steps_does_not_change_cursor(void)
{
    // Arrange.
    RadialSystem* static_system = ignite();

    expand_orbit(static_system, 5);
    expand_orbit(static_system, 10);

    SolarNode* initial_cursor = static_system->cursor;

    // Act.
    rotate(static_system, 0);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(initial_cursor, static_system->cursor);

    shutdown(&static_system);
}

/**
 * @brief Ensures shutdown nullifies the actual pointer.
 */
void test_shutdown_nullifies_pointer_successfully(void)
{
    // Arrange.
    RadialSystem* termination_target = ignite();

    // Act.
    shutdown(&termination_target);

    // Assert.
    TEST_ASSERT_NULL(termination_target);
}

/**
 * @brief Ensures expand_orbit handles null system safely.
 */
void test_expand_orbit_handles_null_system(void)
{
    // Arrange.
    RadialSystem* ghost_system = NULL;

    // Act.
    expand_orbit(ghost_system, 500);

    // Assert.
    TEST_ASSERT_NULL(ghost_system);
}

/**
 * @brief Ensures rotate handles null system safely.
 */
void test_rotate_handles_null_system(void)
{
    // Arrange.
    RadialSystem* ghost_system = NULL;

    // Act.
    rotate(ghost_system, 5);

    // Assert.
    TEST_ASSERT_NULL(ghost_system);
}

/**
 * @brief Ensures collapse_next on an empty system does nothing.
 */
void test_collapse_next_handles_empty_system(void)
{
    // Arrange.
    RadialSystem* empty_system = ignite();

    // Act.
    collapse_next(empty_system);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, empty_system->count);
    TEST_ASSERT_NULL(empty_system->cursor);

    shutdown(&empty_system);
}

/**
 * @brief Ensures expanding after a full collapse works correctly.
 */
void test_expand_after_full_collapse_restarts_orbit(void)
{
    // Arrange.
    RadialSystem* cycle_system = ignite();

    expand_orbit(cycle_system, 10);
    collapse_next(cycle_system);

    // Act.
    expand_orbit(cycle_system, 20);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, cycle_system->count);
    TEST_ASSERT_EQUAL_INT(20, cycle_system->cursor->energy);

    shutdown(&cycle_system);
}

/**
 * @brief Ensures shutdown handles double-null inputs safely.
 */
void test_shutdown_handles_null_address_safely(void)
{
    // Arrange, Act, and Assert.
    shutdown(NULL);
}

/**
 * @brief Ensures broadcast logic can be validated via internal count.
 */
void test_broadcast_structural_integrity(void)
{
    // Arrange.
    RadialSystem* audit_system = ignite();

    expand_orbit(audit_system, 101);
    expand_orbit(audit_system, 202);

    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(2, audit_system->count);
    TEST_ASSERT_EQUAL_PTR(audit_system->cursor->next_orbit->next_orbit, audit_system->cursor);

    shutdown(&audit_system);
}

/**
 * @brief Tests a complex sequence of expansion and rotation.
 */
void test_complex_orbit_maneuvers(void)
{
    // Arrange.
    RadialSystem* complex_ring = ignite();

    // Act.
    expand_orbit(complex_ring, 10);
    expand_orbit(complex_ring, 20);

    rotate(complex_ring, 1);

    expand_orbit(complex_ring, 15);

    rotate(complex_ring, 10);

    // Assert.
    TEST_ASSERT_EQUAL_INT(3, complex_ring->count);
    TEST_ASSERT_NOT_NULL(complex_ring->cursor);
    
    shutdown(&complex_ring);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_ignite_returns_non_null_system);
    RUN_TEST(test_ignite_initializes_zero_count);
    RUN_TEST(test_ignite_initializes_null_cursor);
    RUN_TEST(test_expand_orbit_sets_cursor_on_first_node);
    RUN_TEST(test_expand_orbit_first_node_is_circular);
    RUN_TEST(test_expand_orbit_increments_count_correctly);
    RUN_TEST(test_rotate_one_step_moves_cursor_forward);
    RUN_TEST(test_rotate_full_cycle_returns_to_origin);
    RUN_TEST(test_rotate_modulo_logic_handles_large_values);
    RUN_TEST(test_collapse_next_on_single_node_clears_cursor);
    RUN_TEST(test_collapse_next_removes_correct_node_and_relinks);
    RUN_TEST(test_rotate_zero_steps_does_not_change_cursor);
    RUN_TEST(test_shutdown_nullifies_pointer_successfully);
    RUN_TEST(test_expand_orbit_handles_null_system);
    RUN_TEST(test_rotate_handles_null_system);
    RUN_TEST(test_collapse_next_handles_empty_system);
    RUN_TEST(test_expand_after_full_collapse_restarts_orbit);
    RUN_TEST(test_shutdown_handles_null_address_safely);
    RUN_TEST(test_broadcast_structural_integrity);
    RUN_TEST(test_complex_orbit_maneuvers);

    // Destroy the session.
    return UNITY_END();
}