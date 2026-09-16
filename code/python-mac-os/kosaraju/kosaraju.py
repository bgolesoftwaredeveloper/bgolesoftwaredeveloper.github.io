"""
kosaraju_scc.py

An implementation of Kosaraju's linear-time algorithm for identifying all
Strongly Connected Components (SCCs) within a directed graph structure.

This module provides an object-oriented framework for graph modeling and analysis.
It isolates structural data modeling from topological analysis by decoupling
the network's representation from the component resolution solver. The system
evaluates components in $O(V + E)$ time complexity using two discrete depth-first
search (DFS) sweeps linked by a graph inversion phase.

Design Note:
    The architecture relies on standard Python collections, enforcing type integrity
    and element boundary safety during edge registration and analysis phases. It is
    specifically engineered to handle disconnected topologies and multi-node cycles.
"""
from collections import defaultdict

class Graph:
    """
    Represents a directed network structure using a hash-mapped adjacency list.

    Maintains structural constraints, vertex limits, and directed node-to-node
    pathways, enforcing index ranges during construction.
    """

    def __init__(self, size: int):
        """
        Initializes an empty directed graph matrix allocated to a fixed node scale.

        Args:
            size (int): Total number of independent vertices available (nodes indexed 0 to size-1).

        Raises:
            TypeError: If the size parameter is not an primitive integer.
            ValueError: If the allocated size drops below a singular node configuration.
        """
        if not isinstance(size, int):
            raise TypeError("Graph size must be an integer.")

        if size < 1:
            raise ValueError("Graph must contain at least one node.")

        # Total capacity ceiling regulating allowed node indices.
        self.size = size

        # Structural map capturing directed vectors pointing from an origin to successor targets.
        self.adjacency_map = defaultdict(list)

    def add_edge(self, origin: int, target: int):
        """
        Establishes a directed vector pathway connecting an origin vertex to a target vertex.

        Args:
            origin (int): The starting vertex coordinate of the directed vector.
            target (int): The destination vertex coordinate of the directed vector.

        Raises:
            ValueError: If either index configuration spills outside the pre-allocated size bounds.
        """
        # Validate index parameters against boundaries to maintain network structural safety.
        if not (0 <= origin < self.size) or not (0 <= target < self.size):
            raise ValueError(f"Node index out of bounds for graph size {self.size}.")

        # Register the targeted node inside the origin's collection of successors.
        self.adjacency_map[origin].append(target)

class KosarajuSolver:
    """
    An execution engine responsible for managing operational search states, tracking
    topological histories, and extracting strongly connected graph components.
    """

    def __init__(self, graph: Graph):
        """
        Initializes an independent solver session anchored to a target Graph instance.

        Args:
            graph (Graph): The targeted directed network layout to analyze.

        Raises:
            TypeError: If the supplied parameter fails structural object validation checks.
        """
        if not isinstance(graph, Graph):
            raise TypeError("Graph must be an instance of Graph.")

        # The underlying directed network model submitted for evaluation.
        self.graph = graph

        # State tracking register to prevent redundant node processing loops during traversals.
        self.visited = set()

        # Topological finish order sequence tracking the linear exit sequence of nodes during phase one.
        self.order = []

    def _invert_graph(self) -> dict[int, list[int]]:
        """
        Generates a directional mirror image of the underlying graph by flipping all vector paths.

        Returns:
            dict[int, list[int]]: A newly allocated adjacency map containing inverted vector listings.
        """
        inverted_map = defaultdict(list)

        # Iterate across the existing graph architecture to transpose origin-successor links.
        for origin, successors in self.graph.adjacency_map.items():
            for successor in successors:
                # Append the original source node as a successor to its own child node.
                inverted_map[successor].append(origin)

        return inverted_map

    def _leak_sccs_dfs(self, current: int, inverted_map: dict[int, list[int]], accumulator: list[int]):
        """
        Performs a component extraction sweep on the reversed graph using a depth-first traversal.

        This tracking operation floods backwards from a given node, pulling all mutual cluster
        nodes into the active accumulator before leaking out of the component boundaries.

        Args:
            current (int): The target vertex currently undergoing cluster extraction.
            inverted_map (dict[int, list[int]]): Adjacency listing of the transposed network.
            accumulator (list[int]): Linear collection registering components grouped in the current SCC.
        """
        # Lock the current element node inside the session's shared tracking set.
        self.visited.add(current)

        # Store the node index inside the active component collection.
        accumulator.append(current)

        # Recurse across all adjacent targets inside the reversed graph perspective.
        for successor in inverted_map[current]:
            if successor not in self.visited:
                self._leak_sccs_dfs(successor, inverted_map, accumulator)

    def compute_order_dfs(self, current: int, network_map: dict[int, list[int]]):
        """
        Executes an initial topological sweep across a graph tracking node exhaustion sequences.

        Args:
            current (int): The target vertex being explored down its linear branch pathway.
            network_map (dict[int, list[int]]): The targeted graph adjacency map being analyzed.
        """
        # Register the node to block cyclic trace operations.
        self.visited.add(current)

        # Deeply descend through all accessible branching coordinates first.
        for successor in network_map[current]:
            if successor not in self.visited:
                self.compute_order_dfs(successor, network_map)

        # Push the node onto the stack sequence only after exploring all downstream neighbors.
        self.order.append(current)

    def compute_strongly_connected_components(self) -> list[list[int]]:
        """
        Executes Kosaraju's full two-pass routine to compute all Strongly Connected Components.

        Returns:
            list[list[int]]: A list containing structural partitions, where each nested sub-list
                             represents an isolated Strongly Connected Component.
        """
        # Reset operational state registers before triggering.
        self.visited.clear()
        self.order.clear()

        # Phase 1: Determine topological finishing sequence via DFS.
        for node in range(self.graph.size):
            if node not in self.visited:
                self.compute_order_dfs(node, self.graph.adjacency_map)

        # Phase 2: Invert the graph's directional pathways.
        inverted_map = self._invert_graph()

        # Phase 3: Extract components using inverted pathways tracking the finishing order.
        self.visited.clear()

        strongly_connected_components = []

        # Drain the finishing timeline back-to-front to isolate source components sequentially.
        while self.order:
            node = self.order.pop()

            if node not in self.visited:
                current_component = []

                # Trace and isolate the component boundary limits.
                self._leak_sccs_dfs(node, inverted_map, current_component)
                strongly_connected_components.append(current_component)

        return strongly_connected_components