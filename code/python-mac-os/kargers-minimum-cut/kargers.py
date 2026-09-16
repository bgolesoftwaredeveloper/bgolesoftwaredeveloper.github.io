"""
Karger's Algorithm for Minimum Cut

This module provides an implementation of Karger's randomized algorithm to find
the global minimum cut of a connected, undirected graph. The algorithm uses random
edge contraction to iteratively reduce the number of vertices. Because it is a
probabilistic approach, multiple independent trials are executed to maximize the
likelihood of discovering the true minimum cut.
"""
import copy
import random

def find_minimum_cut(graph: dict, number_of_trials: int):
    """
    Orchestrates multiple independent trials of Karger's algorithm to find the minimum cut.

    Since Karger's algorithm is probabilistic, running it across multiple independent
    iterations ensures that the true global minimum cut is found with high probability.
    It tracks and returns the smallest cut size discovered across all trials.

    Args:
        graph (dict): The adjacency list representation of the input graph.
        number_of_trials (int): The total number of independent contraction trials to run.

    Returns:
        int: The global minimum cut size discovered across all iterations.
    """
    # Initialize the minimum cut to infinity so any valid cut found will be smaller.
    minimum_cut = float("inf")

    # Run teh contraction process independently for the specified number of trials.
    for index in range(number_of_trials):
        current_cut_size = kargers(graph)

        # If the current trial produced a smaller cut, update our global minimum.
        if current_cut_size < minimum_cut:
            minimum_cut = current_cut_size

    # Handle boundary case: If minimum cut is still infinity, return it directly.
    if minimum_cut == float("inf"):
        return minimum_cut

    # Return the smallest cut size found across all random attempts.
    return int(minimum_cut)

def contract_edge(graph: dict, source: int, target: int):
    """
    Contracts a chosen edge by collapsing the target vertex into the source vertex.

    This function merges the neighbor list of the target into the source, updates
    all other vertices in the graph that pointed to the target to now point to the
    source, filters out any newly created self-loops, and deletes the target vertex.

    Args:
        graph (dict): The adjacency list representation of the graph being modified.
        source (int): The vertex absorbing the target vertex.
        target (int): The vertex being dissolved/collapsed.
    """
    # 1. Merge the neighbor list of the target vertex into source vertex.
    graph[source].extend(graph[target])

    # 2. Update all references to target in the graph to point to source.
    for neighbor in graph[target]:
        # Replace the old reference to target with source.
        graph[neighbor] = [
            source
            if value == target
            else value
            for value in graph[neighbor]
        ]

    # 3. Remove self-loops from source (edges connecting it to itself).
    graph[source] = [
        neighbor for neighbor in graph[source] if neighbor != source
    ]

    # 4. Delete the collapsed target vertex from the graph.
    if target in graph:
        del graph[target]

def kargers(graph: dict) -> int:
    """
    Executes a single, isolated run of Karger's randomized contraction algorithm.

    It creates a local copy of the graph and repeatedly contracts randomly selected
    edges until exactly two distinct vertices remain. The size of the cut is then
    determined by counting the edges remaining between those two meta-vertices.

    Args:
        graph (dict): The original adjacency list representation of the graph.

    Returns:
        int: The cut size (number of bridging edges) found during this single execution.
    """
    # Isolate this run by working on a deep copy, protecting the original graph structure.
    graph = copy.deepcopy(graph)

    # Clean up pre-existing self-loops right away.
    for vertex in list(graph.keys()):
        graph[vertex] = [neighbor for neighbor in graph[vertex] if neighbor != vertex]

    # Contract edges until only 2 vertices remain.
    while len(graph) > 2:
        # Build a list of all actual valid edges remaining in the graph.
        all_edges = []

        for source, neighbors in graph.items():
            for target in neighbors:
                all_edges.append([source, target])

        if not all_edges:
            return 0

        # Choose a random starting source vertex.
        source, target = random.choice(all_edges)

        # Collapse the randomly selected edge into a single vertex.
        contract_edge(graph, source, target)

    # Extract the keys of the final two remaining meta-vertices.
    remaining_vertices = list(graph.keys())

    if not remaining_vertices:
        return 0

    vertex = remaining_vertices[0]

    # The number of edges connected to either vertex represents the bridging cross-edges (the cut size).
    return len(graph[vertex])