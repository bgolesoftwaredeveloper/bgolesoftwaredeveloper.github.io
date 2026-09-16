"""
main.py

An execution script demonstrating the practical discovery of Strongly Connected
Components (SCCs) within a directed topology using Kosaraju's algorithm.

This module imports the structural Graph model and the KosarajuSolver engine to
construct a complex, multi-component looping graph. It then executes the solver
to isolate and print the individual partitions.
"""
from kosaraju import Graph, KosarajuSolver

def main():
    # Header.
    print("=" * 60)
    print(" Kosaraju's Strongly Connected Components (SCC) Solver ")
    print("=" * 60)

    # Establish an 8-node directed graph layout.
    node_count = 8

    # Instantiate a new graph.
    network = Graph(node_count)

    # Define a complex structural web containing 3 separate SCC clusters:
    structural_edges = [
        (0, 1),
        (1, 2),
        (2, 0),
        (2, 3),
        (3, 4),
        (4, 5),
        (5, 3),
        (5, 6),
        (6, 7),
        (7, 6),
    ]

    print(f"Constructing directed graph topology with {node_count} nodes...")

    for source, destination in structural_edges:
        network.add_edge(source, destination)
        print(f"  -> Added directed edge: {source} ---> {destination}")

    print()

    print("Initializing the Kosaraju evaluation engine...")

    solver = KosarajuSolver(graph=network)

    print("Computing strongly connected components...")

    discovered_components = solver.compute_strongly_connected_components()

    print()

    print("-" * 60)
    print(f"Analysis Complete. Found {len(discovered_components)} Independent Strongly Connected Components:")
    print("-" * 60)

    # Output the discovered subsets.
    for index, component in enumerate(discovered_components, 1):
        print(f" Component #{index}: {sorted(component)}")

    print("=" * 60)

if __name__ == "__main__":
    main()