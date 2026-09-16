"""
edmonds_blossom.py

An implementation of Edmonds' Blossom Algorithm for finding a maximum cardinality
matching in general undirected graphs.

This module provides structural workflows to resolve the "odd-cycle" bottleneck
inherent to general graphs by dynamically contracting and lifting non-bipartite
cycles (blossoms). It handles paths via a Breadth-First Search (BFS) topology,
leveraging an optimized Union-Find (Disjoint-Set Forest) construct to track
nested structural components in polynomial time.

Design Note:
    This implementation is optimized for algorithmic clarity and structural reference.
    Per structural design constraints, loop controls and sequence trackers utilize
    index ('I') representations, while graph states and component tracking are isolated
    within operational context ('ctx') objects.
"""

class BlossomContext:
    """
    Encapsulates the global state, queue tracking metrics, and matching registries
    for the graph environment. Acts as the structural context ('ctx').
    """

    def __init__(self, number_of_vertices: int):
        """
        Initializes an independent structural context for maximum matching computation.

        Args:
            number_of_vertices (int): Total node volume defining the tracking arrays.
        """
        # The total vertex allocation bounding the structural matrix.
        self.size = number_of_vertices

        # Registry pairing vertices to their matching mates. -1 implies an unmatched state.
        self.mate = [-1] * number_of_vertices

        # Traversal tags indicating the parent/ancestor origin during path exploration.
        self.label = [-1] * number_of_vertices

        # Disjoint-set pointer mapping elements to parent nodes inside contracted blossoms.
        self.parent = list(range(number_of_vertices))

        # Tracks the base vertex (the root) of a contracted pseudo-vertex blossom.
        self.base = list(range(number_of_vertices))

        # Memory queue for managing the Breadth-First Search (BFS) frontier.
        self.queue = []

    def find(self, vertex: int) -> int:
        """
        Resolves the root representative of a contracted component using Path Compression.

        Args:
            vertex (int): The target vertex index evaluated by the disjoint forest.

        Returns:
            int: The primary root identifier holding the compressed set.
        """
        # Ensure vertex index is within bounds before processing.
        if vertex < 0 or vertex >= self.size:
            return -1

        # Locate the root in the parent pointer hierarchy.
        root = vertex

        while root != self.parent[root]:
            root = self.parent[root]

        # Apply path compression: Flatten the tree structure for O(a(N)) performance.
        current = vertex

        while current != root:
            next = self.parent[current]
            self.parent[current] = root

            current = next

        return root

    def union(self, source: int, destination: int):
        """
        Merges two independent blossom substructures inside the disjoint-set forest.

        Args:
            source (int): The index of the first vertex set member.
            destination (int): The index of the second vertex set member.
        """
        # Find the representatives (roots) of both sets.
        source_root = self.find(source)
        destination_root = self.find(destination)

        # Connect the roots if they currently belong to different sets.
        if source_root != destination_root:
            self.parent[source_root] = destination_root

def lowest_common_ancestor(context: BlossomContext, source: int, destination: int) -> int:
    r"""
    Traces alternating tree paths backward to identify the root of an odd cycle (blossom).

    Mathematical Formula:
        Traces paths simultaneously: $P(\text{source}) \cap P(\text{destination}) = \text{base}(B)$

    Args:
        context (BlossomContext): The active configuration and tracking state.
        source (int): Index of the first conflicting edge node.
        destination (int): Index of the second conflicting edge node.

    Returns:
        int: The vertex index acting as the blossom base.
    """
    # Track nodes visited while climbing the alternating tree.
    visited = [False] * context.size

    # Traverse upward until the lowest common ancestor (LCA) is found.
    while True:
        # Step source upward if it hasn't reached an unmatched tree root.
        if source != -1:
            source = context.find(source)

            if visited[source]:
                return source

            visited[source] = True

            # Move to the mate, then the parent, respecting blossom contraction bases.
            if context.mate[source] == -1:
                source = -1
            else:
                source = context.base[context.find(context.mate[source])]

        # Step destination upward if it hasn't reached an unmatched tree root.
        if destination != -1:
            destination = context.find(destination)

            if visited[destination]:
                return destination

            visited[destination] = True

            # Move to the mate, then the parent, respecting blossom contraction bases.
            if context.mate[destination] == -1:
                destination = -1
            else:
                destination = context.base[context.find(context.mate[destination])]


def blossom_contraction(context: BlossomContext, source: int, destination: int, root: int):
    """
    Contracts an odd-length cycle into a singular pseudo-vertex structure.

    Args:
        context (BlossomContext): The active configuration context.
        source (int): Index marking one side of the blossom base.
        destination (int): Index marking the opposite side of the blossom base.
        root (int): The shared lowest common ancestor root of the cycle.
    """

    def trace_blossom_half(node: int, next_vertex: int):
        """Iterates through a single side of the cycle to remap labels and queues."""
        current = context.find(node)

        while current != root:
            # 1. Update the label of the current blossom base.
            context.label[current] = next_vertex

            # 2. Add the mate to the queue to explore further.
            next_node = context.mate[current]

            if context.label[context.find(next_node)] != 0:
                context.label[next_node] = 0
                context.queue.append(next_node)

            # 3. Perform union to collapse the blossom.
            context.union(current, root)

            # 4. Move to the next node in the cycle (the mate of the mate).
            current = context.find(context.label[next_node])

    # Compress both structural arms of the odd-cycle blossom.
    trace_blossom_half(source, destination)
    trace_blossom_half(destination, source)

    context.base[root] = root

def augment_path(context: BlossomContext, initial: int, graph: dict[int, list[int]]) -> bool:
    """
    Searches for an augmenting path using an alternating BFS discovery scheme.

    Args:
        context (BlossomContext): The active environment tracking fields.
        initial (int): The root unmatched index initiating the path discovery.
        graph (dict): Adjacency listing mapping integers to connected index pools.

    Returns:
        bool: True if an augmenting path is found and matching is expanded, else False.
    """
    # Reset path-tracking metrics for new BFS attempt.
    context.label = [-1] * context.size
    context.parent = list(range(context.size))
    context.base = list(range(context.size))
    context.queue = [initial]
    context.label[initial] = 0

    while context.queue:
        current_vertex = context.queue.pop(0)

        if current_vertex == -1:
            continue

        for adjacent_vertex in graph[current_vertex]:
            # Ignore self-loops or loops within an identical contracted blossom base.
            if context.base[context.find(current_vertex)] == context.base[context.find(adjacent_vertex)]:
                continue

            if context.mate[current_vertex] == adjacent_vertex:
                continue

            # Check for blossom conditions: An edge connecting two outer (Even) nodes.
            if adjacent_vertex == initial or (context.label[context.base[context.find(adjacent_vertex)]] == 0):
                base_root = lowest_common_ancestor(context, current_vertex, adjacent_vertex)
                blossom_contraction(context, current_vertex, adjacent_vertex, base_root)
            elif context.label[adjacent_vertex] == -1:
                context.label[adjacent_vertex] = current_vertex

                # Check if an augmenting path has been successfully terminated.
                if context.mate[adjacent_vertex] == -1:
                    current_node = adjacent_vertex
                    seen_nodes = set()

                    # Backtracking through the labels to flip the matching edges along the path.
                    while current_node != -1:
                        if current_node in seen_nodes:
                            break

                        seen_nodes.add(current_node)

                        next_hop = context.label[current_node]

                        temp_mate = context.mate[next_hop]

                        context.mate[current_node] = next_hop
                        context.mate[next_hop] = current_node

                        current_node = temp_mate

                    return True
                else:
                    # Alternating extension: push the mate onto the discovery queue.
                    context.label[context.mate[adjacent_vertex]] = adjacent_vertex
                    context.queue.append(context.mate[adjacent_vertex])

    return False

def find_maximum_matching(graph: dict[int, list[int]], number_of_vertices: int) -> list[tuple[int, int]]:
    """
    Executes Edmonds' Blossom matching logic across the entirety of a general graph.

    Optimization Objective:
        Finds a matching $M$ of maximum cardinality $|M|$ such that no two edges share a vertex.

    Args:
        graph (dict): Adjacency dictionary representing the structural topology.
        number_of_vertices (int): Total quantity of vertices in the graph system.

    Returns:
        list[tuple[int, int]]: A collection of paired tuples indicating the maximum matching configuration.
    """
    # Sanitize input: Ensure graph nodes are within the defined bounds.
    sanitized_graph = {vertex: [] for vertex in range(number_of_vertices)}

    for vertex, neighbors in graph.items():
        if vertex < number_of_vertices:
            sanitized_graph[vertex] = [node for node in neighbors if node != vertex and node < number_of_vertices]

    context = BlossomContext(number_of_vertices)

    # Iterate through each vertex index to trigger path expansions.
    for vertex in range(context.size):
        if context.mate[vertex] == -1:
            augment_path(context, vertex, sanitized_graph)

    # Parse context mapping registry to compile independent pair coordinates.
    matching = []

    # Aggregate and return the final list of matched vertex pairs.
    for vertex in range(context.size):
        if context.mate[vertex] > vertex:
            matching.append((vertex, context.mate[vertex]))

    return matching