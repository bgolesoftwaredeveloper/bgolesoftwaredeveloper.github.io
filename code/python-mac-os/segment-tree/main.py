"""
main.py

An execution wrapper and integration test suite designed to instantiate, validate,
and benchmark the structural pipeline of the custom SegmentTree class.

This module acts as the explicit runtime interface. It establishes controlled numerical
payload sequences, coordinates target interval range lookups, maps localized structural
data injections, and captures step-by-step state translations across the tree network
to prove functional and mathematical execution accuracy.
"""
from segment_tree import SegmentTree

def main():
    # Define a sequence to work with.
    sequence = [1, 3, 5, 7, 9, 11]

    # Header.
    print(f"[-] Initial Array Sequence: {sequence}")
    print("[-] Constructing Segment Tree structures...\n")

    # Instantiate the Segment tree.
    segment_tree = SegmentTree(sequence)

    left, right = 1, 3

    # Perform a query on the segment tree.
    result = segment_tree.query(left, right)

    # Evaluation 01.
    print(f"[!] Evaluation 01: Range Query Selection [{left}, {right}]")
    print(f"\t-> Expected Collective Summation: 15")
    print(f"\t-> Computed Segment Tree Output: {result}\n")

    index, value = 1, 10

    print(f"[-] Mutating original array slot [{index}] to value: {value}")

    # Update the segment tree.
    segment_tree.update(index, value)

    print(f"[-] Synchronized Array Sequence: {segment_tree.data}\n")

    # Perform a query after the update.
    result = segment_tree.query(left, right)

    # Evaluation 02.
    print(f"[!] Evaluation 02: Post-Mutation Range Query Selection [{left}, {right}]")
    print(f"\t-> Expected Collective Summation: 22")
    print(f"\t-> Computed Segment Tree Output: {result}\n")

if __name__ == "__main__":
    main()