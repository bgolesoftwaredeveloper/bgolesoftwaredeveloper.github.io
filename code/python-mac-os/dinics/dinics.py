"""
dinics.py

An object-oriented implementation of Dinic's algorithm optimized for solving
the Maximum Flow problem within structured network topologies.

This module provides an efficient framework for computing the maximum volume of
commodity flow that can traverse a directed capacity graph from a designated source
to a designated sink. It optimizes classic augmenting path routines by constructing
explicit Level Graphs via Breadth-First Search (BFS) and systematically extracting
blocking flows via Depth-First Search (DFS) with dynamic lookahead edge pruning.

Design Note:
    The implementation encapsulates structural mutations directly inside distinct graph
    and edge abstractions, eliminating cryptographic or abbreviated variables in favor
    of expressive domain descriptors. It features integrated boundary protections against
    index violations and cyclic self-loop loops (where source equals sink).
"""
from collections import deque

class FlowEdge:
    """
    Represents a directed edge within the flow network graph, tracking both its
    maximum capacity and its currently assigned flow payload.
    """

    def __init__(self, origin: int, destination: int, capacity: int):
        """
        Initializes an independent network flow edge with localized residual parameters.

        Args:
            origin (int): Integer identifier of the originating source node.
            destination (int): Integer identifier of the terminating target node.
            capacity (int): The maximum total flow volume allowed across this directional arc.
        """
        self.origin = origin
        self.destination = destination
        self.capacity = capacity
        self.current_flow = 0
        self.reverse_edge = None

    def residual_capacity(self):
        """
        Calculates the remaining capacity available along this edge before physical saturation.

        Returns:
            int: The difference between total structural capacity and current active flow.
        """
        return self.capacity - self.current_flow

class DinicsNetwork:
    """
    An object-oriented implementation of Dinic's Algorithm used to solve the
    Maximum Flow problem in a network graph using Level Graphs (BFS) and Blocking Flows (DFS).
    """

    def __init__(self, total_nodes: int):
        """
        Initializes the flow network graph allocations and adjacency tracking maps.

        Args:
            total_nodes (int): The total number of unique vertices/nodes comprising the network.
        """
        self.total_nodes = total_nodes
        self.adjacency_list: dict[int, list[FlowEdge]] = {index: [] for index in range(total_nodes)}
        self.search_depth: list[int] = []
        self.exploration_pointer: list[int] = []

    def _build_level_graph(self, source, sink):
        """
        Constructs a specialized Level Graph via Breadth-First Search (BFS) to group
        reachable nodes by their exact shortest-path distance step limits from the source.

        Args:
            source (int): The node originating the network flow.
            sink (int): The final target destination node for the network flow.

        Returns:
            bool: True if an unsaturated, valid path to the sink is detected; False otherwise.
        """
        # Reset all search depths to unvisit flags (-1).
        self.search_depth = [-1] * self.total_nodes

        # Define the foundational level of the source node as 0.
        self.search_depth[source] = 0

        # Initialize the FIFO lookup queue containing the origin node.
        queue = deque([source])

        while queue:
            current_node = queue.popleft()

            # Inspect all connected paths extending from the current vertex.
            for edge in self.adjacency_list[current_node]:
                if edge.residual_capacity() > 0 and self.search_depth[edge.destination] == -1:
                    self.search_depth[edge.destination] = self.search_depth[current_node] + 1
                    queue.append(edge.destination)

        # The level graph phase continues as long as the sink remains reachable within the residual bounds.
        return self.search_depth[sink] != -1

    def _push_flow(self, current_node, sink, bottleneck_flow):
        """
        Pushes flow volumes down legitimate level graph channels using a Depth-First Search (DFS).
        Leverages an exploration pointer map to automatically skip completely saturated or dead-end paths.

        Args:
            current_node (int): The node currently being evaluated along the DFS path trajectory.
            sink (int): The ultimate target destination vertex absorbing the flow.
            bottleneck_flow (float | int): The restrictive minimum flow constraint tracking the active path.

        Returns:
            float | int: The actual payload volume successfully transmitted down the target path.
        """
        # Terminate traversal if we reach the sink of the residual.
        if current_node == sink or bottleneck_flow == 0:
            return bottleneck_flow

        # Scan edges from where we last paused to avoid checking spent paths multiple times.
        while self.exploration_pointer[current_node] < len(self.adjacency_list[current_node]):
            edge = self.adjacency_list[current_node][self.exploration_pointer[current_node]]

            # Enforce that the path strictly steps forward exactly one level deeper into the level graph.
            if (self.search_depth[edge.destination] == self.search_depth[current_node] + 1
                    and edge.residual_capacity() > 0):
                # Determine the local bottleneck capacity restriction for the subsequent branch.
                limit = min(bottleneck_flow, edge.residual_capacity())
                pushed_flow = self._push_flow(edge.destination, sink, limit)

                # If flow successfully trickled down, update capacities throughout the residual network.
                if pushed_flow > 0:
                    # Update capacities along the network.
                    edge.current_flow += pushed_flow
                    edge.reverse_edge.current_flow -= pushed_flow

                    return pushed_flow

            # Advance pointer if this edge cannot yield any more flow.
            self.exploration_pointer[current_node] += 1

        return 0

    def add_edge(self, from_node, to_node, capacity):
        """
        Injects a forward directed capacity edge into the network alongside its linked
        zero-capacity backward residual companion edge.

        Args:
            from_node (int): The source or originating vertex node index.
            to_node (int): The destination or target vertex node index.
            capacity (int): Total directional flow volume allowed over this link.
        """
        # Construct the explicit forward and backward residual edge models.
        forward_edge = FlowEdge(from_node, to_node, capacity)
        backward_edge = FlowEdge(to_node, from_node, 0)

        # Interlink both edge instances to allow immediate O(1) state coordination during updates.
        forward_edge.reverse_edge = backward_edge
        backward_edge.reverse_edge = forward_edge

        # Append the matching structural edges directly into the respective node adjacency records.
        self.adjacency_list[from_node].append(forward_edge)
        self.adjacency_list[to_node].append(backward_edge)

    def compute_maximum_flow(self, source, sink):
        """
        Executes Dinic's entire algorithm loop pipeline to solve for the maximum achievable
        network flow volume transiting between the source and sink.

        Args:
            source (int): The node index originating the network flow.
            sink (int): The final target destination node index.

        Returns:
            float | int: The total maximized volume of flow successfully pushed across the entire network.

        Raises:
            ValueError: If either the source or sink parameters step outside of valid vertex index ranges.
        """
        # Block initialization if index assertions map outside the allocated topological graph constraints.
        if not (0 <= source < self.total_nodes) or not (0 <= sink < self.total_nodes):
            raise ValueError("Source or sink node index is out of bounds.")

        # Intercept and fast-path trivial cycles where the source and target converge.
        if source == sink:
            return 0

        # Accumulated system maximum flow tracker metric.
        total_maximum_flow = 0

        # Continuously generate fresh level graphs as long as an unsaturated path to the sink exists.
        while self._build_level_graph(source, sink):
            # Reset all vertex exploration trackers to 0 at the start of a brand-new blocking flow phase.
            self.exploration_pointer = [0] * self.total_nodes

            # Exhaustively look for augmenting paths inside the current level graph layout.
            while True:
                pushed = self._push_flow(source, sink, float("inf"))

                # Break out of the current phase once no more blocking flow can be drawn.
                if pushed == 0:
                    break

                # Aggregate the successfully derived pathway volumes into the global sum.
                total_maximum_flow += pushed

        return total_maximum_flow