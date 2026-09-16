"""
kd_tree.py

A complete, self-contained implementation of a K-Dimensional Tree (K-D Tree)
built using foundational pointer structures and dynamic node splits in Python.

This module provides an optimized spatial-partitioning data structure used for
organizing points in a k-dimensional space. It splits coordinates alternatingly
across orthogonal axes at each depth level, minimizing search windows for geometric
queries. This implementation features point insertion, structural visualization,
and an optimized Nearest Neighbor Search (NNS) that trims branches via hypersphere-
hyperplane intersection checks, bounding average lookup performance to O(log N).
"""
from __future__ import annotations

import math

class Node:
    """
    Represents an isolated spatial spatial node bound to a physical multi-dimensional
    point payload container inside the recursive split architecture.
    """

    def __init__(self, point: tuple[float, ...], value: str):
        """
        Initializes a node instance pairing an explicit geometric point coordinate tuple
        with its mapped string payload data.
        """
        # Type enforcement: Ensure coordinates are wrapped in a tuple of numbers.
        if not isinstance(point, tuple) or not all(isinstance(x, (int, float)) for x in point):
            raise TypeError("Point must be a tuple of numerical values.")

        # Type enforcement: Ensure the associated payload is a string.
        if not isinstance(value, str):
            raise TypeError("Value must be a string.")

        # Cast all coordinate elements to floats to maintain precision consistency.
        self.point = tuple(float(x) for x in point)

        self.value = value

        # Initialize hyperplane sub-branches as empty points.
        self.left: Node | None = None
        self.right: Node | None = None

class KDTree:
    """
    Manages structural hyperplanes, recursive splitting dimensions, and spatial bounding
    checks to maintain K-Dimensional tree invariants across points.
    """

    def __init__(self, k: int):
        """
        Instantiates a K-Dimensional tree configured with an explicit dimensional bound.
        """
        # Validate that dimensionality parameter is an integer.
        if not isinstance(k, int):
            raise TypeError("k must be a positive integer.")

        # The tree must operate in a least a 1-Dimensional space.
        if k < 1:
            raise ValueError("Dimensions (k) must be at least 1.")

        self.k = k
        self.root: Node | None = None

    def _euclidean_distance(self, source: tuple[float, ...], target: tuple[float, ...]) -> float:
        """
        Computes standard Euclidean distance between two n-dimensional points.
        """
        # Zip elements together, sum the squared deltas, and compute the square root.
        return math.sqrt(sum((a - b) ** 2 for a, b in zip(source, target)))

    def insert(self, point: tuple[float, ...], value: str) -> None:
        """
        Inserts a unique spatial coordinate-value pair into the tree architecture.
        """
        # Spatial invariant check: The input point must match the established dimensions of the tree.
        if len(point) != self.k:
            raise ValueError("Point dimension mismatch.")

        def _insert_recursive(node: Node | None, depth: int) -> Node:
            """
            Recursively traverses tree layers along alternating dimensions to establish a new leaf
            node or update an identical existing point in place.

            Args:
                node (Node | None): The current branch sub-root node evaluated during traversal.
                depth (int): The current depth tracker layer used to deduce the sorting split axis.

            Returns:
                Node: The updated structural link subtree pointer reference.
            """
            # Base case: An empty pointer slot has been found; plant the new node here.
            if node is None:
                return Node(point, value)

            # Cycle the splitting axis based on current depth.
            axis = depth % self.k

            # Compare target coordinate against the current node's coordinate along the active axis.
            if point[axis] < node.point[axis]:
                # Coordinate is smaller; recursively branch into the left hyperplane.
                node.left = _insert_recursive(node.left, depth + 1)
            else:
                # If all coordinates match perfectly, overwrite the current payload value.
                if point == node.point:
                    node.value = value
                else:
                    # Coordinate is larger or equal on this axis; branch into the right hyperplane.
                    node.right = _insert_recursive(node.right, depth + 1)

            return node

        # Kickstart the insertion from the root at a baseline depth of 0.
        self.root = _insert_recursive(self.root, 0)

    def nearest_neighbor(self, target: tuple[float, ...]) -> tuple[tuple[float, ...], str, float] | None:
        """
        Traverses hyperplanes dynamically to locate the spatially closest node relative to
        a target query point.
        """
        # Catch empty tree scenarios before executing distance calculations.
        if self.root is None:
            return None

        # Ensure the query target matches the spatial dimensional domain.
        if len(target) != self.k:
            raise ValueError("Target dimension mismatch.")

        # Seed global tracking references with the root node configurations.
        best_node = self.root
        best_distance = self._euclidean_distance(target, self.root.point)

        def _search_recursive(node: Node | None, depth: int) -> None:
            """
            Executes a depth-first search down the closest geographic quadrant while backtracking
            selectively using hypersphere bounding cuts to prune alternative branches.

            Args:
                node (Node | None): The sub-root node partition being evaluated.
                depth (int): The current structural depth layer used to isolate the separating axis.
            """
            # Bring down the tracking metrics into local closure scope.
            nonlocal best_node, best_distance

            # Base Case: Stop searching when hitting a dead-end leaf pointer.
            if node is None:
                return

            # Compute the absolute Euclidean distance from the target to the current node.
            current_distance = self._euclidean_distance(target, node.point)

            # If the current node is closer than the current champion, update global trackers.
            if current_distance < best_distance:
                best_distance = current_distance
                best_node = node

            # Isolate the current splitting dimension.
            axis = depth % self.k

            # Determine the spatial preference: Identify which sub-branch side the target falls on.
            if target[axis] < node.point[axis]:
                primary_branch = node.left
                secondary_branch = node.right
            else:
                primary_branch = node.right
                secondary_branch = node.left

            # Always scan down the closer domain space first.
            _search_recursive(primary_branch, depth + 1)

            # Prune or check secondary hyperplane overlay boundaries.
            hyperplane_distance = abs(target[axis] - node.point[axis])

            # If the hypersphere overlaps the hyperplane, the closer node could reside on the other side.
            if hyperplane_distance < best_distance:
                _search_recursive(secondary_branch, depth + 1)

        # Trigger search sequence beginning at the tree origin.
        _search_recursive(self.root, 0)

        # Return the verified nearest coordinate package.
        return best_node.point, best_node.value, best_distance

    def display(self) -> None:
        """
        Renders a structured tree layout visualization mapping nested node elements.
        """
        print("=" * 60)
        print(f"-- K-Dimensional Tree Architecture (K = {self.k}) --")

        def _display_recursive(node: Node | None, depth: int, prefix: str, is_left: bool | None) -> None:
            """
            Recursively compiles structural string buffers to render tree branches down
            to terminal leaf nodes cleanly.

            Args:
                node (Node | None): The current structural node payload being printed.
                depth (int): The structural height indicator tracking sorting planes.
                prefix (str): String format spacer string managing visual indentation tiers.
                is_left (bool | None): Explicit side marker identifier ('L', 'R', or None for Root).
            """
            # Terminate rendering if the target node pointer is None.
            if node is None:
                return

            # Calculate active dimension for visual context.
            axis = depth % self.k

            # Assign branch direction glyph strings.
            if is_left is True:
                pointer = "L── "
            elif is_left is False:
                pointer = "R── "
            else:
                pointer = "Root "

            # Print node metrics accompanied by structural margin indentations.
            print(f"{prefix}{pointer}[Axis {axis}] Point: {node.point} -> Value: '{node.value}'")

            # Dynamically compile structural indentation vertical bars based on current tree depth.
            next_prefix = prefix + ("│   " if is_left is not None else "    ")

            # Recurse down sub-branches if children nodes exist.
            if node.left or node.right:
                if node.left:
                    _display_recursive(node.left, depth + 1, next_prefix, True)
                else:
                    print(f"{next_prefix}L── None")

                if node.right:
                    _display_recursive(node.right, depth + 1, next_prefix, False)
                else:
                    print(f"{next_prefix}R── None")

        # Initiate display logic passing the initial tab separator.
        _display_recursive(self.root, 0, "\t", None)

        print("=" * 60)