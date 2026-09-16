"""
skip_list.py

An implementation of a probabilistic, multi-layered Skip List data structure designed to
provide efficient logarithmic search, insertion, update, and erasure operations.

This module provides a robust, object-oriented structural implementation where elements
are organized into parallel horizontal forward-pointer lanes. It maintains dynamic layer balanced
topologies via geometric distributions without requiring rigid structural tree rotation sequences.

Security & Integrity Note:
    This implementation incorporates localized bounds-checking, duplicate mitigation controls,
    and automatic upper-level structural trimming to guarantee memory footprint efficiency and
    prevent node pointer state desynchronization.
"""
from __future__ import annotations

import random

class Node:
    """
    An internal structural element responsible for managing stored data payloads and
    tracking multi-level forward reference pointer arrays.
    """

    def __init__(self, key, value, level: int):
        """
        Initializes an independent node structure mapped across a specific execution height.

        Args:
            key: The unique, comparable scalar key used to route indexing lookups.
            value: The data payload associated with the unique tracking key.
            level (int): The maximum zero-indexed vertical pointer lane height for this node.
        """
        # Establish the sorting key and its associated metadata payload.
        self.key = key
        self.value = value

        # Array of pointers to downstream next nodes across discrete horizontal tiers.
        # This allocates an individual index reference for each level this node exists on.
        self.forward = [None] * (level + 1)

class SkipList:
    """
    An execution engine responsible for supervising structural lane traversals,
    coin-flipping layer promotions, and pointer redistribution routines.
    """

    def __init__(self, maximum_level: int = 16, probability_factor: float = 0.5):
        """
        Instantiates an empty probabilistic Skip List engine with configurable constraints.

        Args:
            maximum_level (int): The absolute cap on vertical lanes allowed. Defaults to 16.
            probability_factor (float): The geometric probability constraint used for layer promotion. Defaults to 0.5.
        """
        # Validate type compliance and numerical boundaries of configuration elements.
        if not isinstance(maximum_level, int):
            raise TypeError("Maximum level must be an integer.")

        if maximum_level <= 0:
            raise ValueError("Maximum level must be an integer greater than zero.")

        if not isinstance(probability_factor, (int, float)):
            raise TypeError("Probability must be an integer or float.")

        if not (0.0 < probability_factor < 1.0):
            raise ValueError("Probability must be between 0 and 1.0.")

        # Assign the maximum structural height threshold allowed by this instance.
        self.maximum_level = maximum_level

        # Set the probability distribution limit used in geometric coin-tossing routines.
        self.probability_factor = probability_factor

        # Establish an unmapped sentinel dummy head node locked at maximum execution height bounds.
        self.head = Node(key=None, value=None, level=self.maximum_level)

        # The active maximum lane height occupied by nodes inside the list matrix.
        self.level = 0

    def _random_level(self) -> int:
        """
        Returns a random level height using a geometric coin-flipping distribution routine.

        Returns:
            int: A calculated vertical lane height bounded by maximum level limits.
        """
        # Every node starts with a baseline allocation for Level 0.
        level = 0

        # Simulate independent coin flips to determine if the node gets promoted up a level.
        while random.random() < self.probability_factor and level < self.maximum_level:
            level += 1

        return level

    def insert(self, key, value) -> None:
        """
        Introduces a unique key-value element into the list infrastructure and links new pointer lanes.

        Args:
            key: The unique, comparable scalar data key targeting initialization.
            value: The data payload mapping directly to the inserted key.

        Raises:
            ValueError: If the key evaluation tracks to an undefined or None object type.
        """
        # Guard against uninitialized object variants breaking scalar sorting invariants.
        if key is None:
            raise ValueError("Invariance failure: Insertion of undefined or None keys is strictly prohibited.")

        try:
            if self.head.forward[0] is not None:
                _ = key < self.head.forward[0].key
        except TypeError as exception:
            raise exception

        # Intercept double insertions to safely route mutations into the specialized update channel.
        if self.search(key) is not None:
            self.update(key, value)

            return

        # An allocation array tracking predecessor nodes where the search drops down a vertical level.
        array = [None] * (self.maximum_level + 1)

        current = self.head

        # Trace paths from the highest populated structural lane downwards to map link entry points.
        for index in range(self.level, -1, -1):
            while current.forward[index] and current.forward[index].key < key:
                current = current.forward[index]

            array[index] = current

        # Determine the random execution height for this new node entry.
        level = self._random_level()

        # If the generated height exceeds current limits, initialize the predecessor pointers to head.
        if level > self.level:
            for index in range(self.level + 1, level + 1):
                array[index] = self.head

            self.level = level

        # Instantiate a brand new node housing the payload data.
        new_node = Node(key, value, level)

        # Splice the new node references into the forward pointers arrays across all designated levels.
        for index in range(level + 1):
            new_node.forward[index] = array[index].forward[index]
            array[index].forward[index] = new_node

    def search(self, key):
        """
        Executes a fast multi-level traversal to locate a matching target key inside the framework.

        Args:
            key: The comparable element value matching targeted search parameters.

        Returns:
            any | None: The mapped data payload if found; otherwise, None.
        """
        if key is None:
            return None

        current = self.head

        # Drop horizontally and vertically across reference levels to minimize search steps.
        try:
            for index in range(self.level, -1, -1):
                while current.forward[index] and current.forward[index].key < key:
                    current = current.forward[index]
        except TypeError:
            return None

        # Shift pointer horizontally into level 0 data space to evaluate the closest matched key.
        current = current.forward[0]

        # Return the payload if keys align accurately.
        if current and current.key == key:
            return current.value

        return None

    def update(self, key, new_value) -> bool:
        """
        Locates an established key value and swaps its stored payload with minimal overhead.

        Args:
            key: The pre-existing key targeted for mutation.
            new_value: The updated payload data to store.

        Returns:
            bool: True if mutation completed successfully; otherwise, False.
        """
        if key is None:
            return False

        current = self.head

        # Route down appropriate child spaces based on comparison bounds.
        try:
            for index in range(self.level, -1, -1):
                while current.forward[index] and current.forward[index].key < key:
                    current = current.forward[index]
        except TypeError:
            return False

        current = current.forward[0]

        # If the key is verified, update the value inline without altering layer geometry.
        if current and current.key == key:
            current.value = new_value

            return True

        return False

    def erase(self, key) -> bool:
        """
        Removes an element from the list infrastructure and restructures pointer lanes around it.

        Args:
            key: The comparable data value targeted for structural removal.

        Returns:
            bool: True if the element was successfully erased; otherwise, False.
        """
        if key is None:
            return False

        # An allocation array mapping the predecessors flanking the target element.
        array = [None] * (self.maximum_level + 1)

        current = self.head

        # Guard against runtime type verification failures mid-traversal
        try:
            for index in range(self.level, -1, -1):
                while current.forward[index] and current.forward[index].key < key:
                    current = current.forward[index]

                array[index] = current
        except TypeError:
            return False

        current = current.forward[0]

        # Target matched: Restructure pointers to bypass the unlinked node.
        if current and current.key == key:
            for index in range(self.level + 1):
                # Break loop early if the predecessor does not connect directly to our target node.
                if array[index].forward[index] != current:
                    break

                array[index].forward[index] = current.forward[index]

            # Recalculate global maximum level if deletions left the upper tiers entirely unpopulated.
            while self.level > 0 and self.head.forward[self.level] is None:
                self.level -= 1

            return True

        return False

    def display(self) -> None:
        """
        Renders the active skip list level layouts into a human-readable text presentation format.
        """
        print("-- Skip List --")

        for level in range(self.level, -1, -1):
            print(f"\tLevel {level}:", end=" ")

            node = self.head.forward[level]

            while node:
                print(f"({node.key}): {node.value}", end=" -> ")

                node = node.forward[level]

            print("None")