r"""
johnson.py

An implementation of Johnson's algorithm to compute all-pairs shortest paths in a
sparse, weighted directed graph.

This module provides a memory-efficient approach to shortest path discovery by
leveraging both Bellman-Ford for negative weight cycle detection/reweighting
and Dijkstra's algorithm for priority-queue-based traversal across non-negative
edge weights.

Design Note:
    This implementation utilizes Bellman-Ford as an initial transformation layer
    to sanitize negative edge weights, facilitating the application of Dijkstra's
    algorithm ($O(V^2 \log V + VE)$ complexity). It is designed for educational
    analysis of graph reweighting techniques.
"""
import heapq

def bellman_ford(graph: dict[int, list[tuple[int, float]]], source: int, number_of_vertices: int) -> list[float]:
    """
    Computes initial shortest paths from a dummy source to all nodes to determine
    vertex potentials (h-values) and identify negative-weight cycles.

    Args:
        graph dict[int, list[tuple[int, int]]]: Adjacency list representation of the directed graph.
        source (int): The dummy source node index.
        number_of_vertices (int): Total count of vertices including the dummy node.

    Returns:
        list[float]: A vector of calculated potential h-values for reweighting.
    """
    # Initialize distances with infinity.
    distances = [float("inf")] * number_of_vertices
    distances[source] = 0

    # Relax edges |V| - 1 times.
    for _ in range(number_of_vertices - 1):
        for current in graph:
            for neighbor, weight in graph[current]:
                if distances[current] + weight < distances[neighbor]:
                    distances[neighbor] = distances[current] + weight

    # Check for negative weight cycles.
    for current in graph:
        for neighbor, weight in graph[current]:
            if distances[current] != float("inf") and distances[current] + weight < distances[neighbor]:
                raise ValueError("Graph contains a negative weight cycle.")

    return distances

def dijkstra(graph: dict[int, list[tuple[int, int]]], source: int, number_of_vertices: int) -> list[float]:
    """
    Executes a standard Dijkstra shortest-path traversal utilizing a priority queue
    for optimal exploration of non-negative weighted graphs.

    Args:
        graph dict[int, list[tuple[int, int]]: The graph structure containing reweighted, non-negative edges.
        source (int): The starting node for this iteration of the search.
        number_of_vertices (int): The total vertex count for initializing the distance array.

    Returns:
        list[float]: Shortest path distances from the start_node to all reachable nodes.
    """
    shortest_distances = [float("inf")] * number_of_vertices
    shortest_distances[source] = 0

    # Priority queue stores tuples of (accumulated distance, index).
    priority_queue = [(0.0, source)]

    while priority_queue:
        current, node = heapq.heappop(priority_queue)

        # Skip if a more efficient path to this node has already been processed.
        if current > shortest_distances[node]:
            continue

        for neighbor, weight in graph[node]:
            if shortest_distances[node] + weight < shortest_distances[neighbor]:
                shortest_distances[neighbor] = shortest_distances[node] + weight
                heapq.heappush(priority_queue, (shortest_distances[neighbor], neighbor))

    return shortest_distances

def johnson(graph: dict[int, list[tuple[int, float]]], number_of_vertices: int) -> list[list[float]]:
    """
    Orchestrates the all-pairs shortest path computation by reweighting the
    graph to remove negative values and executing Dijkstra across all vertices.

    Args:
        graph dict[int, list[tuple[int, int]]: Adjacency list of the target directed graph.
        number_of_vertices (int): The count of vertices in the original graph.

    Returns:
        list[list[float]]: A 2D matrix where the index [current][neighbor] represents the
        shortest distance between node current and node neighbor.
    """
    # 1. Augment graph with a dummy node connected to all others via 0-weight edges.
    node = number_of_vertices

    # 2. Derive potental h-values via Bellman0Ford to facilitate edge.
    extended_graph = {node: list(edges) for node, edges in graph.items()}
    extended_graph[node] = [(node, 0) for node in range(number_of_vertices)]

    hash_values = bellman_ford(extended_graph, node, number_of_vertices + 1)

    # 3. Create a reweighted adjacency graph.
    reweighted_graph = {node: [] for node in range(number_of_vertices)}

    for current in range(number_of_vertices):
        for neighbor, weight in graph[current]:
            # Apply transformation.
            new_weight = weight + int(hash_values[current] - hash_values[neighbor])
            reweighted_graph[current].append((neighbor, new_weight))

    # 4. Compute the shortest paths from each node using Dijkstra, then translate back.
    shortest_paths = []

    for source in range(number_of_vertices):
        distances_reweighted = dijkstra(reweighted_graph, source, number_of_vertices)

        # Reverse the potential translation for original weight accuracy.
        actual_distances = [
            (distance - hash_values[source] + hash_values[target])
            if distance != float("inf") else float("inf")
            for target, distance in enumerate(distances_reweighted)
        ]

        shortest_paths.append(actual_distances)

    return shortest_paths
