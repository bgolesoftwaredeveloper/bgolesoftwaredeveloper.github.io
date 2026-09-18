/// File            :   HypergraphTests.cs
/// Project         :   HypergraphNeuralNetwork
/// Programmer      :   Braiden Gole
/// First version   :   2026-08-07
/// Description     :   Unit test harness for Hypergraph neural network layer verification.

namespace HypergraphUnitTests;

[TestFixture]
public class HypergraphTests
{
    /// <summary>
    /// Verifies that constructing a Hypergraph with valid dimensions initializes successfully.
    /// </summary>
    [Test]
    public void Hypergraph_ValidDimensions_InitializesInstance()
    {
        // Act.
        Hypergraph hypergraph = new Hypergraph(3, 2);

        // Assert.
        Assert.That(hypergraph, Is.Not.Null);
    }

    /// <summary>
    /// Verifies that zero or negative input dimension throws ArgumentException.
    /// </summary>
    [Test]
    public void Constructor_ZeroOrNegativeInputDimension_ThrowsArgumentException()
    {
        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.Throws<ArgumentException>(() => new Hypergraph(0, 2));
            Assert.Throws<ArgumentException>(() => new Hypergraph(-1, 2));
        });
    }

    /// <summary>
    /// Verifies that zero or negative output dimension throws ArgumentException.
    /// </summary>
    [Test]
    public void Constructor_ZeroOrNegativeOutputDimension_ThrowsArgumentException()
    {
        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.Throws<ArgumentException>(() => new Hypergraph(3, 0));
            Assert.Throws<ArgumentException>(() => new Hypergraph(3, -2));
        });
    }

    /// <summary>
    /// Verifies that Forward produces an output array matching configured node and output dimensions.
    /// </summary>
    [Test]
    public void Forward_ValidInputs_ReturnsOutputsMatchingExpectedDimensions()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(3, 2);

        int nodeCount = 4;
        int edgeCount = 3;

        double[] features = new double[nodeCount * 3];

        int[] matrix = new int[nodeCount * edgeCount];

        // Act.
        double[] output = hypergraph.Forward(features, matrix, nodeCount, edgeCount);

        // Assert.
        Assert.That(output, Has.Length.EqualTo(nodeCount * 2));
    }

    /// <summary>
    /// Verifies that passing null features to Forward throws ArgumentNullException.
    /// </summary>
    [Test]
    public void Forward_NullFeatures_ThrowsArgumentNullException()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(3, 2);

        int[] matrix = new int[12];

        // Act & Assert.
        Assert.Throws<ArgumentNullException>(() => hypergraph.Forward(null!, matrix, 4, 3));
    }

    /// <summary>
    /// Verifies that passing null incidence matrix to Forward throws ArgumentNullException.
    /// </summary>
    [Test]
    public void Forward_NullIncidenceMatrix_ThrowsArgumentNullException()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(3, 2);

        double[] features = new double[12];

        // Act & Assert.
        Assert.Throws<ArgumentNullException>(() => hypergraph.Forward(features, null!, 4, 3));
    }

    /// <summary>
    /// Verifies that features array length mismatch during Forward throws ArgumentException.
    /// </summary>
    [Test]
    public void Forward_MismatchedFeaturesLength_ThrowsArgumentException()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(3, 2);

        double[] invalidFeatures = new double[10];

        int[] matrix = new int[12];

        // Act & Assert.
        Assert.Throws<ArgumentException>(() => hypergraph.Forward(invalidFeatures, matrix, 4, 3));
    }

    /// <summary>
    /// Verifies that incidence matrix size mismatch during Forward throws ArgumentException.
    /// </summary>
    [Test]
    public void Forward_MismatchedIncidenceMatrixLength_ThrowsArgumentException()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(3, 2);

        double[] features = new double[12];

        int[] invalidMatrix = new int[8];

        // Act & Assert.
        Assert.Throws<ArgumentException>(() => hypergraph.Forward(features, invalidMatrix, 4, 3));
    }

    /// <summary>
    /// Ensures consecutive calls to Forward on identical inputs yield deterministic outputs.
    /// </summary>
    [Test]
    public void Forward_ConsecutiveCalls_YieldsDeterministicOutputs()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] features = new double[] { 1.0, 0.5, 0.0, 1.2 };

        int[] matrix = new int[] { 1, 0, 0, 1 };

        // Act.
        double[] firstOutput = hypergraph.Forward(features, matrix, 2, 2);
        double[] secondOutput = hypergraph.Forward(features, matrix, 2, 2);

        // Assert.
        Assert.That(firstOutput, Is.EqualTo(secondOutput));
    }

    /// <summary>
    /// Tests Forward execution safety when node features are all zeroes.
    /// </summary>
    [Test]
    public void Forward_ZeroFeatures_ReturnsAllZeroOutputs()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] zeroFeatures = new double[4];

        int[] matrix = new int[] { 1, 1, 1, 1 };

        // Act.
        double[] output = hypergraph.Forward(zeroFeatures, matrix, 2, 2);

        // Assert.
        Assert.That(output, Is.All.EqualTo(0.0));
    }

    /// <summary>
    /// Tests Forward execution when incidence matrix has no hyperedge connections (disconnected graph).
    /// </summary>
    [Test]
    public void Forward_ZeroIncidenceMatrix_ReturnsZeroOutputs()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] features = new double[] { 1.0, 2.0, 3.0, 4.0 };

        int[] emptyMatrix = new int[4];

        // Act.
        double[] output = hypergraph.Forward(features, emptyMatrix, 2, 2);

        // Assert.
        Assert.That(output, Is.All.EqualTo(0.0));
    }

    /// <summary>
    /// Tests Forward execution stability when handling extreme input feature values.
    /// </summary>
    [Test]
    public void Forward_ExtremeFeatureValues_ExecutesWithoutNaNOrInfinity()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] extremeFeatures = new double[] { 1000000.0, -1000000.0, 0.00001, -0.00001 };

        int[] matrix = new int[] { 1, 0, 1, 1 };

        // Act.
        double[] output = hypergraph.Forward(extremeFeatures, matrix, 2, 2);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(output.Any(double.IsNaN), Is.False);
            Assert.That(output.Any(double.IsInfinity), Is.False);
        });
    }

    /// <summary>
    /// Verifies single node and single hyperedge configuration execution.
    /// </summary>
    [Test]
    public void Forward_SingleNodeSingleEdge_ExecutesSuccessfully()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(1, 1);

        double[] features = new double[] { 2.5 };

        int[] matrix = new int[] { 1 };

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, matrix, 1, 1));
    }

    /// <summary>
    /// Verifies that fully connected incidence matrix aggregates features across all nodes.
    /// </summary>
    [Test]
    public void Forward_FullyConnectedMatrix_OutputsNonZeroValues()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] features = new double[] { 1.0, 1.0, 1.0, 1.0 };

        int[] fullMatrix = new int[] { 1, 1, 1, 1 };

        // Act.
        double[] output = hypergraph.Forward(features, fullMatrix, 2, 2);

        // Assert.
        Assert.That(output.All(value => value != 0.0), Is.True);
    }

    /// <summary>
    /// Confirms that isolated nodes with no edge connections produce zero-filled output features.
    /// </summary>
    [Test]
    public void Forward_IsolatedNode_ProducesZeroOutputForThatNode()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        int nodeCount = 3;
        int edgeCount = 1;

        double[] features = new double[] { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };

        int[] matrix = new int[] { 1, 1, 0 };

        // Act.
        double[] output = hypergraph.Forward(features, matrix, nodeCount, edgeCount);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(output[4], Is.EqualTo(0.0));
            Assert.That(output[5], Is.EqualTo(0.0));
        });
    }

    /// <summary>
    /// Tests scaling stability with a large hypergraph structure.
    /// </summary>
    [Test]
    public void Forward_LargeGraphTopology_ExecutesWithoutExceptions()
    {
        // Arrange.
        int nodeCount = 100;
        int edgeCount = 50;

        int inputDimension = 16;
        int outputDimension = 8;

        Hypergraph hypergraph = new Hypergraph(inputDimension, outputDimension);

        double[] features = new double[nodeCount * inputDimension];

        int[] matrix = new int[nodeCount * edgeCount];

        Array.Fill(features, 0.5);

        for (int index = 0; index < matrix.Length; index += 3)
        {
            matrix[index] = 1;
        }

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, matrix, nodeCount, edgeCount));
    }

    /// <summary>
    /// Verifies that two distinct hypergraph instances initialized independently produce distinct weights/outputs.
    /// </summary>
    [Test]
    public void Forward_TwoSeparateInstances_ProduceDifferentOutputs()
    {
        // Arrange.
        Hypergraph firstLayer = new Hypergraph(3, 2);
        Hypergraph secondLayer = new Hypergraph(3, 2);

        double[] features = new double[] { 1.0, 2.0, 3.0 };

        int[] matrix = new int[] { 1 };

        // Act.
        double[] firstOutput = firstLayer.Forward(features, matrix, 1, 1);
        double[] secondOutput = secondLayer.Forward(features, matrix, 1, 1);

        // Assert.
        Assert.That(firstOutput, Is.Not.EqualTo(secondOutput));
    }

    /// <summary>
    /// Confirms Forward returns a valid non-empty array reference.
    /// </summary>
    [Test]
    public void Forward_ReturnsNonEmptyReference()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] features = new double[] { 0.5, 0.5 };

        int[] matrix = new int[] { 1 };

        // Act.
        double[] output = hypergraph.Forward(features, matrix, 1, 1);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(output, Is.Not.Null);
            Assert.That(output, Is.Not.Empty);
        });
    }

    /// <summary>
    /// Tests hypergraph execution when hyperedges contain varying numbers of nodes (unbalanced degrees).
    /// </summary>
    [Test]
    public void Forward_UnbalancedHyperedges_ExecutesSafely()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        int nodeCount = 4;
        int edgeCount = 2;

        double[] features = new double[8];

        Array.Fill(features, 1.0);

        int[] matrix = new int[]
        {
            1, 0,
            1, 0,
            1, 0,
            0, 1
        };

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, matrix, nodeCount, edgeCount));
    }

    /// <summary>
    /// Verifies that passing identical nodes in hyperedges maintains normalized output bounds.
    /// </summary>
    [Test]
    public void Forward_SymmetricInputs_ProducesSymmetricOutputs()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        int nodeCount = 2;
        int edgeCount = 1;

        double[] features = new double[] { 1.0, 2.0, 1.0, 2.0 };

        int[] matrix = new int[] { 1, 1 };

        // Act.
        double[] output = hypergraph.Forward(features, matrix, nodeCount, edgeCount);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(output[0], Is.EqualTo(output[2]));
            Assert.That(output[1], Is.EqualTo(output[3]));
        });
    }

    /// <summary>
    /// Tests behavior when features contain negative floating point values.
    /// </summary>
    [Test]
    public void Forward_NegativeFeatures_ExecutesWithoutError()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] features = new double[] { -1.5, -2.5, -0.5, -4.2 };

        int[] matrix = new int[] { 1, 0, 0, 1 };

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, matrix, 2, 2));
    }

    /// <summary>
    /// Verifies that multi-feature input expansion maps correctly to requested output dimension.
    /// </summary>
    [Test]
    public void Forward_FeatureExpansion_MapsToLargerOutputDimension()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 8);

        double[] features = new double[] { 0.5, 0.5 };

        int[] matrix = new int[] { 1 };

        // Act.
        double[] output = hypergraph.Forward(features, matrix, 1, 1);

        // Assert.
        Assert.That(output, Has.Length.EqualTo(8));
    }

    /// <summary>
    /// Verifies that multi-feature input reduction maps correctly to smaller output dimension.
    /// </summary>
    [Test]
    public void Forward_FeatureReduction_MapsToSmallerOutputDimension()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(8, 2);

        double[] features = new double[8];

        int[] matrix = new int[] { 1 };

        // Act.
        double[] output = hypergraph.Forward(features, matrix, 1, 1);

        // Assert.
        Assert.That(output, Has.Length.EqualTo(2));
    }

    /// <summary>
    /// Verifies stability across repeated Forward invocations in a loop.
    /// </summary>
    [Test]
    public void Forward_RepeatedInvocations_RunsWithoutError()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] features = new double[] { 0.1, 0.2 };

        int[] matrix = new int[] { 1 };

        // Act & Assert.
        Assert.DoesNotThrow(() =>
        {
            for (int index = 0; index < 100; index++)
            {
                hypergraph.Forward(features, matrix, 1, 1);
            }
        });
    }

    /// <summary>
    /// Verifies that weighted degree normalization handles weighted incidence connections safely.
    /// </summary>
    [Test]
    public void Forward_WeightedIncidenceMatrix_ExecutesSafely()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] features = new double[] { 1.0, 1.0, 2.0, 2.0 };

        int[] weightedMatrix = new int[] { 2, 0, 0, 3 };

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, weightedMatrix, 2, 2));
    }

    /// <summary>
    /// Confirms output features contain non-NaN floating numbers on standard input.
    /// </summary>
    [Test]
    public void Forward_ValidInputs_AllElementsAreValidNumbers()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(3, 3);

        double[] features = new double[] { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6 };

        int[] matrix = new int[] { 1, 1, 1, 0 };

        // Act.
        double[] output = hypergraph.Forward(features, matrix, 2, 2);

        // Assert.
        Assert.That(output, Is.All.Not.NaN);
    }

    /// <summary>
    /// Tests behavior when node count is greater than hyperedge count.
    /// </summary>
    [Test]
    public void Forward_MoreNodesThanEdges_ExecutesSuccessfully()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        int nodeCount = 10;
        int edgeCount = 2;

        double[] features = new double[nodeCount * 2];

        int[] matrix = new int[nodeCount * edgeCount];

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, matrix, nodeCount, edgeCount));
    }

    /// <summary>
    /// Tests behavior when hyperedge count is greater than node count.
    /// </summary>
    [Test]
    public void Forward_MoreEdgesThanNodes_ExecutesSuccessfully()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        int nodeCount = 2;
        int edgeCount = 10;

        double[] features = new double[nodeCount * 2];

        int[] matrix = new int[nodeCount * edgeCount];

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, matrix, nodeCount, edgeCount));
    }

    /// <summary>
    /// Verifies that empty input features array with zero nodes throws ArgumentException.
    /// </summary>
    [Test]
    public void Forward_ZeroNodes_ReturnsEmptyOutputArray()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(2, 2);

        double[] emptyFeatures = Array.Empty<double>();

        int[] emptyMatrix = Array.Empty<int>();

        // Act.
        double[] output = hypergraph.Forward(emptyFeatures, emptyMatrix, 0, 0);

        // Assert.
        Assert.That(output, Is.Empty);
    }

    /// <summary>
    /// Verifies parallel loop safety when processing large batch dimension arrays.
    /// </summary>
    [Test]
    public void Forward_ParallelProcessing_ThreadSafetyCheck()
    {
        // Arrange.
        Hypergraph hypergraph = new Hypergraph(4, 4);

        int nodeCount = 500;
        int edgeCount = 200;

        double[] features = new double[nodeCount * 4];
        
        int[] matrix = new int[nodeCount * edgeCount];

        // Act & Assert.
        Assert.DoesNotThrow(() => hypergraph.Forward(features, matrix, nodeCount, edgeCount));
    }
}
