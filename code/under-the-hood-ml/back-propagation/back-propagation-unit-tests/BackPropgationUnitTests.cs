/// File            :   BackPropagationTests.cs
/// Project         :   BackPropagation
/// Programmer      :   Braiden Gole
/// First version   :   2026-08-01
/// Description     :   This is the Back Propagation unit test harness.

namespace BackPropagationUnitTests;

[TestFixture]
public class BackPropagationUnitTests
{
    /// <summary>
    /// Verifies that constructing a NeuralNetwork with valid dimensions initializes successfully.
    /// </summary>
    [Test]
    public void NeuralNetwork_ValidDimensions_InitializesInstance()
    {
        // Act.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        // Assert.
        Assert.That(network, Is.Not.Null);
    }

    /// <summary>
    /// Verifies that non-positive entry dimension count throws ArgumentOutOfRangeException.
    /// </summary>
    [Test]
    public void Constructor_ZeroOrNegativeEntryDimension_ThrowsArgumentOutOfRangeException()
    {
        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.Throws<ArgumentOutOfRangeException>(() => new NeuralNetwork(0, 3, 1));
            Assert.Throws<ArgumentOutOfRangeException>(() => new NeuralNetwork(-1, 3, 1));
        });
    }

    /// <summary>
    /// Verifies that non-positive intermediate dimension count throws ArgumentOutOfRangeException.
    /// </summary>
    [Test]
    public void Constructor_ZeroOrNegativeIntermediateDimension_ThrowsArgumentOutOfRangeException()
    {
        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.Throws<ArgumentOutOfRangeException>(() => new NeuralNetwork(2, 0, 1));
            Assert.Throws<ArgumentOutOfRangeException>(() => new NeuralNetwork(2, -2, 1));
        });
    }

    /// <summary>
    /// Verifies that non-positive exit dimension count throws ArgumentOutOfRangeException.
    /// </summary>
    [Test]
    public void Constructor_ZeroOrNegativeExitDimension_ThrowsArgumentOutOfRangeException()
    {
        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.Throws<ArgumentOutOfRangeException>(() => new NeuralNetwork(2, 3, 0));
            Assert.Throws<ArgumentOutOfRangeException>(() => new NeuralNetwork(2, 3, -3));
        });
    }

    /// <summary>
    /// Verifies that FeedForward produces an output array matching configured exit dimensions.
    /// </summary>
    [Test]
    public void FeedForward_ValidInputs_ReturnsOutputsMatchingExitDimensionCount()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(3, 4, 2);

        double[] inputs = new double[] { 0.5, 0.1, 0.9 };

        // Act.
        double[] outputs = network.FeedForward(inputs);

        // Assert.
        Assert.That(outputs, Has.Length.EqualTo(2));
    }

    /// <summary>
    /// Confirms that outputs from FeedForward are bounded between 0.0 and 1.0 due to Sigmoid activation.
    /// </summary>
    [Test]
    public void FeedForward_ValidInputs_OutputsAreBoundedBetweenZeroAndOne()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);
        
        double[] inputs = new double[] { 10.0, -10.0 };

        // Act.
        double[] outputs = network.FeedForward(inputs);

        // Assert.
        Assert.That(outputs[0], Is.InRange(0.0, 1.0));
    }

    /// <summary>
    /// Verifies that passing null inputs to FeedForward throws ArgumentNullException.
    /// </summary>
    [Test]
    public void FeedForward_NullInput_ThrowsArgumentNullException()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        // Act & Assert.
        Assert.Throws<ArgumentNullException>(() => network.FeedForward(null!));
    }

    /// <summary>
    /// Verifies that input vector length mismatch during FeedForward throws ArgumentException.
    /// </summary>
    [Test]
    public void FeedForward_MismatchedInputLength_ThrowsArgumentException()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] invalidInputs = new double[] { 0.5, 0.5, 0.5 };

        // Act & Assert.
        Assert.Throws<ArgumentException>(() => network.FeedForward(invalidInputs));
    }

    /// <summary>
    /// Ensures consecutive calls to FeedForward on identical inputs yield deterministic outputs.
    /// </summary>
    [Test]
    public void FeedForward_ConsecutiveCalls_YieldsDeterministicOutputs()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 0.2, 0.8 };

        // Act.
        double[] result = network.FeedForward(inputs);
        double[] outcome = network.FeedForward(inputs);

        // Assert.
        Assert.That(result[0], Is.EqualTo(outcome[0]));
    }

    /// <summary>
    /// Verifies that passing null values to Backpropagate throws ArgumentNullException.
    /// </summary>
    [Test]
    public void Backpropagate_NullInputValues_ThrowsArgumentNullException()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] targets = new double[] { 1.0 };

        // Act & Assert.
        Assert.Throws<ArgumentNullException>(() => network.Backpropagate(null!, targets, 0.1));
    }

    /// <summary>
    /// Verifies that passing null reference target values to Backpropagate throws ArgumentNullException.
    /// </summary>
    [Test]
    public void Backpropagate_NullReferenceValues_ThrowsArgumentNullException()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 0.5, 0.5 };

        // Act & Assert.
        Assert.Throws<ArgumentNullException>(() => network.Backpropagate(inputs, null!, 0.1));
    }

    /// <summary>
    /// Verifies that mismatched target reference vector length during Backpropagate throws ArgumentException.
    /// </summary>
    [Test]
    public void Backpropagate_MismatchedReferenceLength_ThrowsArgumentException()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 0.5, 0.5 };
        double[] invalidTargets = new double[] { 1.0, 0.0 };

        // Act & Assert.
        Assert.Throws<ArgumentException>(() => network.Backpropagate(inputs, invalidTargets, 0.1));
    }

    /// <summary>
    /// Verifies that non-positive step rate (zero or negative) throws ArgumentOutOfRangeException.
    /// </summary>
    [Test]
    public void Backpropagate_ZeroOrNegativeStepRate_ThrowsArgumentOutOfRangeException()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 0.5, 0.5 };
        double[] targets = new double[] { 1.0 };

        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.Throws<ArgumentOutOfRangeException>(() => network.Backpropagate(inputs, targets, 0.0));
            Assert.Throws<ArgumentOutOfRangeException>(() => network.Backpropagate(inputs, targets, -0.5));
        });
    }

    /// <summary>
    /// Verifies that NaN or Infinity step rates throw ArgumentOutOfRangeException.
    /// </summary>
    [Test]
    public void Backpropagate_NonFiniteStepRate_ThrowsArgumentOutOfRangeException()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 0.5, 0.5 };
        double[] targets = new double[] { 1.0 };

        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.Throws<ArgumentOutOfRangeException>(() => network.Backpropagate(inputs, targets, double.NaN));
            Assert.Throws<ArgumentOutOfRangeException>(() => network.Backpropagate(inputs, targets, double.PositiveInfinity));
        });
    }

    /// <summary>
    /// Verifies that executing Backpropagate alters network parameters and output prediction for a given input.
    /// </summary>
    [Test]
    public void Backpropagate_SingleStep_ChangesNetworkPrediction()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 1.0, 0.0 };
        double[] targets = new double[] { 1.0 };

        double initialPrediction = network.FeedForward(inputs)[0];

        // Act.
        network.Backpropagate(inputs, targets, 0.5);

        double updatedPrediction = network.FeedForward(inputs)[0];

        // Assert.
        Assert.That(updatedPrediction, Is.Not.EqualTo(initialPrediction));
    }

    /// <summary>
    /// Verifies that training on a single sample moves prediction output closer to target reference.
    /// </summary>
    [Test]
    public void Backpropagate_SingleSample_ReducesErrorDelta()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 4, 1);

        double[] inputs = new double[] { 0.5, 0.5 };
        double[] targets = new double[] { 0.9 };

        double initialOutput = network.FeedForward(inputs)[0];
        double initialError = Math.Abs(targets[0] - initialOutput);

        // Act.
        network.Backpropagate(inputs, targets, 0.8);

        double newOutput = network.FeedForward(inputs)[0];
        double newError = Math.Abs(targets[0] - newOutput);

        // Assert.
        Assert.That(newError, Is.LessThan(initialError));
    }

    /// <summary>
    /// Tests network convergence on binary OR logic pattern across multiple Backpropagate iterations.
    /// </summary>
    [Test]
    public void Backpropagate_ORPattern_ReducesErrorOverIterations()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[][] trainingInputs = new double[][]
        {
            new double[] { 0, 0 },
            new double[] { 0, 1 },
            new double[] { 1, 0 },
            new double[] { 1, 1 }
        };

        double[][] targets = new double[][]
        {
            new double[] { 0 },
            new double[] { 1 },
            new double[] { 1 },
            new double[] { 1 }
        };

        double CalculateTotalError() => trainingInputs
            .Zip(targets, (input, target) => Math.Abs(target[0] - network.FeedForward(input)[0]))
            .Sum();

        double initialTotalError = CalculateTotalError();

        // Act.
        for (int epoch = 0; epoch < 1000; epoch++)
        {
            for (int index = 0; index < trainingInputs.Length; index++)
            {
                network.Backpropagate(trainingInputs[index], targets[index], 0.5);
            }
        }

        double finalTotalError = CalculateTotalError();

        // Assert.
        Assert.That(finalTotalError, Is.LessThan(initialTotalError));
    }

    /// <summary>
    /// Tests that multi-output network updates all output dimension dimensions during training.
    /// </summary>
    [Test]
    public void Backpropagate_MultiOutputLayer_UpdatesAllExitOutputs()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 2);

        double[] inputs = new double[] { 0.4, 0.6 };
        double[] targets = new double[] { 0.1, 0.9 };

        double[] initialOutputs = (double[])network.FeedForward(inputs).Clone();

        // Act.
        network.Backpropagate(inputs, targets, 0.5);

        double[] updatedOutputs = network.FeedForward(inputs);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(updatedOutputs[0], Is.Not.EqualTo(initialOutputs[0]));
            Assert.That(updatedOutputs[1], Is.Not.EqualTo(initialOutputs[1]));
        });
    }

    /// <summary>
    /// Verifies that larger step rates produce larger initial parameter delta shifts.
    /// </summary>
    [Test]
    public void Backpropagate_HigherStepRate_CausesLargerOutputShift()
    {
        // Arrange.
        NeuralNetwork slowNet = new NeuralNetwork(2, 2, 1);
        NeuralNetwork fastNet = new NeuralNetwork(2, 2, 1);

        double[] inputs = new double[] { 1.0, 1.0 };
        double[] targets = new double[] { 0.0 };

        double initialSlow = slowNet.FeedForward(inputs)[0];
        double initialFast = fastNet.FeedForward(inputs)[0];

        // Act.
        slowNet.Backpropagate(inputs, targets, 0.01);
        fastNet.Backpropagate(inputs, targets, 1.0);

        double slowShift = Math.Abs(slowNet.FeedForward(inputs)[0] - initialSlow);
        double fastShift = Math.Abs(fastNet.FeedForward(inputs)[0] - initialFast);

        // Assert.
        Assert.That(fastShift, Is.GreaterThan(slowShift));
    }

    /// <summary>
    /// Tests FeedForward execution safety with edge-case zero inputs.
    /// </summary>
    [Test]
    public void FeedForward_ZeroInputs_ExecutesSafely()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 2, 1);

        double[] zeroInputs = new double[] { 0.0, 0.0 };

        // Act.
        double[] outputs = network.FeedForward(zeroInputs);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(outputs, Has.Length.EqualTo(1));
            Assert.That(double.IsNaN(outputs[0]), Is.False);
        });
    }

    /// <summary>
    /// Tests FeedForward stability when handling extreme input magnitudes.
    /// </summary>
    [Test]
    public void FeedForward_ExtremeValueInputs_ExecutesWithoutNaNOrInfinity()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 2, 1);

        double[] extremeInputs = new double[] { 1000.0, -1000.0 };

        // Act.
        double[] outputs = network.FeedForward(extremeInputs);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(double.IsNaN(outputs[0]), Is.False);
            Assert.That(double.IsInfinity(outputs[0]), Is.False);
        });
    }

    /// <summary>
    /// Verifies single entry and exit dimension configuration stability.
    /// </summary>
    [Test]
    public void NeuralNetwork_SingleDimensionConfig_ExecutesFeedForwardAndBackpropagate()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(1, 1, 1);

        double[] input = new double[] { 0.5 };
        double[] target = new double[] { 0.2 };

        // Act & Assert.
        Assert.DoesNotThrow(() =>
        {
            network.FeedForward(input);
            network.Backpropagate(input, target, 0.1);
        });
    }

    /// <summary>
    /// Checks performance and stability when evaluating large layer dimensions.
    /// </summary>
    [Test]
    public void NeuralNetwork_LargeDimensionTopology_ExecutesWithoutExceptions()
    {
        // Arrange.
        NeuralNetwork largeNetwork = new NeuralNetwork(50, 100, 10);

        double[] inputs = new double[50];
        double[] targets = new double[10];

        // Act & Assert.
        Assert.DoesNotThrow(() =>
        {
            largeNetwork.FeedForward(inputs);
            largeNetwork.Backpropagate(inputs, targets, 0.1);
        });
    }

    /// <summary>
    /// Confirms that training on target values matching initial predictions results in negligible output change.
    /// </summary>
    [Test]
    public void Backpropagate_TargetMatchesCurrentOutput_MaintainsOutputStability()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 2, 1);

        double[] inputs = new double[] { 0.5, 0.5 };

        double[] currentOutput = network.FeedForward(inputs);
        double[] matchingTarget = new double[] { currentOutput[0] };

        // Act.
        network.Backpropagate(inputs, matchingTarget, 0.1);

        double[] postTrainOutput = network.FeedForward(inputs);

        // Assert.
        Assert.That(postTrainOutput[0], Is.EqualTo(currentOutput[0]).Within(1e-5));
    }

    /// <summary>
    /// Verifies that learning step rate of 1.0 executes valid backpropagation parameter updates.
    /// </summary>
    [Test]
    public void Backpropagate_MaxStepRateOne_ExecutesSuccessfully()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 2, 1);

        double[] inputs = new double[] { 0.3, 0.7 };
        double[] targets = new double[] { 0.8 };

        // Act & Assert.
        Assert.DoesNotThrow(() => network.Backpropagate(inputs, targets, 1.0));
    }

    /// <summary>
    /// Tests that backpropagating negative target values functions without throwing exceptions.
    /// </summary>
    [Test]
    public void Backpropagate_NegativeTargetValues_ExecutesSafely()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 2, 1);

        double[] inputs = new double[] { 0.5, 0.5 };
        double[] negativeTargets = new double[] { -0.5 };

        // Act & Assert.
        Assert.DoesNotThrow(() => network.Backpropagate(inputs, negativeTargets, 0.1));
    }

    /// <summary>
    /// Confirms FeedForward outputs maintain array reference integrity.
    /// </summary>
    [Test]
    public void FeedForward_ReturnsNonEmptyReference()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 0.1, 0.2 };

        // Act.
        double[] outputs = network.FeedForward(inputs);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(outputs, Is.Not.Null);
            Assert.That(outputs, Is.Not.Empty);
        });
    }

    /// <summary>
    /// Verifies stability across repeated Backpropagate invocations on identical inputs.
    /// </summary>
    [Test]
    public void Backpropagate_MultipleConsecutiveCalls_RunsWithoutError()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 3, 1);

        double[] inputs = new double[] { 0.6, 0.4 };
        double[] targets = new double[] { 0.7 };

        // Act & Assert.
        Assert.DoesNotThrow(() =>
        {
            for (int index = 0; index < 50; index++)
            {
                network.Backpropagate(inputs, targets, 0.1);
            }
        });
    }

    /// <summary>
    /// Tests network training convergence on AND logic gates across epochs.
    /// </summary>
    [Test]
    public void Backpropagate_ANDPattern_ReducesOverallError()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 2, 1);

        double[][] trainingInputs = new double[][]
        {
            new double[] { 0, 0 },
            new double[] { 0, 1 },
            new double[] { 1, 0 },
            new double[] { 1, 1 }
        };

        double[][] targets = new double[][]
        {
            new double[] { 0 },
            new double[] { 0 },
            new double[] { 0 },
            new double[] { 1 }
        };

        double GetError() => trainingInputs
            .Zip(targets, (input, target) => Math.Abs(target[0] - network.FeedForward(input)[0]))
            .Sum();

        double initialError = GetError();

        // Act.
        for (int epoch = 0; epoch < 500; epoch++)
        {
            for (int index = 0; index < trainingInputs.Length; index++)
            {
                network.Backpropagate(trainingInputs[index], targets[index], 0.5);
            }
        }

        // Assert.
        Assert.That(GetError(), Is.LessThan(initialError));
    }

    /// <summary>
    /// Verifies that small positive learning steps produce valid small output modifications.
    /// </summary>
    [Test]
    public void Backpropagate_SmallStepRate_ProducesGradualOutputChange()
    {
        // Arrange.
        NeuralNetwork network = new NeuralNetwork(2, 2, 1);

        double[] inputs = new double[] { 0.5, 0.5 };
        double[] targets = new double[] { 1.0 };

        double initialOutput = network.FeedForward(inputs)[0];

        // Act.
        network.Backpropagate(inputs, targets, 0.0001);

        double postOutput = network.FeedForward(inputs)[0];

        double outputDelta = Math.Abs(postOutput - initialOutput);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(outputDelta, Is.GreaterThan(0.0));
            Assert.That(outputDelta, Is.LessThan(0.01));
        });
    }
}