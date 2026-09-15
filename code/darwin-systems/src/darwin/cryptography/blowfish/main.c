/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Test driver for the Blowfish symmetric-key block cipher.
 * @details     :   This program demonstrates the initialization of a Blowfish 
 * context using a secret key, the encryption of a 64-bit 
 * data block (two 32-bit words), and the subsequent 
 * decryption to verify data integrity.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/cryptography/blowfish/main.c ./src/darwin/cryptography/blowfish/blowfish.c -I./include -o ./bin/blowfish_test
 * @version     :   1.0
 * @date        :   2026-05-06
 */
#include <stdio.h>

// Include header files.
#include "darwin/cryptography/blowfish.h"

int main(void)
{
    // Blowfish context structure to hold the P-array and S-boxes.
    Blowfish context;

    // Define the secret key and calculate its length (exluding NULL terminator).
    const uint8_t secret[] = "BlowfishContext";
    size_t size = sizeof(secret) - 1;

    // The 64-bit block of data to be encrypted, split into two 32-bit halves.
    uint32_t left_word_stream = 0xDEADBEEF;
    uint32_t right_word_stream = 0xCAFEBABE;

    printf("%s", "-- BLOWFISH DEMONSTRATION --\n");
    printf("%s %08X %08X%c", "Original data:", left_word_stream, right_word_stream, '\n');

    // Expand the secret key into the subkeys used by the Feistel Network.
    initialize(&context, secret, size);

    // Encrypt the data block in-place.
    blowfish_encrypt_block(&context, &left_word_stream, &right_word_stream);

    // Store encrypted values for printing and. subsequent decryption.
    uint32_t encrypted_left = left_word_stream;
    uint32_t encrypted_right = right_word_stream;

    printf("%s %08X %08X%c", "Encrypted data:", encrypted_left, encrypted_right, '\n');

    // Decrypt the previously encrypted block back to its original state.
    blowfish_decrypt_block(&context, &encrypted_left, &encrypted_right);

    printf("%s %08X %08X%c", "Decrypted data:", encrypted_left, encrypted_right, '\n');

    printf("%c", '\n');

    // Final verification: Compare the decrypted result with the original input.
    if (encrypted_left == 0xDEADBEEF && encrypted_right == 0xCAFEBABE)
    {
        printf("%s", "SUCCESS!\n");
    }
    else
    {
        printf("%s", "FAILURE!\n");
    }

    return 0;
}