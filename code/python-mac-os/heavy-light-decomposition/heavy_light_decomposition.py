"""
heavy_light_decomposition.py

A complete, self-contained implementation of Heavy-Light Decomposition (HLD)
integrated with a Segment Tree backend using primitive Python lists.

This module provides a powerful tree-linearization infrastructure designed to
handle path-based updates and maximum value queries over a tree layout. By
decomposing a tree structure into a series of disjoint, contiguous heavy chains
and arbitrary light paths, this design optimizes worst-case path queries from a
naive linear cost down to an efficient logarithmic time complexity of O(log^2 n).

Design Note:
    This architecture prioritizes memory continuity and structural clarity over
    abstract object graphs. It computes subtree metrics via an structural discovery
    pass, then anchors heavy segments sequentially into an underlying Segment Tree
    to maximize range calculation performance while respecting type and boundary invariants.
"""
from __future__ import annotations

import sys

sys.setrecursionlimit(200000)

class SegmentTree:
    """
    A fixed-size array-backed implementation of a Segment Tree tailored for
    highly optimized Point Update and Range Maximum Query (RMQ) evaluations.
    """

    def __init__(self, size: int):
        """
        Instantiates a zero-initialized Segment Tree bounded by a strict structural size.

        Args:
            size (int): The total linear capacity required by the tree data.
        """
        # Validate baseline input preconditions defensively.
        if not isinstance(size, int):
            raise TypeError("size must be an integer")

        if size <= 0:
            raise ValueError("size must be a positive integer")

        self.size = size
        self.tree = [0] * (4 * size)

    def update(self, node: int, start: int, end: int, index: int, value: int):
        """
        Updates a specific index inside the segment tree footprint with a new primitive value.

        Args:
            node (int): The active tree node tracker index.
            start (int): The lower boundary limit of the segment interval.
            end (int): The upper boundary limit of the segment interval.
            index (int): The targeted array element position to modify.
            value (int): The numerical value payload to store.
        """
        if (not isinstance(node, int)
                or not isinstance(start, int)
                or not isinstance(end, int)
                or not isinstance(index, int)):
            raise TypeError("index must be an integer")

        if not isinstance(value, int):
            raise TypeError("value must be an integer")

        if start == end:
            self.tree[node] = value

            return

        middle = (start + end) // 2

        left_child = 2 * node
        right_child = 2 * node + 1

        if index <= middle:
            self.update(left_child, start, middle, index, value)
        else:
            self.update(right_child, middle + 1, end, index, value)

        self.tree[node] = max(self.tree[left_child], self.tree[right_child])

    def query(self, node: int, start: int, end: int, query_left: int, query_right: int):
        """
        Evaluates the maximal range value within a bounded coordinate query envelope.

        Args:
            node (int): The active tree node tracker index.
            start (int): The lower boundary limit of the current segment interval.
            end (int): The upper boundary limit of the current segment interval.
            query_left (int): The target query range lower limit.
            query_right (int): The target query range upper limit.

        Returns:
            int: The maximum value found within the queried intervals.
        """
        if not isinstance(node, int) or not isinstance(start, int) or not isinstance(end, int):
            raise TypeError("Tree tracking indices must be integers.")

        if not isinstance(query_left, int) or not isinstance(query_right, int):
            raise TypeError("Query range must be an integers.")

        # Range completely missed the segment boundary targets.
        if query_left > end or query_right < start:
            return float('-inf')

        # Range completely encapsulates the active segment block.
        if query_left <= start and end <= query_right:
            return self.tree[node]

        middle = (start + end) // 2

        left_query = self.query(2 * node, start, middle, query_left, query_right)
        right_query = self.query(2 * node + 1, middle + 1, end, query_left, query_right)

        return max(left_query, right_query)

class HeavyLightDecomposition:
    """
    Manages structural metadata discovery, heavy-light array linearization, and path queries
    across an arbitrary tree architecture using parallel metadata vectors.
    """

    def __init__(self, number_of_nodes: int, adjacency_list: dict[int, list[int]]):
        """
        Initializes the HLD context and generates the underlying heavy chains.

        Args:
            number_of_nodes (int): Total number of nodes present within the graph.
            adjacency_list (dict): Node-to-node connectivity description mapping list.
        """
        # Validate baseline graph structural criteria
        if not isinstance(number_of_nodes, int):
            raise TypeError("Number of nodes must be an integer.")

        if number_of_nodes <= 0:
            raise ValueError("Number of nodes must be greater than zero.")

        if not isinstance(adjacency_list, dict):
            raise TypeError("Adjacency list data structure must be a dictionary.")

        self.number_of_nodes = number_of_nodes
        self.adjacency_list = adjacency_list

        # Structural tracking allocation vectors (1-indexed matching graph layout).
        self.parent = [0] * (number_of_nodes + 1)
        self.depth = [0] * (number_of_nodes + 1)
        self.subtree_size = [0] * (number_of_nodes + 1)
        self.heavy_child = [0] * (number_of_nodes + 1)

        # Decomposition tracking vectors.
        self.chain_head = [0] * (number_of_nodes + 1)
        self.segment_tree_position = [0] * (number_of_nodes + 1)

        self.current_position = 1
        self.segment_tree = SegmentTree(number_of_nodes)

        # Run multi-pass structural analyses safely from root node index 1.
        self._dfs_discovery(1, 0, 0)
        self._dfs_chain_decomposition(1, 1)

    def _dfs_discovery(self, root: int, parent: int, depth: int):
        """
        Executes an initial structural discovery sequence to establish node depths, parent
        relationships, and calculate subtree tracking metrics to identify heavy edges.

        Args:
            root (int): Current node context pointer.
            current_parent (int): Intended parent structural index.
            current_depth (int): Absolute layer height distance from system origin root.
        """
        if root not in self.adjacency_list:
            raise ValueError(f"Structural defect: Node indexx '{root}' missing from adjacency map.")

        self.parent[root] = parent
        self.depth[root] = depth
        self.subtree_size[root] = 1

        maximum_subtree_size = -1

        for neighbor in self.adjacency_list[root]:
            if neighbor != parent:
                self._dfs_discovery(neighbor, root, depth + 1)
                self.subtree_size[root] += self.subtree_size[neighbor]

                # Identify heavy child via max child structural weight thresholds.
                if self.subtree_size[neighbor] > maximum_subtree_size:
                    maximum_subtree_size = self.subtree_size[neighbor]

                    self.heavy_child[root] = neighbor

    def _dfs_chain_decomposition(self, root: int, chain_head: int):
        """
        Decomposes the tree structurally into contiguous heavy chains, assigning linear position
        indices inside the Segment Tree layout.

        Args:
            root (int): Current node context pointer.
            current_chain_head (int): Anchor root node for the current heavy chain path.
        """
        self.chain_head[root] = chain_head
        self.segment_tree_position[root] = self.current_position
        self.current_position += 1

        # Process the continuous heavy child chain ahead of all other branches.
        if self.heavy_child[root] != 0:
            self._dfs_chain_decomposition(self.heavy_child[root], chain_head)

        # Spin off new independent heavy path allocations for localized light branches.
        for neighbor in self.adjacency_list[root]:
            if neighbor != self.parent[root] and neighbor != self.heavy_child[root]:
                self._dfs_chain_decomposition(neighbor, neighbor)


    def update_value(self, node: int, value: int):
        """
        Updates a specific tree node index with an integer value inside the segment tree footprint.

        Args:
            node (int): The target graph node identification identifier.
            value (int): The numerical value payload to map.
        """
        if not isinstance(node, int):
            raise TypeError("Node identification index must be an integer.")

        if node < 1 or node > self.number_of_nodes:
            raise ValueError(f"Node identifier '{node}' falls outside valid graph scale definitions.")

        target_position = self.segment_tree_position[node]

        self.segment_tree.update(1, 1, self.number_of_nodes, target_position, value)

    def query_path_maximum(self, start: int, end: int) -> int:
        """
        Traverses complex path connections dynamically across disjoint chain architectures
        to discover the absolute maximum node value bound between start and end.

        Args:
            start (int): Spatial starting target graph node index.
            end (int): Spatial ending target graph node index.

        Returns:
            int: The highest value mapped across the path.
        """
        if not isinstance(start, int) or not isinstance(end, int):
            raise TypeError("Path nodes must be passed as integers.")

        if not (1 <= start <= self.number_of_nodes) or not (1 <= end <= self.number_of_nodes):
            raise ValueError("Target nodes must be bounded within active graph bounds.")

        maximum_value = float('-inf')

        # Leap frog upwards until both indices align into matching heavy chains.
        while self.chain_head[start] != self.chain_head[end]:
            # Always push the node that features the deeper chain head root reference.
            if self.depth[self.chain_head[start]] < self.depth[self.chain_head[end]]:
                start, end = end, start

            head = self.chain_head[start]
            maximum_value = max(maximum_value, self.segment_tree.query(
                1,
                1,
                self.number_of_nodes,
                self.segment_tree_position[head],
                self.segment_tree_position[start]
            ))

            # Move upward beyond the chain anchor head barrier
            start = self.parent[head]

        # Ensure node_u holds the shallower structural depth within the terminal chain sequence
        if self.depth[start] > self.depth[end]:
            start, end = end, start

        maximum_value = max(maximum_value, self.segment_tree.query(
            1,
            1,
            self.number_of_nodes,
            self.segment_tree_position[start],
            self.segment_tree_position[end]
        ))

        return int(maximum_value)