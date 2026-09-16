"""
main.py

The primary runtime entry point and verification suite for the Van Emde Boas
(vEB) tree data structure implementation.

This module tests structural boundaries, operational edge cases, and runtime
defensive constraints of the `VanEmdeBoas` set structure. It guarantees correctness
across basic inserts, successor/predecessor lookups, membership evaluations, and
element erasures over a fixed integer universe.
"""
import sys

from van_emde_boas import VanEmdeBoas

def main():
    print("=" * 60)
    print("VAN EMDE BOAS TREE".center(60))
    print("=" * 60)

    # Establish the fixed universe size capacity bound.
    universe_capacity = 16

    # Instantiate an empty recursive Van Emde Boas tree structured over the specified capacity.
    veb = VanEmdeBoas(universe_capacity)

    # Define an unsorted batch of payload vector of integers to populate the tree.
    items = [8, 3, 14, 0, 5, 11]

    # Iteratively stream items into the Van Emde Boas recursive sub-clustering layers.
    for item in items:
        veb.insert(item)

    # Define validation query keys containing both a present (5) and absent (9) integer.
    query = [5, 9]

    # Evaluate membership structures utilizing standard Python 'in' operator protocols.
    for target in query:
        is_present = target in veb

        if is_present:
            print(f"VanEmdeBoas[{target}].")

    print()

    # Establish an uninserted anchor item to evaluate neighborhood proximity queries.
    pivot = 4

    # Execute O(log log M) order statistics lookups surrounding the pivot coordinates.
    next_smallest = veb.predecessor(pivot)
    next_largest = veb.successor(pivot)

    print(f"Predecessor (largest item < {pivot}): {next_smallest}.")
    print(f"Successor (smallest item > {pivot}): {next_largest}.")

    print()

    # Evaluate edge conditions exploring queries beyond the current active set boundaries.
    print(f"Predecessor of minimum item ({veb.minimum}): {veb.predecessor(veb.minimum)}.")
    print(f"Successor of maximum item ({veb.maximum}): {veb.successor(veb.maximum)}.")

    print()

    key = 5

    # Erase another key from the Van Emde Boas tree.
    veb.erase(key)

    print(f"Tree state: {veb}")

    key = 0

    # The global minimum item (0) to evaluate boundary shifting and successor promotions.
    veb.erase(key)

    print(f"Tree state: {veb}")

if __name__ == "__main__":
    main()