"""
main.py

Execution entry point designed to demonstrate the runtime capabilities and structural
evaluation mechanics of Karger's Minimum Cut engine.

This module loads a connected, undirected graph represented as an adjacency list
and passes it into the randomized contraction handler. It executes the algorithm
over a statistically optimal number of independent trials to discover and print
the global minimum cut size directly to standard output.

Design Note:
    The variable layout uses an explicit adjacency matrix configuration to
    comprehensively execute and evaluate the random vertex-merging and self-loop
    filtering mechanics of the downstream contraction algorithm.
"""
import math

from kargers import find_minimum_cut

def main():
    # Define a graph object to work with.
    interconnected_network = {
        "A": ["B", "C"],
        "B": ["A", "C"],
        "C": ["A", "B", "D"],
        "D": ["C", "E", "F"],
        "E": ["D", "F"],
        "F": ["D", "E"],
    }

    # Determine the number of vertices to calculate the optimal trial count.
    number_of_vertices = len(interconnected_network)

    # Calculate the total independent trials required for a high probability of success.
    total_trials = int(
        (number_of_vertices**2) * math.log(number_of_vertices)
    )

    # Run the minimum cut finder.
    discovered_min_cut = find_minimum_cut(
        interconnected_network, number_of_trials=total_trials
    )

    # Print the final result representing the size of the graph partition.
    print(f"The minimum cut size discovered is: {discovered_min_cut}")

if __name__ == "__main__":
    main()