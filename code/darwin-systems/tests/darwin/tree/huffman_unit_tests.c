/**
 * @file        :   huffman_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Comprehensive unit test suite for the Huffman Coding library.
 * @details     :   Utilizes the Unity Test Project to validate tree construction, 
 * memory management, and bit-mapping logic.
 * Compile:
 * clang ./tests/darwin/tree/huffman_unit_tests.c ./src/darwin/tree/huffman/huffman.c ./src/unity.c -Iinclude -o ./bin/huffman_unit_test
 * @version     :   1.0
 * @date        :   2026-05-07
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Include header files.
#include "unity.h"
#include "darwin/tree/huffman.h"

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
 * @brief Verifies that build_tree returns NULL when given invalid inputs.
 */
void test_build_tree_returns_null_on_invalid_input(void)
{
    // Arrange.
    const char** invalid_alphabet = NULL;
    size_t* invalid_frequencies = NULL;

    // Act.
    HuffmanNode* error_node = build_tree(invalid_alphabet, invalid_frequencies, 0);

    // Assert.
    TEST_ASSERT_NULL(error_node);
}

/**
 * @brief Ensures that get_height correctly returns 0 when the root is NULL.
 */
void test_get_height_returns_zero_for_null_root(void)
{
    // Arrange.
    HuffmanNode* null_root = NULL;

    // Act.
    size_t null_height_result = get_height(null_root);

    // Assert.
    TEST_ASSERT_EQUAL_INT(0, null_height_result);
}

/**
 * @brief Confirms that a tree containing only one character has a height of 1.
 */
void test_single_character_tree_height(void)
{
    // Arrange.
    const char* solo_alpha[] = {"S"};

    size_t solo_frequency[] = {50};
    size_t solo_count = 1;

    // Act.
    HuffmanNode* solo_tree = build_tree(solo_alpha, solo_frequency, solo_count);
    size_t solo_height = get_height(solo_tree);

    // Assert.
    TEST_ASSERT_EQUAL_INT(1, solo_height);

    delete_tree(solo_tree);
}

/**
 * @brief Validates that the root frequency equals the sum of its children's frequencies.
 */
void test_frequency_summation_of_root(void)
{
    // Arrange.
    const char* pair_alphabet[] = {"Y", "Z"};

    size_t pair_frequecies[] = {15, 25};
    size_t pair_size = 2;

    // Act.
    HuffmanNode* summed_root = build_tree(pair_alphabet, pair_frequecies, pair_size);

    // Assert.
    TEST_ASSERT_EQUAL_INT(40, summed_root->frequency);

    delete_tree(summed_root);
}

/**
 * @brief Checks that map_codes produces an empty string or root-only path for one character.
 */
void test_map_codes_single_char_path(void)
{
    // Arrange.
    const char* symbol[] = {"A"};
    size_t frequency[] = {1};

    CodeTable table;

    char buffer[MAXIMUM_HEIGHT];

    for (int index = 0; index < 256; index++)
    {
        table.codes[index] = NULL;
    }

    // Act.
    HuffmanNode* tree_a = build_tree(symbol, frequency, 1);
    map_codes(tree_a, table.codes, buffer, 0);

    // Assert.
    TEST_ASSERT_EQUAL_STRING("", table.codes[(unsigned char)'A']);

    free(table.codes[(unsigned char)'A']);

    delete_tree(tree_a);
}

/**
 * @brief  Verifies that delete_tree handles NULL pointers without causing a crash.
 */
void test_delete_tree_handles_null_gracefully(void)
{
    // Arrange.
    HuffmanNode* empty_node_ptr = NULL;

    // Act.
    delete_tree(empty_node_ptr);

    // Assert.
    TEST_PASS(); 
}

/**
 * @brief Ensures a tree with two characters correctly assigns left and right children.
 */
void test_tree_structure_with_two_nodes(void)
{
    // Arrange.
    const char* duo_alphabet[] = {"H", "I"};
    size_t duo_frequencies[] = {10, 20};

    // Act.
    HuffmanNode* duo_root = build_tree(duo_alphabet, duo_frequencies, 2);

    // Assert.
    TEST_ASSERT_NOT_NULL(duo_root->left_child);
    TEST_ASSERT_NOT_NULL(duo_root->right_child);

    delete_tree(duo_root);
}

/**
 * @brief Confirms that a leaf node stores the correct character data.
 */
void test_leaf_node_character_assignment(void)
{
    // Arrange.
    const char* symbol[] = {"Q"};
    size_t frequency[] = {99};

    // Act.
    HuffmanNode* leaf = build_tree(symbol, frequency, 1);

    // Assert.
    TEST_ASSERT_EQUAL_CHAR('Q', leaf->data);

    delete_tree(leaf);
}

/**
 * @brief Verifies the height calculation for a tree with skewed (unbalanced) frequencies.
 */
void test_height_of_unbalanced_tree(void)
{
    // Arrange.
    const char* skew_chars[] = {"1", "2", "3"};
    size_t skew_weights[] = {1, 5, 25};

    // Act.
    HuffmanNode* skew_tree = build_tree(skew_chars, skew_weights, 3);
    size_t skew_height = get_height(skew_tree);

    // Assert.
    TEST_ASSERT_EQUAL_INT(3, skew_height);

    delete_tree(skew_tree);
}

/**
 * @brief Ensures that multiple characters are successfully mapped to the CodeTable.
 */
void test_map_codes_multiple_entries(void)
{
    // Arrange.
    const char* alphabet[] = {"E", "F"};
    size_t frequencies[] = {10, 10};

    CodeTable multi_table;

    char multi_buffer[MAXIMUM_HEIGHT];

    for (int index = 0; index < 256; index++)
    {
        multi_table.codes[index] = NULL;
    }

    // Act.
    HuffmanNode* multi_root = build_tree(alphabet, frequencies, 2);
    map_codes(multi_root, multi_table.codes, multi_buffer, 0);

    // Assert.
    TEST_ASSERT_NOT_NULL(multi_table.codes[(unsigned char)'E']);
    TEST_ASSERT_NOT_NULL(multi_table.codes[(unsigned char)'F']);

    free(multi_table.codes[(unsigned char)'E']);
    free(multi_table.codes[(unsigned char)'F']);

    delete_tree(multi_root);
}

/**
 * @brief Validates that all internal (non-leaf) nodes are labeled with the '#' symbol.
 */
void test_internal_node_label_consistency(void)
{
    // Arrange.
    const char* internal_set[] = {"A", "B"};
    size_t internal_vals[] = {5, 5};

    // Act.
    HuffmanNode* internal_root = build_tree(internal_set, internal_vals, 2);

    // Assert.
    TEST_ASSERT_EQUAL_CHAR('#', internal_root->data);
    delete_tree(internal_root);
}

/**
 * @brief       :   Confirms that the CodeTable structure can be manually initialized to NULL.
 */
void test_code_table_null_initialization(void)
{
    // Arrange.
    CodeTable manual_table;

    // Act.
    for (int index = 0; index < 256; index++)
    {
        manual_table.codes[index] = NULL;
    }

    // Assert.
    TEST_ASSERT_NULL(manual_table.codes[128]);
    TEST_ASSERT_NULL(manual_table.codes[64]);
}

/**
 * @brief Verifies that a perfectly balanced set of frequencies results in an optimal height.
 */
void test_get_height_of_perfectly_balanced_tree(void)
{
    // Arrange.
    const char* balance_set[] = {"X", "Y", "Z", "W"};
    size_t balance_counts[] = {10, 10, 10, 10};

    // Act.
    HuffmanNode* balanced_root = build_tree(balance_set, balance_counts, 4);
    size_t balanced_height = get_height(balanced_root);

    // Assert.
    TEST_ASSERT_EQUAL_INT(3, balanced_height);

    delete_tree(balanced_root);
}

/**
 * @brief Checks that mapped codes consist only of valid binary character strings.
 */
void test_map_codes_binary_string_content(void)
{
    // Arrange.
    const char* set_binary[] = {"P", "Q"};

    size_t weight_binary[] = {5, 15};

    CodeTable bin_test_table;

    char bin_test_buffer[MAXIMUM_HEIGHT];

    for (int index = 0; index < 256; index++) 
    {
        bin_test_table.codes[index] = NULL;
    }

    // Act.
    HuffmanNode* bin_test_root = build_tree(set_binary, weight_binary, 2);
    map_codes(bin_test_root, bin_test_table.codes, bin_test_buffer, 0);

    // Assert.
    char* result_code = bin_test_table.codes[(unsigned char)'P'];

    TEST_ASSERT_TRUE(result_code[0] == '0' || result_code[0] == '1');

    free(bin_test_table.codes[(unsigned char)'P']);
    free(bin_test_table.codes[(unsigned char)'Q']);

    delete_tree(bin_test_root);
}

/**
 * @brief Ensures the resulting tree height does not exceed the MAXIMUM_HEIGHT definition.
 */
void test_build_tree_with_max_height_constraint(void)
{
    // Arrange.
    const char* deep_alphabet[] = {"A", "B", "C", "D", "E"};
    size_t deep_frequencies[] = {1, 2, 3, 4, 5};

    // Act.
    HuffmanNode* constraint_root = build_tree(deep_alphabet, deep_frequencies, 5);
    size_t constraint_height = get_height(constraint_root);

    // Assert.
    TEST_ASSERT_TRUE(constraint_height <= MAXIMUM_HEIGHT);

    delete_tree(constraint_root);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_build_tree_returns_null_on_invalid_input);
    RUN_TEST(test_get_height_returns_zero_for_null_root);
    RUN_TEST(test_single_character_tree_height);
    RUN_TEST(test_frequency_summation_of_root);
    RUN_TEST(test_map_codes_single_char_path);
    RUN_TEST(test_delete_tree_handles_null_gracefully);
    RUN_TEST(test_tree_structure_with_two_nodes);
    RUN_TEST(test_leaf_node_character_assignment);
    RUN_TEST(test_height_of_unbalanced_tree);
    RUN_TEST(test_map_codes_multiple_entries);
    RUN_TEST(test_internal_node_label_consistency);
    RUN_TEST(test_code_table_null_initialization);
    RUN_TEST(test_get_height_of_perfectly_balanced_tree);
    RUN_TEST(test_map_codes_binary_string_content);
    RUN_TEST(test_build_tree_with_max_height_constraint);

    // Destory the session.
    return UNITY_END();
}