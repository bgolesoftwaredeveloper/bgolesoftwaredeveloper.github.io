"""
merkle_tree.py

An implementation of a cryptographic Merkle Tree (binary hash tree) that
efficiently verifies the integrity and consistency of a dataset.

This module utilizes the SHA-256 algorithm via Python's standard `hashlib` library.
It processes an ordered list of string elements, hashes them to form the base leaf
nodes, and recursively combines adjacent pairs of hashes to construct a tree structure
culminating in a single, definitive "Merkle Root".

Design Note:
    The construction requires $O(N)$ hash operations, where $N$ is the number of
    data elements. If a layer contains an odd number of nodes, the final node is
    duplicated to maintain the binary structure required for pairing.
"""
import hashlib

def calculate_hash(data: str) -> str:
    """
    Computes the SHA-256 hash of a given string.

    Args:
        data (str): The raw text data or concatenated hashes to process.

    Returns:
        str: The hexadecimal representation of the resulting SHA-256 hash.
    """
    return hashlib.sha256(data.encode('utf-8')).hexdigest()

class MerkleTree:
    """
    Encapsulates the construction and representation of a Merkle Tree
    built from a collection of data blocks.
    """

    def __init__(self, data: list[str]):
        """
        Initializes the MerkleTree by hashing the underlying data blocks
        and building the tree up to the root.

        Args:
            data (list[str]): An ordered list of raw data components to secure.
        """
        # Form the bottom layer of the tree by hashing each initial data element.
        self.leaves = [calculate_hash(character) for character in data]
        self.root = ""

        # Only proceed with tree construction if input data was provided.
        if self.leaves:
            self.root = self.build_tree(self.leaves)

    def build_tree(self, nodes: list[str]) -> str:
        """
        Recursively processes a layer of hashes, pairing adjacent entries to
        generate parent nodes until a single cryptographic root remains.

        Args:
            nodes (list[str]): The hashes comprising the current layer of the tree.

        Returns:
            str: The final cryptographic Merkle Root string.
        """
        number_of_nodes = len(nodes)

        # If there is only one node left, it's the root.
        if number_of_nodes == 1:
            return nodes[0]

        parent_level = []

        # Iterate through the current level two nodes at a time.
        for index in range(0, number_of_nodes, 2):
            left_child = nodes[index]

            # If there's an odd number of nodes, duplicate the last one.
            if index + 1 < number_of_nodes:
                right_child = nodes[index + 1]
            else:
                right_child = nodes[index]

            # Combine hashes and compute the parent hash.
            parent_hash = calculate_hash(left_child + right_child)
            parent_level.append(parent_hash)

        # Move up to the next level of the tree.
        return self.build_tree(parent_level)