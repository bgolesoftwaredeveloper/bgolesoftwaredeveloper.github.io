/**
 * @file        :   z_algorithm_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the linear-time Z-algorithm pattern matching engine.
 * @details     :   Validates exact matching scenarios, multiple repeating sequence matches,
 * overlapping patterns, empty text or pattern fields, special character
 * handling, array bound safety constraints, and defensive NULL protection
 * using the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/pattern-matching/z_algorithm_unit_tests.c ./src/darwin/pattern-matching/z-algorithm/z_algorithm.c ./src/unity.c -Iinclude -o ./bin/z_algorithm_unit_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "darwin/pattern-matching/z_algorithm.h"

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
 * @brief Verifies that a standard single pattern match located in the middle of a string is found successfully.
 */
void test_z_algorithm_finds_single_pattern_match_in_middle(void)
{
    // Arrange.
    const char* source_text = "abcdefgh";
    const char* search_pattern = "cde";
    
    size_t out_results_array[8] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(1, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(2, out_results_array[0]);
}

/**
 * @brief Verifies that a pattern sitting at the absolute start index of the target text string matches cleanly.
 */
void test_z_algorithm_matches_pattern_at_absolute_start(void)
{
    // Arrange.
    const char* source_text = "darwin_string_matching";
    const char* search_pattern = "darwin";

    size_t out_results_array[22] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(1, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(0, out_results_array[0]);
}

/**
 * @brief Verifies that a pattern sitting at the absolute final character index of the target text string matches cleanly.
 */
void test_z_algorithm_matches_pattern_at_absolute_end(void)
{
    // Arrange.
    const char* source_text = "pattern_matching_algorithm";
    const char* search_pattern = "algorithm";

    size_t out_results_array[26] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(1, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(17, out_results_array[0]);
}

/**
 * @brief Verifies that multiple separate instances of a pattern are cleanly recorded across different text blocks.
 */
void test_z_algorithm_extracts_multiple_distinct_pattern_matches(void)
{
    // Arrange.
    const char* source_text = "aabaacaadaabaaba";
    const char* search_pattern = "aaba";

    size_t out_results_array[16] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(0, out_results_array[0]);
    TEST_ASSERT_EQUAL_INT(9, out_results_array[1]);
    TEST_ASSERT_EQUAL_INT(12, out_results_array[2]);
}

/**
 * @brief Verifies that overlapping pattern structures are parsed accurately as distinct valid match occurrences.
 */
void test_z_algorithm_handles_closely_overlapping_patterns(void)
{
    // Arrange.
    const char* source_text = "aaaaa";
    const char* search_pattern = "aaa";

    size_t out_results_array[5] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(0, out_results_array[0]);
    TEST_ASSERT_EQUAL_INT(1, out_results_array[1]);
    TEST_ASSERT_EQUAL_INT(2, out_results_array[2]);
}

/**
 * @brief Verifies that looking for a pattern entirely missing from the core string text layout returns false cleanly.
 */
void test_z_algorithm_returns_false_when_pattern_is_completely_missing(void)
{
    // Arrange.
    const char* source_text = "baabaaabaab";
    const char* search_pattern = "xyz";

    size_t out_results_array[11] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies that searching for a pattern identical to the complete source text matches accurately.
 */
void test_z_algorithm_matches_exact_same_length_strings(void)
{
    // Arrange.
    const char* source_text = "identical_match";
    const char* search_pattern = "identical_match";

    size_t out_results_array[15] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(1, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(0, out_results_array[0]);
}

/**
 * @brief Verifies that execution stops and returns false when the search pattern length exceeds the base source text context.
 */
void test_z_algorithm_returns_false_when_pattern_is_longer_than_text(void)
{
    // Arrange.
    const char* source_text = "short";
    const char* search_pattern = "excessively_long_pattern";

    size_t out_results_array[5] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies that character tracking strictly enforces case sensitivity boundaries during window computations.
 */
void test_z_algorithm_pattern_matching_is_case_sensitive(void)
{
    // Arrange.
    const char* source_text = "DarwinAlg";
    const char* search_pattern = "darwin";

    size_t out_results_array[9] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies that single character pattern inputs match accurately across continuous blocks.
 */
void test_z_algorithm_identifies_single_character_matches_correctly(void)
{
    // Arrange.
    const char* source_text = "abaca";
    const char* search_pattern = "a";

    size_t out_results_array[5] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(3, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(0, out_results_array[0]);
    TEST_ASSERT_EQUAL_INT(2, out_results_array[1]);
    TEST_ASSERT_EQUAL_INT(4, out_results_array[2]);
}

/**
 * @brief Verifies that numeric characters embedded in standard text sequences are identified correctly.
 */
void test_z_algorithm_handles_numeric_characters_successfully(void)
{
    // Arrange.
    const char* source_text = "z_code_2026_test_2026_end";
    const char* search_pattern = "2026";

    size_t out_results_array[25] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    // Expecting 2 matches.
    TEST_ASSERT_EQUAL_INT(2, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(7, out_results_array[0]);
    TEST_ASSERT_EQUAL_INT(17, out_results_array[1]);
}

/**
 * @brief Verifies that punctuation symbols and whitespace formatting act as absolute structural matching indicators.
 */
void test_z_algorithm_handles_whitespace_and_punctuation_symbols(void)
{
    // Arrange.
    const char* source_text = "error! codes; error! codes;";
    const char* search_pattern = "! codes;";

    size_t out_results_array[27] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(2, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(5, out_results_array[0]);
    TEST_ASSERT_EQUAL_INT(19, out_results_array[1]);
}

/**
 * @brief Verifies a worst-case pattern matching step where checking breaks precisely on the last character.
 */
void test_z_algorithm_worst_case_mismatch_at_absolute_pattern_end(void)
{
    // Arrange.
    const char* source_text = "aaaaabaaaaabaaaaab";
    const char* search_pattern = "aaaaac";

    size_t out_results_array[18] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies that partial pattern sequences resting at the exact final text string bound return false cleanly.
 */
void test_z_algorithm_partial_match_at_absolute_bound_returns_false(void)
{
    // Arrange.
    const char* source_text = "search_ending_in_dar";
    const char* search_pattern = "darwin";

    size_t out_results_array[20] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies that internal escape markers and control block formatting elements are parsed correctly.
 */
void test_z_algorithm_handles_non_printable_control_sequences(void)
{
    // Arrange.
    const char* source_text = "row1\n\tdata\nrow2\n\tdata";
    const char* search_pattern = "\n\tdata";

    size_t out_results_array[21] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(2, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(4, out_results_array[0]);
    TEST_ASSERT_EQUAL_INT(15, out_results_array[1]);
}

/**
 * @brief Verifies that the internal logic functions cleanly when the structural custom character boundary delimiter '$' is in the text.
 */
void test_z_algorithm_handles_delimiter_character_embedded_in_source_text(void)
{
    // Arrange.
    const char* source_text = "price_is_$100_dollars_$100";
    const char* search_pattern = "$100";

    size_t out_results_array[26] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(2, tracking_match_count);
    TEST_ASSERT_EQUAL_INT(9, out_results_array[0]);
    TEST_ASSERT_EQUAL_INT(22, out_results_array[1]);
}

/**
 * @brief Verifies that an empty pattern string rejects or handles vacuous requirements safely as false.
 */
void test_z_algorithm_empty_pattern_returns_false_safely(void)
{
    // Arrange.
    const char* source_text = "non_empty_text";
    const char* search_pattern = "";

    size_t out_results_array[14] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies defensive parsing when an empty target text container sequence is evaluated.
 */
void test_z_algorithm_empty_text_returns_false_safely(void)
{
    // Arrange.
    const char* source_text = "";
    const char* search_pattern = "find_me";

    size_t out_results_array[1] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies structural integrity rules when the text pointer checks evaluate to NULL.
 */
void test_z_algorithm_null_text_pointer_returns_false_defensively(void)
{
    // Arrange.
    const char* source_text = NULL;
    const char* search_pattern = "darwin";

    size_t out_results_array[1] = { 0 };
    size_t tracking_match_count = 0;

    // Act.
    bool tracking_execution_result = z_search(source_text, search_pattern, out_results_array, &tracking_match_count);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
    TEST_ASSERT_EQUAL_INT(0, tracking_match_count);
}

/**
 * @brief Verifies complete perimeter check behaviors when all target tracking input pointers evaluate to NULL.
 */
void test_z_algorithm_all_parameters_null_returns_false_defensively(void)
{
    // Arrange, Act, Assert.
    bool tracking_execution_result = z_search(NULL, NULL, NULL, NULL);

    TEST_ASSERT_FALSE(tracking_execution_result);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_z_algorithm_finds_single_pattern_match_in_middle);
    RUN_TEST(test_z_algorithm_matches_pattern_at_absolute_start);
    RUN_TEST(test_z_algorithm_matches_pattern_at_absolute_end);
    RUN_TEST(test_z_algorithm_extracts_multiple_distinct_pattern_matches);
    RUN_TEST(test_z_algorithm_handles_closely_overlapping_patterns);
    RUN_TEST(test_z_algorithm_returns_false_when_pattern_is_completely_missing);
    RUN_TEST(test_z_algorithm_matches_exact_same_length_strings);
    RUN_TEST(test_z_algorithm_returns_false_when_pattern_is_longer_than_text);
    RUN_TEST(test_z_algorithm_pattern_matching_is_case_sensitive);
    RUN_TEST(test_z_algorithm_identifies_single_character_matches_correctly);
    RUN_TEST(test_z_algorithm_handles_numeric_characters_successfully);
    RUN_TEST(test_z_algorithm_handles_whitespace_and_punctuation_symbols);
    RUN_TEST(test_z_algorithm_worst_case_mismatch_at_absolute_pattern_end);
    RUN_TEST(test_z_algorithm_partial_match_at_absolute_bound_returns_false);
    RUN_TEST(test_z_algorithm_handles_non_printable_control_sequences);
    RUN_TEST(test_z_algorithm_handles_delimiter_character_embedded_in_source_text);
    RUN_TEST(test_z_algorithm_empty_pattern_returns_false_safely);
    RUN_TEST(test_z_algorithm_empty_text_returns_false_safely);
    RUN_TEST(test_z_algorithm_null_text_pointer_returns_false_defensively);
    RUN_TEST(test_z_algorithm_all_parameters_null_returns_false_defensively);

    // Destroy the session.
    return UNITY_END();
}