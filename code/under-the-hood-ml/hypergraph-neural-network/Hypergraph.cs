/// File            :   Hypergraph.cs
/// Project         :   HypergraphNeuralNetwork
/// Programmer      :   Braiden Gole
/// First version   :   2026-08-07
/// Description     :   Implementation of a Hypergraph Neural Network (HGNN) 
///                     layer performing parallel feature transformations and 
///                     degree-normalized message passing across hyperedges.

namespace HypergraphNeuralNetwork
{
    public class Hypergraph
    {
        private readonly int _inputDimension;
        private readonly int _outputDimension;
        private double[] _learnableWeights = Array.Empty<double>();

        /// <summary>
        /// Initializes a new instance of the <see cref="Hypergraph"/> class with specified feature dimensions.
        /// </summary>
        /// <param name="inputDimension">The size of input node feature vectors.</param>
        /// <param name="outputDimension">The target size of output node feature vectors.</param>
        /// <exception cref="ArgumentException">Thrown when input or output dimension count is less than or equal to zero.</exception>
        public Hypergraph(int inputDimension, int outputDimension)
        {
            // Verify input dimension is positive.
            if (inputDimension <= 0)
            {
                throw new ArgumentException("Input dimension must be greater than zero.", nameof(inputDimension));
            }

            // Verify output dimension is positive,
            if (outputDimension <= 0)
            {
                throw new ArgumentException("Output dimension must be greater than zero.", nameof(outputDimension));
            }

            _inputDimension = inputDimension;
            _outputDimension = outputDimension;

            // Initialize weights.
            InitializeWeights();
        }

        /// <summary>
        /// Initializes learnable weight parameters using Xavier/Glorot uniform initialization distribution.
        /// </summary>
        private void InitializeWeights()
        {   
            _learnableWeights = new double[_inputDimension * _outputDimension];

            double distributionLimit = Math.Sqrt(6.0 / (_inputDimension + _outputDimension));

            for (int index = 0; index < _learnableWeights.Length; index++)
            {
                _learnableWeights[index] = (Random.Shared.NextDouble() * 2.0 - 1.0) * distributionLimit;
            }
        }

        /// <summary>
        /// Calculates inverse degree scaling factors for each node and hyperedge in the hypergraph structure.
        /// </summary>
        /// <param name="matrix">Flattened binary or weighted incidence matrix connecting nodes to hyperedges.</param>
        /// <param name="nodeCount">Total number of nodes represented in the hypergraph.</param>
        /// <param name="edgeCount">Total number of hyperedges represented in the hypergraph.</param>
        /// <returns>A tuple containing node inverse degree factors and hyperedge inverse degree factors.</returns>
        private (double[] InverseNodeDegrees, double[] InverseEdgeDegrees) CalculateInverseDegrees(int[] matrix, int nodeCount, int edgeCount)
        {   
            double[] nodeDegrees = new double[nodeCount];
            double[] edgeDegrees = new double[edgeCount];

            for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
            {
                int nodeRowOffset = nodeIndex * edgeCount;

                for (int edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
                {
                    int connectionWeight = matrix[nodeRowOffset + edgeIndex];

                    // Guard against invalid negative connection weights incidence matrix.
                    if (connectionWeight < 0)
                    {
                        throw new ArgumentException($"Negative incidence weight encountered at node {nodeIndex}.", nameof(matrix));
                    }

                    nodeDegrees[nodeIndex] += connectionWeight;
                    edgeDegrees[edgeIndex] += connectionWeight;
                }
            }

            double[] inverseNodeDegrees = new double[nodeCount];

            for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
            {
                inverseNodeDegrees[nodeIndex] = nodeDegrees[nodeIndex] > 0.0 ? 1.0 / nodeDegrees[nodeIndex] : 0.0;
            }

            double[] inverseEdgeDegrees = new double[edgeCount];

            for (int edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
            {
                inverseEdgeDegrees[edgeIndex] = edgeDegrees[edgeIndex] > 0.0 ? 1.0 / edgeDegrees[edgeIndex] : 0.0;
            }

            return (inverseNodeDegrees, inverseEdgeDegrees);
        }

        /// <summary>
        /// Multiplies node features by learnable layer weights in parallel across nodes.
        /// </summary>
        /// <param name="features">Flattened array of node input features.</param>
        /// <param name="nodeCount">Total number of nodes to process.</param>
        /// <returns>Flattened array of linear transformed node features.</returns>
        private double[] TransformFeaturesParallel(double[] features, int nodeCount)
        {
            double[] outputFeatures = new double[nodeCount * _outputDimension];

            Parallel.For(0, nodeCount, nodeIndex =>
            {
                int inputRowOffset = nodeIndex * _inputDimension;
                int outputRowOffset = nodeIndex * _outputDimension;

                for (int inputFeatureIndex = 0; inputFeatureIndex < _inputDimension; inputFeatureIndex++)
                {
                    double featureValue = features[inputRowOffset + inputFeatureIndex];
                    int weightRowOffset = inputFeatureIndex * _outputDimension;

                    for (int outputFeatureIndex = 0; outputFeatureIndex < _outputDimension; outputFeatureIndex++)
                    {
                        outputFeatures[outputRowOffset + outputFeatureIndex] += 
                            featureValue * _learnableWeights[weightRowOffset + outputFeatureIndex];
                    }
                }
            });

            return outputFeatures;
        }

        /// <summary>
        /// Aggregates transformed node features into hyperedge representations in parallel, applying hyperedge degree normalization.
        /// </summary>
        /// <param name="features">Flattened array of transformed node features.</param>
        /// <param name="matrix">Flattened hypergraph incidence matrix.</param>
        /// <param name="inverseEdgeDegrees">Precomputed inverse hyperedge degree scaling factors.</param>
        /// <param name="nodeCount">Total number of nodes in the graph.</param>
        /// <param name="edgeCount">Total number of hyperedges in the graph.</param>
        /// <returns>Flattened array containing hyperedge feature vectors.</returns>
        private double[] AggregateNodesToHyperedgesParallel(double[] features, int[] matrix, 
            double[] inverseEdgeDegrees, int nodeCount, int edgeCount)
        {
            double[] edgeFeatures = new double[edgeCount * _outputDimension];

            Parallel.For(0, edgeCount, edgeIndex =>
            {
                double scaleFactor = inverseEdgeDegrees[edgeIndex];
                
                if (scaleFactor == 0.0)
                {
                    return;
                }

                int edgeRowOffset = edgeIndex * _outputDimension;

                for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
                {
                    if (matrix[nodeIndex * edgeCount + edgeIndex] > 0)
                    {
                        int nodeRowOffset = nodeIndex * _outputDimension;

                        for (int featureIndex = 0; featureIndex < _outputDimension; featureIndex++)
                        {
                            edgeFeatures[edgeRowOffset + featureIndex] += features[nodeRowOffset + featureIndex] * scaleFactor;
                        }
                    }
                }
            });

            return edgeFeatures;
        }

        /// <summary>
        /// Aggregates hyperedge representations back into node feature vectors in parallel, applying node degree normalization.
        /// </summary>
        /// <param name="features">Flattened array of aggregated hyperedge features.</param>
        /// <param name="matrix">Flattened hypergraph incidence matrix.</param>
        /// <param name="inverseNodeDegrees">Precomputed inverse node degree scaling factors.</param>
        /// <param name="nodeCount">Total number of nodes in the graph.</param>
        /// <param name="edgeCount">Total number of hyperedges in the graph.</param>
        /// <returns>Flattened array containing final transformed node feature vectors.</returns>
        private double[] AggregateHyperedgesToNodesParallel(double[] features, int[] matrix, 
            double[] inverseNodeDegrees, int nodeCount, int edgeCount)
        {
            double[] outputFeatures = new double[nodeCount * _outputDimension];

            Parallel.For(0, nodeCount, nodeIndex =>
            {
                double scaleFactor = inverseNodeDegrees[nodeIndex];

                if (scaleFactor == 0.0)
                {
                    return;
                }

                int nodeOutputOffset = nodeIndex * _outputDimension;
                int nodeIncidenceOffset = nodeIndex * edgeCount;

                for (int edgeIndex = 0; edgeIndex < edgeCount; edgeIndex++)
                {
                    if (matrix[nodeIncidenceOffset + edgeIndex] > 0)
                    {
                        int edgeRowOffset = edgeIndex * _outputDimension;

                        for (int featureIndex = 0; featureIndex < _outputDimension; featureIndex++)
                        {
                            outputFeatures[nodeOutputOffset + featureIndex] += features[edgeRowOffset + featureIndex] * scaleFactor;
                        }
                    }
                }
            });

            return outputFeatures;
        }

        /// <summary>
        /// Executes a full forward message passing evaluation step through the hypergraph convolutional layer.
        /// </summary>
        /// <param name="features">Flattened matrix of node input features of shape (nodeCount * inputDimension).</param>
        /// <param name="matrix">Flattened incidence matrix of shape (nodeCount * edgeCount).</param>
        /// <param name="nodeCount">Total number of node instances.</param>
        /// <param name="edgeCount">Total number of hyperedge instances.</param>
        /// <returns>Flattened array containing transformed output features of shape (nodeCount * outputDimension).</returns>
        /// <exception cref="ArgumentNullException">Thrown when features array or incidence matrix is null.</exception>
        /// <exception cref="ArgumentException">Thrown when array lengths do not match specified node or edge dimensions.</exception>
        public double[] Forward(double[] features, int[] matrix, int nodeCount, int edgeCount)
        {
            // Defensive null check validation.
            ArgumentNullException.ThrowIfNull(features);
            ArgumentNullException.ThrowIfNull(matrix);

            // Defensive dimension check validation.
            if (nodeCount < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(nodeCount), "Node count cannot be negative.");
            }

            if (edgeCount < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(edgeCount), "Edge count cannot be negative.");
            }

            // Defensive array length validation.
            if (features.Length != nodeCount * _inputDimension)
            {
                throw new ArgumentException("Node features array dimension mismatch.", nameof(features));
            }

            if (matrix.Length != nodeCount * edgeCount)
            {
                throw new ArgumentException("Incidence matrix size mismatch.", nameof(matrix));
            }

            // Calculate inverse degree factors.
            var (inverseNodeDegrees, inverseEdgeDegrees) = CalculateInverseDegrees(matrix, nodeCount, edgeCount);

            // Linearly transform node input features using learning weight matrix.
            double[] transformedFeatures = TransformFeaturesParallel(features, nodeCount);

            // Aggregate node feature into hyperedge representations.
            double[] edgeFeatures = AggregateNodesToHyperedgesParallel(transformedFeatures, matrix, 
                inverseEdgeDegrees, nodeCount, edgeCount);

            // Aggregate hyperedge representations back into final output node feature vectors.
            double[] outputFeatures = AggregateHyperedgesToNodesParallel(edgeFeatures, matrix, 
                inverseNodeDegrees, nodeCount, edgeCount);

            return outputFeatures;
        }
    }
}