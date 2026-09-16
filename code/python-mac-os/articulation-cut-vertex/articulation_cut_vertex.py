"""
articulation_cut_vertex.py

An implementation of Tarjan's algorithm for identifying articulation points
(cut vertices) within an undirected, unweighted network topology.

This module provides an object-oriented framework for discovering critical structural
bottlenecks in graph representations. An articulation point is defined as any vertex
whose removal increases the number of connected components in the graph. The discovery
is accomplished in linear time, $O(V + E)$, by leveraging a specialized Depth-First
Search (DFS) traversal that tracks discovery times and lowest backward-reachable tree
ancestors.

Design Note:
    This engine utilizes standard adjacency list representations based on Python's native
    collections module. It accounts for edge cases including disconnected components,
    self-loops, multi-edges, and root-node evaluation anomalies.
"""
from collections import defaultdict

class TopologicalNetwork:
    """
    An analytical engine responsible for managing structural graph properties, executing
    depth-first topological traversals, and evaluating network articulation metrics.
    """

    def __init__(self):
        """
        Initializes an empty network graph topology with isolated discovery configurations.
        """
        self.adjacency_list = defaultdict(list)
        self.time = 0

    def add_edge(self, source: int, destination: int):
        """
        Populates the underlying adjacency structures by adding an unweighted,
        undirected relationship between two operational nodes.

        Args:
            source (int): The identifier matching the origin vertex parameter.
            destination (int): The identifier matching the target vertex parameter.
        """
        if not isinstance(source, int) or isinstance(source, bool):
            raise TypeError("Source node identifier must be an integer primitive.")
        if not isinstance(destination, int) or isinstance(destination, bool):
            raise TypeError("Destination node identifier must be an integer primitive.")

        if source == destination:
            raise ValueError("Self-loops are mathematically invalid for articulation point analysis.")

        if destination in self.adjacency_list[source]:
            return

        self.adjacency_list[source].append(destination)
        self.adjacency_list[destination].append(source)

    def _find_cut_vertices(self,
                           current_node: int,
                           visited: dict,
                           discovery_time: dict,
                           lowest_reachable_time: dict,
                           parent_nodes: dict,
                           cut_vertices: set):
        """
        Executes a recursive Depth-First Search subroutine to compute component low-points
        and evaluate graph articulation boundaries.

        Args:
            current_node (int): The active vertex identifier under traversal analysis.
            visited (dict): A mapping tracking the visitation state of all system nodes.
            discovery_time (dict): The absolute timestamp tracking when a node was first evaluated.
            lowest_reachable_time (dict): The minimum discovery timestamp accessible from the node
                                          via a single backward tree edge traversal.
            parent_nodes (dict): Operational tree pointers tracing back to the DFS ancestor nodes.
            cut_vertices (set): A collection mutated in-place to store confirmed structural cut points.
        """
        # Local counter tracing immediate child paths stemming from the current node in the DFS tree.
        children_count = 0

        visited[current_node] = True
        discovery_time[current_node] = self.time
        lowest_reachable_time[current_node] = self.time

        self.time += 1

        # Iterate sequentially over adjacent neighbor vertices.
        for neighbor in self.adjacency_list[current_node]:\
            # Guard against immediate parent backtracking loops.
            if neighbor == parent_nodes[current_node]:
                continue

            if visited[neighbor]:
                # If the node has already been visited, update the minimum reachable boundary tracker.
                lowest_reachable_time[current_node] = min(lowest_reachable_time[current_node], discovery_time[neighbor])
            else:
                # Assign parental bounds and scale the current tree depth parameters.
                parent_nodes[neighbor] = current_node
                children_count += 1

                # Recursively descend further down the execution tree.
                self._find_cut_vertices(neighbor,
                                        visited,
                                        discovery_time,
                                        lowest_reachable_time,
                                        parent_nodes,
                                        cut_vertices)

                # Using the ancestral discovery coordinates (Back-edge evaluation).
                lowest_reachable_time[current_node] = min(lowest_reachable_time[current_node],
                                                          lowest_reachable_time[neighbor])

                # Root node evaluation. If the node has no parent and generates multiple split subtrees,
                # it constitutes an articulation bottleneck.
                if parent_nodes[current_node] is None and children_count > 1:
                    cut_vertices.add(current_node)

                # Non-root node evaluation. If the child subtree low-time matches or exceeds the discovery timeline
                # marker of the parent, isolating the parent disconnects the child.
                if (parent_nodes[current_node] is not None
                        and lowest_reachable_time[neighbor] >= discovery_time[current_node]):
                    cut_vertices.add(current_node)

    def retrieve_articulation_points(self) -> set:
        """
        Finds and extracts all articulation points across all connected components
        within the configured network topology.

        Returns:
            set[int]: A unique set of node identifiers representing confirmed graph cut vertices.
        """
        visited = {node: False for node in self.adjacency_list}
        discovery_time = {node: float("inf") for node in self.adjacency_list}
        lowest_reachable_time = {node: float("inf") for node in self.adjacency_list}
        parent_nodes = {node: None for node in self.adjacency_list}
        cut_vertices = set()

        # Reset global execution timeline context.
        self.time = 0

        # Loop guarantees isolated sub-graphs / disconnected elements are evaluated systematically.
        for node in list(self.adjacency_list.keys()):
            if not visited[node]:
                self._find_cut_vertices(node,
                                        visited,
                                        discovery_time,
                                        lowest_reachable_time,
                                        parent_nodes,
                                        cut_vertices)

        return cut_vertices