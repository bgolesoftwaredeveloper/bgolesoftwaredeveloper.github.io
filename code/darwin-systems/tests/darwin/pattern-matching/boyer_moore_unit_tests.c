/**
 * @file        :   boyer_moore_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Boyer-Moore string pattern matching algorithm.
 * @details     :   Validates exact matching boundaries, bad character shift limits, good suffix 
 * heurisitics, edge conditions, repetitive structures, case sensitivity bounds,
 * numeric arrays, non-printables, and defensive NULL protection blocks 
 * using the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/pattern-matching/boyer_moore_unit_tests.c ./src/darwin/pattern-matching/boyer-moore/boyer_moore.c ./src/unity.c -Iinclude -o ./bin/boyer_moore_unit_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/pattern-matching/boyer_moore.h"

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
 * @brief Verifies that a basic matching pattern configuration yields true.
 */
void test_boyer_moore_finds_standard_pattern_match(void)
{
    // Arrange.
    const char* target_text = "ABAAABCDABCDEABCDABCDEF";
    const char* search_pattern = "ABCD";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, search_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that searching for a pattern missing entirely from the text returns false cleanly.
 */
void test_boyer_moore_returns_false_when_pattern_is_missing(void)
{
    // Arrange.
    const char* target_text = "AXYZBXYZCXYZ";
    const char* non_existent_pattern = "ABCD";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, non_existent_pattern);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that an exact full length matching pattern configuration returns true.
 */
void test_boyer_moore_matches_exact_same_length_strings(void)
{
    // Arrange.
    const char* target_text = "Darwin";
    const char* identical_pattern = "Darwin";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, identical_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that searching for a pattern longer than the target text layout rejects instantly.
 */
void test_boyer_moore_returns_false_when_pattern_is_longer_than_text(void)
{
    // Arrange.
    const char* short_text = "Short";
    const char* excessively_long_pattern = "MuchLongerPattern";

    // Act.
    bool tracking_execution_result = boyer_moore(short_text, excessively_long_pattern);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that scanning paths respect explicit character case sensitivity boundaries.
 */
void test_boyer_moore_pattern_matching_is_case_sensitive(void)
{
    // Arrange.
    const char* target_text = "Darwin";
    const char* cased_mismatch_pattern = "darwin";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, cased_mismatch_pattern);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that finding a single character pattern at the exact start of the text returns true.
 */
void test_boyer_moore_finds_single_character_at_absolute_start(void)
{
    // Arrange.
    const char* target_text = "Darwin";
    const char* single_char_start_pattern = "D";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, single_char_start_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that finding a single character pattern at the absolute end of the text returns true.
 */
void test_boyer_moore_finds_single_character_at_absolute_end(void)
{
    // Arrange.
    const char* target_text = "Darwin";
    const char* single_char_end_pattern = "n";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, single_char_end_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that an empty pattern string safely returns false to prevent infinite loop underflows.
 */
void test_boyer_moore_empty_pattern_returns_false(void)
{
    // Arrange.
    const char* target_text = "Darwin";
    const char* vacant_search_pattern = "";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, vacant_search_pattern);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that numeric text characters embedded inside strings are parsed effectively.
 */
void test_boyer_moore_handles_numeric_characters_successfully(void)
{
    // Arrange.
    const char* target_text = "Year_2026.";
    const char* numeric_crossing_pattern = "2026";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, numeric_crossing_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that whitespace blocks and punctuation symbols are structural matches when evaluated.
 */
void test_boyer_moore_handles_whitespace_and_punctuation_symbols(void)
{
    // Arrange.
    const char* target_text = "Hello! World";
    const char* sequence_with_spaces_pattern = "! Wo";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, sequence_with_spaces_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies a worst-case pattern scanning run where a match fails precisely at the terminal index of the sequence.
 */
void test_boyer_moore_worst_case_mismatch_at_absolute_pattern_end(void)
{
    // Arrange.
    const char* target_text = "AAAAAAAAB";
    const char* lookahead_mismatch_pattern = "AAAAAAB";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, lookahead_mismatch_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that partial matches resting immediately against the maximum search bounds return false cleanly.
 */
void test_boyer_moore_partial_match_at_absolute_bound_returns_false(void)
{
    // Arrange.
    const char* target_text = "Darwin";
    const char* trailing_partial_pattern = "winX";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, trailing_partial_pattern);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that embedded specialized escape sequences and non-printable structures match securely inside strings.
 */
void test_boyer_moore_handles_non_printable_control_sequences(void)
{
    // Arrange.
    const char* target_text = "Tab\tNL\n";
    const char* control_char_pattern = "\tNL\n";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, control_char_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that defensive pointer inspection logic catches instances where the text node evaluated is NULL.
 */
void test_boyer_moore_null_text_pointer_returns_false(void)
{
    // Arrange.
    const char* completely_null_text = NULL;
    const char* fallback_search_pattern = "Darwin";

    // Act.
    bool tracking_execution_result = boyer_moore(completely_null_text, fallback_search_pattern);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies that defensive pointer inspection logic catches instances where the pattern node evaluated is NULL.
 */
void test_boyer_moore_null_pattern_pointer_returns_false(void)
{
    // Arrange.
    const char* target_text = "Darwin";
    const char* completely_null_pattern = NULL;

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, completely_null_pattern);

    // Assert.
    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies total safety parameters when all input structures evaluate simultaneously to NULL pointers.
 */
void test_boyer_moore_all_parameters_null_returns_false_defensively(void)
{
    // Arrange, Act, Assert.
    bool tracking_execution_result = boyer_moore(NULL, NULL);

    TEST_ASSERT_FALSE(tracking_execution_result);
}

/**
 * @brief Verifies performance optimization logic when processing a text sequence completely populated by one repetitive character.
 */
void test_boyer_moore_highly_repetitive_single_character_text(void)
{
    // Arrange.
    const char* target_text = "ZZZZZZZZZZZZZZZZZZZZZZZZ";
    const char* multi_char_pattern = "ZZZZ";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, multi_char_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies optimization steps using patterns where bad-character rules force maximum alignment jumps across unmatched characters.
 */
void test_boyer_moore_triggers_large_bad_character_shifts(void)
{
    // Arrange.
    const char* target_text = "EFGHIJKLMNOPQRSTUVWXYZ_ABCDEF";
    const char* far_pattern = "ABCDEF";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, far_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies specific good-suffix lookahead paths using a nested repeating layout structure.
 */
void test_boyer_moore_utilizes_good_suffix_heuristic_shifts(void)
{
    // Arrange.
    const char* target_text = "ANANABANANA";
    const char* repeating_suffix_pattern = "BANANA";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, repeating_suffix_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

/**
 * @brief Verifies that binary-style string blocks consisting of alternating sequences pass tracking correctly.
 */
void test_boyer_moore_handles_alternating_binary_style_sequences(void)
{
    // Arrange.
    const char* target_text = "01010101010101011010";
    const char* binary_sub_pattern = "10110";

    // Act.
    bool tracking_execution_result = boyer_moore(target_text, binary_sub_pattern);

    // Assert.
    TEST_ASSERT_TRUE(tracking_execution_result);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_boyer_moore_finds_standard_pattern_match);
    RUN_TEST(test_boyer_moore_returns_false_when_pattern_is_missing);
    RUN_TEST(test_boyer_moore_matches_exact_same_length_strings);
    RUN_TEST(test_boyer_moore_returns_false_when_pattern_is_longer_than_text);
    RUN_TEST(test_boyer_moore_pattern_matching_is_case_sensitive);
    RUN_TEST(test_boyer_moore_finds_single_character_at_absolute_start);
    RUN_TEST(test_boyer_moore_finds_single_character_at_absolute_end);
    RUN_TEST(test_boyer_moore_empty_pattern_returns_false);
    RUN_TEST(test_boyer_moore_handles_numeric_characters_successfully);
    RUN_TEST(test_boyer_moore_handles_whitespace_and_punctuation_symbols);
    RUN_TEST(test_boyer_moore_worst_case_mismatch_at_absolute_pattern_end);
    RUN_TEST(test_boyer_moore_partial_match_at_absolute_bound_returns_false);
    RUN_TEST(test_boyer_moore_handles_non_printable_control_sequences);
    RUN_TEST(test_boyer_moore_null_text_pointer_returns_false);
    RUN_TEST(test_boyer_moore_null_pattern_pointer_returns_false);
    RUN_TEST(test_boyer_moore_all_parameters_null_returns_false_defensively);
    RUN_TEST(test_boyer_moore_highly_repetitive_single_character_text);
    RUN_TEST(test_boyer_moore_triggers_large_bad_character_shifts);
    RUN_TEST(test_boyer_moore_utilizes_good_suffix_heuristic_shifts);
    RUN_TEST(test_boyer_moore_handles_alternating_binary_style_sequences);

    // Destroy the session.
    return UNITY_END();
}