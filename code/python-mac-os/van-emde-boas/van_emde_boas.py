r"""
van_emde_boas.py

An implementation of a Van Emde Boas (vEB) tree, an advanced data structure
that maintains a dynamic set of integers over a bounded universe and supports
search, insertion, deletion, predecessor, and successor queries.

This module achieves $O(\log \log M)$ time complexity for all operational primitives,
where $M$ is the universe size. This is accomplished by recursively clustering the
universe into $\sqrt{M}$ sub-trees, managing summary structures to track which
clusters are occupied, and optimizing minimum/maximum tracking to bypass recursive
calls in baseline operations.

Design Note:
    To maintain $O(\log \log M)$ operational efficiency, the universe size $M$ must
    be a power of 2 ($M = 2^{2^k}$). The tree explicitly separates the absolute
    minimum element of a structure from its sub-clusters, a property crucial for
    efficient empty-tree checking and short-circuiting recursive lookups.
"""
import math

class VanEmdeBoas:
    """
    Encapsulates a recursive Van Emde Boas tree structure. Handles integer keys
    constrained to the range [0, universe_size - 1].
    """

    def __init__(self, universe_size: int):
        r"""
        Initializes an empty Van Emde Boas tree for a given universe capacity.

        Args:
            universe_size (int): The maximum bound ($M$) of the universe. Must be
                                 a power of 2 ($2, 4, 16, 256, \dots$).
        """
        self.universe_size = universe_size
        self.minimum = None
        self.maximum = None

        # A universe of size 2 handles bits 0 and 1 directly.
        if universe_size == 2:
            self.lower_square_root = 1
            self.upper_square_root = 1

            self.summary = None
            self.cluster = None

            return

        # Analytical bit divisor: Calculate split criteria for asymmetric universe targets.
        bit_size = int(math.log2(self.universe_size))

        # Lower square root denotes the capacity of individual.
        self.lower_square_root = 1 << (bit_size // 2)

        # Upper square root denotes the total indexable count of sub-clusters.
        self.upper_square_root = 1 << (bit_size - bit_size // 2)

        # Summary tracks cluster allocations; cluster contains sub-veb nodes.
        self.summary = VanEmdeBoas(self.upper_square_root)
        self.cluster = [VanEmdeBoas(self.lower_square_root) for _ in range(self.upper_square_root)]

    def _low(self, x: int) -> int:
        """
        Extracts the localized offset index of x within its specific sub-cluster.

        Args:
            x (int): The absolute universe integer to slice.

        Returns:
            int: The lower-order bits relative to the local cluster bounds.
        """
        return x % self.lower_square_root

    def _high(self, x: int) -> int:
        """
        Extracts the parent cluster ID indicating which sub-vEB instance stores x.

        Args:
            x (int): The absolute universe integer to slice.

        Returns:
            int: The higher-order bits acting as the cluster array pointer.
        """
        return x // self.lower_square_root

    def _index(self, cluster_index: int, index: int) -> int:
        """
        Reconstructs a global universe index from internal cluster coordinates.

        Args:
            cluster_index (int): The identifier tracking the containing sub-cluster.
            index (int): The localized offset inside that identified cluster.

        Returns:
            int: The absolute integrated universe integer representation.
        """
        return cluster_index * self.lower_square_root + index

    def __contains__(self, x: int) -> bool:
        """
        Evaluates the existence of element x in the tree layout in O(log log M) time.

        Args:
            x (int): The target element to search for.

        Returns:
            bool: True if the element exists in the tree, False otherwise.
        """
        # Short-circuit checking if item matches cached tree boundary markers.
        if x == self.minimum or x == self.maximum:
            return True

        # If limits missed at size 2, item is absent.
        if self.universe_size == 2:
            return False

        # Recurse downwards into the target sub-cluster tracking spatial layouts.
        return self.cluster[self._high(x)].__contains__(self._low(x))

    def __getitem__(self, key: int):
        """
        Subscript interface mimicking standard dictionary mappings.

        Args:
            key (int): The integer target to search for.

        Returns:
            int: The valid item key payload if present.
        """
        if key not in self:
            raise KeyError(f"Key {key} not found in the universe.")

        if key == self.minimum:
            return self.minimum

        return self.cluster[self._high(key)][self._low(key)]

    def insert(self, x: int):
        """
        Inserts an integer element into the sorted tree topology in O(log log M) time.

        Args:
            x (int): The valid integer payload to append.
        """
        # Active tree instance is empty.
        if self.minimum is None:
            self.minimum = x
            self.maximum = x

            return

        # Item belongs at the front. Swap to maintain separate minimum caching.
        if x < self.minimum:
            x, self.minimum = self.minimum, x

        if self.universe_size > 2:
            cluster_index = self._high(x)
            index = self._low(x)

            # If the selected cluster is empty, update the summary and initialize.
            if self.cluster[cluster_index].minimum is None:
                self.summary.insert(cluster_index)

                self.cluster[cluster_index].minimum = index
                self.cluster[cluster_index].maximum = index
            else:
                # Recurse down into the active populated sub-cluster line.
                self.cluster[cluster_index].insert(index)

        # Continually maintain dynamic global maximum bounds.
        if x > self.maximum:
            self.maximum = x

    def predecessor(self, x: int) -> int | None:
        """
        Finds the largest element in the tree structurally smaller than x.

        Args:
            x (int): The pivot query value.

        Returns:
            int | None: The mathematical predecessor value, or None if missing.
        """
        # Evaluating constraints on atomic size 2 variables.
        if self.universe_size == 2:
            if x == 1 and self.minimum == 0:
                return 0

            return None

        # Short-circuit validation if target value exceeds historical maximum boundaries.
        if self.maximum is not None and x > self.maximum:
            return self.maximum

        cluster_index = self._high(x)
        index = self._low(x)

        # If lower items exist inside the target's cluster, explore internally.
        if self.cluster[cluster_index].minimum is not None and index > self.cluster[cluster_index].minimum:
            offset = self.cluster[cluster_index].predecessor(index)

            return self._index(cluster_index, offset)
        else:
            # Fall back to checking preceding cluster using the summary frameworks.
            predecessor_cluster = self.summary.predecessor(cluster_index)

            if predecessor_cluster is None:
                # Fall back check assessing the globally cached standalone minimum.
                if self.minimum is not None and x > self.minimum:
                    return self.minimum

                return None

            # Retrieve the maximum item from the adjacent lower active cluster.
            offset = self.cluster[predecessor_cluster].maximum

            return self._index(predecessor_cluster, offset)

    def successor(self, x: int) -> int | None:
        """
        Finds the smallest element in the tree structurally greater than x.

        Args:
            x (int): The pivot query value.

        Returns:
            int | None: The mathematical successor value, or None if missing.
        """
        # Evaluate constraints on atomic size 2 variables.
        if self.universe_size == 2:
            if x == 0 and self.maximum == 1:
                return 1

            return None

        # Short-circuit check assessing the globally cached standalone minimum.
        if self.minimum is not None and x < self.minimum:
            return self.minimum

        cluster_index = self._high(x)
        index = self._low(x)

        # If higher items exist inside the target's cluster, explore internally.
        if self.cluster[cluster_index].maximum is not None and index < self.cluster[cluster_index].maximum:
            offset = self.cluster[cluster_index].successor(index)

            return self._index(cluster_index, offset)
        else:
            # Fall back to checking succeeding cluster using the summary framework.
            successor_cluster = self.summary.successor(cluster_index)

            if successor_cluster is None:
                return None

            # Retrieve the minimum item from the adjacent higher active cluster.
            offset = self.cluster[successor_cluster].minimum

            return self._index(successor_cluster, offset)

    def erase(self, x: int):
        """
        Removes an integer target from the active layout, preserving tracking indexes.

        Args:
            x (int): The integer target to remove.
        """
        # If item is absent, bypass erasing sequences.
        if not self.__contains__(x):
            return

        # Singlet elimination. Collapse structures cleanly.
        if self.minimum == self.maximum:
            self.minimum = None
            self.maximum = None

            return

        # Hand off binary transformations for size 2 fields.
        if self.universe_size == 2:
            if x == 0:
                self.minimum = 1
            else:
                self.minimum = 0

            self.maximum = self.minimum

            return

        # The target is the absolute minimum.
        if x == self.minimum:
            cluster = self.summary.minimum
            x = self._index(cluster, self.cluster[cluster].minimum)

            self.minimum = x

        cluster_index = self._high(x)
        index = self._low(x)

        # Recursively slice away target references inside internal branches.
        self.cluster[cluster_index].erase(index)

        # Structural repair: If a cluster goes empty, update the summary tracker.
        if self.cluster[cluster_index].minimum is None:
            self.summary.erase(cluster_index)

            # Re-evaluate global maximum if the edge item was erased.
            if x == self.maximum:
                summary_maximum = self.summary.maximum

                if summary_maximum is None:
                    self.maximum = self.minimum
                else:
                    self.maximum = self._index(summary_maximum, self.cluster[summary_maximum].maximum)
        elif x == self.maximum:
            # Update local bounds if internal items were stripped away.
            self.maximum = self._index(cluster_index, self.cluster[cluster_index].maximum)

    def to_list(self) -> list[int]:
        """
        Flattens tree tracking sequences into an ordered array using linear successor jumps.

        Returns:
            list[int]: An ordered sequence containing stored integers.
        """
        # Allocate an empty dynamic container to old the ordered output keys.
        items = []

        # Bootstrap the traversal using the absolute structural minimum of the tree.
        current = self.minimum

        # If the minimum is not None, the tree is populated; initiate the traversal.
        if current is not None:
            # Capture the absolute minimum as the first sorted item.
            items.append(current)

            # Loop continuously across successive logical neighbor boundaries.
            while True:
                # Query the tree for the next smallest item greater than 'current'.
                current = self.successor(current)

                # A return value of None means the absolute maximum was exceeded.
                if current is None:
                    break

                # Append the validated successor key into our tracking sequence.
                items.append(current)

        return items

    def __str__(self):
        """
        Constructs string maps formatting underlying data array visualizations.
        """
        return str(self.to_list())