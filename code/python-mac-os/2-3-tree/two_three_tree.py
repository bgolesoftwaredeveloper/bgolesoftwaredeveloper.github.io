"""
two_three_tree.py

An implementation of a self-balancing 2-3 Tree data structure designed to maintain
perfect height balance and optimize logarithmic search, insertion, and retrieval workflows.

This module provides an advanced, object-oriented structural implementation where nodes
dynamically mutate between 2-nodes and 3-nodes. It enforces strict type uniformity,
structural invariants, and defensive checks to prevent memory corruption or state desynchronization.

Security & Integrity Note:
    This implementation incorporates strict runtime assertions and structural validation
    sweeps (`_validate_invariants`) to actively guard against node overflow and unbalanced
    tree drift during execution.
"""
from __future__ import annotations

class Node:
    """
    An internal structural element responsible for managing a variable number of keys
    and child references within the 2-3 Tree topology.
    """

    def __init__(self, key = None, left: Node | None = None, middle: Node | None = None):
        """
        Initializes an independent node structure with optional initial keys and child allocations.

        Args:
            key: The initial scalar data key to store within the node structure. Defaults to None.
            left Node | None: The left sub-tree branch containing smaller elements. Defaults to None.
            middle Node | None: The center sub-tree branch containing intermediate elements. Defaults to None.
        """
        # Array tracking the scalar keys managed inside this node block.
        self.keys = []

        # If an initial key is supplied, establish it as the node's baseline value.
        if key is not None:
            self.keys.append(key)

        # Array tracking pointers to downstream children structures.
        self.children = []

        # Populate initial downstream links if the left or middle configurations are supplied.
        if left is not None:
            self.children.append(left)

        if middle is not None:
            self.children.append(middle)

    def is_leaf(self) -> bool:
        """
        Determines if the current node is an external leaf structure lacking child sub-trees.

        Returns:
            bool: True if the node contains no children; otherwise, False.
        """
        return len(self.children) == 0

    def is_full(self) -> bool:
        """
        Checks if the node has entered an illegal, temporary 4-node state requiring a split.

        Returns:
            bool: True if the node contains exactly 3 keys; otherwise, False.
        """
        return len(self.keys) == 3

    def insert_key(self, key) -> None:
        """
        Inserts a structural key into the node collection and dynamically enforces sorted order.

        Args:
            key: The comparable data value targeting insertion.

        Raises:
            ValueError: If the key parameter evaluates to a None type object.
        """
        # Guard against uninitialized object variants breaking data sorting invariants.
        if key is None:
            raise ValueError("Invariance failure: Insert of None type keys is strictly prohibited.")

        # Append the incoming scalar value and resort the keys array inline.
        self.keys.append(key)
        self.keys.sort()

    def insert_child(self, child: Node | None) -> None:
        """
        Appends an external sub-tree child node and re-orders pointers based on child boundary keys.

        Args:
            child: Node | None: The node instance to associate as a structural child.

        Raises:
            TypeError: If the provided parameter is not a valid Node instance.
            ValueError: If the target node is uninitialized or lacks key fields.
        """
        # Ensure teh object matches structural data types before interacting with memory paths.
        if not isinstance(child, Node):
            raise TypeError("Child must be a valid Node instance.")

        # Guard against downstream elements having undefined values.
        if not child.keys:
            raise ValueError("Invariance failure: Cannot append an uninitialized or empty child node.")

        # Wire up the tracking reference and sort children using their lowest boundary elements.
        self.children.append(child)
        self.children.sort(key = lambda node: node.keys[0])

class TwoThreeTree:
    """
    An execution engine responsible for supervising structural mutations, tree traversals,
    and balancing guarantees across a 2-3 self-balancing tree pipeline.
    """
    
    def __init__(self):
        """
        Instantiates an empty self-balancing 2-3 Tree engine.
        """
        self.root = None

    def _validate_invariants(self, node: Node | None) -> int:
        """
        Defensively validates sorting, sizing, and perfectly uniform global height limits recursively.

        Args:
            node Node | None: The specific node branch context being evaluated.

        Returns:
            int: The exact sub-tree path height depth calculated from the evaluated point.

        Raises:
            ValueError: If structural parameters, sorting sequences, or branch heights drift.
        """
        # Reaching a null path marks a terminal base boundary layer.
        if node is None:
            return 0

        # Structural limit check: A legal 2-3 node can only hold either 1 or 2 keys.
        if len(node.keys) < 1 or len(node.keys) > 2:
            raise ValueError(f"Structural integrity violation: Node possesses illegal key count ({len(node.keys)}).")

        # Invariant check: Assert that spatial sequences within this node remain sorted.
        if node.keys != sorted(node.keys):
            raise ValueError("Data integrity violation: Local scalar keys have fallen out of sorted sequence.")

        # Base case: Leaf node must have zero child sub-trees linked.
        if node.is_leaf():
            if len(node.children) != 0:
                raise ValueError("Structural defect: Leaf node possesses invalid child pointers.")

            return 1

        # Balancing check: Internal node child structures must always equal key lengths + 1.
        expected_children = len(node.keys) + 1

        if len(node.children) != expected_children:
            raise ValueError(f"Integrity defect: Node child count ({len(node.children)}) mismatches key bounds.")

        # Verify height consistency: Trace the leftmost path to set a baseline depth expectation.
        left_height = self._validate_invariants(node.children[0])

        # Walk through subsequent children branches to guarantee tree height matches the baseline.
        for child in node.children[1:]:
            if self._validate_invariants(child) != left_height:
                raise ValueError("Tree balance failure: The leaf structures do not share uniform global depth.")

        # Return computed path length incremented up to the structural parent layers.
        return left_height + 1

    def _split(self, node: Node) -> Node:
        """
        Splits an over-saturated, temporary 4-node into two legal 2-nodes and isolates a parent value.

        Args:
            node Node: The overflowed target element containing exactly 3 keys.

        Returns:
            Node: A packaging node tracking the promoted middle value and the new right sibling sub-tree.

        Raises:
            ValueError: If the node layout lacks the 3 necessary elements required to split.
        """
        # Intercept call if it targets structural spaces that haven't actually overflowed.
        if len(node.keys) != 3:
            raise ValueError("Invariance failure: Node split routine invoked on a non-saturated node layout.")

        # Isolate the middle value (median intended to bubble up to upper structural branches.
        promoted_key = node.keys[1]

        # Construct a right-hand sibling node storing the highest value element.
        right_node = Node(node.keys[2])

        # Keep the lowest element in the current left node block, shifting its state down to a 2-node.
        node.keys = [node.keys[0]]

        # If this isn't a leaf layer, redistribute the 4 children evenly across split branches.
        if not node.is_leaf():
            if len(node.children) != 4:
                raise ValueError("Structural defect: Unstable internal 4- node must possess exactly 4 sub-children.")

            # Assign the 2 higher children elements to the newly minted right sub-tree.
            right_node.children = [node.children[2], node.children[3]]

            # Retain the 2 lower children elements inside the original left sub-tree.
            node.children = [node.children[0], node.children[1]]

        # Return a tracking wrapper carrying the promoted element alongside the new right branch.
        return Node(promoted_key, right_node)

    def _search(self, node: Node | None, key) -> bool:
        """
        Internal recursive worker executing localized boundary routing configurations across sub-trees.

        Args:
            node Node | None: The specific node branch context currently targeted for examination.
            key: The comparable element matching targeted search goals.

        Returns:
            bool: True if the element exists within the current sub-tree structure; otherwise, False.

        Raises:
            TypeError: If the key parameter value type cannot be checked against existing keys.
            ValueError: If a 3-node branch is missing its structural third child reference.
        """
        # Exit route if search coordinates look into empty regions.
        if node is None:
            return False

        # Attempt lookups across local key inside the current ode boundary.
        try:
            if key in node.keys:
                return True
        except TypeError as exception:
            raise exception

        # If item isn't located locally and node is a leaf, element cannot exist in the tree.
        if node.is_leaf():
            return False

        # Route down appropriate child spaces based on comparison bounds.
        if key < node.keys[0]:
            return self._search(node.children[0], key)
        elif len(node.keys) == 1 or key < node.keys[1]:
            return self._search(node.children[1], key)
        else:
            # Assert that navigation paths exist before attempting lookups.
            if len(node.children) < 3:
                raise ValueError("Structural corruption: Navigating 3-node branch with insufficient child counts.")

            return self._search(node.children[2], key)

    def _insert(self, node: Node, key) -> Node | None:
        """
        Recursively penetrates the structural tree hierarchy to find leaves and passes splits upward.

        Args:
            node Node: The local sub-tree root element targeted for evaluation.
            key: The comparable data value to incorporate into the tree topology.

        Returns:
            Node | None: A split node container if structural overflow triggers; otherwise, None.

        Raises:
            IndexError: If evaluated boundary indexing directions fall out of actual child arrays.
        """
        # Base case: Insert element directly if parsing hits a terminal leaf layer.
        if node.is_leaf():
            node.insert_key(key)
        else:
            # Determine appropriate internal child target branch tracking indices.
            if key < node.keys[0]:
                child_index = 0
            elif len(node.keys) == 1 or key < node.keys[1]:
                child_index = 1
            else:
                child_index = 2

            # Confirm evaluated index directions don't mismatch actual child counts.
            if child_index >= len(node.children):
                raise IndexError("Structural defect: Evaluated child target branch index falls out of node bounds.")

            # Continue traversing deeper toward targeted branch coordinates.
            split_child = self._insert(node.children[child_index], key)

            # If downstream sub-trees split, pull up the mid element and link its right-hand sibling.
            if split_child:
                node.insert_key(split_child.keys[0])
                node.children.insert(child_index + 1, split_child.children[0])

        # Intercept node states immediately if key sizes transform it into an illegal 4-node structure.
        if node.is_full():
            return self._split(node)

        return None

    def search(self, key) -> bool:
        """
        Executes a targeted traversal to find a matching target key inside the global framework.

        Args:
            key (Any): The element value target targeted for query evaluation.

        Returns:
            bool: True if the key entry exists inside the tree network; otherwise, False.
        """
        # Check against empty object comparisons.
        if key is None:
            return False

        # Delegate execution sequence out to recursive lookup functions.
        return self._search(self.root, key)

    def insert(self, key) -> None:
        """
        Introduces a unique value element into the tree infrastructure and resolves structural overflows.

        Args:
            key: The unique, comparable data value targeted for initialization.

        Raises:
            ValueError: If the item value maps to a None object type or matches duplicate elements.
        """
        # Prevent runtime execution failures caused by uninitialized arguments.
        if key is None:
            raise ValueError("Execution error: Cannot insert an undefined or None values into teh hierarchy.")

        # If the root reference is unpopulated, establish the starting layer right here.
        if self.root is None:
            self.root = Node(key)

            return

        # Explicitly guard against double insertion mutations affecting uniqueness parameters.
        if self.search(key):
            raise ValueError(f"Invariance constraint: Key duplicate '{key}' detected.")

        # Begin recursive data injection loop starting at the root node structure.
        split_node = self._insert(self.root, key)

        # Root overflow: If the root node split, expand tree layers upward by making a new root element.
        if split_node:
            if len(split_node.keys) < 1 or len(split_node.children) < 1:
                raise ValueError("Structural corruption: Received an invalid split return bundle at root level.")

            # Instantiate a brand new parent node housing the promoted middle key.
            new_root = Node(split_node.keys[0])

            # Link the original left node branch layout.
            new_root.children.append(self.root)

            # Link the new right sibling node split layout.
            new_root.children.append(split_node.children[0])

            # Redirect tree pointer controls over to our newly established global root node layer.
            self.root = new_root

        # Execute automated validation sweeps across invariants to ensure the entire tree remains balanced.
        self._validate_invariants(self.root)

    def _display(self, node: Node | None, level: int) -> None:
        """
        Internal traversal printer that renders tree layers rotated horizontally on their side.

        Args:
            node Node | None: The current branch level component scheduled for console rendering.
            level int: An indentation scaling index representing current depth tier heights.
        """
        if node is not None:
            if len(node.children) == 3:
                self._display(node.children[2], level + 1)

                print("\t" * level + f"[{node.keys[1]}]")

                self._display(node.children[1], level + 1)

                print("\t" * level + f"[{node.keys[0]}]")

                self._display(node.children[0], level + 1)
            else:
                if len(node.children) >= 2:
                    self._display(node.children[1], level + 1)

                print("\t" * level + str(node.keys))

                if len(node.children) >= 1:
                    self._display(node.children[0], level + 1)

    def display(self) -> None:
        """
        Renders the active hierarchy structure into a human-readable text presentation layout.
        """
        # Initialize rendering sweep starting from the global tree root node layer.
        self._display(self.root, 0)