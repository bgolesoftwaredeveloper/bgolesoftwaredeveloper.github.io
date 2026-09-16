"""
main.py

An execution wrapper and demonstration script for the Edmonds' Blossom Maximum
Cardinality Matching pipeline.

This script constructs representative graph payloads—ranging from standard bipartite
topologies to classic non-bipartite odd-length cycles—and routes them through the
blossom core logic to resolve and verify optimal vertex pairing matrices.
"""
from edmonds_blossom import find_maximum_matching

def main():
    # Test Case 1: Standard 4-node bipartite path graph.
    # Max matching should yield 2 independent edge pairs.
    path_graph_vertices = 4
    path_graph = {
        0: [1],
        1: [0, 2],
        2: [1, 3],
        3: [2]
    }

    print("--- Running: Simple 4-Node Path Graph ---")
    print(f"\tGraph Structure: {path_graph}.")

    path_matching = find_maximum_matching(path_graph, path_graph_vertices)

    print(f"\tCalculated Pairs : {path_matching}.")
    print(f"\tTotal Edges Matched: {len(path_matching)}.\n")

    # Test Case 2: 5-node pristine odd cycle (0-1-2-3-4-0).
    # Triggers standard blossom contraction since a perfect matching is impossible.
    # Max matching should yield floor(5/2) = 2 pairs, leaving 1 node unmatched.
    cycle_graph_vertices = 5
    cycle_graph = {
        0: [1, 4],
        1: [0, 2],
        2: [1, 3],
        3: [2, 4],
        4: [3, 0]
    }

    print("--- Running: 5-Node Odd Cycle (Blossom Baseline) ---")
    print(f"\tGraph Structure: {cycle_graph}.")

    cycle_matching = find_maximum_matching(cycle_graph, cycle_graph_vertices)

    print(f"\tCalculated Pairs : {cycle_matching}.")
    print(f"\tTotal Edges Matched: {len(cycle_matching)}.\n")

    # Test Case 3: 5-node odd cycle with a cross chord edge between 1 and 4.
    # Evaluates nested, overlapping blossom contraction structures.
    # Max matching is still capped at floor(5/2) = 2 pairs.
    chord_graph_vertices = 5
    chord_graph = {
        0: [1, 4],
        1: [0, 2, 4],
        2: [1, 3],
        3: [2, 4],
        4: [3, 0, 1]
    }

    print("--- Running: 5-Node Odd Cycle with 1-4 Cross Chord ---")
    print(f"\tGraph Structure: {chord_graph}.")

    chord_matching = find_maximum_matching(chord_graph, chord_graph_vertices)

    print(f"\tCalculated Pairs : {chord_matching}.")
    print(f"\tTotal Edges Matched: {len(chord_matching)}.\n")

if __name__ == "__main__":
    main()