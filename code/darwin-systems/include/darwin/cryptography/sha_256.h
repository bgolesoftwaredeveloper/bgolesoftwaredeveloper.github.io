/**
 * @file        :   sha_256.h
 * @author      :   Braiden Gole
 * @brief       :   Header file for the SHA-256 cryptographic hash implementation.
 * @details     :   This module defines the structures, macros, and function prototypes 
 * required for the SHA-256 (Secure Hash Algorithm 256) as 
 * specified in FIPS PUB 180-4. 
 * 
 * The implementation utilizes a Merkle-Damgård construction 
 * supporting message blocks of 512 bits. It provides bitwise 
 * manipulation macros for the internal Sigma functions, 
 * logical functions (Choose and Majority), and circular 
 * right rotations necessary for the 64-round compression process.
 * @date        :   2026-05-05
 */
#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stddef.h>

#define ROTATE_RIGHT(value, shift) (((value) >> (shift)) | ((value) << (32 - (shift))))

#define SIGMA_UPPERCASE_0(word) (ROTATE_RIGHT(word, 2)  ^ ROTATE_RIGHT(word, 13) ^ ROTATE_RIGHT(word, 22))
#define SIGMA_UPPERCASE_1(word) (ROTATE_RIGHT(word, 6)  ^ ROTATE_RIGHT(word, 11) ^ ROTATE_RIGHT(word, 25))
#define SIGMA_LOWERCASE_0(word) (ROTATE_RIGHT(word, 7)  ^ ROTATE_RIGHT(word, 18) ^ ((word) >> 3))
#define SIGMA_LOWERCASE_1(word) (ROTATE_RIGHT(word, 17) ^ ROTATE_RIGHT(word, 19) ^ ((word) >> 10))

#define CHOOSE(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJORITY(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

typedef struct
{
    uint8_t buffer[64];
    uint32_t buffer_length;
    uint64_t total_bit_count;
    uint32_t registers[8];
} SHA256;

// Function prototypes.
void sha_256_initialize(SHA256* context);
void sha_256_update(SHA256* context, const uint8_t* data, size_t size);
void sha_256_finalize(SHA256* context, uint8_t digest[32]);

#endif