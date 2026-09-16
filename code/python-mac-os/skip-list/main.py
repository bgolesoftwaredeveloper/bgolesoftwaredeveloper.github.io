"""
main.py

An execution driver script designed to validate and demonstrate the runtime behavioral
characteristics of a probabilistic, multi-layered Skip List data structure.

This module provides a structured simulation workspace that populates an empty skip list matrix,
evaluates fast horizontal/vertical search traversals, modifies existing key payloads, and executes
dynamic pointer-unlinking routines via item erasure.

Security & Integrity Note:
    This verification harness prints real-time structural snapshots using uniform alignment
    formatting to visually confirm topological integrity and lane-promotion mechanics.
"""
from skip_list import SkipList

def main():
    print("=" * 60)
    print("-- Skip List --".center(60))
    print("=" * 60)

    # Instantiate a skip list instance.
    skip_list = SkipList()

    # Initialize items to work with.
    items =  [(3, "Apple"), (12, "Banana"), (7, "Cherry"), (19, "Date"), (5, "Elderberry")]

    # Insert the items.
    for key, value in items:
        skip_list.insert(key, value)

    # Show the skip list after insertion's
    skip_list.display()

    # Define search keys to query.
    search_keys = [7, 100]

    # Attempt to search for keys.
    for key in search_keys:
        result = skip_list.search(key)

        print(f"\tSearching for key: {key:3d} | Found: {result}.")

    target_key, target_value = 7, "Cranberry"

    # Update a value in the skip list.
    skip_list.update(target_key, target_value)

    print(f"\tVerification lookup for key {target_key} | Current value: {skip_list.search(target_key)}")

    removal_key = 12

    # Erase an item from the skip list.
    success = skip_list.erase(removal_key)

    print(f"\tAttempting to erase key: {removal_key} | Operation successful: {success}.")

if __name__ == "__main__":
    main()