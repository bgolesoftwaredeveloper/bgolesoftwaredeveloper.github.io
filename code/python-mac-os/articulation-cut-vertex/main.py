"""
main.py

An execution entry point designed to instantiate the TopologicalNetwork graph engine,
populate an operational sample topology, and trigger articulation point discovery.

This script demonstrates the usage of the Tarjan-based cut vertex detection algorithm,
validating network structural vulnerabilities on a known connected graph component.
"""
from articulation_cut_vertex import TopologicalNetwork

def main():
    # Instantiate an unweighted, undirected graph analysis environment.
    network = TopologicalNetwork()

    # Construct a sample network topology featuring clear bottleneck nodes:
    # 1 --- 0 --- 3
    # |   /       |
    # |  /        |
    # 2           4
    network.add_edge(1, 0)
    network.add_edge(0, 2)
    network.add_edge(2, 1)
    network.add_edge(0, 3)
    network.add_edge(3, 4)

    # Execute linear-time depth-first evaluation metrics over the graph structure.
    cut_vertices = network.retrieve_articulation_points()

    # Stream the discovered structural points directly to standard output.
    print(f"Discovered Network Articulation Points (Cut Vertices): {cut_vertices}.")

if __name__ == "__main__":
    main()