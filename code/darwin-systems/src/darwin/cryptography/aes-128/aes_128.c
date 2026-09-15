/**
 * @file        :   aes_128.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Advanced Encryption Standard (AES) with a 128-bit key.
 * @details     :   This module provides a standalone implementation of the AES-128 encryption 
 *                  and decryption algorithms as specified in FIPS PUB 197. 
 *                  
 *                  The implementation follows the Substitution-Permutation Network (SPN) 
 *                  design, utilizing a 128-bit block size and a 128-bit key length 
 *                  processed through 10 rounds of transformation. Key features include:
 *                  - Non-linear substitution using a pre-computed S-Box.
 *                  - Diffusion through row shifting and Galois Field column mixing.
 *                  - Galois Field arithmetic implemented via the Peasant's Algorithm 
 *                    over GF(2^8) with the irreducible polynomial 0x1b.
 *                  - Support for both encryption and decryption using the Equivalent 
 *                    Inverse Cipher structure.
 * 
 *                  Note: This implementation expects a pre-expanded 176-byte key schedule 
 *                  as input for the top-level encryption and decryption functions.
 * @version     :   1.0
 * @date        :   2026-05-02
 */
#include <stdio.h>
#include <string.h>

// Include header files.
#include "darwin/cryptography/aes_128.h"

/**
 * @brief       :   Forward Substitution Box (S-Box).
 * @details     :   A non-linear lookup table used in the SubBytes step. It is 
 * derived from the multiplicative inverse over GF(2^8) 
 * combined with an affine transformation to provide 
 * confusion by breaking linear relationships.
 */
static const uint8_t s_box[256] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/**
 * @brief       :   Inverse Substitution Box (RS-Box).
 * @details     :   The reverse lookup table used in the InverseSubBytes step. 
 * It undoes the non-linear substitution performed by the 
 * forward S-Box during decryption.
 */
static const uint8_t inverse_s_box[256] =
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

/**
 * @brief       :   Combines the current state with a portion of the key schedule.
 * @details     :   Performs a bitwise XOR operation between each byte of the state 
 *                  and the corresponding byte of the round key.
 * @param       :   state:     The current 16-byte internal AES state.
 * @param       :   round_key: The 16-byte key buffer for the current round.
 * @return      :   void
 */
void add_round_key(uint8_t state[16], uint8_t round_key[16])
{
    for (int index = 0; index < 16; index++)
    {
        state[index] ^= round_key[index];
    }
}

/**
 * @brief       :   Performs a non-linear byte substitution using the S-Box.
 * @param       :   state: The current 16-byte internal AES state.
 * @return      :   void
 */
void sub_bytes(uint8_t state[16])
{
    for (int index = 0; index < 16; index++)
    {
        state[index] = s_box[state[index]];
    }
}

/**
 * @brief       :   Reverses the byte substitution using the Inverse S-Box.
 * @param       :   state: The current 16-byte internal AES state.
 * @return      :   void
 */
void inverse_sub_bytes(uint8_t state[16])
{
    for (int index = 0; index < 16; index++)
    {
        state[index] = inverse_s_box[state[index]];
    }
}

/**
 * @brief       :   Shifts the rows of the state matrix by different offsets.
 * @details     :   The first row is unchanged, while subsequent rows are shifted 
 *                  left by 1, 2, and 3 bytes respectively.
 * @param       :   state: The current 16-byte internal AES state.
 * @return      :   void
 */
void shift_rows(uint8_t state[16])
{
    uint8_t temp = state[1];
        state[1] = state[5];
        state[5] = state[9];
        state[9] = state[13];
        state[13] = temp;

    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;

    temp = state[3];
        state[3] = state[15];
        state[15] = state[11];
        state[11] = state[7];
        state[7] = temp;
}

/**
 * @brief       :   Reverses the row shifting transformation.
 * @details     :   Shifts the rows of the state matrix to the right by 1, 2, and 3 
 *                  bytes respectively.
 * @param       :   state: The current 16-byte internal AES state.
 * @return      :   void
 */
void inverse_shift_rows(uint8_t state[16])
{
    uint8_t temp = state[13];
        state[13] = state[9];
        state[9] = state[5];
        state[5] = state[1];
        state[1] = temp;

    temp = state[2]; state[2] = state[10]; state[10] = temp;
    temp = state[6]; state[6] = state[14]; state[14] = temp;

    temp = state[3];
        state[3] = state[7];
        state[7] = state[11];
        state[11] = state[15];
        state[15] = temp;
}

/**
 * @brief       :   Multiplies two bytes in the Galois Field (2^8).
 * @details     :   Uses the peasant's algorithm with the irreducible polynomial 0x1b.
 * @param       :   multiplicand: The first byte.
 * @param       :   multiplier:   The second byte.
 * @return      :   uint8_t:      The product of the multiplication.
 */
uint8_t galois_field_multiplication(uint8_t multiplicand, uint8_t multiplier)
{
    uint8_t product = 0;
    uint8_t current_multiplicand_shift = multiplicand;
    uint8_t multiplier_bits_remaining = multiplier;

    for (int bit_index = 0; bit_index < 8; bit_index++)
    {
        if (multiplier_bits_remaining & 0x01)
        {
            product ^= current_multiplicand_shift;
        }

        uint8_t high_bit_set = current_multiplicand_shift & 0x80;
        
        current_multiplicand_shift <<= 1;

        if (high_bit_set)
        {
            current_multiplicand_shift ^= 0x1b;
        }

        multiplier_bits_remaining >>= 1;
    }

    return product;
}

/**
 * @brief       :   Mixes the columns of the state for diffusion.
 * @details     :   Treats each column as a polynomial over GF(2^8) and multiplies 
 *                  it modulo x^4 + 1 with a fixed polynomial.
 * @param       :   state: The current 16-byte internal AES state.
 * @return      :   void
 */
void mix_columns(uint8_t state[16])
{
    uint8_t temp[16];

    for (int index = 0; index < 4; index++)
    {
        int column = index * 4;

        temp[column] = galois_field_multiplication(0x02, state[column]) ^ 
            galois_field_multiplication(0x03, state[column + 1]) ^ state[column + 2] ^ state[column + 3];

        temp[column + 1] = state[column] ^ galois_field_multiplication(0x02, state[column + 1]) ^ 
            galois_field_multiplication(0x03, state[column + 2]) ^ state[column + 3];

        temp[column + 2] = state[column] ^ state[column + 1] ^
             galois_field_multiplication(0x02, state[column + 2]) ^ galois_field_multiplication(0x03, state[column + 3]);

        temp[column + 3] = galois_field_multiplication(0x03, state[column]) ^ state[column + 1] ^ state[column + 2] ^
            galois_field_multiplication(0x02, state[column + 3]);
    }

    memcpy(state, temp, 16);
}

/**
 * @brief       :   Reverses the MixColumns transformation.
 * @details     :   Multiplies the columns by the inverse fixed polynomial in GF(2^8).
 * @param       :   state: The current 16-byte internal AES state.
 * @return      :   void
 */
void inverse_mix_columns(uint8_t state[16])
{
    uint8_t temp[16];

    for (int index = 0; index < 4; index++)
    {
        int column = index * 4;

        temp[column] = galois_field_multiplication(0x0e, state[column]) ^ 
            galois_field_multiplication(0x0b, state[column+1]) ^ 
            galois_field_multiplication(0x0d, state[column+2]) ^ 
            galois_field_multiplication(0x09, state[column+3]);

        temp[column + 1] = galois_field_multiplication(0x09, state[column]) ^ 
            galois_field_multiplication(0x0e, state[column+1]) ^ 
            galois_field_multiplication(0x0b, state[column+2]) ^ 
            galois_field_multiplication(0x0d, state[column+3]);

        temp[column + 2] = galois_field_multiplication(0x0d, state[column]) ^ 
            galois_field_multiplication(0x09, state[column+1]) ^ 
            galois_field_multiplication(0x0e, state[column+2]) ^ 
            galois_field_multiplication(0x0b, state[column+3]);

        temp[column + 3] = galois_field_multiplication(0x0b, state[column]) ^ 
            galois_field_multiplication(0x0d, state[column+1]) ^ 
            galois_field_multiplication(0x09, state[column+2]) ^ 
            galois_field_multiplication(0x0e, state[column+3]);
    }

    memcpy(state, temp, 16);
}

/**
 * @brief       :   Encrypts a single 128-bit block using AES-128.
 * @details     :   Performs 10 rounds of transformations including SubBytes, 
 *                  shift_rows, mix_columns, and add_round_key.
 * @param       :   input:    16-byte buffer containing the plaintext.
 * @param       :   schedule: 176-byte expanded key schedule.
 * @param       :   output:   16-byte buffer to store the resulting ciphertext.
 * @return      :   void
 */
void aes_128_encrypt(uint8_t input[16], uint8_t schedule[176], uint8_t output[16])
{
    if (input == NULL || schedule == NULL || output == NULL)
    {
        return;
    }

    uint8_t state[16];

    // Initial round key addition.
    memcpy(state, input, 16);

    add_round_key(state, schedule);

    // Main rounds (1 to 9).
    for (int round = 1; round < 10; round++)
    {
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, &schedule[round * 16]);
    }

    // Final round (Round 10) - mix_columns is omitted.
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, &schedule[160]);

    memcpy(output, state, 16);
}

/**
 * @brief       :   Decrypts a single 128-bit block using AES-128.
 * @details     :   Reverses the encryption process using inverse transformations 
 *                  and the key schedule in reverse order.
 * @param       :   ciphertext: 16-byte buffer containing the encrypted data.
 * @param       :   schedule:   176-byte expanded key schedule.
 * @param       :   plaintext:  16-byte buffer to store the decrypted plaintext.
 * @return      :   void
 */
void aes_128_decrypt(uint8_t ciphertext[16], uint8_t schedule[176], uint8_t plaintext[16])
{
    if (ciphertext == NULL || schedule == NULL || plaintext == NULL)
    {
        return;
    }

    uint8_t state[16];

    memcpy(state, ciphertext, 16);

    // Initial round key addition using the last round key.
    add_round_key(state, &schedule[160]);

    // Reverse main rounds.
    for (int round = 9; round >= 1; round--)
    {
        inverse_shift_rows(state);
        inverse_sub_bytes(state);
        
        add_round_key(state, &schedule[round * 16]);

        inverse_mix_columns(state);
    }

    // Final round.
    inverse_shift_rows(state);
    inverse_sub_bytes(state);

    add_round_key(state, schedule);

    memcpy(plaintext, state, 16);
}