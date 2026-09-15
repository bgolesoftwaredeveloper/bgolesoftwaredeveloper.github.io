/**
 * @file        :   blowfish.h
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Blowfish symmetric-key block cipher.
 * @details     :   This header defines the Blowfish structure and provides prototypes 
 * for key initialization, encryption, and decryption. Blowfish 
 * operates on 64-bit blocks using a Feistel network with 
 * key-dependent S-boxes to provide high security and performance
 * @date        :   2026-05-06
 */
#ifndef BLOWFISH_H
#define BLOWFISH_H

#include <stdint.h>
#include <stddef.h>

extern const uint32_t initial_permuation_values[18];
extern const uint32_t initial_substitution_values[4][256];

/**
 * @struct Blowfish
 * @brief  Container for the internal state of the Blowfish cipher.
 */
typedef struct 
{
    uint32_t permutation_array[18];
    uint32_t substitution_boxes[4][256];
} Blowfish;


// Function prototypes.
void initialize(Blowfish* context, const uint8_t* secret_key, size_t length);
void blowfish_encrypt_block(const Blowfish* context, uint32_t* left_half, uint32_t* right_half);
void blowfish_decrypt_block(const Blowfish* context, uint32_t* left_half, uint32_t* right_half);

#endif