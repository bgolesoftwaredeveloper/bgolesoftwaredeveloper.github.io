/**
 * @file        :   autoencoder.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a Dense Autoencoder neural network.
 * @details     :   This module implements a dense multi-layer autoencoder 
 *                  architecture in Standard C. Features include:
 *                  - Sigmoid non-linear activation with numerical bounds 
 *                    clamping to prevent floating-point overflow.
 *                  - Forward propagation routines mapping inputs to compressed 
 *                    latent representations and decoded outputs.
 *                  - Full backpropagation training loop using Mean Squared Error 
 *                    (MSE) loss and gradient descent optimization.
 *                  - Defensive validation guards against NULL pointers, invalid 
 *                    buffer sizes, and non-finite calculation steps.
 * @version     :   1.0
 * @date        :   2026-08-03
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include header files.
#include "../include/autoencoder.h"

/**
 * @brief       :   Computes the Sigmoid activation function.
 * @details     :   Maps real-valued inputs into the range $(0, 1)$. Includes 
 *                  numerical saturation bounds at $\pm 45.0$ to prevent 
 *                  floating-point overflow in `expf()` and validates result 
 *                  finiteness using `isfinite()`. Marked inline for efficiency.
 * @param       :   value: Input scalar activation sum.
 * @return      :   float: Standard sigmoid response value between 0.0 and 1.0.
 */
static inline float sigmoid(float value)
{
    if (value > 45.0f)
    {
        return 1.0f;
    }

    if (value < -45.0)
    {
        return 0.0f;
    }

    float result = 1.0f / (1.0f + expf(-value));

    return isfinite(result) ? result : 0.0f;
}

/**
 * @brief       :   Computes derivative of the Sigmoid activation function.
 * @details     :   Calculates the derivative $\sigma'(x) = \sigma(x)(1 - \sigma(x))$ 
 *                  directly using the post-activation output scalar value.
 * @param       :   activated_value: The output value after applying sigmoid.
 * @return      :   float: Computed gradient multiplier factor.
 */
static inline float sigmoid_derivative(float activated_value)
{
    return activated_value * (1.0f - activated_value);
}

/**
 * @brief       :   Generates a pseudo-random initial weight value.
 * @details     :   Generates a floating-point scalar value uniformly distributed 
 *                  across the range $[-1.0, 1.0]$ for model parameter setup.
 * @return      :   float: Uniform random floating-point parameter value.
 */
static inline float random_weight(void)
{
    return ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
}

/**
 * @brief       :   Initializes autoencoder network weights and biases.
 * @details     :   Fills encoder and decoder weight matrices with uniform 
 *                  random values between $[-1.0, 1.0]$ and sets all layer 
 *                  bias terms to zero.
 * @param       :   context: Pointer to the Autoencoder Context controller.
 * @return      :   void
 */
void initialize(Context* context)
{
    // Guard against NULL pointer dereference.
    if (context == NULL)
    {
        return;
    }

   // Initialize encoder weights.
    for (int input_index = 0; input_index < INPUT_DIMENSION; input_index++)
    {
        for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
        {
            context->encoder_weights[input_index][latent_index] = random_weight();
        }
    }

    // Initialize encoder biases.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        context->encoder_bias[latent_index] = 0.0f;
    }

    // Initialize decoder weights.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
        {
            context->decoder_weights[latent_index][output_index] = random_weight();
        }
    }

    // Initialize decoder biases.
    for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
    {
        context->decoder_bias[output_index] = 0.0f;
    }
}

/**
 * @brief       :   Executes forward propagation pass across network layers.
 * @details     :   Transforms an input vector into a compressed latent space 
 *                  representation via the encoder, then maps the latent state 
 *                  back into a reconstructed feature output via the decoder.
 * @param       :   context: Pointer to trained model network parameters.
 * @param       :   input_vector: Source feature array of size INPUT_DIMENSION.
 * @param       :   latent_vector: Buffer to populate with latent features.
 * @param       :   output_vector: Buffer to populate with reconstructed output.
 * @return      :   void
 */
void forward(Context* context, 
    const float input_vector[INPUT_DIMENSION], 
    float latent_vector[LATENT_DIMENSION], 
    float output_vector[OUTPUT_DIMENSION])
{
    // Validate input buffer pointers.
    if (context == NULL || input_vector == NULL || latent_vector == NULL || output_vector == NULL)
    {
        return;
    }
    
    // Encoder forward pass: Input Layer -> Latent Layer.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        float weighted_sum = context->encoder_bias[latent_index];

        for (int input_index = 0; input_index < INPUT_DIMENSION; input_index++)
        {
            weighted_sum += input_vector[input_index] * context->encoder_weights[input_index][latent_index];
        }

        latent_vector[latent_index] = sigmoid(weighted_sum);
    }

    // Decoder forward pass: Latent Layer -> Output Layer.
    for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
    {
        float weighted_sum = context->decoder_bias[output_index];

        for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
        {
            weighted_sum += latent_vector[latent_index] * context->decoder_weights[latent_index][output_index];
        }

        output_vector[output_index] = sigmoid(weighted_sum);
    }
}

/**
 * @brief       :   Trains autoencoder parameters using backpropagation.
 * @details     :   Performs iterative stochastic gradient descent over a specified 
 *                  number of training epochs. Calculates output and hidden layer 
 *                  error gradients with respect to Mean Squared Error (MSE) loss 
 *                  and updates model weights and biases according to LEARNING_RATE.
 * @param       :   context: Pointer to initialized model parameter context.
 * @param       :   dataset: Array containing training feature vectors.
 * @return      :   bool: true if training finished successfully; false if error occurs.
 */
bool train(Context* context, float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION])
{
    // Validate dataset and parameter pointers.
    if (context == NULL || dataset == NULL)
    {
        return false;
    }

    // Validate sample count bounds.
    if (NUMBER_OF_SAMPLES <= 0)
    {
        return false;
    }

    float latent_vector[LATENT_DIMENSION];
    float output_vector[OUTPUT_DIMENSION];

    // Main training iteration loop.
    for (int epoch = 0; epoch <= TOTAL_EPOCHS; epoch++)
    {
        float total_loss = 0.0f;

        // Process each training sample in the dataset.
        for (int sample_index = 0; sample_index < NUMBER_OF_SAMPLES; sample_index++)
        {
            float* current = dataset[sample_index];

            // 1. Perform forward pass.
            forward(context, current, latent_vector, output_vector);

            // Accumulate mean squared error (MSE) loss.
            for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
            {
                float error = output_vector[output_index] - current[output_index];

                total_loss += 0.5f * error * error;
            }

            // 2. Calculate decoder layer gradients.
            float output_gradients[OUTPUT_DIMENSION];

            for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
            {
                float error = output_vector[output_index] - current[output_index];

                output_gradients[output_index] = error * sigmoid_derivative(output_vector[output_index]);
            }

            // 3. Backpropagate errors to encoder (Latent layer gradients).
            float latent_gradients[LATENT_DIMENSION];

            for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
            {
                float accumulated_error = 0.0f;

                for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
                {
                    accumulated_error += output_gradients[output_index] * context->decoder_weights[latent_index][output_index];
                }

                latent_gradients[latent_index] = accumulated_error * sigmoid_derivative(latent_vector[latent_index]);
            }

            // Update decoder weights via gradient descent.
            for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
            {
                for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
                {
                    context->decoder_weights[latent_index][output_index] -= LEARNING_RATE * output_gradients[output_index] * latent_vector[latent_index];
                }
            }

            // Update decoder biases.
            for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
            {
                context->decoder_bias[output_index] -= LEARNING_RATE * output_gradients[output_index];
            }

            // 5. Update encoder weights via gradient descent.
            for (int input_index = 0; input_index < INPUT_DIMENSION; input_index++)
            {
                for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
                {
                    context->encoder_weights[input_index][latent_index] -= LEARNING_RATE * latent_gradients[latent_index] * current[input_index];
                }
            }

            // Update encoder biases.
            for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
            {
                context->encoder_bias[latent_index] -= LEARNING_RATE * latent_gradients[latent_index];
            }
        }

        // Output training progress report periodically.
        if (epoch % 4000 == 0)
        {
            printf("Epoch %5d | MSE Loss: %.6f\n", epoch, total_loss / NUMBER_OF_SAMPLES);
        }
    }

    return true;
}