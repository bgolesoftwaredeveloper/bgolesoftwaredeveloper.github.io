/// File            :   Program.cs
/// Project         :   GRASP
/// Programmer      :   Braiden Gole
/// First version   :   2026-07-29
/// Description     :   Entry point for the GRASP algorithm implementation.
/// Create, Build, Compile:
/// dotnet new console --force -n greedy-random-adaptive-search-procedure
/// dotnet new nunit -o grasp-unit-tests -n GreedyRandomAdaptiveSearchProcedureUnitTest
/// dotnet add grasp-unit-tests/GreedyRandomAdaptiveSearchProcedure.csproj reference greedy-random-adaptive-search-procedure.csproj
/// dotnet new sln --format slnx -n GreedyRandomAdaptiveSearchProcedure
/// dotnet sln GreedyRandomAdaptiveSearchProcedure.slnx add greedy-random-adaptive-search-procedure.csproj
/// dotnet sln GreedyRandomAdaptiveSearchProcedure.slnx add grasp-unit-tests/GreedyRandomAdaptiveSearchProcedureUnitTest.csproj --solution-folder grasp-unit-tests
/// find . -type d \( -name "obj" -o -name "bin" \) -exec rm -rf {} +
/// dotnet build GreedyRandomAdaptiveSearchProcedure.slnx
/// dotnet run --project greedy-random-adaptive-search-procedure.csproj

namespace GreedyRandomAdaptiveSearchProcedure
{
    internal class Program
    {
        static void Main(string[] args)
        {
            // Define the set of candidate items available for selection (Id, Value, Weight)/
            List<Item> items = new List<Item>
            {
                new Item(1, 60, 10),
                new Item(2, 100, 20),
                new Item(3, 120, 30),
                new Item(4, 90, 15),
                new Item(5, 75, 25),
                new Item(6, 110, 22)
            };

            // Set the operational parameters for the Knapsack problem and GRASP execution.
            int capacity = 50;
            int maximumIterations = 100;

            // Alpha controls the Restritcted Candidate List (RCL) threshold (0.0 = pure random, 1.0 = pure greedy).
            double alpha = 0.5;

            Console.WriteLine($"Running GRASP (Iterations: {maximumIterations}, Alpha: {alpha}...)\n");

            // Instantiate the GRASP solver with problem data.
            Grasp solver = new Grasp(items, capacity);

            // Execute the main GRASP optimization loop to locate the optimal solution.
            List<Item> bestSolution = solver.Solve(maximumIterations, alpha);

            Console.WriteLine("-- Best solution found --");

            foreach (Item item in bestSolution)
            {
                // Output the selected items from the best solution found.
                Console.WriteLine($"\tItem {item.Id}: Value = {item.Value}, Weight = {item.Weight}.");
            }

            Console.WriteLine("-------------------------");
            Console.WriteLine($"Total value: {bestSolution.Sum(index => index.Value)}.");
            Console.WriteLine($"Total weight: {bestSolution.Sum(index => index.Weight)} / {capacity}.");
        }
    }
}