"""
main.py

Execution entry point demonstrating the operational pipeline of Dinic's maximum flow algorithm.

This script constructs a classic 6-node network topology, inserts weighted capacity edges,
and triggers the evaluation engine to compute the bottleneck capacity separating the designated
source vertex from the destination sink vertex. It serves as a structural validation baseline.
"""
from dinics import DinicsNetwork

def main():
    # Instantiate a network topology space reserving exactly 6 nodes (labeled 0 through 5).
    network = DinicsNetwork(6)

    # Construct standard forward capacity linkages along with explicit backward residual allocations.
    network.add_edge(0, 1, 16)
    network.add_edge(0, 2, 13)
    network.add_edge(1, 2, 10)
    network.add_edge(1, 3, 12)
    network.add_edge(2, 1, 4)
    network.add_edge(2, 4, 14)
    network.add_edge(3, 2, 9)
    network.add_edge(3, 5, 20)
    network.add_edge(4, 3, 7)
    network.add_edge(4, 5, 4)

    # Compute the global network maximum flow from origin node 0 to destination node 5.
    maximum_flow_result = network.compute_maximum_flow(source=0, sink=5)

    return maximum_flow_result

if __name__ == "__main__":
    print(f"The maximum flow is {main()}.")