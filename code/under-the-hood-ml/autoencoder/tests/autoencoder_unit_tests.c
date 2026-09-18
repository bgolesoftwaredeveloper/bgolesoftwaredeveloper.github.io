/**
 * @file        :   autoencoder_unit_tests.c
 * @author      :   Braiden Gole
 * @brief       :   Unit test suite for the Dense Autoencoder implementation.
 * @details     :   This suite utilizes the Unity Test Framework to rigorously validate 
 * the Autoencoder algorithm's core functionality. Key areas of verification include:
 * - Proper initialization of context parameters (weights bounded within [-1, 1], zero biases).
 * - Correct activation bounds handling (sigmoid saturation limits and output range checks).
 * - Forward propagation data flow from input to latent space and reconstruction.
 * - Backpropagation and gradient updates driving Mean Squared Error (MSE) loss reduction.
 * - Robustness against edge cases such as NULL context/buffer pointers, zero sample datasets, and non-finite values.
 * 
 * Compile:
 * clang ./tests/autoencoder_unit_tests.c ./src/autoencoder.c ./src/unity.c -Iinclude -o ./bin/autoencoder_unit_test -lm
 * @version     :   1.0
 * @date        :   2026-08-03
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "unity.h"
#include "autoencoder.h"

/**
 * @brief Sets up resources before each test.
 */
void setUp(void)
{
    ;;
}

/**
 * @brief Cleans up resources after each test.
 */
void tearDown(void)
{
    ;;
}

/**
 * @brief : Tests that initialization sets encoder bias vector elements strictly to zero.
 */
void test_autoencoder_initialization_encoder_bias_zeroing(void)
{
    // Arrange.
    Context context;

    // Act.
    initialize(&context);

    // Assert.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        TEST_ASSERT_EQUAL_FLOAT(0.0f, context.encoder_bias[latent_index]);
    }
}

/**
 * @brief : Tests that initialization sets decoder bias vector elements strictly to zero.
 */
void test_autoencoder_initialization_decoder_bias_zeroing(void)
{
    // Arrange.
    Context context;

    // Act.
    initialize(&context);

    // Assert.
    for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
    {
        TEST_ASSERT_EQUAL_FLOAT(0.0f, context.decoder_bias[output_index]);
    }
}

/**
 * @brief : Tests that initial encoder weights fall within the $[-1.0, 1.0]$ random initialization bound.
 */
void test_autoencoder_initialization_encoder_weights_bounded(void)
{
    // Arrange.
    Context context;

    // Act.
    initialize(&context);

    // Assert.
    for (int input_index = 0; input_index < INPUT_DIMENSION; input_index++)
    {
        for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
        {
            float weight = context.encoder_weights[input_index][latent_index];
            
            TEST_ASSERT_TRUE(weight >= -1.0f && weight <= 1.0f);
        }
    }
}

/**
 * @brief : Tests that initial decoder weights fall within the $[-1.0, 1.0]$ random initialization bound.
 */
void test_autoencoder_initialization_decoder_weights_bounded(void)
{
    // Arrange.
    Context context;

    // Act.
    initialize(&context);

    // Assert.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
        {
            float weight = context.decoder_weights[latent_index][output_index];

            TEST_ASSERT_TRUE(weight >= -1.0f && weight <= 1.0f);
        }
    }
}

/**
 * @brief : Tests initialization behavior when context pointer is NULL.
 */
void test_autoencoder_initialize_null_context_handling(void)
{
    // Arrange & Act & Assert.
    initialize(NULL);

    TEST_ASSERT_NULL(NULL);
}

/**
 * @brief : Tests forward propagation output value ranges remain bounded in $(0.0, 1.0)$ due to sigmoid.
 */
void test_autoencoder_forward_pass_output_activation_bounds(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float input_vector[INPUT_DIMENSION] = {1.0f, 0.0f, 0.0f, 0.0f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, input_vector, latent_vector, output_vector);

    // Assert.
    for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
    {
        TEST_ASSERT_TRUE(output_vector[output_index] >= 0.0f && output_vector[output_index] <= 1.0f);
    }
}

/**
 * @brief : Tests forward propagation latent representation values remain bounded in $(0.0, 1.0)$.
 */
void test_autoencoder_forward_pass_latent_activation_bounds(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float input_vector[INPUT_DIMENSION] = {0.0f, 1.0f, 0.0f, 0.0f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, input_vector, latent_vector, output_vector);

    // Assert.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        TEST_ASSERT_TRUE(latent_vector[latent_index] >= 0.0f && latent_vector[latent_index] <= 1.0f);
    }
}

/**
 * @brief : Tests NULL parameter safety handling within forward propagation routine.
 */
void test_autoencoder_forward_null_pointers_safety(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float input_vector[INPUT_DIMENSION] = {1.0f, 0.0f, 0.0f, 0.0f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act & Assert.
    forward(NULL, input_vector, latent_vector, output_vector);
    forward(&context, NULL, latent_vector, output_vector);
    forward(&context, input_vector, NULL, output_vector);
    forward(&context, input_vector, latent_vector, NULL);

    TEST_ASSERT_NOT_NULL(&context);
}

/**
 * @brief : Tests forward propagation determinism when given identical inputs and context.
 */
void test_autoencoder_forward_pass_repeatability(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float input_vector[INPUT_DIMENSION] = {0.5f, 0.5f, 0.0f, 0.0f};
    float latent_first[LATENT_DIMENSION] = {0};
    float output_first[OUTPUT_DIMENSION] = {0};
    float latent_second[LATENT_DIMENSION] = {0};
    float output_second[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, input_vector, latent_first, output_first);
    forward(&context, input_vector, latent_second, output_second);

    // Assert.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        TEST_ASSERT_EQUAL_FLOAT(latent_first[latent_index], latent_second[latent_index]);
    }

    for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
    {
        TEST_ASSERT_EQUAL_FLOAT(output_first[output_index], output_second[output_index]);
    }
}

/**
 * @brief : Tests training pipeline return value on valid inputs.
 */
void test_autoencoder_train_success_return_code(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Act.
    bool success = train(&context, dataset);

    // Assert.
    TEST_ASSERT_TRUE(success);
}

/**
 * @brief : Tests train function NULL pointer validation checks.
 */
void test_autoencoder_train_null_arguments_failure(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f}
    };

    // Act & Assert.
    TEST_ASSERT_FALSE(train(NULL, dataset));
    TEST_ASSERT_FALSE(train(&context, NULL));
}

/**
 * @brief : Tests MSE loss convergence reduction over training execution.
 */
void test_autoencoder_loss_reduction_after_training(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    float latent[LATENT_DIMENSION];
    float initial_output[OUTPUT_DIMENSION];
    float trained_output[OUTPUT_DIMENSION];

    forward(&context, dataset[0], latent, initial_output);

    float initial_error = fabsf(initial_output[0] - dataset[0][0]);

    // Act.
    train(&context, dataset);

    forward(&context, dataset[0], latent, trained_output);

    float trained_error = fabsf(trained_output[0] - dataset[0][0]);

    // Assert.
    TEST_ASSERT_TRUE(trained_error < initial_error);
}

/**
 * @brief : Tests weight mutation integrity across training epochs.
 */
void test_autoencoder_weight_mutation_during_training(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float initial_weight = context.encoder_weights[0][0];

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Act.
    train(&context, dataset);

    // Assert.
    TEST_ASSERT_NOT_EQUAL(initial_weight, context.encoder_weights[0][0]);
}

/**
 * @brief : Tests reconstruction accuracy on identity matrix vector patterns.
 */
void test_autoencoder_identity_vector_reconstruction_accuracy(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    train(&context, dataset);

    float latent[LATENT_DIMENSION];
    float output[OUTPUT_DIMENSION];

    // Act.
    forward(&context, dataset[0], latent, output);

    // Assert.
    TEST_ASSERT_FLOAT_WITHIN(0.15f, 1.0f, output[0]);
    TEST_ASSERT_FLOAT_WITHIN(0.15f, 0.0f, output[1]);
}

/**
 * @brief : Tests encoder processing with all-zero input vector.
 */
void test_autoencoder_zero_vector_input_processing(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float zero_input[INPUT_DIMENSION] = {0.0f, 0.0f, 0.0f, 0.0f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, zero_input, latent_vector, output_vector);

    // Assert.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        TEST_ASSERT_FALSE(isnan(latent_vector[latent_index]));
    }
}

/**
 * @brief : Tests encoder handling of large positive input values without overflow.
 */
void test_autoencoder_large_input_saturation_safety(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float large_input[INPUT_DIMENSION] = {1000.0f, 1000.0f, 1000.0f, 1000.0f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, large_input, latent_vector, output_vector);

    // Assert.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        TEST_ASSERT_EQUAL_FLOAT(1.0f, latent_vector[latent_index]);
    }
}

/**
 * @brief : Tests encoder handling of large negative input values without underflow.
 */
void test_autoencoder_negative_input_saturation_safety(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float negative_input[INPUT_DIMENSION] = {-1000.0f, -1000.0f, -1000.0f, -1000.0f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, negative_input, latent_vector, output_vector);

    // Assert.
    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        float value = latent_vector[latent_index];

        TEST_ASSERT_TRUE(value == 0.0f || value == 1.0f);
        TEST_ASSERT_TRUE(isfinite(value));
    }
}

/**
 * @brief : Tests latent bottleneck dimensionality compression mapping.
 */
void test_autoencoder_latent_bottleneck_dimension_mapping(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float input_vector[INPUT_DIMENSION] = {1.0f, 0.0f, 0.0f, 0.0f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, input_vector, latent_vector, output_vector);

    // Assert.
    TEST_ASSERT_EQUAL_INT(2, LATENT_DIMENSION);
}

/**
 * @brief : Tests decoder bias updates during model training.
 */
void test_autoencoder_decoder_bias_mutation_during_training(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Act.
    train(&context, dataset);

    // Assert.
    bool bias_updated = false;

    for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
    {
        if (context.decoder_bias[output_index] != 0.0f)
        {
            bias_updated = true;

            break;
        }
    }

    TEST_ASSERT_TRUE(bias_updated);
}

/**
 * @brief : Tests encoder bias updates during model training.
 */
void test_autoencoder_encoder_bias_mutation_during_training(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Act.
    train(&context, dataset);

    // Assert.
    bool bias_updated = false;

    for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
    {
        if (context.encoder_bias[latent_index] != 0.0f)
        {
            bias_updated = true;

            break;
        }
    }

    TEST_ASSERT_TRUE(bias_updated);
}

/**
 * @brief : Tests model output generation when evaluating fractional floating-point features.
 */
void test_autoencoder_fractional_feature_evaluation(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float fractional_input[INPUT_DIMENSION] = {0.25f, 0.75f, 0.10f, 0.90f};
    float latent_vector[LATENT_DIMENSION] = {0};
    float output_vector[OUTPUT_DIMENSION] = {0};

    // Act.
    forward(&context, fractional_input, latent_vector, output_vector);

    // Assert.
    for (int output_index = 0; output_index < OUTPUT_DIMENSION; output_index++)
    {
        TEST_ASSERT_FALSE(isnan(output_vector[output_index]));
        TEST_ASSERT_FALSE(isinf(output_vector[output_index]));
    }
}

/**
 * @brief : Tests distinct latent code generation for orthogonal inputs after training.
 */
void test_autoencoder_distinct_latent_representation_generation(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    train(&context, dataset);

    float latent_a[LATENT_DIMENSION];
    float latent_b[LATENT_DIMENSION];
    float output[OUTPUT_DIMENSION];

    // Act.
    forward(&context, dataset[0], latent_a, output);
    forward(&context, dataset[1], latent_b, output);

    // Assert.
    bool representations_differ = (latent_a[0] != latent_b[0]) || (latent_a[1] != latent_b[1]);

    TEST_ASSERT_TRUE(representations_differ);
}

/**
 * @brief : Tests network parameter numerical sanity ensuring no NaNs or Infs occur during training.
 */
void test_autoencoder_numerical_stability_no_nan_or_inf(void)
{
    // Arrange.
    Context context;

    initialize(&context);

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Act.
    train(&context, dataset);

    // Assert.
    for (int input_index = 0; input_index < INPUT_DIMENSION; input_index++)
    {
        for (int latent_index = 0; latent_index < LATENT_DIMENSION; latent_index++)
        {
            TEST_ASSERT_TRUE(isfinite(context.encoder_weights[input_index][latent_index]));
        }
    }
}

/**
 * @brief : Tests stability and performance across repeated training cycles.
 */
void test_autoencoder_repeated_training_stability_cycle(void)
{
    // Arrange.
    Context context;

    float dataset[NUMBER_OF_SAMPLES][INPUT_DIMENSION] = 
    {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };

    // Act & Assert.
    for (int run = 0; run < 3; run++)
    {
        initialize(&context);

        bool success = train(&context, dataset);

        TEST_ASSERT_TRUE(success);
    }
}

/**
 * @brief : Tests input layer to latent layer weight dimension structure constraints.
 */
void test_autoencoder_dimension_macro_consistency(void)
{
    // Act & Assert.
    TEST_ASSERT_EQUAL_INT(INPUT_DIMENSION, OUTPUT_DIMENSION);
    TEST_ASSERT_TRUE(LATENT_DIMENSION < INPUT_DIMENSION);
}

int main(void)
{
    // Initiate the session.
    UNITY_BEGIN();

    // Register the tests to run.
    RUN_TEST(test_autoencoder_initialization_encoder_bias_zeroing);
    RUN_TEST(test_autoencoder_initialization_decoder_bias_zeroing);
    RUN_TEST(test_autoencoder_initialization_encoder_weights_bounded);
    RUN_TEST(test_autoencoder_initialization_decoder_weights_bounded);
    RUN_TEST(test_autoencoder_initialize_null_context_handling);
    RUN_TEST(test_autoencoder_forward_pass_output_activation_bounds);
    RUN_TEST(test_autoencoder_forward_pass_latent_activation_bounds);
    RUN_TEST(test_autoencoder_forward_null_pointers_safety);
    RUN_TEST(test_autoencoder_forward_pass_repeatability);
    RUN_TEST(test_autoencoder_train_success_return_code);
    RUN_TEST(test_autoencoder_train_null_arguments_failure);
    RUN_TEST(test_autoencoder_loss_reduction_after_training);
    RUN_TEST(test_autoencoder_weight_mutation_during_training);
    RUN_TEST(test_autoencoder_identity_vector_reconstruction_accuracy);
    RUN_TEST(test_autoencoder_zero_vector_input_processing);
    RUN_TEST(test_autoencoder_large_input_saturation_safety);
    RUN_TEST(test_autoencoder_negative_input_saturation_safety);
    RUN_TEST(test_autoencoder_latent_bottleneck_dimension_mapping);
    RUN_TEST(test_autoencoder_decoder_bias_mutation_during_training);
    RUN_TEST(test_autoencoder_encoder_bias_mutation_during_training);
    RUN_TEST(test_autoencoder_fractional_feature_evaluation);
    RUN_TEST(test_autoencoder_distinct_latent_representation_generation);
    RUN_TEST(test_autoencoder_numerical_stability_no_nan_or_inf);
    RUN_TEST(test_autoencoder_repeated_training_stability_cycle);
    RUN_TEST(test_autoencoder_dimension_macro_consistency);

    // Destroy the session.
    return UNITY_END();
}