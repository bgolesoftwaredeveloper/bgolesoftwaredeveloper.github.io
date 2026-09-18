/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Driver program to test and demonstrate the autoencoder neural network.
 * @details     :   Initializes a dense autoencoder architecture, trains it on a set of 
 *                  4-dimensional one-hot vectors using backpropagation, and evaluates its 
 *                  ability to compress features into a 2D latent space and reconstruct inputs.
 * Compile:
 * clang -Wall -Wextra -Wpedantic ./src/main.c ./src/autoencoder.c -I./include -o ./bin/autoencoder_test
 * @version     :   1.0
 * @date        :   2026-08-03
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Include header files.
#include "../include/autoencoder.h"

int main(void)
{
    // Seed the psudo-random number generator.
    srand((unsigned int)time(NULL));

    // Define a sample dataset (4-dimensional one-hot encoded vectors).
    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] =
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Instantiate autoencoder context.
    Context context;

    // Print header.
    printf("%s", "====================================\n");
    printf("%s", "Autoencoder training initialization.\n");
    printf("%s", "====================================\n\n");

    // Initalize the autoencoder.
    initialize(&context);

    printf("%s %i %s", "Starting training for", TOTAL_EPOCHS, "epochs...\n\n");

    // Train the model.
    bool success = train(&context, dataset);

    if (!success)
    {
        fprintf(stderr, "Fatal error: Training fialed due to invalid argument or context.\n");

        return 1;
    }

    // Evaluate model outputs against inputs.
    for (int sample_index = 0; sample_index < NUMBER_OF_SAMPLES; sample_index++)
    {
        float latent_vector[LATENT_DIMENSION] = {0};
        float output_vector[OUTPUT_DIMENSION] = {0};

        forward(&context, dataset[sample_index], latent_vector, output_vector);

        printf("%s [%i]%c", "Sample", sample_index + 1, '\n');

        // Display the original input vector..
        printf("%s", "\tInput: [");

        for (int index = 0; index < INPUT_DIMENSION; index++)
        {
            if (index < INPUT_DIMENSION - 1)
            {
                printf("%.1f, ", dataset[sample_index][index]);
            }
            else
            {
                printf("%.1f", dataset[sample_index][index]);
            }
        }

        printf("%s", "]\n");

        // Display compressed latent representation.
        printf("%s", "\tLatent: [");

        for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
        {
            if (latent_index < LATENT_DIMENSION - 1)
            {
                printf("%.3f, ", latent_vector[latent_index]);
            }
            else
            {
                printf("%.3f", latent_vector[latent_index]);
            }
        }

        printf("%s", "]\n");
        
        // Display reconstructed output vector.
        printf("%s", "\tOutput: [");

        for (int k = 0; k < OUTPUT_DIMENSION; k++)
        {
            if (k < OUTPUT_DIMENSION - 1)
            {
                printf("%.3f, ", output_vector[k]);
            }
            else
            {
                printf("%.3f", output_vector[k]);
            }
        }

        printf("%s", "]\n");
    }

    return 0;
}