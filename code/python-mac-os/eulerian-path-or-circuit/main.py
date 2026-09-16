"""
main.py

Driver execution environment for validating and demonstrating Hierholzer's
Eulerian Path and Circuit discovery algorithm across directed graph topologies.

Overview:
    This module serves as the primary entry point to instantiate a specific network
    topology, display its adjacency configuration, and pass the structure to the
    algorithmic pipeline for evaluation. It isolates the setup layer from the
    underlying graph-traversal engine, verifying whether a given layout complies
    with Eulerian pathing constraints.

Topological Case Reference:
    The driver initializes a directed, asymmetric graph structure containing
    a localized cycle and a distinct terminal sink node.

    Layout Mapping:
        A -> B
        B -> C, D
        C -> B
        D -> [] (Sink)

    Mathematical Distribution:
        - Node A: Out-degree = 1, In-degree = 0 (Net Source)
        - Node B: Out-degree = 2, In-degree = 2 (Balanced)
        - Node C: Out-degree = 1, In-degree = 1 (Balanced)
        - Node D: Out-degree = 0, In-degree = 1 (Net Sink)

    Expected Outcome:
        Valid Eulerian Path starting at 'A' and terminating at 'D':
        [ A -> B -> C -> B -> D ]

Execution Flow:
    1. Parse and print the input network configuration (Adjacency List).
    2. Route the adjacency payload to the Hierholzer verification engine.
    3. Catch the return sequence state:
       - On success: Format and output the complete structural traversal trail.
       - On failure: Raise a topological constraint exception warning.
"""
from eulerian_path import eulerian_path_or_circuit

def main():
    print("=" * 60)
    print("Hierholzer's Algorithm Execution Pipeline".center(60))
    print("=" * 60)

    # Topological Setup: Representing an asymmetric path layout.
    network_topology = {
        "A": ["B"],
        "B": ["C", "D"],
        "C": ["B"],
        "D": [],
    }

    print("\n[Input] Adjacency List Configuration:")

    # Display the network.
    for vertex, neighbors in network_topology.items():
        print(f"\tVertex {vertex} -> Connected to: {neighbors}")

    print()

    print("[Processing] Evaluating structural laws and tracing trail...\n")

    # Execute Eulerian Path or Circuit.
    resolved_trail = eulerian_path_or_circuit(network_topology)

    print("[Output] Resolution State:")

    # Resolved?
    if resolved_trail:
        formatted_trail = " -> ".join(resolved_trail)
        print(f"\tSuccessfully verified Eulerian layout.")
        print(f"\tTraced Sequence: [ {formatted_trail} ]")
    else:
        print("\tFailed Verification: Topology breaks Eulerian constraints.")

    print("\n" + "=" * 60)

if __name__ == "__main__":
    main()