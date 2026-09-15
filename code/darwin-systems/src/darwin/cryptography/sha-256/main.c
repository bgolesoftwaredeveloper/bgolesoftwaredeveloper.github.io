/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   This is the test driver for the SHA-256 implementation.
 * @details     :   This program demonstrates the use of a streaming SHA-256 
 * cryptographic hash. It initializes a context, updates 
 * the state with a password string, and finalizes the 
 * digest for output in hexadecimal format.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/cryptography/sha-256/main.c ./src/darwin/cryptography/sha-256/sha_256.c -I./include -o ./bin/sha_256_test
 * @version     :   1.0
 * @date        :   2026-05-05
 */
#include <stdio.h>
#include <string.h>

// Include header files/
#include "darwin/cryptography/sha_256.h"

// Function prototypes.
void print_sha256_hash(const uint8_t digest[32]);

int main(void)
{
    SHA256 context;
    uint8_t digest[32];

    const char* password = "secret-key";

    printf("%s %s%c", "Input:", password, '\n');

    // Initialize the hashing state.
    sha_256_initialize(&context);

    // Update the state with the password bytes.
    sha_256_update(&context, (const uint8_t*)password, strlen(password));

    // Finalize the hash and generate the 32-byte digest. 
    sha_256_finalize(&context, digest);

    printf("%s", "SHA256: ");

    // Output the resulting hash.
    print_sha256_hash(digest);
    
    return 0;
}

/**
 * @brief Prints the 32-byte digest in hex format.
 */
void print_sha256_hash(const uint8_t digest[32])
{
    if (digest == NULL)
    {
        return;
    }

    for (int byte_index = 0; byte_index < 32; byte_index++)
    {
        printf("%02x", digest[byte_index]);
    }

    printf("%c", '\n');
}