r"""
johnson.py

An implementation of Johnson's algorithm to compute all-pairs shortest paths in a
sparse, weighted directed graph.

This module provides a memory-efficient approach to shortest path discovery by
leveraging both Bellman-Ford for negative weight cycle detection/reweighting
and Dijkstra's algorithm for priority-queue-based traversal across non-negative
edge weights.

Design Note:
    This implementation utilizes Bellman-Ford as an initial transformation layer
    to sanitize negative edge weights, facilitating the application of Dijkstra's
    algorithm ($O(V^2 \log V + VE)$ complexity). It is designed for educational
    analysis of graph reweighting techniques.
"""
from johnsons import johnson

def main():
    # Define a graph as an adjacency list.
    graph = {
        0: [(1, 5), (2, 2)],
        1: [(2, 1)],
        2: []
    }

    # Calculate the number of vertices in the graph.
    number_of_vertices = len(graph)

    try:
        print("Calculating all-pairs shortest paths...")

        # Execute Johnson's algorithm on the graph.
        shortest_paths = johnson(graph, number_of_vertices)

        # Summary.
        print(f"{'Source':<8} | {'To 0':<6} {'To 1':<6} {'To 2':<6}")
        print("-" * 30)

        for source, distances in enumerate(shortest_paths):
            # Format distances: use 'inf' if reachable, otherwise format as integer
            formatted_distances = [
                str(int(d)) if d != float("inf") else "inf"
                for d in distances
            ]

            result = " ".join(f"{d:<6}" for d in formatted_distances)

            # Print the result.
            print(f"{source:<8} | {result}")
    except ValueError as exception:
        print(exception)

if __name__ == "__main__":
    main()