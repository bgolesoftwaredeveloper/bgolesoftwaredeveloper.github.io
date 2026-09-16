"""
edmonds_karp.py

An implementation of the Edmonds-Karp Algorithm for calculating the maximum flow
in a directed capacity network.

This module provides structural workflows to resolve the maximum flow problem by
iteratively computing shortest augmenting paths from a source node to a sink node.
Paths are discovered via an optimized Breadth-First Search (BFS) scheme, ensuring
polynomial runtime bounds of $O(V \cdot E^2)$ by selecting paths with the minimum
number of edges.

Design Note:
    This implementation is optimized for algorithmic clarity and structural reference.
    Graph definitions utilize an adjacency matrix representation for simplified
    residual tracking, while edge modifications scale forward and backward layers in
    parallel to maintain flow network conservation laws.
"""
from collections import deque

def bfs(matrix: list[list[int]], source: int, sink: int, map: list[int]) -> bool:
    """
        Searches for an augmenting path with available residual capacity using BFS.

    Mathematical Traversal:
        Explores the graph frontier layer-by-layer where edge capacity bounds
        satisfy $c_f(u, v) > 0$.

    Args:
        matrix (list[list[int]]): The active network state tracking residual capacities.
        source (int): The starting vertex index initiating path exploration.
        sink (int): The target termination vertex index.
        map (list[int]): Registry tracking parent arrays to reconstruct the pathway.

    Returns:
        bool: True if a viable path reaches the sink node, otherwise False.
    """
    number_of_vertices = len(matrix)

    # Tracking registry to prevent cyclic re-evaluation of structural nodes.
    visited = [False] * number_of_vertices

    # Memory queue for managing the Breadth-First Search (BFS) frontier.
    queue = deque([source])
    visited[source] = True

    while queue:
        node = queue.popleft()

        for neighbor in range(number_of_vertices):
            # Evaluate neighbors with unexhausted forward or reverse.
            if not visited[neighbor] and matrix[node][neighbor] > 0:
                queue.append(neighbor)
                visited[neighbor] = True

                # Log parent linkage map for subsequent path backtracking.
                map[neighbor] = node

                # Early exit condition if the sink boundary is successfully closed.
                if neighbor == sink:
                    return True

    return False

def edmonds_karp(matrix: list[list[int]], source: int, sink: int) -> int:
    r"""
    Executes the Edmonds-Karp maximum flow optimization across a network topology.

    Optimization Objective:
        Maximizes total throughput $f$ leaving the source and entering the sink,
        subject to capacity constraints $f(u, v) \leq c(u, v)$ and flow conservation.

    Args:
        matrix (list[list[int]]): Square matrix defining structural edge capacity bounds.
        source (int): The index of the flow-generating source vertex.
        sink (int): The index of the flow-absorbing sink vertex.

    Returns:
        int: The verified maximum flow capacity value of the system payload.

    Raises:
        ValueError: If input payload structures fail square-matrix constraints.
        IndexError: If target routing indexes break vertex boundary maps.
    """
    # Check if the matrix is completely empty.
    if not matrix or not matrix[0]:
        raise ValueError("The capacity matrix cannot be empty.")

    number_of_vertices = len(matrix)

    # Verify structural integrity: Matrix dimensions must scale uniformly.
    if any(len(row) != number_of_vertices for row in matrix):
        raise ValueError("The capacity matrix must be square.")

    # Validate boundaries of the designated operational terminals.
    if not (0 <= source < number_of_vertices):
        raise IndexError(f"Source index {source} is out of bounds.")

    if not (0 <= sink < number_of_vertices):
        raise IndexError(f"Sink index {sink} is out of bounds.")

    # Handle identities where source and sink occupy the ame vertex index.
    if source == sink:
        return 0

    # Initialize the residual network as a copy of the original capacities.
    residual_matrix = [row[:] for row in matrix]

    # This array will store the augmenting path structure found by BFS.
    map = [-1] * number_of_vertices
    maximum_flow = 0

    # Iteratively evaluate augmenting paths until capacity tracking drops to zero.
    while bfs(residual_matrix, source, sink, map):
        # Determine the maximum bottleneck flow permitted along the traced path.
        path_flow_bottleneck = float("inf")
        node = sink

        # Backtrack path links to discover the limiting edge capacity.
        while node != source:
            parent_node = map[node]
            path_flow_bottleneck = min(path_flow_bottleneck, residual_matrix[parent_node][node])
            node = parent_node

        node = sink

        # Re-traverse path links to update forward and backward residual structures.
        while node != source:
            parent_node = map[node]

            # Decrease the capacity of the forward edge.
            residual_matrix[parent_node][node] -= path_flow_bottleneck

            # Increase the capacity of the reverse edge.
            residual_matrix[node][parent_node] += path_flow_bottleneck

            node = parent_node

        # Add the bottleneck flow of the current path to the total maximum flow.
        maximum_flow += path_flow_bottleneck

    return maximum_flow