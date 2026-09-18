/// File            :   NeuralNetwork.cs
/// Project         :   BackPropagation
/// Programmer      :   Braiden Gole
/// First version   :   2026-08-01
/// Description     :   Implementation of a multi-layer feedforward neural network trained via Backpropagation.
using System;

namespace BackPropagation
{
    public class NeuralNetwork
    {
        private readonly int _entryDimensionCount;
        private readonly int _intermediateDimensionCount;
        private readonly int _exitDimensionCount;

        private double[,] _matrix;
        private double[] _biasVector;

        private double[,] _transformationMatrix;
        private double[] _transformationBiasVector;

        private double[] _intermediateTotals;
        private double[] _intermediateOutputs;
        private double[] _exitTotals;
        private double[] _exitOutputs;

        private readonly Random _generator = new Random();

        /// <summary>
        /// Initializes a new instance of the <see cref="NeuralNetwork"/> class with layer dimension configurations.
        /// </summary>
        /// <param name="entryDimensionCount">The number of input layer dimensions.</param>
        /// <param name="intermediateDimensionCount">The number of hidden layer dimensions.</param>
        /// <param name="exitDimensionCount">The number of output layer dimensions.</param>
        /// <exception cref="ArgumentOutOfRangeException">Thrown when any dimension count is less than or equal to zero.</exception>
        public NeuralNetwork(int entryDimensionCount, int intermediateDimensionCount, int exitDimensionCount)
        {
            // Validate parameters.
            if (entryDimensionCount <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(entryDimensionCount), "Entry dimension count must be greater than zero.");    
            }

            if (intermediateDimensionCount <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(intermediateDimensionCount), "Intermediate dimension count must be greate than zero.");
            }

            if (exitDimensionCount <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(exitDimensionCount), "Exit dimension count must be greateer than zero.");
            }

            this._entryDimensionCount = entryDimensionCount;
            this._intermediateDimensionCount = intermediateDimensionCount;
            this._exitDimensionCount = exitDimensionCount;

            // Initialize weight matrix and bias vector for entry-to-intermediate layer.
            this._matrix = InitializeParameters(entryDimensionCount, intermediateDimensionCount);
            this._biasVector = new double[intermediateDimensionCount];

            // Initialize weight matrix and bias vector for intermdiate-to-exit layer.
            this._transformationMatrix = InitializeParameters(intermediateDimensionCount, exitDimensionCount);
            this._transformationBiasVector = new double[exitDimensionCount];

            // Allocate internal activation state arrays.
            this._intermediateTotals = new double[intermediateDimensionCount];
            this._intermediateOutputs = new double[intermediateDimensionCount];

            this._exitTotals = new double[exitDimensionCount];
            this._exitOutputs = new double[exitDimensionCount];
        }

        /// <summary>
        /// Initializes a parameter matrix with randomized double values scaled between -1.0 and 1.0.
        /// </summary>
        /// <param name="sourceDimension">The number of source dimensions (rows).</param>
        /// <param name="targetDimension">The number of target dimensions (columns).</param>
        /// <returns>A initialized 2D matrix populated with random weight values.</returns>
        /// <exception cref="ArgumentOutOfRangeException">Thrown when source or target dimension is less than or equal to zero.</exception>
        private double[,] InitializeParameters(int sourceDimension, int targetDimension)
        {
            if (sourceDimension <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(sourceDimension), "Source dimension must be greater than zero.");
            }

            if (targetDimension <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(targetDimension), "Target dimension must be greater than zero.");
            }

            double[,] parameterGrid = new double[sourceDimension, targetDimension];

            for (int sourcePosition = 0; sourcePosition < sourceDimension; sourcePosition++)
            {
                for (int targetPosition = 0; targetPosition < targetDimension; targetPosition++)
                {   
                    // Scale random float values from [0.0, 1.0] to [-1.0, 1.0].
                    parameterGrid[sourcePosition, targetPosition] = _generator.NextDouble() * 2.0 - 1.0;
                }
            }

            return parameterGrid;
        }

        /// <summary>
        /// Computes the Sigmoid activation mapping function.
        /// </summary>
        /// <param name="value">The raw input total to map.</param>
        /// <returns>The activated output mapped between 0.0 and 1.0.</returns>
        private double Sigmoid(double value) => 1.0 / (1.0 + Math.Exp(-value));

        /// <summary>
        /// Computes the derivative of the Sigmoid activation function given an activated value.
        /// </summary>
        /// <param name="value">The post-activation value.</param>
        /// <returns>The computed derivative gradient.</returns>
        private double SigmoidDerivative(double value) => value * (1.0 - value);

        /// <summary>
        /// Performs a forward evaluation pass through the network layers given an input sample vector.
        /// </summary>
        /// <param name="values">The array of input values to process.</param>
        /// <returns>The network output predictions vector.</returns>
        /// <exception cref="ArgumentNullException">Thrown when values array is null.</exception>
        /// <exception cref="ArgumentException">Thrown when input vector length does not match entry dimension count.</exception>
        public double[] FeedForward(double[] values)
        {
            if (values == null)
            {
                throw new ArgumentNullException(nameof(values), "Input values cannot be null.");
            }

            if (values.Length != _entryDimensionCount)
            {
                throw new ArgumentException($"Input vector length ({values.Length}) must match entry dimension count ({_entryDimensionCount}).", nameof(values));
            }

            _intermediateTotals = new double[_intermediateDimensionCount];
            _intermediateOutputs = new double[_intermediateDimensionCount];

            // Compute activations for the intermediate (hidden) layer.
            for (int intermediatePosition = 0; intermediatePosition < _intermediateDimensionCount; intermediatePosition++)
            {
                double sum = _biasVector[intermediatePosition];

                for (int entryPosition = 0; entryPosition < _entryDimensionCount; entryPosition++)
                {
                    sum += values[entryPosition] * _matrix[entryPosition, intermediatePosition];
                }

                _intermediateTotals[intermediatePosition] = sum;
                _intermediateOutputs[intermediatePosition] = Sigmoid(sum);
            }

            _exitTotals = new double[_exitDimensionCount];
            _exitOutputs = new double[_exitDimensionCount];

            // Compute activations for the exit (output) layer.
            for (int exitPosition = 0; exitPosition < _exitDimensionCount; exitPosition++)
            {
                double sum = _transformationBiasVector[exitPosition];

                for (int intermediatePosition = 0; intermediatePosition < _intermediateDimensionCount; intermediatePosition++)
                {
                    sum += _intermediateOutputs[intermediatePosition] * _transformationMatrix[intermediatePosition, exitPosition];
                }

                _exitTotals[exitPosition] = sum;
                _exitOutputs[exitPosition] = Sigmoid(sum);
            }

            return _exitOutputs;
        }

        /// <summary>
        /// Executes backpropagation training by computing error gradients and updating parameters.
        /// </summary>
        /// <param name="values">The input feature vector.</param>
        /// <param name="referenceValues">The target reference output vector.</param>
        /// <param name="step">The learning rate factor applied to parameter gradient updates.</param>
        /// <exception cref="ArgumentNullException">Thrown when referenceValues array is null.</exception>
        /// <exception cref="ArgumentException">Thrown when target vector length does not match exit dimension count.</exception>
        /// <exception cref="ArgumentOutOfRangeException">Thrown when learning step rate is invalid or non-positive.</exception>
        public void Backpropagate(double[] values, double[] referenceValues, double step)
        {
            if (referenceValues == null)
            {
                throw new ArgumentNullException(nameof(referenceValues), "Reference target values array cannot be null.");    
            }

            if (referenceValues.Length != _exitDimensionCount)
            {
                throw new ArgumentException($"Reference target vector length ({referenceValues.Length}) must match exit dimension count ({_exitDimensionCount}).", nameof(referenceValues));    
            }

            if (step <= 0.0 || double.IsNaN(step) || double.IsInfinity(step))
            {
                throw new ArgumentOutOfRangeException("Learning step rate must be a positive finite number.");
            }

            // Compute initial predictions via forward evaluation pass.
            FeedForward(values);

            // Compute gradient deltas for the exit (output) layer.
            double[] exitGradients = new double[_exitDimensionCount];

            for (int exitPosition = 0; exitPosition < _exitDimensionCount; exitPosition++)
            {
                double residualDifference = referenceValues[exitPosition] - _exitOutputs[exitPosition];

                exitGradients[exitPosition] = residualDifference * SigmoidDerivative(_exitOutputs[exitPosition]);
            }

            // Compute gradient deltas propagated backward to intermediate (hidden) layer.
            double[] intermediateGradients = new double[_intermediateDimensionCount];

            for (int intermediatePosition = 0; intermediatePosition < _intermediateDimensionCount; intermediatePosition++)
            {
                double accumulatedGradient = 0.0;

                for (int exitPosition = 0; exitPosition < _exitDimensionCount; exitPosition++)
                {
                    accumulatedGradient += exitGradients[exitPosition] * _transformationMatrix[intermediatePosition, exitPosition];
                }

                intermediateGradients[intermediatePosition] = accumulatedGradient * SigmoidDerivative(_intermediateOutputs[intermediatePosition]);
            }

            // Apply gradient updates to intermediate-to-exit transformation weights.
            for (int intermediatePosition = 0; intermediatePosition < _intermediateDimensionCount; intermediatePosition++)
            {
                for (int exitPosition = 0; exitPosition < _exitDimensionCount; exitPosition++)
                {
                    _transformationMatrix[intermediatePosition, exitPosition] += step * exitGradients[exitPosition] * _intermediateOutputs[intermediatePosition];
                }
            }

            // Apply gradient updates to exit layer bias offsets.
            for (int exitPosition = 0; exitPosition < _exitDimensionCount; exitPosition++)
            {
                _transformationBiasVector[exitPosition] += step * exitGradients[exitPosition];
            }

            // Apply gradient updates to entry-to-intermediate primary weights.
            for (int entryPosition = 0; entryPosition < _entryDimensionCount; entryPosition++)
            {
                for (int intermediatePosition = 0; intermediatePosition < _intermediateDimensionCount; intermediatePosition++)
                {
                    _matrix[entryPosition, intermediatePosition] += step * intermediateGradients[intermediatePosition] * values[entryPosition];
                }
            }

            // Apply gradient updates to intermediate layer bias offsets.
            for (int intermediatePosition = 0; intermediatePosition < _intermediateDimensionCount; intermediatePosition++)
            {
                _biasVector[intermediatePosition] += step * intermediateGradients[intermediatePosition];
            }
        }
    }
}