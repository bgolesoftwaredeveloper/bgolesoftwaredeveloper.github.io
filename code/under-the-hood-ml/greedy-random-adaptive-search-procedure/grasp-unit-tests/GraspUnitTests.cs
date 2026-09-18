/// File            :   GraspTests.cs
/// Project         :   GRASP
/// Programmer      :   Braiden Gole
/// First version   :   2026-07-29
/// Description     :   This is the GRASP unit test harness.

namespace GraspUnitTests;

[TestFixture]
public class GraspUnitTests
{
    /// <summary>
    /// Verifies that Item correctly computes its value-to-weight efficiency ratio.
    /// </summary>
    [Test]
    public void Ratio_ValidInput_CalculatesCorrectEfficiencyRatio()
    {
        // Arrange.
        Item standardItem = new Item(1, 100, 20);

        // Act.
        double calculatedRatio = standardItem.Ratio;

        // Assert.
        Assert.That(calculatedRatio, Is.EqualTo(5.0));
    }

    /// <summary>
    /// Validates that fractional ratios are computed accurately without truncation.
    /// </summary>
    [Test]
    public void Ratio_FractionalResult_CalculatesExactDoubleValue()
    {
        // Arrange.
        Item fractionalItem = new Item(1, 10, 3);

        // Act.
        double calculatedRatio = fractionalItem.Ratio;

        // Assert.
        Assert.That(calculatedRatio, Is.EqualTo(10.0 / 3.0).Within(0.0001));
    }

    /// <summary>
    /// Validates that records with identical properties evaluate as equal.
    /// </summary>
    [Test]
    public void Item_MatchingProperties_EvaluatesAsEqualRecord()
    {
        // Arrange.
        Item primaryItem = new Item(1, 50, 10);
        Item secondaryItem = new Item(1, 50, 10);

        // Act & Assert.
        Assert.That(primaryItem, Is.EqualTo(secondaryItem));
    }

    /// <summary>
    /// Ensures that an empty list of items returns an empty solution without error.
    /// </summary>
    [Test]
    public void Solve_EmptyItemList_ReturnsEmptySolution()
    {
        // Arrange.
        List<Item> emptyPool = new List<Item>();

        Grasp graspSolver = new Grasp(emptyPool, 50);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 0.5);

        // Assert.
        Assert.That(solution, Is.Empty);
    }

    /// <summary>
    /// Tests that a knapsack with zero capacity returns an empty solution.
    /// </summary>
    [Test]
    public void Solve_ZeroCapacity_ReturnsEmptySolution()
    {
        // Arrange.
        List<Item> items = new List<Item> { new Item(1, 10, 5) };

        Grasp zeroCapacitySolver = new Grasp(items, 0);

        // Act.
        List<Item> solution = zeroCapacitySolver.Solve(5, 0.5);

        // Assert.
        Assert.That(solution, Is.Empty);
    }

    /// <summary>
    /// Verifies that no items are selected when every available item exceeds capacity limits.
    /// </summary>
    [Test]
    public void Solve_AllItemsExceedCapacity_ReturnsEmptyList()
    {
        // Arrange.
        List<Item> oversizedItems = new List<Item>
        {
            new Item(1, 100, 50),
            new Item(2, 200, 60)
        };

        Grasp graspSolver = new Grasp(oversizedItems, 30);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 0.5);

        // Assert.
        Assert.That(solution, Is.Empty);
    }

    /// <summary>
    /// Ensures total accumulated solution weight never strictly exceeds target capacity across various alpha values.
    /// </summary>
    [Test]
    public void Solve_VariousAlphaParameters_NeverExceedsCapacity()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 10, 5),
            new Item(2, 20, 10),
            new Item(3, 30, 15),
            new Item(4, 40, 20)
        };

        int capacity = 25;

        Grasp graspSolver = new Grasp(items, capacity);

        double[] testAlphas = { 0.0, 0.25, 0.5, 0.75, 1.0 };

        // Act & Assert.
        Assert.Multiple(() =>
        {
            foreach (double alpha in testAlphas)
            {
                List<Item> solution = graspSolver.Solve(10, alpha);

                int totalWeight = solution.Sum(item => item.Weight);

                Assert.That(totalWeight, Is.LessThanOrEqualTo(capacity));
            }
        });
    }

    /// <summary>
    /// Tests that the solver can completely fill a knapsack when items exactly equal total capacity.
    /// </summary>
    [Test]
    public void Solve_ExactCapacityMatch_FillsKnapsackToCapacity()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 10, 10),
            new Item(2, 20, 20)
        };

        Grasp graspSolver = new Grasp(items, 30);

        // Act.
        List<Item> solution = graspSolver.Solve(1, 1.0);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(solution.Sum(item => item.Weight), Is.EqualTo(30));
            Assert.That(solution, Has.Count.EqualTo(2));
        });
    }

    /// <summary>
    /// Validates that pure greedy configuration (alpha = 1.0) selects highest ratio candidates first.
    /// </summary>
    [Test]
    public void Solve_PureGreedyAlpha_SelectsHighestEfficiencyRatioItem()
    {
        // Arrange.
        Item superiorRatioItem = new Item(1, 100, 10);
        Item inferiorRatioItem = new Item(2, 20, 10);

        List<Item> itemPool = new List<Item> { inferiorRatioItem, superiorRatioItem };

        Grasp pureGreedySolver = new Grasp(itemPool, 10);

        // Act.
        List<Item> solution = pureGreedySolver.Solve(1, 1.0);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(solution, Contains.Item(superiorRatioItem));
            Assert.That(solution, Does.Not.Contain(inferiorRatioItem));
        });
    }

    /// <summary>
    /// Ensures that pure random configuration (alpha = 0.0) yields valid feasible solutions.
    /// </summary>
    [Test]
    public void Solve_PureRandomAlpha_ReturnsFeasibleSolution()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 10, 5),
            new Item(2, 20, 5),
            new Item(3, 30, 5)
        };

        Grasp randomSolver = new Grasp(items, 10);

        // Act.
        List<Item> solution = randomSolver.Solve(10, 0.0);

        // Assert.
        Assert.That(solution.Sum(item => item.Weight), Is.LessThanOrEqualTo(10));
    }

    /// <summary>
    /// Verifies that single candidate fitting within bounds is reliably chosen.
    /// </summary>
    [Test]
    public void Solve_SingleFittingItem_SelectsSingleCandidate()
    {
        // Arrange.
        Item targetItem = new Item(1, 50, 10);

        Grasp graspSolver = new Grasp(new List<Item> { targetItem }, 15);

        // Act.
        List<Item> solution = graspSolver.Solve(5, 0.5);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(solution, Has.Count.EqualTo(1));
            Assert.That(solution[0], Is.EqualTo(targetItem));
        });
    }

    /// <summary>
    /// Confirms that identical unique Item instances are not selected multiple times in a single solution.
    /// </summary>
    [Test]
    public void Solve_DistinctItemsInPool_DoesNotDuplicateSelectedInstances()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 10, 2),
            new Item(2, 20, 3),
            new Item(3, 30, 4)
        };

        Grasp graspSolver = new Grasp(items, 50);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 0.5);

        int uniqueCount = solution.Select(item => item.Id).Distinct().Count();

        // Assert.
        Assert.That(uniqueCount, Is.EqualTo(solution.Count));
    }

    /// <summary>
    /// Tests solver stability when handling candidates with identical value-to-weight ratios.
    /// </summary>
    [Test]
    public void Solve_EqualRatios_HandlesTiesWithoutExceptions()
    {
        // Arrange.
        List<Item> tiedRatioItems = new List<Item>
        {
            new Item(1, 20, 10),
            new Item(2, 40, 20),
            new Item(3, 60, 30)
        };

        Grasp graspSolver = new Grasp(tiedRatioItems, 30);

        // Act.
        List<Item> solution = graspSolver.Solve(5, 1.0);

        // Assert.
        Assert.That(solution.Sum(item => item.Weight), Is.LessThanOrEqualTo(30));
    }

    /// <summary>
    /// Checks that identical values and weights across different IDs select up to capacity limits.
    /// </summary>
    [Test]
    public void Solve_DuplicateValuesAndWeights_SelectsMaxCapacityFit()
    {
        // Arrange.
        List<Item> identicalItems = new List<Item>
        {
            new Item(1, 10, 5),
            new Item(2, 10, 5),
            new Item(3, 10, 5)
        };

        Grasp graspSolver = new Grasp(identicalItems, 10);

        // Act.
        List<Item> solution = graspSolver.Solve(5, 0.5);

        // Assert.
        Assert.That(solution, Has.Count.EqualTo(2));
    }

    /// <summary>
    /// Tests local search 1-exchange swap mechanism to replace lower value item for higher total yield.
    /// </summary>
    [Test]
    public void LocalSearch_HigherValueSwap_SwapsItemsForBetterSolution()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 12, 4),
            new Item(2, 20, 10)
        };

        Grasp graspSolver = new Grasp(items, 10);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 1.0);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(solution.Sum(item => item.Value), Is.EqualTo(20));
            Assert.That(solution, Contains.Item(items[1]));
        });
    }

    /// <summary>
    /// Verifies that solver recovers true optimal solution on known benchmark knapsack setup.
    /// </summary>
    [Test]
    public void Solve_KnownOptimalProblem_FindsGlobalOptimum()
    {
        // Arrange.
        List<Item> benchmarkItems = new List<Item>
        {
            new Item(1, 60, 10),
            new Item(2, 100, 20),
            new Item(3, 120, 30)
        };

        Grasp graspSolver = new Grasp(benchmarkItems, 50);

        // Act.
        List<Item> solution = graspSolver.Solve(20, 0.5);

        // Assert.
        Assert.That(solution.Sum(item => item.Value), Is.EqualTo(220));
    }

    /// <summary>
    /// Verifies that exceptionally high value items with low weight are prioritized into final selection.
    /// </summary>
    [Test]
    public void Solve_HighValueLowWeightItem_AlwaysIncludesDominantItem()
    {
        // Arrange.
        Item highValueItem = new Item(99, 1000, 1);

        List<Item> items = new List<Item>
        {
            new Item(1, 10, 10),
            new Item(2, 20, 10),
            highValueItem
        };

        Grasp graspSolver = new Grasp(items, 15);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 0.8);

        // Assert.
        Assert.That(solution, Contains.Item(highValueItem));
    }

    /// <summary>
    /// Ensures single iteration executions execute without error.
    /// </summary>
    [Test]
    public void Solve_SingleIteration_ExecutesSuccessfully()
    {
        // Arrange.
        List<Item> items = new List<Item> { new Item(1, 10, 5) };
        
        Grasp graspSolver = new Grasp(items, 10);

        // Act.
        List<Item> solution = graspSolver.Solve(1, 0.5);

        // Assert.
        Assert.That(solution, Has.Count.EqualTo(1));
    }

    /// <summary>
    /// Confirms that zero requested iterations return an empty best solution.
    /// </summary>
    [Test]
    public void Solve_ZeroIterations_ReturnsEmptySolution()
    {
        // Arrange.
        List<Item> items = new List<Item> { new Item(1, 10, 5) };

        Grasp graspSolver = new Grasp(items, 10);

        // Act.
        List<Item> solution = graspSolver.Solve(0, 0.5);

        // Assert.
        Assert.That(solution, Is.Empty);
    }

    /// <summary>
    /// Tests that executing higher iteration counts maintains or improves solution value quality.
    /// </summary>
    [Test]
    public void Solve_IncreasingIterations_MaintainsOrImprovesSolutionQuality()
    {
        // Arrange.
        List<Item> items = Enumerable.Range(1, 20)
            .Select(id => new Item(id, id * 10, id * 2))
            .ToList();

        Grasp graspSolver = new Grasp(items, 30);

        // Act.
        List<Item> singlePassResult = graspSolver.Solve(1, 0.3);
        List<Item> multiPassResult = graspSolver.Solve(50, 0.3);

        int singlePassValue = singlePassResult.Sum(item => item.Value);
        int multiPassValue = multiPassResult.Sum(item => item.Value);

        // Assert.
        Assert.That(multiPassValue, Is.GreaterThanOrEqualTo(singlePassValue));
    }

    /// <summary>
    /// Checks performance and execution safety across larger synthetic item datasets.
    /// </summary>
    [Test]
    public void Solve_LargeDataset_RunsWithoutThrowingExceptions()
    {
        // Arrange.
        Random deterministicRandom = new Random(42);
        List<Item> largeDataset = Enumerable.Range(1, 100)
            .Select(index => new Item(index, deterministicRandom.Next(1, 100), deterministicRandom.Next(1, 50)))
            .ToList();

        Grasp graspSolver = new Grasp(largeDataset, 200);

        // Act & Assert.
        Assert.DoesNotThrow(() => graspSolver.Solve(30, 0.5));
    }

    /// <summary>
    /// Verifies solver handling for non-positive input values and weights without crashing.
    /// </summary>
    [Test]
    public void Solve_NegativeValueAndWeightInputs_ExecutesSafely()
    {
        // Arrange.
        List<Item> negativeValuedItems = new List<Item>
        {
            new Item(1, -10, 5),
            new Item(2, 20, -5)
        };

        Grasp graspSolver = new Grasp(negativeValuedItems, 10);

        // Act & Assert.
        Assert.DoesNotThrow(() => graspSolver.Solve(5, 0.5));
    }

    /// <summary>
    /// Ensures consecutive method calls on a single instance maintain internal stability.
    /// </summary>
    [Test]
    public void Solve_MultipleConsecutiveInvocations_ReturnsValidOutputs()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 50, 10),
            new Item(2, 30, 5)
        };

        Grasp graspSolver = new Grasp(items, 10);

        // Act.
        List<Item> initialRun = graspSolver.Solve(10, 0.5);
        List<Item> subsequentRun = graspSolver.Solve(10, 0.5);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(initialRun, Is.Not.Null);
            Assert.That(subsequentRun, Is.Not.Null);
        });
    }

    /// <summary>
    /// Verifies that alpha values outside standard limits execute safely without unhandled errors.
    /// </summary>
    [Test]
    public void Solve_AlphaOutOfBounds_ExecutesWithoutThrowing()
    {
        // Arrange.
        List<Item> items = new List<Item> { new Item(1, 10, 5) };

        Grasp graspSolver = new Grasp(items, 10);

        // Act & Assert.
        Assert.Multiple(() =>
        {
            Assert.DoesNotThrow(() => graspSolver.Solve(2, 1.5));
            Assert.DoesNotThrow(() => graspSolver.Solve(2, -0.5));
        });
    }

    /// <summary>
    /// Verifies correct item filtering when capacity permits only a single candidate.
    /// </summary>
    [Test]
    public void Solve_CapacityLimitsToSingleItem_SelectsExactFitOnly()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 100, 10),
            new Item(2, 200, 20)
        };

        Grasp graspSolver = new Grasp(items, 10);

        // Act.
        List<Item> solution = graspSolver.Solve(5, 1.0);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(solution, Has.Count.EqualTo(1));
            Assert.That(solution[0].Id, Is.EqualTo(1));
        });
    }

    /// <summary>
    /// Tests that zero-weight items are included up to capacity constraints without division errors.
    /// </summary>
    [Test]
    public void Solve_ZeroWeightItems_IncludesAllZeroWeightCandidates()
    {
        // Arrange.
        List<Item> zeroWeightItems = new List<Item>
        {
            new Item(1, 10, 0),
            new Item(2, 20, 0),
            new Item(3, 30, 0)
        };

        Grasp graspSolver = new Grasp(zeroWeightItems, 10);

        // Act.
        List<Item> solution = graspSolver.Solve(5, 0.5);

        // Assert.
        Assert.That(solution, Has.Count.EqualTo(3));
    }

    /// <summary>
    /// Ensures empty collection is returned when all item weights exceed knapsack capacity.
    /// </summary>
    [Test]
    public void Solve_NoFittingItems_ReturnsEmptyList()
    {
        // Arrange.
        List<Item> heavyItems = new List<Item>
        {
            new Item(1, 50, 100),
            new Item(2, 60, 200)
        };

        Grasp graspSolver = new Grasp(heavyItems, 50);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 0.5);

        // Assert.
        Assert.That(solution, Is.Empty);
    }

    /// <summary>
    /// Confirms that returned solution items belong strictly to original input collection.
    /// </summary>
    [Test]
    public void Solve_ValidOutput_ContainsSubsetOfOriginalItems()
    {
        // Arrange.
        List<Item> itemPool = new List<Item>
        {
            new Item(1, 10, 2),
            new Item(2, 20, 4)
        };

        Grasp graspSolver = new Grasp(itemPool, 10);

        // Act.
        List<Item> solution = graspSolver.Solve(5, 0.5);

        // Assert.
        Assert.That(solution, Is.SubsetOf(itemPool));
    }

    /// <summary>
    /// Verifies aggregate weight and value calculations across selected solution set.
    /// </summary>
    [Test]
    public void Solve_SolutionMetrics_CalculatesAccurateSums()
    {
        // Arrange.
        List<Item> items = new List<Item>
        {
            new Item(1, 15, 3),
            new Item(2, 25, 5),
            new Item(3, 35, 7)
        };

        Grasp graspSolver = new Grasp(items, 12);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 0.5);

        int totalWeight = solution.Sum(item => item.Weight);
        int totalValue = solution.Sum(item => item.Value);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(totalWeight, Is.LessThanOrEqualTo(12));
            Assert.That(totalValue, Is.GreaterThan(0));
        });
    }

    /// <summary>
    /// Validates balanced greediness and exploration at alpha value 0.5.
    /// </summary>
    [Test]
    public void Solve_BalancedAlpha_GeneratesFeasibleSolution()
    {
        // Arrange.
        List<Item> items = Enumerable.Range(1, 10)
            .Select(index => new Item(index, index * 5, index * 2))
            .ToList();

        Grasp graspSolver = new Grasp(items, 15);

        // Act.
        List<Item> solution = graspSolver.Solve(10, 0.5);

        // Assert.
        Assert.Multiple(() =>
        {
            Assert.That(solution, Is.Not.Empty);
            Assert.That(solution.Sum(item => item.Weight), Is.LessThanOrEqualTo(15));
        });
    }
}