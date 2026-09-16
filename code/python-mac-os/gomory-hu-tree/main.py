"""
main.py

Demonstration module for the Gomory-Hu tree implementation.

This script initializes a sample undirected graph representing network capacities
and constructs its corresponding Gomory-Hu tree. It outputs the tree structure,
demonstrating how the minimum cut values between arbitrary node pairs are
represented as paths within the tree.
"""
from gomory_hu_tree import GomoryHuTree

def main():
    # Define a network to work with.
    network = [
        [0, 10, 0, 5],
        [10, 0, 5, 0],
        [0, 5, 0, 10],
        [5, 0, 10, 0]
    ]

    # Instantiate the Gomory-Hu tree class.
    gomory = GomoryHuTree(network)

    # Build the tree.
    tree = gomory.construct(gomory.matrix)

    # Summary.
    print("=" * 60)
    print("Gomory-Hu tree:".center(60))
    print("=" * 60)

    # Keep track of the edges already printed to prevent duplicates,
    # as the tree is represented by bidirectional entries in the adjacency list.
    processed_edges = set()

    # Iterate through the dictionary to display each unique edge and its weight.
    for node, edges in tree.items():
        for neighbor, weight in edges:
            # Sort the tuple to ensure that (0, 1) and (1, 0) are treated as the same edge.
            edge = tuple(sorted((node, neighbor)))

            # Only print the edge if it hasn't been displayed yet.
            if edge not in processed_edges:
                print(f"Edge {edge[0]} <---> {edge[1]} | Minimum-cut weight: {weight}.")

                # Mark this edge as processed.
                processed_edges.add(edge)

if __name__ == "__main__":
    main()