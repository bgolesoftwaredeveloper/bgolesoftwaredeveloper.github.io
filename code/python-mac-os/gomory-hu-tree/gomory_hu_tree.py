r"""
gomory_hu.py

An implementation of the Gomory–Hu tree algorithm, which provides a compact
representation of all-pairs minimum cuts in an undirected, weighted graph.

This module leverages the Edmonds-Karp algorithm to iteratively compute
s-t cuts, using the resulting partitions to refine the tree structure. The
resulting Gomory–Hu tree guarantees that the minimum capacity path between any
two nodes in the tree equals the minimum cut capacity between them in the
original graph.

Design Note:
    The construction requires $N-1$ max-flow computations, where $N$ is the number
    of vertices. It is optimized for structural analysis of network connectivity
    and bottleneck identification.
"""
from collections import deque

class GomoryHuTree:
    """
    Encapsulates the construction of a Gomory-Hu tree from an undirected,
    weighted adjacency matrix.
    """

    def __init__(self, matrix: list[list[int]]):
        """
        Initializes the GomoryHuTree with a symmetric capacity matrix.
        """
        self.matrix = matrix
        self.number_of_nodes = len(matrix)

    def _find_augmenting_path(self, graph: list[list[int]], source: int, sink: int, parent_map: dict[int, int]) -> bool:
        """
        Performs a Breadth-First Search (BFS) to find an augmenting path from source
        to sink in the residual graph.

        Args:
            graph list[list[int]]: The current residual capacity adjacency matrix.
            source (int): The starting node index.
            sink (int): The destination node index.
            parent_map dict[int, int]: A map to store predecessors for path reconstruction.

        Returns:
            bool: True if an augmenting path exists, False otherwise.
        """
        visited = {source}

        queue = deque([source])

        while queue:
            current = queue.popleft()

            # Check all possible neighboring nodes.
            for neighbor, capacity in enumerate(graph[current]):
                # Path must have remaining capacity and not be visited.
                if neighbor not in visited and capacity > 0:
                    parent_map[neighbor] = current
                    visited.add(neighbor)

                    # Early exit if we find the sink node.
                    if neighbor == sink:
                        return True

                    queue.append(neighbor)

        return False

    def _retreive_minimum_cut(self, matrix: list[list[int]], source: int, sink: int) -> tuple[int, list[int]]:
        """
        Computes the maximum flow and the corresponding minimum cut partition
        using the Edmonds-Karp algorithm.

        Args:
            matrix list[list[int]]: The capacity adjacency matrix of the graph.
            source (int): The source node index.
            sink (int): The sink node index.

        Returns:
            tuple[int, list[int]]: A tuple containing the maximum flow value and a
            list of nodes belonging to the source-side partition of the min-cut.
        """
        residual_graph = [row[:] for row in matrix]
        parent_map = {}
        maximum_flow = 0

        # Continue augmenting flow as long as a path exists from source to sink.
        while self._find_augmenting_path(residual_graph, source, sink, parent_map):
            path_flow = float("inf")

            current = sink

            # Determine the bottleneck capacity along the path found by BFS.
            while current != source:
                previous = parent_map[current]
                path_flow = min(path_flow, residual_graph[previous][current])

                current = previous

            # Accumulate the total flow.
            maximum_flow += int(path_flow)
            current = sink

            # Update residual capacities along the path and its reverse edges.
            while current != source:
                previous = parent_map[current]

                residual_graph[previous][current] -= int(path_flow)
                residual_graph[current][previous] += int(path_flow)

                current = previous

            # Reset the path map for the next augmentation iteration.
            parent_map.clear()

        # The minimum-cut partition consists of all nodes reachable from source in residual graph.
        reachable_nodes = []
        queue = deque([source])
        visited = {source}

        while queue:
            current = queue.popleft()
            reachable_nodes.append(current)

            for vertex, capacity in enumerate(residual_graph[current]):
                if vertex not in visited and capacity > 0:
                    visited.add(vertex)
                    queue.append(vertex)

        return int(maximum_flow), reachable_nodes

    def construct(self, matrix: list[list[int]]) -> dict[int, list[tuple[int, int]]]:
        """
        Constructs a Gomory-Hu tree from a symmetric, weighted capacity matrix.

        Args:
            matrix list[list[int]]: A square, symmetric adjacency matrix representing
                                    edge capacities.

        Returns:
            dict[int, list[tuple[int, int]]]: An adjacency list representing the
            resulting tree, where edges store the weight of the minimum cut.
        """
        # Tree adjacency maps each node to a list of (connected node, weight).
        tree_adjacency = {index: [] for index in range(self.number_of_nodes)}

        # Initialize parent mapping. Initially all nodes are connected to node 0.
        parent_mapping = [0] * self.number_of_nodes

        # The Gomory-Hu algorithm iterates through each node 1..N-1.
        for current in range(1, self.number_of_nodes):
            source = current
            sink = parent_mapping[current]

            # Calculate the actual minimum-cut between current node and its current tree parent.
            minimum_cut_value, source_side_nodes = self._retreive_minimum_cut(matrix, source, sink)

            # Refine tree structure by re-parenting nodes based on the cut partition.
            for node in range(current + 1, self.number_of_nodes):
                if parent_mapping[node] == sink and node in source_side_nodes:
                    parent_mapping[node] = source

            # Add the computed minimum-cut edge to the Gomory-Hu tree structure.
            tree_adjacency[source].append((sink, minimum_cut_value))
            tree_adjacency[sink].append((source, minimum_cut_value))

        return tree_adjacency