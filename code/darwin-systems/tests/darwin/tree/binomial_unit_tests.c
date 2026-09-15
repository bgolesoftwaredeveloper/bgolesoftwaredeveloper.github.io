/**
 * @file        :   bionomial_unit_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite verifying the integrity, boundaries, and 
 * algorithmic state machines of the Binomial Heap API.
 * @details     :   Validates structural properties including node initialization,
 * link mutations, consolidation merges, edge-case null protections, min extractions,
 * and memory cleanup workflows using the Unity framework.
 * Compile:
 * clang ./tests/darwin/tree/binomial_unit_tests.c ./src/darwin/tree/binomial-heap/binomial_heap.c ./src/unity.c -Iinclude -o ./bin/binomial_unit_test
 * @version     :   1.0
 * @date        :   2026-04-19
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/binomial_heap.h"

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
 * @brief Verifies that create returns a valid node with default attributes.
 */
void test_create_node_initializes_fields_correctly(void)
{
    // Arrange.
    int target_payload = 42;

    // Act.
    BinomialNode* node = create(target_payload);

    // Assert.
    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_EQUAL_INT(target_payload, node->key);
    TEST_ASSERT_EQUAL_UINT(0, node->degree);

    TEST_ASSERT_NULL(node->child);
    TEST_ASSERT_NULL(node->sibling);
    TEST_ASSERT_NULL(node->parent);

    free_heap(node);
}

/**
 * @brief Verifies retrieve_minimum fails gracefully when the heap is completely empty.
 */
void test_retrieve_minimum_returns_false_on_null_heap(void)
{
    // Arrange.
    BinomialNode* empty_heap = NULL;

    int validation_buffer = 0;

    // Act.
    bool tracking_status = retrieve_minimum(empty_heap, &validation_buffer);

    // Assert.
    TEST_ASSERT_FALSE(tracking_status);
}

/**
 * @brief Verifies retrieve_minimum fails gracefully when the output destination pointer is null.
 */
void test_retrieve_minimum_returns_false_on_null_destination_pointer(void)
{
    // Arrange.
    BinomialNode* active_node = create(10);

    // Act.
    bool tracking_status = retrieve_minimum(active_node, NULL);

    // Assert.
    TEST_ASSERT_FALSE(tracking_status);

    free_heap(active_node);
}

/**
 * @brief Verifies extract_minimum returns false safely when working on an unallocated heap pointer.
 */
void test_extract_minimum_returns_false_on_null_heap_reference(void)
{
    // Arrange.
    BinomialNode* unallocated_heap = NULL;

    int extraction_buffer = 0;

    // Act.
    bool operational_status = extract_minimum(&unallocated_heap, &extraction_buffer);

    // Assert.
    TEST_ASSERT_FALSE(operational_status);
}

/**
 * @brief Verifies extract_minimum handles null destination buffers safely.
 */
void test_extract_minimum_returns_false_on_null_key_destination(void)
{
    // Arrange.
    BinomialNode* single_node_heap = create(5);

    // Act.
    bool operational_status = extract_minimum(&single_node_heap, NULL);

    // Assert.
    TEST_ASSERT_FALSE(operational_status);

    free_heap(single_node_heap);
}

/**
 * @brief Verifies binomial_union yields the active heap unchanged when combined with a null heap.
 */
void test_binomial_union_with_null_heap_returns_original_intact(void)
{
    // Arrange.
    BinomialNode* standalone_heap = create(15);
    BinomialNode* supplementary_null_heap = NULL;

    // Act.
    BinomialNode* unified_heap_result = binomial_union(standalone_heap, supplementary_null_heap);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(standalone_heap, unified_heap_result);

    free_heap(unified_heap_result);
}

/**
 * @brief Verifies binomial_union yields the baseline heap unchanged when the calling stream is null.
 */
void test_binomial_union_with_null_stream_returns_heap_intact(void)
{
    // Arrange.
    BinomialNode* baseline_heap = create(22);
    BinomialNode* introductory_null_stream = NULL;

    // Act.
    BinomialNode* unified_heap_result = binomial_union(introductory_null_stream, baseline_heap);

    // Assert.
    TEST_ASSERT_EQUAL_PTR(baseline_heap, unified_heap_result);

    free_heap(unified_heap_result);
}

/**
 * @brief Verifies retrieve_minimum correctly fetches the only existing payload in a single-element tree.
 */
void test_retrieve_minimum_extracts_payload_from_single_node(void)
{
    // Arrange.
    BinomialNode* minimal_heap = create(99);

    int retrieved_key = 0;

    // Act.
    bool extraction_success = retrieve_minimum(minimal_heap, &retrieved_key);

    // Assert.
    TEST_ASSERT_TRUE(extraction_success);
    TEST_ASSERT_EQUAL_INT(99, retrieved_key);

    free_heap(minimal_heap);
}

/**
 * @brief Verifies union consolidation orders two elements correctly based on minimum keys.
 */
void test_binomial_union_consolidates_two_distinct_single_nodes(void)
{
    // Arrange.
    BinomialNode* dominant_root = create(10);
    BinomialNode* subordinate_root = create(20);

    // Act.
    BinomialNode* combined_heap = binomial_union(dominant_root, subordinate_root);

    // Assert.
    TEST_ASSERT_NOT_NULL(combined_heap);
    TEST_ASSERT_EQUAL_INT(10, combined_heap->key);
    TEST_ASSERT_EQUAL_UINT(1, combined_heap->degree);
    TEST_ASSERT_NOT_NULL(combined_heap->child);
    TEST_ASSERT_EQUAL_INT(20, combined_heap->child->key);

    free_heap(combined_heap);
}

/**
 * @brief Verifies union inversion logic makes the lower value node the structural root.
 */
void test_binomial_union_handles_inverted_value_ordering(void)
{
    // Arrange.
    BinomialNode* larger_valued_node = create(50);
    BinomialNode* smaller_valued_node = create(5);

    // Act.
    BinomialNode* combined_heap = binomial_union(larger_valued_node, smaller_valued_node);

    // Assert.
    TEST_ASSERT_NOT_NULL(combined_heap);
    TEST_ASSERT_EQUAL_INT(5, combined_heap->key);
    TEST_ASSERT_NOT_NULL(combined_heap->child);
    TEST_ASSERT_EQUAL_INT(50, combined_heap->child->key);

    free_heap(combined_heap);
}

/**
 * @brief Verifies that extract_minimum leaves the heap completely empty when removing the final node.
 */
void test_extract_minimum_clears_out_final_remaining_node(void)
{
    // Arrange.
    BinomialNode* terminal_heap = create(77);

    int value_capture_buffer = 0;

    // Act.
    bool procedural_outcome = extract_minimum(&terminal_heap, &value_capture_buffer);

    // Assert.
    TEST_ASSERT_TRUE(procedural_outcome);
    TEST_ASSERT_EQUAL_INT(77, value_capture_buffer);
    TEST_ASSERT_NULL(terminal_heap);
}

/**
 * @brief Verifies retrieve_minimum performs accurate linear scanning across a collection of roots.
 */
void test_retrieve_minimum_scans_across_multiple_sibling_trees(void)
{
    // Arrange.
    BinomialNode* first_order_tree = create(30);
    BinomialNode* second_order_tree = create(12);
    BinomialNode* third_order_tree = create(45);

    first_order_tree->sibling = second_order_tree;
    second_order_tree->sibling = third_order_tree;

    int tracking_minimum_value = 0;

    // Act.
    bool scanning_outcome = retrieve_minimum(first_order_tree, &tracking_minimum_value);

    // Assert.
    TEST_ASSERT_TRUE(scanning_outcome);
    TEST_ASSERT_EQUAL_INT(12, tracking_minimum_value);
    
    first_order_tree->sibling = NULL;
    second_order_tree->sibling = NULL;

    free_heap(first_order_tree);
    free_heap(second_order_tree);
    free_heap(third_order_tree);
}

/**
 * @brief Verifies sequential unions correctly execute cascaded linking patterns up to order 2.
 */
void test_cascaded_unions_generate_higher_degree_binomial_trees(void)
{
    // Arrange.
    BinomialNode* prime_heap = create(100);
    BinomialNode* secondary_heap = create(200);
    BinomialNode* tertiary_heap = create(150);
    BinomialNode* quaternary_heap = create(50);

    // Act.
    BinomialNode* processing_stage_one = binomial_union(prime_heap, secondary_heap);
    BinomialNode* processing_stage_two = binomial_union(tertiary_heap, quaternary_heap);
    BinomialNode* final_consolidated_heap = binomial_union(processing_stage_one, processing_stage_two);

    // Assert.
    TEST_ASSERT_NOT_NULL(final_consolidated_heap);
    TEST_ASSERT_EQUAL_UINT(2, final_consolidated_heap->degree);
    TEST_ASSERT_EQUAL_INT(50, final_consolidated_heap->key);

    free_heap(final_consolidated_heap);
}

/**
 * @brief Verifies that extracting the root of a tree correctly splits it and reverses child components.
 */
void test_extract_minimum_promotes_and_reverses_children_correctly(void)
{
    // Arrange.
    BinomialNode* lead_node = create(10);
    BinomialNode* trailing_node = create(20);
    BinomialNode* dynamic_heap = binomial_union(lead_node, trailing_node);

    int numeric_payload_sink = 0;

    // Act.
    bool processing_status = extract_minimum(&dynamic_heap, &numeric_payload_sink);

    // Assert.
    TEST_ASSERT_TRUE(processing_status);
    TEST_ASSERT_EQUAL_INT(10, numeric_payload_sink);
    TEST_ASSERT_NOT_NULL(dynamic_heap);
    TEST_ASSERT_EQUAL_INT(20, dynamic_heap->key);
    TEST_ASSERT_EQUAL_UINT(0, dynamic_heap->degree);

    free_heap(dynamic_heap);
}

/**
 * @brief Verifies identical priority payloads group systematically without memory corruption or lock errors.
 */
void test_binomial_union_maintains_integrity_with_duplicate_keys(void)
{
    // Arrange.
    BinomialNode* primary_duplicate = create(15);
    BinomialNode* secondary_duplicate = create(15);

    // Act.
    BinomialNode* unified_duplicate_heap = binomial_union(primary_duplicate, secondary_duplicate);

    // Assert.
    TEST_ASSERT_NOT_NULL(unified_duplicate_heap);
    TEST_ASSERT_EQUAL_INT(15, unified_duplicate_heap->key);
    TEST_ASSERT_EQUAL_UINT(1, unified_duplicate_heap->degree);

    free_heap(unified_duplicate_heap);
}

/**
 * @brief Verifies that free_heap handles empty structures safely without throwing faults.
 */
void test_free_heap_handles_null_addresses_without_faulting(void)
{
    // Arrange.
    BinomialNode* completely_vacant_reference = NULL;

    // Act.
    free_heap(completely_vacant_reference);

    // Assert.
    TEST_ASSERT_NULL(completely_vacant_reference);
}

/**
 * @brief Verifies print_tree handles unallocated tracking bounds cleanly.
 */
void test_print_tree_handles_null_addresses_without_faulting(void)
{
    // Arrange.
    BinomialNode* unallocated_visualization_target = NULL;

    // Act.
    print_tree(unallocated_visualization_target);

    // Assert.
    TEST_ASSERT_NULL(unallocated_visualization_target);
}

/**
 * @brief Verifies that a series of structural modifications returns the true minimum tracking sequence.
 */
void test_comprehensive_insert_and_extraction_sequence(void)
{
    // Arrange.
    BinomialNode* primary_execution_heap = NULL;

    int items_to_process[] = {40, 10, 30, 20};
    int structural_sink_variable = 0;

    // Act.
    for (size_t track_index = 0; track_index < 4; track_index++)
    {
        BinomialNode* elemental_node = create(items_to_process[track_index]);
        primary_execution_heap = binomial_union(primary_execution_heap, elemental_node);
    }

    extract_minimum(&primary_execution_heap, &structural_sink_variable);

    int current_minimum_value = 0;

    retrieve_minimum(primary_execution_heap, &current_minimum_value);

    // Assert.
    TEST_ASSERT_EQUAL_INT(10, structural_sink_variable);
    TEST_ASSERT_EQUAL_INT(20, current_minimum_value);

    free_heap(primary_execution_heap);
}

/**
 * @brief Verifies that extraction correctly processes a tree with multi-level depth layouts.
 */
void test_extract_minimum_from_multi_level_tree_maintains_heap_property(void)
{
    // Arrange.
    BinomialNode* source_heap = NULL;

    int dataset[] = {80, 70, 60, 50, 40};

    for (size_t track_index = 0; track_index < 5; track_index++)
    {
        BinomialNode* elemental_node = create(dataset[track_index]);
        source_heap = binomial_union(source_heap, elemental_node);
    }

    int extracted_key_payload = 0;

    // Act.
    extract_minimum(&source_heap, &extracted_key_payload);

    int secondary_minimum_value = 0;

    retrieve_minimum(source_heap, &secondary_minimum_value);

    // Assert.
    TEST_ASSERT_EQUAL_INT(40, extracted_key_payload);
    TEST_ASSERT_EQUAL_INT(50, secondary_minimum_value);

    free_heap(source_heap);
}

/**
 * @brief Verifies structural consistency when performing operations on pre-sorted array sets.
 */
void test_binomial_union_preserves_validity_with_monotonically_increasing_elements(void)
{
    // Arrange.
    BinomialNode* organized_heap = NULL;

    int linear_sequence[] = {1, 2, 3, 4};

    // Act.
    for (size_t track_index = 0; track_index < 4; track_index++)
    {
        BinomialNode* discrete_node = create(linear_sequence[track_index]);
        organized_heap = binomial_union(organized_heap, discrete_node);
    }

    int discovered_minimum = 0;

    retrieve_minimum(organized_heap, &discovered_minimum);

    // Assert.
    TEST_ASSERT_NOT_NULL(organized_heap);
    TEST_ASSERT_EQUAL_INT(1, discovered_minimum);

    free_heap(organized_heap);
}

/**
 * @brief Execution entry point parsing and executing all registered unit tests.
 */
int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_create_node_initializes_fields_correctly);
    RUN_TEST(test_retrieve_minimum_returns_false_on_null_heap);
    RUN_TEST(test_retrieve_minimum_returns_false_on_null_destination_pointer);
    RUN_TEST(test_extract_minimum_returns_false_on_null_heap_reference);
    RUN_TEST(test_extract_minimum_returns_false_on_null_key_destination);
    RUN_TEST(test_binomial_union_with_null_heap_returns_original_intact);
    RUN_TEST(test_binomial_union_with_null_stream_returns_heap_intact);
    RUN_TEST(test_retrieve_minimum_extracts_payload_from_single_node);
    RUN_TEST(test_binomial_union_consolidates_two_distinct_single_nodes);
    RUN_TEST(test_binomial_union_handles_inverted_value_ordering);
    RUN_TEST(test_extract_minimum_clears_out_final_remaining_node);
    RUN_TEST(test_retrieve_minimum_scans_across_multiple_sibling_trees);
    RUN_TEST(test_cascaded_unions_generate_higher_degree_binomial_trees);
    RUN_TEST(test_extract_minimum_promotes_and_reverses_children_correctly);
    RUN_TEST(test_binomial_union_maintains_integrity_with_duplicate_keys);
    RUN_TEST(test_free_heap_handles_null_addresses_without_faulting);
    RUN_TEST(test_print_tree_handles_null_addresses_without_faulting);
    RUN_TEST(test_comprehensive_insert_and_extraction_sequence);
    RUN_TEST(test_extract_minimum_from_multi_level_tree_maintains_heap_property);
    RUN_TEST(test_binomial_union_preserves_validity_with_monotonically_increasing_elements);

    // Destroy the session.
    return UNITY_END();
}