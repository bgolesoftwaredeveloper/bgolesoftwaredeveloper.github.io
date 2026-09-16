"""
ford_fulkerson.py

Ford-Fulkerson Algorithm (Edmonds-Karp Implementation)

This module provides an implementation of the Ford-Fulkerson method using a
Breadth-First Search (BFS) to discover augmenting paths (the Edmonds-Karp algorithm).
It computes the maximum network flow from a source vertex to a sink vertex in a
directed graph represented as a capacity adjacency matrix.
"""
from collections import deque

def bfs(graph: list[list[int]], source: int, sink: int, parent_map: dict[int, float]) -> bool:
    """
    Performs a Breadth-First Search (BFS) on the residual graph to find the shortest
    available augmenting path from the source node to the sink node.

    Args:
        graph (list[list[int]]): The current state of the residual capacity graph.
        source (int): The starting vertex index.
        sink (int): The destination/terminal vertex index.
        parent_map (dict[int, int]): A dictionary updated in-place to map each
                                     visited node to its immediate predecessor.

    Returns:
        bool: True if an open path with remaining capacity exists to the sink,
              False otherwise.
    """
    # Track visited vertices during this specific traversal to prevent cycles.
    visited = set()

    # Initialize the BFS queue with the source node.
    queue = deque([source])

    visited.add(source)

    while queue:
        node = queue.popleft()

        # Iterate through all potential neighbor nodes and their remaining edge capacities.
        for neighbor, capacity in enumerate(graph[node]):
            # An edge is viable only if the neighbor is unvisited and has available capacity.
            if neighbor not in visited and capacity > 0:
                queue.append(neighbor)
                visited.add(neighbor)

                # Record how we reached this neighbor to reconstruct the path later.
                parent_map[neighbor] = node

                # If we reach the sink, an augmenting path exists.
                if neighbor == sink:
                    return True

    return False

def ford_fulkerson(graph: list[list[int]], source: int, sink: int) -> int:
    """
    Computes the maximum flow of a network graph from source to sink.

    This function repeatedly finds augmenting paths with available capacity,
    determines their bottleneck flow, and updates the forward and reverse
    residual edge values until no more valid paths can be found.

    Args:
        graph (list[list[int]]): A square matrix representing edge capacities.
        source (int): The source node index.
        sink (int): The sink node index.

    Returns:
        int: The maximum total volume/flow that can move from source to sink.
    """
    number_of_nodes = len(graph)

    # Create a deep copy of the original graph to track evolving residual capacities.
    residual_graph = [row[:] for row in graph]

    # Map used to store path backtracking structures populated by the BFS.
    parent_map = {}
    maximum_flow = 0

    # Keep searching for a valid path from source to sink as long as one exists.
    while bfs(residual_graph, source, sink, parent_map):
        bottleneck_capacity = float("inf")
        node = sink

        # Backtrack from sink to source to determine the tightest constraint (bottleneck) on the path.
        while node != source:
            parent_node = parent_map[node]
            path_capacity = residual_graph[parent_node][node]

            # If the current edge capacity is smaller, it becomes the new bottleneck.
            if path_capacity < bottleneck_capacity:
                bottleneck_capacity = path_capacity

            node = parent_node

        # Update the residual capacities of the edges and reverse edges along the path.
        current = sink

        while current != source:
            parent_node = parent_map[current]

            # Subtract the bottleneck from the forward edge capacity.
            residual_graph[parent_node][current] -= int(bottleneck_capacity)

            # Add the bottleneck to the reverse edge capacity (allows flow to be pushed back).
            residual_graph[current][parent_node] += int(bottleneck_capacity)

            current = parent_node

        # Add the bottleneck flow of this path to the total maximum flow.
        maximum_flow += int(bottleneck_capacity)

        # Clear the map for the next iteration's path discovery.
        parent_map.clear()

    return maximum_flow