"""
main.py

Demonstration entry point for the Hungarian assignment algorithm.

This script instantiates the HungarianAssigner with a sample bipartite matching
problem (e.g., matching workers to tasks with minimum total overhead cost)
and prints out the resulting optimal assignments along with total minimized cost.
"""
from hungarian import HungarianAssigner

def main():
    # Define the cost matrix.
    matrix = [[9, 2, 7, 8], [6, 4, 3, 7], [5, 8, 1, 8], [7, 6, 9, 4]]

    print("-- Hungarian assignment --")
    print("Initial state:")

    for row in matrix:
        print(f"\t{row}")

    print("-" * 42)

    try:
        # Step 1: Instantiate the combinatorial optimization solver.
        assigner = HungarianAssigner(matrix)

        # Step 2: Trigger structural processing to locate the optimal configuration coordinates.
        optimal_pairs = assigner.compute_assignment()

        print("Optimal assignment:")

        cumulative_minimized_cost = 0

        # Step 3: Iterate through coordinates to map solutions back to the baseline profile layout metrics.
        for worker, task in optimal_pairs:
            cost_value = matrix[worker][task]
            cumulative_minimized_cost += cost_value

            print(f"\t-> Worker {worker} is allocated to Task {task} (Individual Cost: {cost_value}.")

        print("-" * 42)
        print(f"Minimum total systemic cost: {cumulative_minimized_cost}.")
    except (TypeError, ValueError, RuntimeError) as exception:
        # Cleanly intercept any structural data assertion configuration errors.
        print(f"{exception}")

if __name__ == "__main__":
    main()
