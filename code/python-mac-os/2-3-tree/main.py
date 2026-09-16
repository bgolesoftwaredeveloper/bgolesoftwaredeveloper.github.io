"""
main.py

Demonstration harness for verifying structural mutations, insertion cascades,
and balancing invariants of a self-balancing 2-3 Tree implementation.

This script executes a deterministic insertion sequence designed to trigger leaf splits,
parent promotions, and root-level structural expansions. It prints the tree layout
after each operational step to verify that the topology maintains a uniform leaf depth.
"""
from two_three_tree import TwoThreeTree

def main():
    print("=" * 60)
    print("2-3 Tree Architectural demonstration".center(60))
    print("=" * 60)

    # Instantiate an empty self-balancing 2-3 tree execution engine.
    tree = TwoThreeTree()

    # Define a sequence of keys engineered to systematically force node saturation (4-nodes),
    # upward median promotion, and tree-height growth.
    sequence = [7, 14, 21, 5, 9, 18, 28, 35, 12]

    print(f"Insertion sequence: {sequence}.\n")

    # Progressively ingest keys and inspect the evolving tree layout after each mutation.
    for index, key in enumerate(sequence, start=1):
        print(f"-- Step {index}: Inserting key [{key}] --")

        try:
            # Insert the scalar key; the tree will automatically resolve overflows.
            tree.insert(key)

            # Print the structure rotated horizontally on its side.
            tree.display()

            print()
        except ValueError as exception:
            raise exception

    print("=" * 60)
    print("Query verification".center(60))
    print("=" * 60)

    # List of keys to query against the constructed tree to verify that routing
    # boundaries (left, middle, right child navigation paths) match expectation.
    targets = [14, 9, 99]

    for target in targets:
        # Traverse down logical node keys recursively to discover element presence.
        found = tree.search(target)

        print(f"Searching for target key [{target:2}: {"Found" if found else "Not found"}]")

if __name__ == "__main__":
    main()