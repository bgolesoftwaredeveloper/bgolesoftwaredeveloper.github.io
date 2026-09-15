/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   A demonstration of the AES-128 algorithm.
 * @details     :   This program demonstrates the full cryptographic cycle:
 * 1. Initializing a 128-bit key and plaintext block.
 * 2. Expanding the key (via manual schedule setup in this version).
 * 3. Encrypting the plaintext to generate ciphertext.
 * 4. Decrypting the ciphertext to recover the original plaintext.
 * 5. Verifying data integrity via memory comparison.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/cryptography/aes-128/main.c ./src/darwin/cryptography/aes-128/aes_128.c -I./include -o ./bin/aes_128_test
 * @version     :   1.0
 * @date        :   2026-05-02
 */
#include <stdio.h>
#include <string.h>

// Include header files.
#include "darwin/cryptography/aes_128.h"

/**
 * @brief       :   Prints an array of bytes in hexadecimal format.
 * @details     :   Formats the output with a label followed by a continuous 
 *                  string of hex characters, ending with a newline.
 * @param       :   label:  A descriptive string prefix for the data.
 * @param       :   data:   Pointer to the byte array to be printed.
 * @param       :   size:   The number of bytes to print.
 * @return      :   void
 */
void print_hex(const char* label, uint8_t* data, size_t size)
{
    printf("%-12s:", label);

    for (size_t index = 0; index < size; index++)
    {
        printf("%02x", data[index]);
    }

    printf("%c", '\n');
}

int main(void)
{
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t plaintext[16] = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };

    uint8_t schedule[176];
    uint8_t ciphertext[16];
    uint8_t decrypted[16];

    memset(schedule, 0, 176);
    memcpy(schedule, key, 16);

    aes_128_encrypt(plaintext, schedule, ciphertext);

    print_hex("Ciphertext", ciphertext, 16);

    aes_128_decrypt(ciphertext, schedule, decrypted);

    print_hex("Decrypted", decrypted, 16);

    if (memcmp(plaintext, decrypted, 16) == 0) 
    {
        printf("%s", "Verification: Success\n");
    } 
    else 
    {
        printf("%s", "Verification: Failed\n");
    }

    return 0;
}