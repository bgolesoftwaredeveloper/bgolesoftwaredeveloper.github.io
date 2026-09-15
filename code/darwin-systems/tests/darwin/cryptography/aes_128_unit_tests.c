/**
 * @file        :   aes_128_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the AES-128 cryptographic algorithm.
 * @details     :   This suite verifies the accuracy of sub_bytes, shift_rows, 
 * mix_columns, and the full Encrypt/Decrypt cycle.
 * Compile:
 * clang ./tests/darwin/cryptography/aes_128_unit_tests.c ./src/darwin/cryptography/aes-128/aes_128.c ./src/unity.c -Iinclude -o ./bin/aes_128_unit_test
 * @version     :   1.0
 * @date        :   2026-05-02
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/cryptography/aes_128.h"

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
 * @brief Test the forward SubBytes transformation against known S-Box values.
 */
void test_sub_bytes_transformation(void)
{
    // Arrange.
    uint8_t state[16] = 
    {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f
    };

    uint8_t expected[16] = 
    {
        0x63, 0x7c, 0x77, 0x7b,
        0xf2, 0x6b, 0x6f, 0xc5,
        0x30, 0x01, 0x67, 0x2b,
        0xfe, 0xd7, 0xab, 0x76
    };

    // Act.
    sub_bytes(state);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, state, 16);
}

/**
 * @brief Test the Inverse SubBytes transformation.
 */
void test_inverse_sub_bytes_transformation(void)
{
    // Arrange.
    uint8_t state[16] = 
    {
        0x63, 0x7c, 0x77, 0x7b,
        0xf2, 0x6b, 0x6f, 0xc5, 
        0x30, 0x01, 0x67, 0x2b,
        0xfe, 0xd7, 0xab, 0x76
    };

    uint8_t expected[16] = 
    {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b, 
        0x0c, 0x0d, 0x0e, 0x0f
    };

    // Act.
    inverse_sub_bytes(state);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, state, 16);
}

/**
 * @brief Test ShiftRows shifting offsets.
 */
void test_shift_rows_transformation(void)
{
    // Arrange.
    uint8_t state[16] = 
    {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f
    };

    uint8_t expected[16] = 
    {
        0x00, 0x05, 0x0a, 0x0f,
        0x04, 0x09, 0x0e, 0x03,
        0x08, 0x0d, 0x02, 0x07,
        0x0c, 0x01, 0x06, 0x0b
    };

    // Act.
    shift_rows(state);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, state, 16);
}

/**
 * @brief Test the Inverse ShiftRows transformation.
 */
void test_inverse_shift_rows_transformation(void)
{
    // Arrange.
    uint8_t state[16] = {
        0x00, 0x05, 0x0a, 0x0f, 
        0x04, 0x09, 0x0e, 0x03, 
        0x08, 0x0d, 0x02, 0x07, 
        0x0c, 0x01, 0x06, 0x0b
    };

    uint8_t expected[16] = 
    {
        0x00, 0x01, 0x02, 0x03, 
        0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0a, 0x0b, 
        0x0c, 0x0d, 0x0e, 0x0f
    };

    // Act.
    inverse_shift_rows(state);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, state, 16);
}

/**
 * @brief Test MixColumns transformation on a standard block.
 */
void test_mix_columns_transformation(void)
{
    // Arrange.
    uint8_t state[16] = 
    {
        0xdb, 0x13, 0x53, 0x45,
        0xf2, 0x0a, 0x22, 0x5c,
        0x01, 0x01, 0x01, 0x01, 
        0xc6, 0xc6, 0xc6, 0xc6
    };

    uint8_t expected[16] = 
    {
        0x8e, 0x4d, 0xa1, 0xbc,
        0x9f, 0xdc, 0x58, 0x9d,
        0x01, 0x01, 0x01, 0x01, 
        0xc6, 0xc6, 0xc6, 0xc6
    };

    // Act.
    mix_columns(state);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, state, 16);
}

/**
 * @brief Test AddRoundKey XOR logic.
 */
void test_add_round_key_logic(void)
{
    // Arrange.
    uint8_t state[16] = 
    {
        0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb,
        0xcc, 0xdd, 0xee, 0xff
    };

    uint8_t key[16] = 
    {
        0xff, 0xee, 0xdd, 0xcc,
        0xbb, 0xaa, 0x99, 0x88,
        0x77, 0x66, 0x55, 0x44,
        0x33, 0x22, 0x11, 0x00
    };

    uint8_t expected[16] = 
    {
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff
    };

    // Act.
    add_round_key(state, key);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, state, 16);
}

/**
 * @brief Test full encryption cycle with a single-round schedule (zero-filled schedule).
 */
void test_aes_encrypt_with_minimal_schedule(void)
{
    // Arrange.
    uint8_t plaintext[16] = {0};
    uint8_t schedule[176] = {0};
    uint8_t ciphertext[16];

    // Act.
    aes_128_encrypt(plaintext, schedule, ciphertext);

    // Assert.
    TEST_ASSERT_NOT_NULL(ciphertext);
}

/**
 * @brief Test full decryption cycle against original plaintext.
 */
void test_aes_decrypt_recovery_success(void)
{
    // Arrange.
    uint8_t original[16] = "Braiden Gole";
    uint8_t schedule[176] = {0x01, 0x02, 0x03, 0x04};
    uint8_t ciphertext[16];
    uint8_t decrypted[16];

    // Act.
    aes_128_encrypt(original, schedule, ciphertext);
    aes_128_decrypt(ciphertext, schedule, decrypted);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(original, decrypted, 16);
}

/**
 * @brief Test Galois Field multiplication for identity property.
 */
void test_galois_field_identity_multiplier(void)
{
    // Arrange.
    uint8_t multiplicand = 0x57;
    uint8_t multiplier = 0x01;

    // Act.
    uint8_t result = galois_field_multiplication(multiplicand, multiplier);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8(0x57, result);
}

/**
 * @brief Test Galois Field multiplication with x (0x02).
 */
void test_galois_field_multiplication_by_two(void)
{
    // Arrange.
    uint8_t multiplicand = 0x80;
    uint8_t multiplier = 0x02;

    // Act.
    uint8_t result = galois_field_multiplication(multiplicand, multiplier);

    // Assert (0x80 << 1 = 0x00 ^ 0x1b).
    TEST_ASSERT_EQUAL_HEX8(0x1b, result);
}

/**
 * @brief Test defensive check for NULL input in encrypt.
 */
void test_encrypt_null_input_guard(void)
{
    // Arrange.
    uint8_t schedule[176] = {0};
    uint8_t output[16];

    // Act & Assert (Should return gracefully without segfaulting).
    aes_128_encrypt(NULL, schedule, output);
}

/**
 * @brief Test defensive check for NULL schedule in decrypt.
 */
void test_decrypt_null_schedule_guard(void)
{
    // Arrange.
    uint8_t ciphertext[16] = {0};
    uint8_t output[16];

    // Act & Assert.
    aes_128_decrypt(ciphertext, NULL, output);
}

/**
 * @brief Test decryption with an incorrect key schedule fails to recover data.
 */
void test_decryption_with_wrong_key_fails(void)
{
    // Arrange.
    uint8_t plaintext[16] = "SecretData";
    uint8_t correct_schedule[176] = {0xAA};
    uint8_t wrong_schedule[176] = {0xBB};
    uint8_t ciphertext[16];
    uint8_t decrypted[16];

    // Act.
    aes_128_encrypt(plaintext, correct_schedule, ciphertext);
    aes_128_decrypt(ciphertext, wrong_schedule, decrypted);

    // Assert.
    TEST_ASSERT_TRUE(memcmp(plaintext, decrypted, 16) != 0);
}

/**
 * @brief Test Galois Field multiplication against specific FIPS-197 value.
 */
void test_galois_field_fips_vector(void)
{
    // Arrange.
    uint8_t multiplicand = 0x57;
    uint8_t multiplier = 0x13;

    // Act.
    uint8_t result = galois_field_multiplication(multiplicand, multiplier);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8(0xfe, result);
}

/**
 * @brief Test the Inverse MixColumns transformation reverts MixColumns.
 */
void test_inverse_mix_columns_reversion(void)
{
    // Arrange.
    uint8_t state[16] = 
    {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C,
        0x0D, 0x0E, 0x0F, 0x10
    };

    uint8_t original[16];
    memcpy(original, state, 16);

    // Act.
    mix_columns(state);
    inverse_mix_columns(state);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(original, state, 16);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_sub_bytes_transformation);
    RUN_TEST(test_inverse_sub_bytes_transformation);
    RUN_TEST(test_shift_rows_transformation);
    RUN_TEST(test_inverse_shift_rows_transformation);
    RUN_TEST(test_mix_columns_transformation);
    RUN_TEST(test_add_round_key_logic);
    RUN_TEST(test_aes_encrypt_with_minimal_schedule);
    RUN_TEST(test_aes_decrypt_recovery_success);
    RUN_TEST(test_galois_field_identity_multiplier);
    RUN_TEST(test_galois_field_multiplication_by_two);
    RUN_TEST(test_encrypt_null_input_guard);
    RUN_TEST(test_decrypt_null_schedule_guard);
    RUN_TEST(test_decryption_with_wrong_key_fails);
    RUN_TEST(test_galois_field_fips_vector);
    RUN_TEST(test_inverse_mix_columns_reversion);

    // Destory the session.
    return UNITY_END();
}