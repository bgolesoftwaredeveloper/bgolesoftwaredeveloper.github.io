/**
 * @file        :   blowfish_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Blowfish symmetric-key block cipher.
 * @details     :   This suite utilizes the Unity Test Framework to verify the 
 * functional correctness of the Blowfish implementation. It 
 * covers encryption/decryption round-trips, key expansion 
 * logic, avalanche effect, and boundary conditions such as 
 * minimum/maximum key lengths and NULL pointer safety
 * Compile:
 * clang ./tests/darwin/cryptography/blowfish_unit_tests.c ./src/darwin/cryptography/blowfish/blowfish.c ./src/unity.c -Iinclude -o ./bin/blowfish_unit_test
 * @version     :   1.0
 * @date        :   2026-05-05
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/cryptography/blowfish.h"

static Blowfish context;

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    memset(&context, 0, sizeof(Blowfish));
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    ;;
}

/**
 * @brief Validates that a standard message can be recovered after encryption.
 */
void test_encryption_decryption_cycle(void)
{
    // Arrange.
    const uint8_t key[] = "StandardKey";

    uint32_t left = 0x01234567;
    uint32_t right = 0x89ABCDEF;
    
    initialize(&context, key, strlen((char*)key));

    // Act.
    blowfish_encrypt_block(&context, &left, &right);
    blowfish_decrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(0x01234567, left);
    TEST_ASSERT_EQUAL_UINT32(0x89ABCDEF, right);
}

/**
 * @brief Ensures the cipher produces different output for different inputs (diffusion).
 */
void test_ciphertext_uniqueness(void)
{
    // Arrange.
    const uint8_t key[] = "Secret";

    uint32_t left_1 = 0xAAAAAABB;
    uint32_t right_1 = 0xCCCCCCDD;

    uint32_t left_2 = 0xAAAAAABC;
    uint32_t right_2 = 0xCCCCCCDD;

    initialize(&context, key, strlen((char*)key));

    // Act.
    blowfish_encrypt_block(&context, &left_1, &right_1);
    blowfish_encrypt_block(&context, &left_2, &right_2);

    // Assert.
    TEST_ASSERT_FALSE(left_1 == left_2 && right_1 == right_2);
}

/**
 * @brief Tests the minimum valid key length (4 bytes).
 */
void test_minimum_key_length(void)
{
    // Arrange.
    const uint8_t key[] = "1234";

    uint32_t left = 0;
    uint32_t right = 0;

    // Act.
    initialize(&context, key, 4);

    blowfish_encrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_FALSE(left == 0 && right == 0);
}

/**
 * @brief Tests the maximum supported key length (56 bytes/448 bits).
 */
void test_maximum_key_length(void)
{
    // Arrange.
    uint8_t key[56];

    memset(key, 0xFF, 56);

    uint32_t left = 0x11223344;
    uint32_t right = 0x55667788;

    initialize(&context, key, 56);

    // Act.
    blowfish_encrypt_block(&context, &left, &right);
    blowfish_decrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(0x11223344, left);
}

/**
 * @brief Validates "All Zero" block encryption.
 */
void test_zero_block_encryption(void)
{
    // Arrange.
    const uint8_t key[] = "Key";

    uint32_t left = 0;
    uint32_t right = 0;

    initialize(&context, key, 3);

    // Act.
    blowfish_encrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_NOT_EQUAL(0, left);
    TEST_ASSERT_NOT_EQUAL(0, right);
}

/**
 * @brief Ensures initialization fails gracefully with NULL context.
 */
void test_null_context_handling(void)
{
    // Arrange.
    const uint8_t key[] = "Key";

    // Act & Assert.
    initialize(NULL, key, 3);
}

/**
 * @brief Ensures encryption fails gracefully with NULL data pointers.
 */
void test_null_data_pointers(void)
{
    // Arrange.
    uint32_t value = 0x1234;

    initialize(&context, (uint8_t*)"key", 3);

    // Act & Assert.
    blowfish_encrypt_block(&context, NULL, &value);
    blowfish_encrypt_block(&context, &value, NULL);
}

/**
 * @brief Tests if a change in one key bit results in a different ciphertext.
 */
void test_key_avalanche_effect(void)
{
    // Arrange.
    uint8_t key_1[] = "Key1";
    uint8_t key_2[] = "Key2";

    uint32_t left_1 = 0x1234;
    uint32_t right_1 = 0x5678;

    uint32_t left_2 = 0x1234;
    uint32_t right_2 = 0x5678;

    // Act.
    initialize(&context, key_1, 4);

    blowfish_encrypt_block(&context, &left_1, &right_1);
    
    initialize(&context, key_2, 4);

    blowfish_encrypt_block(&context, &left_2, &right_2);

    // Assert.
    TEST_ASSERT_NOT_EQUAL(left_1, left_2);
}

/**
 * @brief Tests block-to-block independence (ECB property).
 */
void test_block_independence(void)
{
    // Arrange.
    const uint8_t key[] = "Key";

    uint32_t left = 0x1;
    uint32_t right = 0x1;

    initialize(&context, key, 3);

    // Act.
    blowfish_encrypt_block(&context, &left, &right);

    uint32_t first_left = left;
    uint32_t first_right = right;
    
    left = 0x1;
    right = 0x1;

    blowfish_encrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(first_left, left);
    TEST_ASSERT_EQUAL_UINT32(first_right, right);
}

/**
 * @brief Validates recovery of a single-bit message.
 */
void test_single_bit_message(void)
{
    // Arrange.
    const uint8_t key[] = "Key";

    uint32_t left = 0x00000001;
    uint32_t right = 0x00000000;

    initialize(&context, key, 3);

    // Act.
    blowfish_encrypt_block(&context, &left, &right);
    blowfish_decrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(1, left);
    TEST_ASSERT_EQUAL_UINT32(0, right);
}

/**
 * @brief Tests repeated initialization with the same context.
 */
void test_reinitialization(void)
{
    // Arrange.
    const uint8_t key[] = "Key";

    uint32_t left = 0xDEAD;
    uint32_t right = 0xBEEF;

    // Act.
    initialize(&context, key, 3);
    initialize(&context, key, 3);

    blowfish_encrypt_block(&context, &left, &right);
    blowfish_decrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(0xDEAD, left);
}

/**
 * @brief Tests encryption with a single-byte key.
 */
void test_extremely_short_key(void)
{
    // Arrange.
    const uint8_t key[] = "A";

    uint32_t left = 0x1111;
    uint32_t right = 0x2222;

    // Act.
    initialize(&context, key, 1);

    blowfish_encrypt_block(&context, &left, &right);
    blowfish_decrypt_block(&context, &left, &right);

    // Assert.
    TEST_ASSERT_EQUAL_UINT32(0x1111, left);
}

/**
 * @brief Verifies that the P-array is modified after initialization.
 */
void test_p_array_modification(void)
{
    // Arrange.
    const uint8_t key[] = "CheckP";

    uint32_t initial_p0 = initial_permuation_values[0];

    // Act.
    initialize(&context, key, 6);

    // Assert.
    TEST_ASSERT_NOT_EQUAL(initial_p0, context.permutation_array[0]);
}

/**
 * @brief Tests performance/stability over multiple encryptions.
 */
void test_consecutive_blocks(void)
{
    // Arrange.
    const uint8_t key[] = "StreamTest";

    uint32_t left = 0xAAAA;
    uint32_t right = 0xBBBB;

    initialize(&context, key, 10);

    // Act.
    for(int index = 0; index < 100; index++)
    {
        blowfish_encrypt_block(&context, &left, &right);
    }

    // Assert.
    TEST_ASSERT_NOT_EQUAL(0xAAAA, left);
}

/**
 * @brief Validates the Feistel network property: Encryption != Decryption.
 */
void test_asymmetric_operation(void)
{
    // Arrange.
    const uint8_t key[] = "Fix";

    uint32_t left = 0x1234;
    uint32_t right = 0x5678;

    uint32_t left_encrypted = left;
    uint32_t right_encrypted = right;

    uint32_t left_decrypted = left;
    uint32_t right_decrypted = right;

    initialize(&context, key, 3);

    // Act.
    blowfish_encrypt_block(&context, &left_encrypted, &right_encrypted);
    blowfish_decrypt_block(&context, &left_decrypted, &right_decrypted);

    // Assert.
    TEST_ASSERT_FALSE(left_encrypted == left_decrypted && right_encrypted == right_decrypted);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();
    
    // Register the tests to run.
    RUN_TEST(test_encryption_decryption_cycle);
    RUN_TEST(test_ciphertext_uniqueness);
    RUN_TEST(test_minimum_key_length);
    RUN_TEST(test_maximum_key_length);
    RUN_TEST(test_zero_block_encryption);
    RUN_TEST(test_null_context_handling);
    RUN_TEST(test_null_data_pointers);
    RUN_TEST(test_key_avalanche_effect);
    RUN_TEST(test_block_independence);
    RUN_TEST(test_single_bit_message);
    RUN_TEST(test_reinitialization);
    RUN_TEST(test_extremely_short_key);
    RUN_TEST(test_p_array_modification);
    RUN_TEST(test_consecutive_blocks);
    RUN_TEST(test_asymmetric_operation);
    
    // Destroy the session.
    return UNITY_END();
}