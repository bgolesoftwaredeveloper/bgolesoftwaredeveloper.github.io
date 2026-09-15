/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration and testing suite for the dynamic Dictionary library.
 * @details     :   This program serves as a practical example of how to utilize the 
 * Dictionary API. It demonstrates:
 * - Memory-safe initialization of the data structure.
 * - Generic storage of stack-allocated variables (int, double, char*).
 * - Type-casting retrieved void pointers back to original types.
 * - Handling key updates to prevent duplicate entries.
 * - Removal of entries and subsequent verification.
 * - Complete resource cleanup to prevent memory leaks.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/darwin/dictionary/main.c ./src/darwin/dictionary/dictionary.c -I./include -o ./bin/dictionary_test
 * @version     :   1.0
 * @date        :   2026-05-02
 */
#include <stdio.h>

// Include header files.
#include "darwin/dictionary/dictionary.h"

int main(void)
{
    // Initialize the dictionary.
    Dictionary* dictionary = initialize();

    if (dictionary == NULL)
    {
        fprintf(stderr, "Error out of memory!.\n");

        return 1;
    }

    printf("%s", "-- Dictionary --\n");

    // Define various data types to demonstrate the felxibility of void pointers.
    int id = 1024;
    double score = 95.5;
    char* status = "PASSED";

    // Map keys to the memory addresses of our local variables.
    insert(dictionary, "id", &id);
    insert(dictionary, "score", &score);
    insert(dictionary, "status", status);

    // Retrieve values from the dictionary using their unique string keys.
    int* retrieved_id = (int*)retrieve(dictionary, "id");
    double* retrieved_score = (double*)retrieve(dictionary, "score");
    char* retrieved_status = (char*)retrieve(dictionary, "status");

    // Verify and print the retrieved ID if it exists.
    if (retrieved_id != NULL)
    {
        printf("%c%s %i%c", '\t', "ID:", *(int*)retrieved_id, '\n');
    }

    // Verify and print the retrieved score if it exists.
    if (retrieved_score != NULL)
    {
        printf("%c%s %.2lf%c", '\t', "Score:", *(double*)retrieved_score, '\n');
    }

    // Verify and print the retrieved status string if it exists.
    if (retrieved_status != NULL)
    {
        printf("%c%s %s%c", '\t', "Status:", (char*)retrieved_status, '\n');
    }

    // Demonstrate the dictionary's ability to update a value for an existing key.
    int new_id = 2048;

    insert(dictionary, "id", &new_id);

    // Retrieve and dereference the updated integer value.
    printf("%s %i%c", "Updated ID to:", *(int*)retrieve(dictionary, "id"), '\n');

    printf("%s", "Removing 'status'...\n");

    // Remove the status field.
    erase(dictionary, "status");

    // Confirm it was removed.
    if (retrieve(dictionary, "status") == NULL)
    {
        printf("%s", "Status successfully removed.\n");
    }

    // Uninitialize the dictionary.
    uninitialize(dictionary);

    return 0;
}