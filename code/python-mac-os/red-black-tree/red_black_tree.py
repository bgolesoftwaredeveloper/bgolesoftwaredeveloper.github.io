"""
red_black_tree.py

A complete, self-contained implementation of a self-balancing Red-Black Binary
Search Tree (BST) built using fundamental primitive types in Python.

This module provides structural classes managing node allocation and tree balancing.
It establishes standard node insertion and deletion pipelines, implementing the
O(1) tree rotation mechanics and coloring rules required to strictly guarantee
logarithmic height boundaries across dynamic data mutations.

Design Note:
    This implementation prioritizes explicit pointer manipulation and code
    transparency. It leverages a single, shared sentinel NIL node to manage
    boundary layouts, ensuring predictable memory handling and defensive
    null-safety throughout complex structural rebalancing sequences.
"""
from __future__ import annotations

RED = 0
BLACK = 1

class Node:
    """
    Represents an isolated structural element containing pointers and a data
    payload within the Red-Black Tree architecture.
    """

    def __init__(self, value: int | None):
        """
        Initializes a node instance containing structural pointers and an
        explicit validation color.

        Args:
            value (int | None): The primitive payload stored within the node.
                                Passes None exclusively for sentinel structures.
        """
        self.value = value
        self.color = RED

        self.left = None
        self.right = None
        self.parent = None

class RedBlackTree:
    """
    Manages pointer routing, structural transformations, and rebalancing suites
    to preserve Red-Black Tree invariants across elements.
    """

    def __init__(self):
        """
        Instantiates an empty Red-Black Tree anchored by a shared, black-colored
        sentinel NIL leaf node to simplify edge boundaries.
        """
        self.nil = Node(None)
        self.nil.color = BLACK

        # Anchor the sentinel's internal children to point circularly.
        self.nil.left = self.nil
        self.nil.right = self.nil

        # An empty tree's root points directly to the sentinel leaf.
        self.root = self.nil

    def _minimum(self, node: Node) -> Node:
        """
        Locates the leftmost descendant node in the specified tree branch layout.

        Args:
            node (Node): The root node of the active subtree to inspect.

        Returns:
            Node: The node containing the smallest sorted value in that subtree.
        """
        # Continuously descend left until hitting the boundary.
        while node.left != self.nil:
            node = node.left

        return node

    def _transplant(self, subtree: Node, new_subtree: Node):
        """
        Traverses sorted branch trajectories to retrieve a specific unique node
        by its payload value.

        Args:
            value (int): The integer target to search for.

        Returns:
            Node: The matching node instance, or the sentinel self.nil if missing.
        """
        # If replacing the root, update the tree's main entry point.
        if subtree.parent is None:
            self.root = new_subtree
        elif subtree == subtree.parent.left:
            # If the old node is a left child, update parent's left pointer.
            subtree.parent.left = new_subtree
        else:
            # If the old node is a right child, update parent's right pointer.
            subtree.parent.right = new_subtree

        # Link the new node back up to the old parent container.
        new_subtree.parent = subtree.parent

    def _search(self, value: int) -> Node:
        """
        Traverses sorted branch trajectories to retrieve a specific unique node
        by its payload value.

        Args:
            value (int): The integer target to search for.

        Returns:
            Node: The matching node instance, or the sentinel self.nil if missing.
        """
        current = self.root

        # Traverse downwards using standard BST binary properties.
        while current != self.nil and value != current.value:
            # Shift left if target value is lower, otherwise shift right.
            current = current.left if value < current.value else current.right

        return current

    def _rotate_left(self, node: Node):
        """
        Executes a localized, O(1) counter-clockwise pivot around the target
        node to rebalance subtrees.

        Args:
            node (Node): The pivot parent node initiating the leftward shift.
        """
        # Guard against invalid rotations on dead-ends or sentinel leaves.
        if node == self.nil or node.right == self.nil:
            return

        # Identify the right child which will become the new parent (pivot).
        pivot = node.right

        # Turn pivot's left subtree into node's right subtree.
        node.right = pivot.left

        # Update the parent link of the shifted subtree if it isn't a sentinel.
        if pivot.left != self.nil:
            pivot.left.parent = node

        # Link pivot directly to node's original parent space.
        pivot.parent = node.parent

        # Re-attach pivot to teh rest of the higher tree hierarchy.
        if node.parent is None:
            self.root = pivot
        elif node == node.parent.left:
            node.parent.left = pivot
        else:
            node.parent.right = pivot

        # Attach original node to the left of the new pivot container.
        pivot.left = node
        node.parent = pivot

    def _rotate_right(self, node: Node):
        """
        Executes a localized, O(1) clockwise pivot around the target node to
        rebalance subtrees.

        Args:
            node (Node): The pivot parent node initiating the rightward shift.
        """
        # Guard against invalid rotations on dead-end or sentinel leaves.
        if node == self.nil or node.left == self.nil:
            return

        # Identify the left child which will become the new parent (pivot).
        pivot = node.left

        # Turn pivot's right subtree into node's left subtree.
        node.left = pivot.right

        # Update the parent linik of the shifted subtree if it isn't a sentinel.
        if pivot.right != self.nil:
            pivot.right.parent = node

        # Link pivot directly to node's original parent space.
        pivot.parent = node.parent

        # Re-attach pivot to the rest of the higher tree hierarchy.
        if node.parent is None:
            self.root = pivot
        elif node == node.parent.right:
            node.parent.right = pivot
        else:
            node.parent.left = pivot

        # Attach original node to teh right of the new pivot container.
        pivot.right = node
        node.parent = pivot

    def _fix_insertion(self, node: Node):
        """
        Restores balance and structural invariants when sequential allocations
        produce adjacent red nodes.

        Args:
            node (Node): The newly inserted red node currently violating constraints.
        """
        # Loop continues while we have consecutive RED nodes down a path.
        while node.parent and node.parent.color == RED:
            grandparent = node.parent.parent

            if not grandparent:
                break

            # Parent is a left child of grandparent.
            if node.parent == grandparent.left:
                uncle = grandparent.right

                # Uncle is RED -> Handle color reflection.
                if uncle.color == RED:
                    node.parent.color = BLACK
                    uncle.color = BLACK
                    grandparent.color = RED

                    node = grandparent
                else:
                    # Uncle is BLACK and node is a right child -> Zig-Zag alignment.
                    if node == node.parent.right:
                        node = node.parent

                        self._rotate_left(node)

                    # Uncle is BLACK and node is a left child -> Straight-line structural shift.
                    if node.parent:
                        node.parent.color = BLACK

                    grandparent.color = RED

                    self._rotate_right(grandparent)
            else:
                # Parent is a right child of grandparent (Mirror case).
                uncle = grandparent.left

                # Uncle is RED -> Handle color reflection.
                if uncle.color == RED:
                    node.parent.color = BLACK
                    uncle.color = BLACK
                    grandparent.color = RED

                    node = grandparent
                else:
                    # Uncle is BLACK and node is a left child -> Zig-Zag alignment.
                    if node == node.parent.left:
                        node = node.parent

                        self._rotate_right(node)

                    # Uncle is BLACK and node is a right child -> Straight-line structural shift.
                    if node.parent:
                        node.parent.color = BLACK

                    grandparent.color = RED

                    self._rotate_left(grandparent)

        self.root.color = BLACK

    def _fix_erase(self, node: Node):
        """
        Resolves black-height deficiencies ("double-black" nodes) by shifting
        and breaking up color structures up to the root element.

        Args:
            node (Node): The root node of the active black-deficient subtree boundary.
        """
        while node != self.root and node.color == BLACK:
            # Active node is a left child.
            if node == node.parent.left:
                sibling = node.parent.right

                # Sibling is RED -> Adjust colors and rotate to reveal black sibling.
                if sibling.color == RED:
                    sibling.color = BLACK
                    node.parent.color = RED

                    self._rotate_left(node.parent)

                    sibling = node.parent.right

                # Sibling and both its children are BLACK -> Degenerate black weight up.
                if sibling.left.color == BLACK and sibling.right.color == BLACK:
                    sibling.color = RED
                    node = node.parent
                else:
                    # Sibling is BLACK, inner child is RED, outer child is BLACK -> Straighten.
                    if sibling.right.color == BLACK:
                        sibling.left.color = BLACK
                        sibling.color = RED

                        self._rotate_right(sibling)

                        sibling = node.parent.right

                    # Sibling is BLACK, outer child is RED -> Color flip and final left rotation.
                    sibling.color = node.parent.color
                    node.parent.color = BLACK
                    sibling.right.color = BLACK

                    self._rotate_left(node.parent)

                    node = self.root
            else:
                # Activate node is a right child (Mirror Case).
                sibling = node.parent.left

                # Sibling is RED -> Adjust colors and rotate to reveal black sibling.
                if sibling.color == RED:
                    sibling.color = BLACK
                    node.parent.color = RED

                    self._rotate_right(node.parent)

                    sibling = node.parent.left

                # Sibling and both its children are BLACK -> Degenerate black weight up.
                if sibling.left.color == BLACK and sibling.right.color == BLACK:
                    sibling.color = RED
                    node = node.parent
                else:
                    # Sibling is BLACK, inner child is RED, outer child is BLACK -> Straighten.
                    if sibling.left.color == BLACK:
                        sibling.right.color = BLACK
                        sibling.color = RED

                        self._rotate_left(sibling)

                        sibling = node.parent.left

                    # Sibling is BALCK, outer child is RED -> Color flip and final right rotation.
                    sibling.color = node.parent.color
                    node.parent.color = BLACK
                    sibling.left.color = BLACK

                    self._rotate_right(node.parent)

                    node = self.root

        # Cleanly wipe out double-black designations on exit boundaries.
        node.color = BLACK

    def insert(self, value: int):
        """
        Inserts a unique integer data point into the sorted binary topology.

        Args:
            value (int): The integer payload key to append.
        """
        if not isinstance(value, int):
            raise TypeError("Value must be an integer.")

        # Allocate a new leaf node shell linked up to the sentinel space.
        new_node = Node(value)

        new_node.left = self.nil
        new_node.right = self.nil

        current = self.root
        trailing = None

        # Classic binary search tree positioning traversal.
        while current != self.nil:
            trailing = current

            # Explicit check defending against duplicate keys.
            if value == current.value:
                raise ValueError(f"Insertion error: Duplicate key. '{value}' already exists.")

            current = current.left if value < current.value else current.right

        # Set parent link to the last valid tracking element discovered.
        new_node.parent = trailing

        # Splice node into position within parent layout.
        if trailing is None:
            self.root = new_node
        elif value < trailing.value:
            trailing.left = new_node
        else:
            trailing.right = new_node

        # Activate rebalancing fixes over structural updates.
        self._fix_insertion(new_node)

    def erase(self, value: int):
        """
        Removes an integer node from the tree layout, invoking targeted adjustments
        if the deleted item breaks color balance properties.

        Args:
            value (int): The structural key target to erase.
        """
        # Find the target node using our BST search method.
        node = self._search(value)

        # Throw an error check if target values are missing from the structure.
        if node == self.nil:
            raise ValueError(f"Erase error: Value '{value}' does not exist.")

        # Keep track of node being physically removed or shifted.
        y = node
        original_color = y.color

        # Target node lacks a left child link.
        if node.left == self.nil:
            x = node.right

            self._transplant(node, node.right)
        elif node.right == self.nil:
            # Target node lacks a right child link.
            x = node.left

            self._transplant(node, node.left)
        else:
            # Target node has two operational child lines.
            y = self._minimum(node.right)
            original_color = y.color
            x = y.right

            # Link successor into replacement slots
            if y.parent == node:
                x.parent = y
            else:
                self._transplant(y, y.right)

                y.right = node.right
                y.right.parent = y

            # Replace node with its successor y in the hierarchy layout.
            self._transplant(node, y)

            y.left = node.left

            y.left = node.left
            y.left.parent = y
            y.color = node.color

        # Trigger erasure repairs exclusively if black elements are extracted.
        if original_color == BLACK:
            self._fix_erase(x)

    def display(self, node: Node | None = None, prefix: str = "", is_left: bool = True):
        """
        Renders a rotated visual topology graph to standard output using Unicode branch curves.

        Args:
            node (Node | None): The subtree node entry point. Defaults to the root element.
            prefix (str): Tracked indent spacing parameters representing branch levels.
            is_left (bool): Traversal tracker indicating active layout orientation.
        """
        # Default initialization points directly to the tree root.
        if node is None:
            node = self.root

        # Terminate display loops cleanly when dropping into empty sentinel leafs.
        if node == self.nil:
            return

        # Recursive right subtree traversal (printed on top half).
        if node.right != self.nil:
            next_prefix = prefix + ("│   " if is_left else "    ")

            self.display(node.right, next_prefix, False)

        # Print layout indents followed by appropriate directional Unicode bracket indicators.
        print(prefix, end="")

        if node == self.root:
            print("─── ", end="")
        elif is_left:
            print("└── ", end="")
        else:
            print("┌── ", end="")

        # Output the node metadata value and associated validation color code flag.
        color = "R" if node.color == RED else "B"

        print(f"[{node.value}:{color}]")

        # Recursive left subtree traversal (printed on bottom half).
        if node.left != self.nil:
            next_prefix = prefix + ("    " if is_left else "│   ")

            self.display(node.left, next_prefix, True)