
"""
main.py

An execution wrapper and demonstration script for the Edmonds-Karp Maximum
Flow pipeline.

This script constructs a representative directed graph network using an adjacency
capacity matrix, establishes source and sink node parameters, and routes the network
payload through the Edmonds-Karp core logic to calculate optimal throughput limits.
"""
from edmonds_karp import edmonds_karp

def main():
    print("=" * 60)
    print("\tEdmonds-Karp Maximum Flow Algorithm")
    print("=" * 60)

    # Represents a classic graph topology where node 0 acts as the source and node 5 acts as the sink
    network_capacities = [
        [0, 16, 13, 0, 0, 0],
        [0, 0, 10, 12, 0, 0],
        [0, 4, 0, 0, 14, 0],
        [0, 0, 9, 0, 0, 20],
        [0, 0, 0, 7, 0, 4],
        [0, 0, 0, 0, 0, 0],
    ]

    source = 0
    sink = 5

    print("Initializing capacity matrix:")

    for index, row in enumerate(network_capacities):
        print(f"\tNode {index} capacity routing: {row}.")

    print("Executing algorithm from source to sink.")

    try:
        # Route the capacity topology through the Edmonds-Karp pipeline.
        maximum_flow = edmonds_karp(network_capacities, source, sink)

        print("=" * 60)
        print(f"\tSuccess: The Maximum Flow of the network is {maximum_flow}.")
        print("=" * 60)
    except Exception as exception:
        print(f"Algorithm tracking failed unexpectedly: {exception}.")

if __name__ == "__main__":
    main()