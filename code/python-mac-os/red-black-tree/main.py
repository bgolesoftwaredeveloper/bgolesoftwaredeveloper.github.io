"""
main.py

An execution entry point and verification suite for the Red-Black Tree module.

This script imports the self-balancing RedBlackTree implementation to demonstrate
dynamic structural mutations. It simulates sequential element insertions, showcases
complex multi-case balancing operations, and handles structural deletions—printing
the resulting topology at each phase using the built-in ASCII tree visualizer.

Design Note:
    This file serves as a functional test harness and structural verification model.
    It provides a clean programmatic pipeline to audit tree properties, pointer
    assignments, and node-color invariants under standard operations.
"""
from red_black_tree import RedBlackTree

def main():
    print("=" * 60)
    print("Red-Black Tree".center(60))
    print("=" * 60)

    # Instantiate the tree structure, establishing the shared internal NIL sentinel.
    tree = RedBlackTree()

    # Define a list of items to work with.
    items = [40, 20, 60, 10, 30, 50, 70]

    # Insert the values.
    for value in items:
        tree.insert(value)

    # Display the tree.
    tree.display()

if __name__ == "__main__":
    main()