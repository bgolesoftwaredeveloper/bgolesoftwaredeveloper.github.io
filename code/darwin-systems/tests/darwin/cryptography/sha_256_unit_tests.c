/**
 * @file        :   sha_256_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit tests for the SHA-256 cryptographic hash algorithm.
 * @details     :   This suite verifies the accuracy of the SHA-256 implementation 
 * against NIST FIPS 180-4 test vectors. It validates the 
 * initialization state, streaming (multi-part) updates, 
 * padding boundary conditions (56 and 64 bytes), and 
 * defensive null-pointer handling.
 * Compile:
 * clang ./tests/darwin/cryptography/sha_256_unit_tests.c ./src/darwin/cryptography/sha-256/sha_256.c ./src/unity.c -Iinclude -o ./bin/sha_256_unit_test
 * @version     :   1.0
 * @date        :   2026-05-05
 */
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "darwin/cryptography/sha_256.h"

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
 * @brief Test SHA-256 hash of an empty string.
 */
void test_sha256_empty_string(void)
{
    // Arrange.
    SHA256 context = {0};
    uint8_t digest[32];
    uint8_t expected[32] = {
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
    };

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)"", 0);
    sha_256_finalize(&context, digest);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, digest, 32);
}

/**
 * @brief Test SHA-256 with standard NIST vector "abc".
 */
void test_sha256_standard_vector_abc(void)
{
    // Arrange.
    SHA256 context = {0};
    uint8_t digest[32];
    uint8_t expected[32] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)"abc", 3);
    sha_256_finalize(&context, digest);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, digest, 32);
}

/**
 * @brief Test multi-part (streaming) update consistency.
 */
void test_sha256_streaming_consistency(void)
{
    // Arrange.
    SHA256 context = {0};
    uint8_t digest_stream[32];
    uint8_t digest_single[32];

    const char* part1 = "The quick brown fox ";
    const char* part2 = "jumps over the lazy dog";
    const char* full  = "The quick brown fox jumps over the lazy dog";

    // Act.
    sha_256_initialize(&context);

    sha_256_update(&context, (const uint8_t*)part1, strlen(part1));
    sha_256_update(&context, (const uint8_t*)part2, strlen(part2));

    sha_256_finalize(&context, digest_stream);

    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)full, strlen(full));
    sha_256_finalize(&context, digest_single);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(digest_single, digest_stream, 32);
}

/**
 * @brief Test initialization resets the internal bit counter.
 */
void test_sha256_initialization_resets_counter(void)
{
    // Arrange.
    SHA256 context = {0};

    context.total_bit_count = 1024;
    
    // Act.
    sha_256_initialize(&context);
    
    // Assert.
    TEST_ASSERT_EQUAL_UINT64(0, context.total_bit_count);
}

/**
 * @brief Test registers are initialized to correct H constants.
 */
void test_sha256_initial_register_values(void)
{
    // Arrange.
    SHA256 context = {0};

    // Act.
    sha_256_initialize(&context);

    // Assert.
    TEST_ASSERT_EQUAL_HEX32(0x6a09e667, context.registers[0]);
    TEST_ASSERT_EQUAL_HEX32(0x5be0cd19, context.registers[7]);
}

/**
 * @brief Test defensive check for NULL context in initialize.
 */
void test_sha256_null_initialize_guard(void)
{
    // Arrange.
    SHA256* context = NULL;

    // Act & Assert.
    sha_256_initialize(context);
}

/**
 * @brief Test defensive check for NULL data in update.
 */
void test_sha256_null_update_guard(void)
{
    // Arrange.
    SHA256 context = {0};

    sha_256_initialize(&context);

    // Act.
    sha_256_update(&context, NULL, 10);
    
    // Assert.
    TEST_ASSERT_EQUAL_UINT32(0, context.buffer_length);
}

/**
 * @brief Test bit count accumulation for 128 bytes.
 */
void test_sha256_bit_count_accumulation(void)
{
    // Arrange.
    SHA256 context = {0};
    
    uint8_t data[128] = {0};

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, data, 128);
    
    // Assert.
    TEST_ASSERT_EQUAL_UINT64(1024, context.total_bit_count);
}

/**
 * @brief Test zero-length update does not change state.
 */
void test_sha256_zero_length_update(void)
{
    // Arrange.
    SHA256 context = {0};

    sha_256_initialize(&context);

    uint32_t initial_register = context.registers[0];
    
    // Act.
    sha_256_update(&context, (const uint8_t*)"ignored", 0);
    
    // Assert.
    TEST_ASSERT_EQUAL_HEX32(initial_register, context.registers[0]);
}

/**
 * @brief Test that finalize produces a unique hash for different inputs.
 */
void test_sha256_avalanche_simple_check(void)
{
    // Arrange.
    SHA256 context = {0};

    uint8_t digest_a[32];
    uint8_t digest_b[32];

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)"The cat", 7);
    sha_256_finalize(&context, digest_a);

    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)"The car", 7);
    sha_256_finalize(&context, digest_b);

    // Assert.
    TEST_ASSERT_TRUE(memcmp(digest_a, digest_b, 32) != 0);
}

/**
 * @brief Test SHA-256 with 55 bytes (Maximum single-block message).
 * @details 55 bytes + 1 byte (0x80) + 8 bytes (length) = 64 bytes (1 block).
 */
void test_sha256_55_bytes(void)
{
    // Arrange.
    SHA256 context = {0};

    uint8_t digest[32];
    uint8_t data[55];

    memset(data, 'a', 55);

    uint8_t expected[32] = {
        0x9f, 0x43, 0x90, 0xf8, 0xd3, 0x0c, 0x2d, 0xd9,
        0x2e, 0xc9, 0xf0, 0x95, 0xb6, 0x5e, 0x2b, 0x9a,
        0xe9, 0xb0, 0xa9, 0x25, 0xa5, 0x25, 0x8e, 0x24,
        0x1c, 0x9f, 0x1e, 0x91, 0x0f, 0x73, 0x43, 0x18
    };

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, data, 55);
    sha_256_finalize(&context, digest);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, digest, 32);
}

/**
 * @brief Test SHA-256 with 56 bytes (Triggers double-block padding).
 * @details 56 bytes + 1 byte (0x80) exceeds 56-byte boundary, forcing length into next block.
 */
void test_sha256_56_bytes_spill(void)
{
    // Arrange.
    SHA256 context = {0};

    uint8_t digest[32];
    uint8_t data[56];

    memset(data, 'a', 56);

    uint8_t expected[32] = {
        0xb3, 0x54, 0x39, 0xa4, 0xac, 0x6f, 0x09, 0x48,
        0xb6, 0xd6, 0xf9, 0xe3, 0xc6, 0xaf, 0x0f, 0x5f,
        0x59, 0x0c, 0xe2, 0x0f, 0x1b, 0xde, 0x70, 0x90,
        0xef, 0x79, 0x70, 0x68, 0x6e, 0xc6, 0x73, 0x8a
    };

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, data, 56);
    sha_256_finalize(&context, digest);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, digest, 32);
}

/**
 * @brief Test SHA-256 with exactly 64 bytes (Full buffer edge case).
 */
void test_sha256_64_bytes_full_buffer(void)
{
    // Arrange.
    SHA256 context = {0};

    uint8_t digest[32];
    uint8_t data[64];

    memset(data, 'a', 64);

    uint8_t expected[32] = {
        0xff, 0xe0, 0x54, 0xfe, 0x7a, 0xe0, 0xcb, 0x6d,
        0xc6, 0x5c, 0x3a, 0xf9, 0xb6, 0x1d, 0x52, 0x09,
        0xf4, 0x39, 0x85, 0x1d, 0xb4, 0x3d, 0x0b, 0xa5,
        0x99, 0x73, 0x37, 0xdf, 0x15, 0x46, 0x68, 0xeb
    };

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, data, 64);
    sha_256_finalize(&context, digest);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, digest, 32);
}

/**
 * @brief Test SHA-256 with a long multi-block NIST vector.
 */
void test_sha256_long_message(void)
{
    // Arrange.
    SHA256 context = {0};

    uint8_t digest[32];

    const char* input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

    uint8_t expected[32] = {
        0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8, 0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
        0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67, 0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
    };

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)input, strlen(input));
    sha_256_finalize(&context, digest);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, digest, 32);
}

/**
 * @brief Test that a context can be re-initialized and re-used correctly.
 */
void test_sha256_context_reusability(void)
{
    // Arrange.
    SHA256 context = {0};

    uint8_t digest_trash[32];
    uint8_t digest_final[32];
    
    uint8_t expected_abc[32] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };

    // Act.
    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)"random data", 11);
    sha_256_finalize(&context, digest_trash);

    // Re-use.
    sha_256_initialize(&context);
    sha_256_update(&context, (const uint8_t*)"abc", 3);
    sha_256_finalize(&context, digest_final);

    // Assert.
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_abc, digest_final, 32);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_sha256_empty_string);
    RUN_TEST(test_sha256_standard_vector_abc);
    RUN_TEST(test_sha256_streaming_consistency);
    RUN_TEST(test_sha256_initialization_resets_counter);
    RUN_TEST(test_sha256_initial_register_values);
    RUN_TEST(test_sha256_null_initialize_guard);
    RUN_TEST(test_sha256_null_update_guard);
    RUN_TEST(test_sha256_bit_count_accumulation);
    RUN_TEST(test_sha256_zero_length_update);
    RUN_TEST(test_sha256_avalanche_simple_check);
    RUN_TEST(test_sha256_55_bytes);
    RUN_TEST(test_sha256_56_bytes_spill);
    RUN_TEST(test_sha256_64_bytes_full_buffer);
    RUN_TEST(test_sha256_long_message);
    RUN_TEST(test_sha256_context_reusability);

    // Destory the session.
    return UNITY_END();
}