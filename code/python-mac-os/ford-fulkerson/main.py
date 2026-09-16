r"""
Graph Maximum Flow Execution Script

This script demonstrates the usage of the Ford-Fulkerson algorithm (specifically
the Edmonds-Karp implementation) to find the maximum possible flow in a directed
flow network.

The network is represented as an adjacency matrix where graph[i][j] indicates
the directed edge capacity from node i to node j. A capacity of 0 means no
direct edge exists between those two nodes.

Graph Structure Visualization:
    (1) --12--> (3)
   ^  ^          | \
  /    \         |  20
 16    10        9    \
/        \       v     v
(0)       (2) <--+    (5) [Sink]
\        /       ^     ^
 13    4        /     /
  v  /         7     4
    (2) --14-> (4) -+
"""
from ford_fulkerson import ford_fulkerson

def main():
    # Define the flow network using an adjacency matrix.
    graph = [
        [0, 16, 13, 0, 0, 0],
        [0, 0, 10, 12, 0, 0],
        [0, 4, 0, 0, 14, 0],
        [0, 0, 9, 0, 0, 20],
        [0, 0, 0, 7, 0, 4],
        [0, 0, 0, 0, 0, 0]
    ]

    # Define the starting point (source) and ending point (sink) for the flow.
    source = 0
    sink = 5

    # Execute the Ford-Fulkerson algorithm to compute the maximum.
    total_flow = ford_fulkerson(graph, source, sink)

    # Output the final result to the console.
    print(f"The maximum possible flow from node {source} to node {sink} is: {total_flow}.")

if __name__ == "__main__":
    main()