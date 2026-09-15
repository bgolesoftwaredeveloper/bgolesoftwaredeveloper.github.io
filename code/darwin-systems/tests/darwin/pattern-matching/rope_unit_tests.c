/**
 * @file        :   rope_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Rope string pattern matching algorithm.
 * @details     :   Validates dynamic tree traversals, cross-boundary leaf sequences,
 * unbalanced hierarchies, empty strings, exact edge bounds, index 
 * extractions, and defensive NULL protection using the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/pattern-matching/rope_unit_tests.c ./src/darwin/pattern-matching/rope/rope.c ./src/unity.c -Iinclude -o ./bin/rope_unit_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/pattern-matching/rope.h"

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
 * @brief Verifies that a pattern matching sequence completely localized to a single leaf node is found successfully.
 */
void test_rope_finds_pattern_localized_to_single_leaf(void)
{
    // Arrange.
    Node single_leaf_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };

    const char* search_pattern = "arw";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&single_leaf_node, search_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(1, matched_index_output);
}

/**
 * @brief Verifies that a pattern spanning across the boundaries of two distinct leaf nodes is resolved correctly.
 */
void test_rope_finds_pattern_spanning_across_two_leaves(void)
{
    // Arrange.
    Node boundary_suffix_leaf = { .left = NULL, .right = NULL, .weight = 4, .leaf = "_is_" };
    Node boundary_prefix_leaf = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };
    Node horizontal_join_node = { .left = &boundary_prefix_leaf, .right = &boundary_suffix_leaf, .weight = 6, .leaf = NULL };
    
    const char* cross_boundary_pattern = "win_i";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&horizontal_join_node, cross_boundary_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, matched_index_output);
}

/**
 * @brief Verifies that a deep complex tree accurately processes pattern boundaries split across nested internal branches.
 */
void test_rope_finds_pattern_deeply_nested_across_multiple_subtrees(void)
{
    // Arrange.
    Node deep_suffix_leaf = { .left = NULL, .right = NULL, .weight = 7, .leaf = "awesome" };
    Node deep_infix_leaf = { .left = NULL, .right = NULL, .weight = 4, .leaf = "_is_" };
    Node intermediate_join_node = { .left = &deep_infix_leaf, .right = &deep_suffix_leaf, .weight = 4, .leaf = NULL };
    Node deep_prefix_leaf = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };
    Node root_join_node = { .left = &deep_prefix_leaf, .right = &intermediate_join_node, .weight = 6, .leaf = NULL };

    const char* deep_spanning_pattern = "win_is_awe";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&root_join_node, deep_spanning_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, matched_index_output);
}

/**
 * @brief Verifies that searching for a pattern missing entirely from all nodes returns false cleanly.
 */
void test_rope_returns_false_when_pattern_is_missing(void)
{
    // Arrange.
    Node target_leaf_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };

    const char* non_existent_pattern = "robot";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&target_leaf_node, non_existent_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that an exact full length matching pattern configuration returns true at index zero.
 */
void test_rope_matches_exact_same_length_strings(void)
{
    // Arrange.
    Node single_leaf_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };

    const char* identical_pattern = "Darwin";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&single_leaf_node, identical_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, matched_index_output);
}

/**
 * @brief Verifies that searching for a pattern longer than the cumulative rope layout rejects instantly.
 */
void test_rope_returns_false_when_pattern_is_longer_than_total_rope(void)
{
    // Arrange.
    Node tiny_leaf_node = { .left = NULL, .right = NULL, .weight = 3, .leaf = "Cat" };

    const char* excessively_long_pattern = "Catastrophic";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&tiny_leaf_node, excessively_long_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that tree structure scanning paths respect explicit case sensitivity boundaries.
 */
void test_rope_pattern_matching_is_case_sensitive(void)
{
    // Arrange.
    Node target_leaf_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };

    const char* cased_mismatch_pattern = "darwin";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&target_leaf_node, cased_mismatch_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that finding a single character pattern at the exact start of the root left leaf returns true.
 */
void test_rope_finds_single_character_at_absolute_start(void)
{
    // Arrange.
    Node target_leaf_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };

    const char* single_char_start_pattern = "D";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&target_leaf_node, single_char_start_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, matched_index_output);
}

/**
 * @brief Verifies that finding a single character pattern at the absolute end of a deeply right-nested leaf returns true.
 */
void test_rope_finds_single_character_at_absolute_end(void)
{
    // Arrange.
    Node trailing_leaf_node = { .left = NULL, .right = NULL, .weight = 7, .leaf = "awesome" };
    Node leading_leaf_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };
    Node root_join_node = { .left = &leading_leaf_node, .right = &trailing_leaf_node, .weight = 6, .leaf = NULL };
    
    const char* single_char_end_pattern = "e";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&root_join_node, single_char_end_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(8, matched_index_output);
}

/**
 * @brief Verifies that an empty pattern string safely returns true at index zero per vacuous truth standards.
 */
void test_rope_empty_pattern_returns_true_with_zero_index(void)
{
    // Arrange.
    Node standard_tree_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };

    const char* vacant_search_pattern = "";
    size_t matched_index_output = 999; 

    // Act.
    bool tracking_execution_result = rope(&standard_tree_node, vacant_search_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, matched_index_output);
}

/**
 * @brief Verifies that highly unbalanced left-skewed trees resolve character mapping lookups across deep linear hierarchies.
 */
void test_rope_handles_highly_unbalanced_left_skewed_trees(void)
{
    // Arrange.
    Node internal_leaf_c = { .left = NULL, .right = NULL, .weight = 2, .leaf = "in" };
    Node left_skew_join_b = { .left = &internal_leaf_c, .right = NULL, .weight = 2, .leaf = NULL };
    Node left_skew_join_a = { .left = &left_skew_join_b, .right = NULL, .weight = 2, .leaf = NULL };
    
    const char* sub_segment_pattern = "in";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&left_skew_join_a, sub_segment_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, matched_index_output);
}

/**
 * @brief Verifies that numeric text characters embedded inside discrete leaf configurations are parsed effectively.
 */
void test_rope_handles_numeric_characters_successfully(void)
{
    // Arrange.
    Node numeric_right_leaf = { .left = NULL, .right = NULL, .weight = 5, .leaf = "2026." };
    Node text_left_leaf = { .left = NULL, .right = NULL, .weight = 5, .leaf = "Year_" };
    Node numeric_join_node = { .left = &text_left_leaf, .right = &numeric_right_leaf, .weight = 5, .leaf = NULL };

    const char* numeric_crossing_pattern = "r_20";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&numeric_join_node, numeric_crossing_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, matched_index_output);
}

/**
 * @brief Verifies that whitespace blocks and punctuation symbols are structural matches when evaluated across junctions.
 */
void test_rope_handles_whitespace_and_punctuation_symbols(void)
{
    // Arrange.
    Node punctuation_leaf = { .left = NULL, .right = NULL, .weight = 2, .leaf = "! " };
    Node lead_phrase_leaf = { .left = NULL, .right = NULL, .weight = 5, .leaf = "Hello" };
    Node symbol_join_node = { .left = &lead_phrase_leaf, .right = &punctuation_leaf, .weight = 5, .leaf = NULL };

    const char* sequence_with_spaces_pattern = "lo! ";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&symbol_join_node, sequence_with_spaces_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, matched_index_output);
}

/**
 * @brief Verifies a worst-case pattern scanning run where a match fails precisely at the terminal index of the sequence.
 */
void test_rope_worst_case_mismatch_at_absolute_pattern_end(void)
{
    // Arrange.
    Node repeating_right_leaf = { .left = NULL, .right = NULL, .weight = 4, .leaf = "AAAB" };
    Node repeating_left_leaf = { .left = NULL, .right = NULL, .weight = 4, .leaf = "AAAA" };
    Node repeating_join_node = { .left = &repeating_left_leaf, .right = &repeating_right_leaf, .weight = 4, .leaf = NULL };

    const char* lookahead_mismatch_pattern = "AAAAB";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&repeating_join_node, lookahead_mismatch_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, matched_index_output);
}

/**
 * @brief Verifies that partial matches resting immediately against the maximum search bounds return false cleanly.
 */
void test_rope_partial_match_at_absolute_bound_returns_false(void)
{
    // Arrange.
    Node ending_leaf_node = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };

    const char* trailing_partial_pattern = "winX";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&ending_leaf_node, trailing_partial_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that tracking logic maintains correctness when multiple occurrences of a pattern exist in separate nodes.
 */
void test_rope_extracts_first_chronological_match_for_duplicate_patterns(void)
{
    // Arrange.
    Node duplicate_right_leaf = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };
    Node duplicate_left_leaf = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };
    Node master_join_node = { .left = &duplicate_left_leaf, .right = &duplicate_right_leaf, .weight = 6, .leaf = NULL };

    const char* recurring_pattern = "win";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&master_join_node, recurring_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, matched_index_output);
}

/**
 * @brief Verifies that consecutive recursive steps evaluate correctly when a node points cleanly to a right child with no left leaf.
 */
void test_rope_handles_internal_node_with_missing_left_child_correctly(void)
{
    // Arrange.
    Node dangling_right_leaf = { .left = NULL, .right = NULL, .weight = 6, .leaf = "Darwin" };
    Node malformed_internal_node = { .left = NULL, .right = &dangling_right_leaf, .weight = 0, .leaf = NULL };

    const char* lookup_target_pattern = "Dar";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&malformed_internal_node, lookup_target_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that embedded specialized escape sequences and non-printable structures match securely inside leaf blocks.
 */
void test_rope_handles_non_printable_control_sequences(void)
{
    // Arrange.
    Node escape_leaf_node = { .left = NULL, .right = NULL, .weight = 8, .leaf = "Tab\tNL\n" };

    const char* control_char_pattern = "\tNL\n";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(&escape_leaf_node, control_char_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, matched_index_output);
}

/**
 * @brief Verifies that defensive pointer inspection logic catches instances where the root node evaluated is NULL.
 */
void test_rope_null_root_node_pointer_returns_false(void)
{
    // Arrange.
    const Node* completely_null_rope_root = NULL;

    const char* fallback_search_pattern = "Darwin";
    size_t matched_index_output = 0;

    // Act.
    bool tracking_execution_result = rope(completely_null_rope_root, fallback_search_pattern, &matched_index_output);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies total safety parameters when all input structures evaluate simultaneously to NULL pointers.
 */
void test_rope_all_parameters_null_returns_false_defensively(void)
{
    // Arrange, Act, Assert.
    bool tracking_execution_result = rope(NULL, NULL, NULL);

    TEST_ASSERT_FALSE(tracking_execution_result);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_rope_finds_pattern_localized_to_single_leaf);
    RUN_TEST(test_rope_finds_pattern_spanning_across_two_leaves);
    RUN_TEST(test_rope_finds_pattern_deeply_nested_across_multiple_subtrees);
    RUN_TEST(test_rope_returns_false_when_pattern_is_missing);
    RUN_TEST(test_rope_matches_exact_same_length_strings);
    RUN_TEST(test_rope_returns_false_when_pattern_is_longer_than_total_rope);
    RUN_TEST(test_rope_pattern_matching_is_case_sensitive);
    RUN_TEST(test_rope_finds_single_character_at_absolute_start);
    RUN_TEST(test_rope_finds_single_character_at_absolute_end);
    RUN_TEST(test_rope_empty_pattern_returns_true_with_zero_index);
    RUN_TEST(test_rope_handles_highly_unbalanced_left_skewed_trees);
    RUN_TEST(test_rope_handles_numeric_characters_successfully);
    RUN_TEST(test_rope_handles_whitespace_and_punctuation_symbols);
    RUN_TEST(test_rope_worst_case_mismatch_at_absolute_pattern_end);
    RUN_TEST(test_rope_partial_match_at_absolute_bound_returns_false);
    RUN_TEST(test_rope_extracts_first_chronological_match_for_duplicate_patterns);
    RUN_TEST(test_rope_handles_internal_node_with_missing_left_child_correctly);
    RUN_TEST(test_rope_handles_non_printable_control_sequences);
    RUN_TEST(test_rope_null_root_node_pointer_returns_false);
    RUN_TEST(test_rope_all_parameters_null_returns_false_defensively);

    // Destroy the session.
    return UNITY_END();
}