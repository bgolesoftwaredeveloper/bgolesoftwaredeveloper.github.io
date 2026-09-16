"""
eulerian_path.py

An implementation of Hierholzer's Algorithm for identifying Eulerian Paths and
Circuits within a directed graph network.

This module provides structural workflows to analyze node degree distributions,
validate Eulerian properties, and reconstruct the exact edge-traversal sequence.
The process is executed in linear time $O(V + E)$ by tracing sub-cycles and
interlocking them via a stack-based depth-first approach, ensuring every
edge is consumed exactly once.

Design Note:
    This implementation is optimized for algorithmic clarity and structural reference.
    Graph definitions utilize an adjacency list map tracking explicit destinations,
    while destructive edge-popping ensures distinct trails are resolved without
    re-evaluating exhausted transitions.
"""
from collections import defaultdict, deque

def eulerian_path_or_circuit(adjacency_list: dict) -> list | None:
    """
    Evaluates a directed graph topology to resolve and return its Eulerian trail.

    Mathematical Traversal:
        Verifies degree balanced constraints across all active boundaries before
        reconstructing a continuous sequence using a localized Hierholzer loop scheme.

    Args:
        adjacency_list (dict): A dictionary mapping source nodes to lists of
          destination nodes.

    Returns:
        list: A sequence of vertices detailing the complete structural path, or
        None if structural traits fail configuration laws.
    """
    # Track incoming and outgoing edge counts to evaluate graph layout integrity.
    in_degree = defaultdict(int)
    out_degree = defaultdict(int)

    # Compile the comprehensive structural frontier.
    all_nodes = set(adjacency_list.keys())

    for destinations in adjacency_list.values():
        for vertex in destinations:
            all_nodes.add(vertex)

    total_edges = 0

    for vertex, destinations in adjacency_list.items():
        out_degree[vertex] = len(destinations)
        total_edges += len(destinations)

        for vertex in destinations:
            in_degree[vertex] += 1

    # If the graph topology has no structural edges, return an empty trail layout.
    if total_edges == 0:
        return []

    # Structural tracking lists to locate the entry and exit points of an asymmetric path.
    initial_candidates = []
    end_candidates = []

    balanced_node_count = 0

    # Evaluate individual node balances to determine if the graph is Eulerian.
    for current in all_nodes:
        out_count = out_degree.get(current, 0)
        in_count = in_degree.get(current, 0)

        if out_count - in_count == 1:
            initial_candidates.append(current)
        elif in_count - out_count == 1:
            end_candidates.append(current)
        elif out_count == in_count:
            balanced_node_count += 1
        else:
            # Asymmetry breaks both Path and Circuit topology constraints.
            return None

    total_nodes = len(all_nodes)

    initial_node = None

    # Assign traversal entry nodes depending on the confirmed topological layout.
    if balanced_node_count == total_nodes:
        # Eulerian Circuit: Traversal can begin at any node with active outgoing vectors.
        try:
            initial_node = next(junction for junction in adjacency_list if out_degree.get(junction, 0) > 0)
        except StopIteration:
            return []
    elif len(initial_candidates) == 1 and len(end_candidates) == 1 and balanced_node_count == total_nodes - 2:
        # Eulerian Path: Traversal must initiate precisely at the net source node.
        initial_node = initial_candidates[0]
    else:
        # Asymmetry mismatch across candidates invalidates execution.
        return None

    # Construct mutable edge registries to safely pop elements during inspection.
    remaining_edges = {node: deque() for node in all_nodes}

    for vertex, destinations in adjacency_list.items():
        remaining_edges[vertex] = deque(destinations)

    # Backtracking stack for managing the Hierholzer exploration path.
    stack = [initial_node]

    # Double-ended queue for stitching finalized cycles into structural alignment.
    trail = deque()

    while stack:
        current = stack[-1]

        # If unvisited paths exit the current vertex, push deeper along the branch.
        if remaining_edges[current]:
            next_node = remaining_edges[current].popleft()
            stack.append(next_node)
        else:
            # If a terminal bottleneck is encountered, shift the vertex to the output trail.
            trail.appendleft(stack.pop())

    # Ensure all distinct edges were visited and the graph was fully connected.
    if len(trail) == total_edges + 1:
        return list(trail)
    else:
        return None