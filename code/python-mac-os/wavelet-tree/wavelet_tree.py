"""
wavelet_tree.py

A class-based implementation of a Wavelet Tree data structure designed to provide
efficient rank operations over an arbitrary discrete integer sequence.

This module provides an elegant, recursive tree architecture that partitions an
alphabet alphabet-wise rather than sequence-wise. It reduces large alphabet rank
queries into nested binary selections, leveraging precomputed bit-vector metadata
to achieve optimal time complexities.

Design Note:
    This implementation prioritizes structural clarity and explicit educational formatting.
    It builds stationary lookup matrices (`left_counts`) at every nested node level to enable
    sub-linear O(1) tracking adjustments during tree traversals without relying on expensive,
    dynamic list-slicing mechanisms.
"""

class Node:
    """
    Represents an individual partitioning node within the Wavelet Tree structure.

    Each node controls a specific sub-range of the total alphabet and maintains a
    binary bit-vector indicating whether elements at the current level belong to the
    lower or upper half of its assigned alphabet scope.
    """

    def __init__(self, data: list, low: int, high: int):
        """
        Initializes an internal or leaf node, constructing its localized alphabet partition table.

        Args:
            data (list): The sequence of integer values passing through this specific node.
            low (int): The lower inclusive boundary of the active alphabet partition.
            high (int): The upper inclusive boundary of the active alphabet partition.
        """
        self.low = low
        self.high = high
        self.left_child = None
        self.right_child = None

        # If the range contains only one element or data is empty.
        if self.low == self.high or not data:
            self.bit_vector = []
            self.left_counts = []

            return

        # Calculate the midpoint to splot the alphabet range.
        self.midpoint = (self.low + self.high) // 2

        # Create the bit vector and separate data for the left and right children.
        self.bit_vector = []

        left_data = []
        right_data = []

        # Route elements into subsets based on alphabetical boundaries relative to the midpoint.
        for value in data:
            if value <= self.midpoint:
                self.bit_vector.append(0)
                left_data.append(value)
            else:
                self.bit_vector.append(1)
                right_data.append(value)

        # Precompute sequential tracking counters to enable O(1) rank projections.
        self.left_counts = [0] * (len(self.bit_vector) + 1)

        for index, bit in enumerate(self.bit_vector):
            self.left_counts[index + 1] = self.left_counts[index] + (1 if bit == 0 else 0)

        # Recursively construct downstream child components over bounded sub-alphabets.
        self.left_child = Node(left_data, low, self.midpoint)
        self.right_child = Node(right_data, self.midpoint + 1, self.high)

    def rank(self, index: int, value: int) -> int:
        """
        Recursively counts occurrences of a target value within a specific prefix range.

        Args:
            index (int): The exclusive upper index limit of the prefix slice being queried.
            value (int): The specific integer target frequency to isolate.

        Returns:
            int: Total occurrences of the target value located inside data[0...index-1].
        """
        # When boundaries converge, the remaining offset represents the precise count.
        if self.low == self.high:
            return index

        if index > len(self.bit_vector):
            index = len(self.bit_vector)

        # Extract precise index projections using local bit-vector metadata records.
        zeros_count = self.left_counts[index]
        ones_count = index - zeros_count

        # Route query into the matching child node based on alphabet thresholds.
        if value <= self.midpoint:
            return self.left_child.rank(zeros_count, value)
        else:
            return self.right_child.rank(ones_count, value)

class WaveletTree:
    """
    An interface wrapper overseeing a Wavelet Tree's root management workflows.

    Coordinates primary instantiation mechanics, calculates general input sequence
    alphabet boundaries, and guards execution loops against boundary-breaking query violations.
    """

    def __init__(self, data: list):
        """
        Establishes a Wavelet Tree system from a raw input integer sequence.

        Args:
            data (list): The sequence of discrete integer data to index.
        """
        if not data:
            self.root = None

            return

        # Dynamically determine complete alphabet dimensions across the sequence.
        self.minimum_value = min(data)
        self.maximum_value = max(data)

        # Kickstart the hierarchical node construction pipeline.
        self.root = Node(data, self.minimum_value, self.maximum_value)

    def rank(self, index: int, value: int) -> int:
        """
        Executes a global Rank query tracking occurrences of a value up to a specified index limit.

        Args:
            index (int): The exclusive upper-bound index constraint of the tracking range.
            value (int): The specific integer target frequency to isolate.

        Returns:
            int: Frequency count of 'value' inside data[0...index-1], or 0 if outside initialization scopes.
        """
        # Fail-fast check to catch out-of-bounds symbol queries or empty data topologies safely.
        if not self.root or value < self.minimum_value or value > self.maximum_value:
            return 0

        return self.root.rank(index, value)