"""
segment_tree.py

A manual array-partitioning and associative query acceleration structure designed to implement
a Segment Tree class directly from fundamental indexing primitives without relying on external
geometric or visual rendering hierarchies.

This module provides a standalone, modular class executing discrete range aggregation operations.
It establishes a balanced binary tree topology inside a linear 1D list structure, handles
isolated node-merging mechanics, and coordinates recursive divide-and-conquer processing
pipelines to optimize arbitrary subset lookups and mutable value injections.

Design Note:
    This implementation prioritizes architectural transparency and explicit algorithmic mapping.
    It leverages in-memory explicit partitioning bounds and 0-based array index manipulation,
    optimizing for isolated educational modeling and algorithmic auditing over native C-optimized
    interval-tree structures.
"""


class SegmentTree:
    """
    Manages a balanced binary tree layout mapped onto a flat linear array to achieve sub-linear
    range query evaluation and single-element update intervals.

    Partitions data segments recursively into discrete halves, maintaining associative aggregates
    across logarithmic tree layers.
    """

    def __init__(self, data: list):
        """
        Initializes the tree topology and builds internal cache spaces matching the input data set.

        Args:
            data (list): The raw sequence of starting numeric values to be encapsulated.
        """
        self.size = len(data)
        self.data = data

        # Allocate flat structural space; worst-case pointer representation requires up to 4n slots.
        self.tree = [0] * (4 * self.size)

        if self.size > 0:
            # Initiate the cascading recursive tree-generation phase starting at the root index.
            self._build(0, 0, self.size - 1)

    def _merge(self, left: int, right: int) -> int:
        """
        Combines two evaluation parameters using the underlying associative operation.

        Args:
            left (int): The current aggregated value of the left sibling node.
            right (int): The current aggregated value of the right sibling node.

        Returns:
            int: The combined composite state value.
        """
        # Design Note: Alter this operator logic to transition between Sum, Min, or Max query trees.
        return left + right

    def _build(self, node: int, start: int, end: int):
        """
        Recursively constructs the tree node hierarchy across the data segment space.

        Args:
            node (int): The current active structural slot index within the internal 1D tree array.
            start (int): The inclusive left array-index boundary the active node is responsible for.
            end (int): The inclusive right array-index boundary the active node is responsible for.
        """
        # Base Case: When the boundaries collapse onto a single element, a leaf node is established.
        if start == end:
            self.tree[node] = self.data[start]
            return

        # Midpoint calculation designed to isolate and prevent localized integer overflow thresholds.
        middle = start + (end - start) // 2

        # 0-based binary heap indexing formulas to find subsequent memory address offsets.
        left_child = 2 * node + 1
        right_child = 2 * node + 2

        # Populate subordinate left and right sub-allocations.
        self._build(left_child, start, middle)
        self._build(right_child, middle + 1, end)

        # Pull computed results upwards to form the current parent node's composite data value.
        self.tree[node] = self._merge(self.tree[left_child], self.tree[right_child])

    def _query(self, node: int, start: int, end: int, left: int, right: int) -> int:
        """
        Internal recursive worker executing subset interval isolation over the segmented space.

        Args:
            node (int): The active tree index layout coordinate.
            start (int): The inclusive left array-index boundary managed by the active node.
            end (int): The inclusive right array-index boundary managed by the active node.
            left (int): The absolute left index marker of the target evaluation window.
            right (int): The absolute right index marker of the target evaluation window.

        Returns:
            int: The isolated target aggregate segment result.
        """
        # Scenario 1: Total Overlap - Node boundary lies entirely within target query range.
        if left <= start and end <= right:
            return self.tree[node]

        # Scenario 2: Zero Overlap - Node boundary falls completely outside target query range.
        if end < left or start > right:
            return 0  # Returns the mathematical identity element (0 for Sum, infinity for Min).

        # Scenario 3: Partial Overlap - Intersecting boundaries demand deep subtree evaluation.
        mid = start + (end - start) // 2

        left_child = 2 * node + 1
        right_child = 2 * node + 2

        # Subdivide queries recursively downward through both children branches.
        left_result = self._query(left_child, start, mid, left, right)
        right_result = self._query(right_child, mid + 1, end, left, right)

        # Re-merge branching fractions together to fulfill intermediate structural layer returns.
        return self._merge(left_result, right_result)

    def _update(self, node: int, start: int, end: int, index: int, value: int):
        """
        Internal recursive worker traversing target node vectors to mutate a singular point data cell.

        Args:
            node (int): The active tree index layout coordinate.
            start (int): The inclusive left array-index boundary managed by the active node.
            end (int): The inclusive right array-index boundary managed by the active node.
            index (int): The target position index in the underlying original array being updated.
            value (int): The brand-new value replacement mapping.
        """
        # Tree search path terminates exactly at the matching atomic leaf cell node.
        if start == end:
            self.tree[node] = value
            return

        middle = start + (end - start) // 2

        left_child = 2 * node + 1
        right_child = 2 * node + 2

        # Direct search routing to target left or right child paths based on index orientation.
        if start <= index <= middle:
            self._update(left_child, start, middle, index, value)
        else:
            self._update(right_child, middle + 1, end, index, value)

        # Force a cascading upward synchronization layer step to update parent components.
        self.tree[node] = self._merge(self.tree[left_child], self.tree[right_child])

    def query(self, left: int, right: int) -> int:
        """
        Public wrapper to safely query the range [left, right] (inclusive).

        Args:
            left (int): The absolute lower index constraint to be fetched.
            right (int): The absolute upper index constraint to be fetched.

        Returns:
            int: The calculated associative value spanning the requested sequence slice.
        """
        # Safeguard boundaries to deflect negative index wrapped queries or inverse tracking layouts.
        if left < 0 or right >= self.size or left > right:
            return 0

        return self._query(0, 0, self.size - 1, left, right)

    def update(self, index: int, value: int):
        """
        Public wrapper to safely update a single element at a given index.

        Args:
            index (int): The relative data array positioning slot designated for modification.
            value (int): The brand-new replacement tracking value.
        """
        # Guarantee memory boundaries remain stable prior to parsing internal modification steps.
        if 0 <= index < self.size:
            self._update(0, 0, self.size - 1, index, value)
            self.data[index] = value