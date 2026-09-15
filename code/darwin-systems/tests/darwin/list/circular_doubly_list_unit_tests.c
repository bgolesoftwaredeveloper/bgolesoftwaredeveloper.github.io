/**
 * @file        :   circular_doubly_list_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Circular Doubly Linked List.
 * @details     :   Utilizes the Unity Test Framework to validate the integrity of the 
 * Galaxy container and OrbitalNode mutations. Tests cover O(1) 
 * appends, bidirectional linking, circular traversal limits, 
 * and robust memory management including edge-case erasures.
 * Compile:
 * clang ./tests/darwin/list/circular_doubly_list_unit_tests.c ./src/darwin/list/circular-doubly-list/circular_doubly_linked_list.c ./src/unity.c -Iinclude -o ./bin/circular_doubly_list_unit_test
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "darwin/list/circular_doubly_linked_list.h"

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
 * @brief Ensures a new Galaxy starts with null pointers and zero count.
 */
void test_galaxy_initialization_sets_null_sentinel_and_zero_count(void)
{
    // Arrange.
    Galaxy* celestial_void;

    // Act.
    celestial_void = new_galaxy();

    // Assert.
    TEST_ASSERT_NOT_NULL(celestial_void);
    TEST_ASSERT_NULL(celestial_void->sentinel);
    TEST_ASSERT_EQUAL_UINT(0, celestial_void->population_count);

    free(celestial_void);
}

/**
 * @brief Validates that the first appended node points to itself.
 */
void test_append_single_orbital_node_establishes_self_circularity(void)
{
    // Arrange.
    Galaxy* solar_system = new_galaxy();

    int initial_energy = 500;

    // Act.
    append(solar_system, initial_energy);

    // Assert.
    TEST_ASSERT_EQUAL_INT(initial_energy, solar_system->sentinel->rhythmic_data);
    TEST_ASSERT_EQUAL_PTR(solar_system->sentinel, solar_system->sentinel->forward_link);
    TEST_ASSERT_EQUAL_PTR(solar_system->sentinel, solar_system->sentinel->backward_link);

    clear(solar_system);

    free(solar_system);
}

/**
 * @brief Verifies pointer linkage between two nodes.
 */
void test_append_dual_nodes_connects_forward_and_backward_links(void)
{
    // Arrange.
    Galaxy* binary_cluster = new_galaxy();

    // Act.
    append(binary_cluster, 101);
    append(binary_cluster, 202);

    // Assert.
    OrbitalNode* primary_star = binary_cluster->sentinel;
    OrbitalNode* companion_star = primary_star->forward_link;

    TEST_ASSERT_EQUAL_PTR(companion_star, primary_star->forward_link);
    TEST_ASSERT_EQUAL_PTR(primary_star, companion_star->forward_link);
    TEST_ASSERT_EQUAL_PTR(companion_star, primary_star->backward_link);
    TEST_ASSERT_EQUAL_PTR(primary_star, companion_star->backward_link);

    clear(binary_cluster);

    free(binary_cluster);
}

/**
 * @brief Checks if find correctly locates an existing value.
 */
void test_find_existing_rhythmic_data_returns_correct_node_pointer(void)
{
    // Arrange.
    Galaxy* search_nebula = new_galaxy();

    append(search_nebula, 77);
    append(search_nebula, 88);
    append(search_nebula, 99);

    // Act.
    OrbitalNode* targeted_node = find(search_nebula, 88);

    // Assert.
    TEST_ASSERT_NOT_NULL(targeted_node);
    TEST_ASSERT_EQUAL_INT(88, targeted_node->rhythmic_data);

    clear(search_nebula);

    free(search_nebula);
}

/**
 * @brief Verifies update changes data without altering pointers.
 */
void test_update_modifies_payload_value_at_correct_orbital_position(void)
{
    // Arrange.
    Galaxy* kinetic_belt = new_galaxy();

    append(kinetic_belt, 10);
    append(kinetic_belt, 20);

    // Act.
    bool was_updated = update(kinetic_belt, 20, 99);

    // Assert.
    TEST_ASSERT_TRUE(was_updated);
    TEST_ASSERT_EQUAL_INT(99, kinetic_belt->sentinel->forward_link->rhythmic_data);

    clear(kinetic_belt);

    free(kinetic_belt);
}

/**
 * @brief Ensures erase removes the only node and nullifies sentinel.
 */
void test_erase_last_remaining_node_resets_galaxy_to_void(void)
{
    // Arrange.
    Galaxy* lonely_star = new_galaxy();

    append(lonely_star, 1);

    // Act.
    bool success_purge = erase(lonely_star, 1);

    // Assert.
    TEST_ASSERT_TRUE(success_purge);
    TEST_ASSERT_NULL(lonely_star->sentinel);
    TEST_ASSERT_EQUAL_UINT(0, lonely_star->population_count);

    free(lonely_star);
}

/**
 * @brief Verifies sentinel shifts when the head node is erased.
 */
void test_erase_sentinel_node_shifts_apex_pointer_to_next_orbital(void)
{
    // Arrange.
    Galaxy* sequence_array = new_galaxy();

    append(sequence_array, 10);
    append(sequence_array, 20);

    // Act.
    erase(sequence_array, 10);

    // Assert.
    TEST_ASSERT_EQUAL_INT(20, sequence_array->sentinel->rhythmic_data);
    TEST_ASSERT_EQUAL_PTR(sequence_array->sentinel, sequence_array->sentinel->forward_link);

    clear(sequence_array);

    free(sequence_array);
}

/**
 * @brief Checks if population count decrements on erasure.
 */
void test_erase_middle_node_maintains_correct_population_count(void)
{
    // Arrange.
    Galaxy* counting_cluster = new_galaxy();

    append(counting_cluster, 5);
    append(counting_cluster, 10);
    append(counting_cluster, 15);

    // Act.
    erase(counting_cluster, 10);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(2, counting_cluster->population_count);

    clear(counting_cluster);

    free(counting_cluster);
}

/**
 * @brief Verifies clear deallocates all nodes but keeps the Galaxy alive.
 */
void test_clear_removes_all_nodes_leaving_container_struct_intact(void)
{
    // Arrange.
    Galaxy* crowded_system = new_galaxy();

    for (int mass = 0; mass < 10; mass++) 
    {
        append(crowded_system, mass);
    }

    // Act.
    clear(crowded_system);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, crowded_system->population_count);
    TEST_ASSERT_NULL(crowded_system->sentinel);

    free(crowded_system);
}

/**
 * @brief Ensures find returns null for missing data.
 */
void test_find_non_existent_data_returns_null_pointer_safety(void)
{
    // Arrange.
    Galaxy* empty_void = new_galaxy();

    // Act.
    OrbitalNode* phantom_result = find(empty_void, 404);

    // Assert.
    TEST_ASSERT_NULL(phantom_result);

    free(empty_void);
}

/**
 * @brief Validates update fails correctly on missing target.
 */
void test_update_returns_false_when_target_value_is_not_located(void)
{
    // Arrange.
    Galaxy* static_field = new_galaxy();

    append(static_field, 100);

    // Act.
    bool status_failure = update(static_field, 500, 600);

    // Assert.
    TEST_ASSERT_FALSE(status_failure);
    TEST_ASSERT_EQUAL_INT(100, static_field->sentinel->rhythmic_data);

    clear(static_field);

    free(static_field);
}

/**
 * @brief Ensures bidirectional links are bridged after middle deletion.
 */
void test_erase_middle_node_relinks_neighboring_forward_and_backward_pointers(void)
{
    // Arrange.
    Galaxy* bridge_chain = new_galaxy();

    append(bridge_chain, 10);
    append(bridge_chain, 20);
    append(bridge_chain, 30);

    // Act.
    erase(bridge_chain, 20);

    // Assert.
    OrbitalNode* start = bridge_chain->sentinel;
    OrbitalNode* end = start->forward_link;

    TEST_ASSERT_EQUAL_PTR(end, start->forward_link);
    TEST_ASSERT_EQUAL_PTR(start, end->backward_link);

    clear(bridge_chain);

    free(bridge_chain);
}

/**
 * @brief Checks if append increases population count.
 */
void test_append_multiple_items_increments_population_counter_accurately(void)
{
    // Arrange.
    Galaxy* mass_test = new_galaxy();

    // Act.
    append(mass_test, 1);
    append(mass_test, 2);
    append(mass_test, 3);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(3, mass_test->population_count);

    clear(mass_test);

    free(mass_test);
}

/**
 * @brief Verifies find respects the search for the sentinel value.
 */
void test_find_can_locate_data_stored_at_sentinel_apex_position(void)
{
    // Arrange.
    Galaxy* apex_nebula = new_galaxy();

    append(apex_nebula, 999);

    // Act.
    OrbitalNode* apex_node = find(apex_nebula, 999);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(apex_nebula->sentinel, apex_node);

    clear(apex_nebula);

    free(apex_nebula);
}

/**
 * @brief Tests empty list behavior for erase.
 */
void test_erase_on_empty_galaxy_returns_false_and_does_not_crash(void)
{
    // Arrange.
    Galaxy* vacuum = new_galaxy();

    // Act.
    bool empty_erase_status = erase(vacuum, 10);

    // Assert.
    TEST_ASSERT_FALSE(empty_erase_status);

    free(vacuum);
}

/**
 * @brief Ensures update fails gracefully on an empty list.
 */
void test_update_on_empty_galaxy_returns_false_without_segfault(void)
{
    // Arrange.
    Galaxy* void_manifest = new_galaxy();

    // Act.
    bool update_void_status = update(void_manifest, 1, 2);

    // Assert.
    TEST_ASSERT_FALSE(update_void_status);

    free(void_manifest);
}

/**
 * @brief Verifies tail connectivity after multiple appends.
 */
void test_append_updates_sentinel_backward_link_to_newest_tail_node(void)
{
    // Arrange.
    Galaxy* tail_tracking_system = new_galaxy();

    // Act.
    append(tail_tracking_system, 10);
    append(tail_tracking_system, 20);
    append(tail_tracking_system, 30);

    // Assert.
    OrbitalNode* head = tail_tracking_system->sentinel;

    TEST_ASSERT_EQUAL_INT(30, head->backward_link->rhythmic_data);

    clear(tail_tracking_system);

    free(tail_tracking_system);
}

/**
 * @brief Validates clearing an already empty list.
 */
void test_clear_on_already_empty_galaxy_performs_no_ops_safely(void)
{
    // Arrange.
    Galaxy* zero_node_galaxy = new_galaxy();

    // Act.
    clear(zero_node_galaxy);

    // Assert.
    TEST_ASSERT_EQUAL_UINT(0, zero_node_galaxy->population_count);
    TEST_ASSERT_NULL(zero_node_galaxy->sentinel);

    free(zero_node_galaxy);
}

/**
 * @brief Tests erase for a value at the absolute end of the circle.
 */
void test_erase_tail_node_preserves_circular_linkage_to_sentinel(void)
{
    // Arrange.
    Galaxy* tail_deletion_test = new_galaxy();

    append(tail_deletion_test, 100);
    append(tail_deletion_test, 200);

    // Act.
    erase(tail_deletion_test, 200);

    // Assert.
    OrbitalNode* new_tail = tail_deletion_test->sentinel->backward_link;

    TEST_ASSERT_EQUAL_PTR(tail_deletion_test->sentinel, new_tail);
    TEST_ASSERT_EQUAL_INT(100, new_tail->rhythmic_data);

    clear(tail_deletion_test);

    free(tail_deletion_test);
}

/**
 * @brief Verifies that find stops searching after one full rotation.
 */
void test_find_traverses_entire_circle_but_stops_at_sentinel_origin(void)
{
    // Arrange.
    Galaxy* circular_search = new_galaxy();

    append(circular_search, 1);
    append(circular_search, 2);

    // Act.
    OrbitalNode* result_found = find(circular_search, 3);

    // Assert.
    TEST_ASSERT_NULL(result_found);

    clear(circular_search);

    free(circular_search);
}

int main(void)
{
    // initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_galaxy_initialization_sets_null_sentinel_and_zero_count);
    RUN_TEST(test_clear_removes_all_nodes_leaving_container_struct_intact);
    RUN_TEST(test_clear_on_already_empty_galaxy_performs_no_ops_safely);
    RUN_TEST(test_append_single_orbital_node_establishes_self_circularity);
    RUN_TEST(test_append_dual_nodes_connects_forward_and_backward_links);
    RUN_TEST(test_append_multiple_items_increments_population_counter_accurately);
    RUN_TEST(test_append_updates_sentinel_backward_link_to_newest_tail_node);
    RUN_TEST(test_find_existing_rhythmic_data_returns_correct_node_pointer);
    RUN_TEST(test_find_non_existent_data_returns_null_pointer_safety);
    RUN_TEST(test_find_can_locate_data_stored_at_sentinel_apex_position);
    RUN_TEST(test_find_traverses_entire_circle_but_stops_at_sentinel_origin);
    RUN_TEST(test_update_modifies_payload_value_at_correct_orbital_position);
    RUN_TEST(test_update_returns_false_when_target_value_is_not_located);
    RUN_TEST(test_update_on_empty_galaxy_returns_false_without_segfault);
    RUN_TEST(test_erase_last_remaining_node_resets_galaxy_to_void);
    RUN_TEST(test_erase_sentinel_node_shifts_apex_pointer_to_next_orbital);
    RUN_TEST(test_erase_middle_node_maintains_correct_population_count);
    RUN_TEST(test_erase_middle_node_relinks_neighboring_forward_and_backward_pointers);
    RUN_TEST(test_erase_on_empty_galaxy_returns_false_and_does_not_crash);
    RUN_TEST(test_erase_tail_node_preserves_circular_linkage_to_sentinel);

    // Destory the session.
    return UNITY_END();
}