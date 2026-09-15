/**
 * @file        :   sha_256.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the SHA-256 cryptographic hash algorithm.
 * @details     :   This module provides a standalone implementation of the SHA-256 
 * algorithm as specified in FIPS PUB 180-4.
 * * The algorithm processes message blocks of 512 bits to produce a 
 * 256-bit message digest. Key features include:
 * 
 * - Merkle-Damgård construction with a streaming API (Initiialize, Update, and Final).
 * - Message schedule expansion from 16 to 64 words.
 * - Compression function utilizing 64 rounds of logical operations, 
 *               bitwise rotations, and modular addition.
 * 
 * - Standard NIST padding including the bit-length append step.
 * @version     :   1.0
 * @date        :   2026-05-05
 */
#include <stdio.h>
#include <string.h>

// Include header files.
#include "darwin/cryptography/sha_256.h"

/**
 * @brief       :   Initial Hash State.
 * @details     :   The initial hash value, H(0), consists of the first 32 bits 
 *                  of the fractional parts of the square roots of the first 
 *                  eight prime numbers.
 */
static const uint32_t initial_hash_state[8] =
{
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

/**
 * @brief       :   Round Entropy Keys (K).
 * @details     :   A sequence of sixty-four constant 32-bit words representing 
 *                  the first 32 bits of the fractional parts of the cube 
 *                  roots of the first sixty-four prime numbers.
 */
static const uint32_t round_entropy_keys[64] = 
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/**
 * @brief       :   Core SHA-256 compression function.
 * @details     :   Processes a 64-byte block by expanding it into a 64-word 
 *                  message schedule and applying 64 rounds of bitwise 
 *                  transformations using working registers alpha through theta.
 * @param       :   context: The current SHA256 context containing state registers.
 * @param       :   block:   The 512-bit (64-byte) message block to transform.
 * @return      :   void
 */
static inline void sha256_transform_block(SHA256* context, const uint8_t* block)
{
    uint32_t message_schedule[64];

    // 1. Message Preprocessing: Map input blocks (bytes) to 32-bit words (Big-Endian).
    for (int index = 0; index < 16; index++)
    {
        message_schedule[index] = ((uint32_t)block[index * 4] << 24) |
            ((uint32_t)block[index * 4 + 1] << 16) |
            ((uint32_t)block[index * 4 + 2] << 8) |
            ((uint32_t)block[index * 4 + 3]);
    }

    // 2. Message Expansion: Expand the 16 initial words into a 64-word schedule.
    for (int index = 16; index < 64; index++)
    {
        message_schedule[index] = SIGMA_LOWERCASE_1(message_schedule[index - 2]) +
            message_schedule[index - 7] +
            SIGMA_LOWERCASE_0(message_schedule[index - 15]) +
            message_schedule[index - 16];
    }

    // 3. Initialize working registers with the current intermediate hash values.
    uint32_t alpha = context->registers[0];
    uint32_t beta = context->registers[1];
    uint32_t gamma = context->registers[2];
    uint32_t delta = context->registers[3];
    uint32_t epsilon = context->registers[4];
    uint32_t zeta = context->registers[5];
    uint32_t eta = context->registers[6];
    uint32_t theta = context->registers[7];

    // 4. Compression Loop: Perform 64 rounds of non-linear mixing.
    for (int round = 0; round < 64; round++)
    {
        uint32_t entropy_accumulator = theta + SIGMA_UPPERCASE_1(epsilon) + 
            CHOOSE(epsilon, zeta, eta) + round_entropy_keys[round] + message_schedule[round];

        uint32_t majority_mix_output = SIGMA_UPPERCASE_0(alpha) + MAJORITY(alpha, beta, gamma);

        // Shift registers.
        theta = eta;
        eta = zeta;
        zeta = epsilon;
        epsilon = delta + entropy_accumulator;
        delta = gamma;
        gamma = beta;
        beta = alpha;
        alpha = entropy_accumulator + majority_mix_output;
    }

    // 5. Update intermediate hash: Add compressed block to previous registers.
    context->registers[0] += alpha;
    context->registers[1] += beta;
    context->registers[2] += gamma;
    context->registers[3] += delta;
    context->registers[4] += epsilon;
    context->registers[5] += zeta;
    context->registers[6] += eta;
    context->registers[7] += theta;
}

/**
 * @brief       :   Initializes the SHA256 context.
 * @details     :   Sets the initial hash state and resets the buffer length 
 *                  and bit counter.
 * @param       :   context: The SHA256 context to initialize.
 * @return      :   void
 */
void sha_256_initialize(SHA256* context)
{
    if (context == NULL)
    {
        return;
    }

    // Reset state and counters to initial standard values.
    context->buffer_length = 0;
    context->total_bit_count = 0;

    memcpy(context->registers, initial_hash_state, 32);
}

/**
 * @brief       :   Appends data to the SHA256 context.
 * @details     :   Buffers data and triggers a transformation block every time 
 *                  the internal 64-byte buffer is filled.
 * @param       :   context: The active SHA256 context.
 * @param       :   data:    Input byte array to hash.
 * @param       :   size:    Size of the input data in bytes.
 * @return      :   void
 */
void sha_256_update(SHA256* context, const uint8_t* data, size_t size)
{
    if (context == NULL || data == NULL || size == 0)
    {
        return;
    }
    
    // Process input data in 64-byte chunks.
    while (size > 0)
    {
        size_t space = 64 - context->buffer_length;
        size_t to_copy = (size < space) ? size : space;

        // Fill internal buffer with available input.
        memcpy(&context->buffer[context->buffer_length], data, to_copy);

        context->buffer_length += (uint32_t)to_copy;

        // Move pointers and update global bit counter.
        data += to_copy;
        size -= to_copy;

        context->total_bit_count += (uint64_t)to_copy * 8;

        // If buffer is full (512 bits), run the compression function.
        if (context->buffer_length == 64)
        {
            sha256_transform_block(context, context->buffer);
            
            context->buffer_length = 0;
        }
    }
}

/**
 * @brief       :   Finalizes the hash and produces the digest.
 * @details     :   Appends the required padding (0x80 followed by zeros) and 
 *                  the 64-bit message length, then extracts the final hash value.
 * @param       :   context: The active SHA256 context.
 * @param       :   digest:  32-byte output buffer for the final hash.
 * @return      :   void
 */
void sha_256_finalize(SHA256* context, uint8_t digest[32])
{
    if (context == NULL || digest == NULL)
    {
        return;
    }

    // 1. Save the bit count before we start messing with the buffer.
    uint64_t total_bits = context->total_bit_count;
    
    // 2. Add the padding bit.
    context->buffer[context->buffer_length++] = 0x80;

    // 3. If no room for 8-byte length field (needs to be at index 56).
    if (context->buffer_length > 56)
    {
        // Fill remainder of the current block with zeros.
        while (context->buffer_length < 64)
        {
            context->buffer[context->buffer_length++] = 0;
        }

        sha256_transform_block(context, context->buffer);

        // Reset for the new, final block.
        context->buffer_length = 0;
        
        memset(context->buffer, 0, 56);
    }

    // 4. Pad with zeros until index 56.
    while (context->buffer_length < 56)
    {
        context->buffer[context->buffer_length++] = 0;
    }

    // 5. Append total bit length as 64-bit big-endian.
    for (int index = 0; index < 8; index++)
    {
        context->buffer[56 + index] = (uint8_t)(total_bits >> (56 - index * 8));
    }

    // 6. Final transformation.
    sha256_transform_block(context, context->buffer);

    // 7. Extract digest.
    for (int index = 0; index < 8; index++)
    {
        digest[index * 4] = (uint8_t)(context->registers[index] >> 24);
        digest[index * 4 + 1] = (uint8_t)(context->registers[index] >> 16);
        digest[index * 4 + 2] = (uint8_t)(context->registers[index] >> 8);
        digest[index * 4 + 3] = (uint8_t)(context->registers[index]);
    }
}
