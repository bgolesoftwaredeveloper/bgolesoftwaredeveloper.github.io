/// File            :   Grasp.cs
/// Project         :   GRASP
/// Programmer      :   Braiden Gole
/// First version   :   2026-07-29
/// Description     :   Implementation of the Greedy Randomized Adaptive Search Procedure.
/// Compile:
/// dotnet new nunit -n GraspUnitTests -o grasp-unit-tests
/// dotnet add grasp-unit-tests/GraspUnitTests.csproj reference greedy-random-adaptive-search-procedure.csproj
/// dotnet new sln -n GreedyRandomAdaptiveSearchProcedure
/// dotnet sln add greedy-random-adaptive-search-procedure.csproj
/// dotnet sln add grasp-unit-tests/GraspUnitTests.csproj --solution-folder grasp-unit-test
using System;
using System.Collections.Generic;
using System.Linq;

namespace GreedyRandomAdaptiveSearchProcedure
{
    /// <summary>
    /// Represents an item available for selection in the Knapsack Problem.
    /// </summary>
    /// <param name="Id">Unique identifier for the item.</param>
    /// <param name="Value">Monetary or strategic value of the item.</param>
    /// <param name="Weight">Weight or resource consumption of the item.</param>
    public record Item(int Id, int Value, int Weight)
    {
        /// <summary>
        /// Gets the value-to-weight efficiency ratio used for greedy heuristic evaluation.
        /// </summary>
        public double Ratio => (double)Value / Weight;
    }

    /// <summary>
    /// Manages the GRASP metaheuristic optimization process for the Knapsack Problem.
    /// </summary>
    public class Grasp
    {
        private readonly Random _random = new Random();
        private readonly int _capacity;
        private readonly List<Item> _items;
        
        /// <summary>
        /// Initializes a new instance of the <see cref="Grasp"/> solver with problem parameters.
        /// </summary>
        /// <param name="items">The complete pool of items available for selection.</param>
        /// <param name="capacity">The maximum total weight capacity of the knapsack.</param>
        public Grasp(List<Item> items, int capacity)
        {
            this._items = items;
            this._capacity = capacity;
        }
        
        /// <summary>
        /// Phase 1: Builds a feasible candidate solution using a Restricted Candidate List (RCL).
        /// Combines greedy efficiency scoring with random selection based on threshold 'alpha'.
        /// </summary>
        /// <param name="alpha">Greediness factor (0.0 = pure random, 1.0 = pure greedy).</param>
        /// <returns>A constructed feasible solution (list of selected items).</returns>
        private List<Item> ConstructGraspSolution(double alpha)
        {
            // Initialize an empty solution list to store chosen items.
            List<Item> solution = new List<Item>();

            // Track current accumulated weight of selected items.
            int weight = 0;

            // Filter initial candidate pool to items that fit within capacity limits.
            List<Item> candidateList = _items.Where(index => index.Weight <= _capacity).ToList();

            while (candidateList.Count > 0)
            {
                // Calculate minimum and maximum efficiency ratios across candidate items.
                double minimumRatio = candidateList.Min(index => index.Ratio);
                double maximumRatio = candidateList.Max(index => index.Ratio);

                // Compute cutoff threshold boundary for Restricted Candidate List entry.
                double threshold = double.IsNaN(maximumRatio - minimumRatio) || maximumRatio == minimumRatio
                    ? minimumRatio 
                    : maximumRatio - alpha * (maximumRatio - minimumRatio);

                // Build Restricted Candidate List using calculated threshold condition.
                List<Item> restrictedCandidateList = candidateList.Where(index => index.Ratio >= threshold).ToList();

                // Select a random candidate element from Restricted Candidate List.
                Item selectedItem = restrictedCandidateList[_random.Next(restrictedCandidateList.Count)];

                // Append selected item to active working solution list.
                solution.Add(selectedItem);

                // Update aggregate weight sum with selected item weight.
                weight += selectedItem.Weight;

                // Refresh candidate list by removing selected items and items exceeding capacity.
                candidateList = candidateList.Where(index => !solution.Contains(index) && (weight + index.Weight <= _capacity)).ToList();
            }

            return solution;
        }

        /// <summary>
        /// Phase 2: Explores the local neighborhood (1-exchange swaps) to improve the initial candidate solution.
        /// Iteratively replaces an item in the solution with an unselected item if it yields a higher total value within capacity.
        /// </summary>
        /// <param name="solution">The initial solution generated during the constructive phase.</param>
        /// <returns>A locally optimal solution after applying local search moves.</returns>
        private List<Item> LocalSearch(List<Item> solution)
        {
            bool improvement = true;

            // Create working copy of solution to perform local search modifications.
            List<Item> currentSolution = new List<Item>(solution);

            while (improvement)
            {
                // Reset improvement status at start of each neighborhood evaluation cycle.
                improvement = false;

                // Compute total current weight and value for active solution.
                int weight = currentSolution.Sum(index => index.Weight);
                int value = currentSolution.Sum(index => index.Value);

                // Identify candidate elements from main item set currently excluded from solution.
                List<Item> unselectedItems = _items.Except(currentSolution).ToList();

                foreach (Item inItem in unselectedItems)
                {
                    if (weight + inItem.Weight <= _capacity)
                    {
                        currentSolution.Add(inItem);

                        improvement = true;

                        break;    
                    }

                    foreach (Item outItem in currentSolution)
                    {
                        // Calculate weight and value metrics resulting from potential element swap.
                        int newWeight = weight - outItem.Weight + inItem.Weight;
                        int newValue = value - outItem.Value + inItem.Value;

                        // Swap elements if modification remains feasible and increases overall value.
                        if (newWeight <= _capacity && newValue > value)
                        {
                            currentSolution.Remove(outItem);
                            currentSolution.Add(inItem);

                            // Flag improvement detected to trigger subsequent neighborhood search pass.
                            improvement = true;

                            break;
                        }
                    }

                    // Exit inner loop sequence when no valid improvement move is located.
                    if (improvement)
                    {
                        break;
                    }
                }
            }

            return currentSolution;
        }

        /// <summary>
        /// Executes the primary GRASP loop across multiple iterations to find the global optimal solution.
        /// </summary>
        /// <param name="maximumIterations">The number of GRASP iterations to perform.</param>
        /// <param name="alpha">Greediness parameter controlling RCL threshold (0.0 to 1.0).</param>
        /// <returns>The best solution found across all iterations.</returns>
        public List<Item> Solve(int maximumIterations, double alpha)
        {
            // Track globally optimal solution across GRASP iterations.
            List<Item> bestSolution = new List<Item>();

            // Initialize baseline value comparison threshold.
            int bestValue = 0;

            for (int index = 0; index < maximumIterations; index++)
            {
                // Construct semi-greedy initial solution candidate.
                List<Item> candidateSolution = ConstructGraspSolution(alpha);

                // Refine candidate solution quality using local search neighborhood exploration.
                candidateSolution = LocalSearch(candidateSolution);

                // Compute aggregate value of current iteration candidate.
                int candidateValue = candidateSolution.Sum(item => item.Value);
                
                // Update best global solution if current iteration yields higher total value.
                if (candidateValue > bestValue)
                {
                    bestValue = candidateValue;
                    bestSolution = candidateSolution;
                }
            }

            return bestSolution;
        }
    }
}