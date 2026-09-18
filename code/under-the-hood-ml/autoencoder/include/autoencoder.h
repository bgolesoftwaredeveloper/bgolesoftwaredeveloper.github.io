/**
 * @file        :   autoencoder.h
 * @author      :   Braiden Gole
 * @brief       :   Header definitions and context interface for a Dense Autoencoder.
 * @details     :   This header defines the primary architecture hyperparameters, structural 
 *                  containers, and public API function signatures for the dense autoencoder. 
 *                  It provides configuration macros for dimension sizes, sample counts, learning 
 *                  rates, and the Context struct tracking encoder/decoder parameters.
 * @version     :   1.0
 * @date        :   2026-08-03
 */
#ifndef AUTOENCODER_H
#define AUTOENCODER_H

#include <stdbool.h>

#define INPUT_DIMENSION 4
#define LATENT_DIMENSION 2
#define OUTPUT_DIMENSION 4

#define NUMBER_OF_SAMPLES 4
#define TOTAL_EPOCHS 20000
#define LEARNING_RATE 0.2

/**
 * @struct      :   Context
 * @brief       :   Primary operational context holding network weights and biases.
 * @details     :   Stores layer parameters for both the encoding (compression) and decoding 
 *                  (reconstruction) networks, including input-to-latent weight matrices, 
 *                  latent-to-output weight matrices, and their respective bias vectors.
 */
typedef struct 
{
    float encoder_weights[INPUT_DIMENSION][LATENT_DIMENSION];
    float decoder_weights[LATENT_DIMENSION][OUTPUT_DIMENSION];

    float encoder_bias[LATENT_DIMENSION];
    float decoder_bias[OUTPUT_DIMENSION];
} Context;

// Function prototypes.
void initialize(Context* context);

void forward(Context* context, 
    const float input_vector[INPUT_DIMENSION], 
    float latent_vector[LATENT_DIMENSION], 
    float output_vector[OUTPUT_DIMENSION]);

bool train(Context* context, float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION]);

#endif