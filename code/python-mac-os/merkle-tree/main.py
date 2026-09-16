"""
main.py

Demonstration script for the MerkleTree implementation.

This entry point file passes sample data blocks into the Merkle Tree, executes
the tree's cryptographic construction, and outputs the resulting leaf hashes
and the definitive Merkle Root to the console.
"""
from merkle_tree import MerkleTree

def main():
    # Define a sequential dataset of general string blocks.
    dataset = ["block_0", "block_1", "block_2", "block_3"]

    print(f"Input Data Blocks: {dataset}\n")

    # Initialize the Merkle Tree, which automatically triggers leaf hashing
    # and recursive parent computation up to the root.
    merkle_tree = MerkleTree(dataset)

    print("=" * 82)
    print("Leaf Nodes".center(82))
    print("=" * 82)

    for index, leaf_hash in enumerate(merkle_tree.leaves):
        print(f"\tLeaf {index} Hash: {leaf_hash}")

    print("=" * 82)

    # Summary
    print()
    print(f"Computed Merkle Root: {merkle_tree.root}")

if __name__ == "__main__":
    main()