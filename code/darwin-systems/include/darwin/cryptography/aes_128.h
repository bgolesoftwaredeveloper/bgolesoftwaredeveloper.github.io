/**
 * @file        :   aes_128.h
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Advanced Encryption Standard (AES) with a 128-bit key.
 * @details     :   This file holds the function prototypes to be called in main or test program.
 * @version     :   1.0
 * @date        :   2026-05-02
 */
#ifndef AES_128_H
#define AES_128_H

#include <stdint.h>

// Function prototypes.
void add_round_key(uint8_t state[16], uint8_t round_key[16]);
void sub_bytes(uint8_t state[16]);
void inverse_sub_bytes(uint8_t state[16]);
void shift_rows(uint8_t state[16]);
void inverse_shift_rows(uint8_t state[16]);
void mix_columns(uint8_t state[16]);
void inverse_mix_columns(uint8_t state[16]);

uint8_t galois_field_multiplication(uint8_t multiplicand, uint8_t multiplier);

void aes_128_encrypt(uint8_t input[16], uint8_t schedule[176], uint8_t output[16]);
void aes_128_decrypt(uint8_t ciphertext[16], uint8_t schedule[176], uint8_t plaintext[16]);

#endif