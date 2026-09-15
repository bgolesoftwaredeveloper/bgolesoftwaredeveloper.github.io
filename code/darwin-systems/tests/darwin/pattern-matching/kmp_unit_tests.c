/**
 * @file        :   kmp_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the Knuth-Morris-Pratt (KMP) string pattern matching algorithm.
 * @details     :   Validates boundary limits, exact matching sequences, overlapping patterns,
 * empty input edge cases, defensive NULL pointer protection, and distinct character 
 * states using the Unity Test Framework.
 * Compile:
 * clang ./tests/darwin/pattern-matching/kmp_unit_tests.c ./src/darwin/pattern-matching/kmp/kmp.c ./src/unity.c -Iinclude -o ./bin/kmp_unit_test
 * @version     :   1.0
 * @date        :   2026-06-06
 */
#include <stdio.h>

#include "unity.h"
#include "darwin/pattern-matching/kmp.h"

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
 * @brief Verifies that a standard pattern matching sample sequence is found successfully.
 */
void test_kmp_finds_standard_matching_pattern(void)
{
    // Arrange.
    const char* text = "ABAAABCDABCDE";
    const char* pattern = "ABCD";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies that searching for a pattern missing entirely from the text returns false.
 */
void test_kmp_returns_false_when_pattern_is_missing(void)
{
    // Arrange.
    const char* text = "XYZXYZXYZ";
    const char* pattern = "ABC";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that an exact, full length matching string returns true.
 */
void test_kmp_matches_exact_same_length_strings(void)
{
    // Arrange.
    const char* text = "HELLO";
    const char* pattern = "HELLO";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies that searching for a pattern longer than the text returns false cleanly.
 */
void test_kmp_returns_false_when_pattern_is_longer_than_text(void)
{
    // Arrange.
    const char* text = "SHORT";
    const char* pattern = "LONGERPATTERN";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that the matching process is case sensitive.
 */
void test_kmp_is_case_sensitive(void)
{
    // Arrange.
    const char* text = "abcdef";
    const char* pattern = "ABC";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that finding a single character pattern at the starting index returns true.
 */
void test_kmp_finds_single_character_at_start(void)
{
    // Arrange.
    const char* text = "ZXYWVUTS";
    const char* pattern = "Z";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies that finding a single character pattern at the final index returns true.
 */
void test_kmp_finds_single_character_at_end(void)
{
    // Arrange.
    const char* text = "ABCDEFGHZ";
    const char* pattern = "Z";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies that an empty pattern matching string inputs returns false per defensive requirements.
 */
void test_kmp_empty_pattern_returns_false(void)
{
    // Arrange.
    const char* text = "SAMPLETEXT";
    const char* pattern = "";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that scanning an empty text container context returns false.
 */
void test_kmp_empty_text_returns_false(void)
{
    // Arrange.
    const char* text = "";
    const char* pattern = "NEEDLE";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that both elements empty evaluate logically to false per implementation guidelines.
 */
void test_kmp_both_strings_empty_returns_false(void)
{
    // Arrange.
    const char* text = "";
    const char* pattern = "";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that repeating sequences with overlapping segments match successfully using the lps table.
 */
void test_kmp_handles_overlapping_repeating_patterns(void)
{
    // Arrange.
    const char* text = "AAAAAAAB";
    const char* pattern = "AAAB";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies that numeric text characters are correctly evaluated.
 */
void test_kmp_handles_numeric_characters(void)
{
    // Arrange.
    const char* text = "1234567890";
    const char* pattern = "567";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies that alphanumeric text configurations resolve matches without failure.
 */
void test_kmp_handles_special_and_whitespace_characters(void)
{
    // Arrange.
    const char* text = "Hello, World! Welcome to 2026.";
    const char* pattern = ", Wo";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies a worst-case scanning layout where a mismatch happens only at the last character.
 */
void test_kmp_worst_case_mismatch_at_pattern_end(void)
{
    // Arrange.
    const char* text = "AAAAAAAAAAAAAAAAAAB";
    const char* pattern = "AAAAAAB";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies a partial mismatch near the absolute termination edge of the text array framework.
 */
void test_kmp_partial_match_at_very_end_returns_false(void)
{
    // Arrange.
    const char* text = "ABACABAC";
    const char* pattern = "ABADE";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that the system can match long multi-word phrase patterns.
 */
void test_kmp_matches_long_sentences_with_spaces(void)
{
    // Arrange.
    const char* text = "The quick brown fox jumps over the lazy dog";
    const char* pattern = "fox jumps over";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies behavior with repeated distinct character blocks that lack the required ending sequence.
 */
void test_kmp_repeated_blocks_without_target_suffix(void)
{
    // Arrange.
    const char* text = "XYZ-XYZ-XYZ-XY";
    const char* pattern = "XYZ-XYZ-XYZ-XYZ";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

/**
 * @brief Verifies that non-printable control characters evaluate seamlessly if matched exactly.
 */
void test_kmp_handles_non_printable_control_characters(void)
{
    // Arrange.
    const char* text = "Line1\nLine2\tTabbed\r";
    const char* pattern = "Line2\tTab";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies pattern mapping behavior when text is filled with single unique repeating symbol structures.
 */
void test_kmp_all_identical_characters_matching(void)
{
    // Arrange.
    const char* text = "XXXXXXXXXXXXXXXXXXXX";
    const char* pattern = "XXXX";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_TRUE(result);
}

/**
 * @brief Verifies defensive protection when text container inputs evaluate to NULL.
 */
void test_kmp_null_text_returns_false(void)
{
    // Arrange.
    const char* text = NULL;
    const char* pattern = "TEST";

    // Act.
    bool result = kmp(text, pattern);

    // Assert.
    TEST_ASSERT_FALSE(result);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_kmp_finds_standard_matching_pattern);
    RUN_TEST(test_kmp_returns_false_when_pattern_is_missing);
    RUN_TEST(test_kmp_matches_exact_same_length_strings);
    RUN_TEST(test_kmp_returns_false_when_pattern_is_longer_than_text);
    RUN_TEST(test_kmp_is_case_sensitive);
    RUN_TEST(test_kmp_finds_single_character_at_start);
    RUN_TEST(test_kmp_finds_single_character_at_end);
    RUN_TEST(test_kmp_empty_pattern_returns_false);
    RUN_TEST(test_kmp_empty_text_returns_false);
    RUN_TEST(test_kmp_both_strings_empty_returns_false);
    RUN_TEST(test_kmp_handles_overlapping_repeating_patterns);
    RUN_TEST(test_kmp_handles_numeric_characters);
    RUN_TEST(test_kmp_handles_special_and_whitespace_characters);
    RUN_TEST(test_kmp_worst_case_mismatch_at_pattern_end);
    RUN_TEST(test_kmp_partial_match_at_very_end_returns_false);
    RUN_TEST(test_kmp_matches_long_sentences_with_spaces);
    RUN_TEST(test_kmp_repeated_blocks_without_target_suffix);
    RUN_TEST(test_kmp_handles_non_printable_control_characters);
    RUN_TEST(test_kmp_all_identical_characters_matching);
    RUN_TEST(test_kmp_null_text_returns_false);

    // Destroy the session.
    return UNITY_END();
}