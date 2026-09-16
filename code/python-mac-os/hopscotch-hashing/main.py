"""
main.py

An execution harness demonstrating the runtime behavior and spatial properties
of the Hopscotch Hash Table implementation.

This script instantiates a bounded Hopscotch Hash Table, populates it with a
sequence of conflicting keys to deliberately force hash collisions, and shows
how the underlying structure handles localized bitmask updates and element
displacements without dropping into classic open-addressing primary clusters.
"""
from hopscotch import HopscotchTable

def main():
    # Define the parameters to work with.
    capacity = 8
    neighborhood_radius = 4

    # Instantiate the Hopscotch table.
    hopscotch_table = HopscotchTable(capacity, neighborhood_radius)

    # Display the empty table.
    hopscotch_table.display()

    # Insert items.
    hopscotch_table.insert(2, "Alpha")
    hopscotch_table.insert(10, "Beta")
    hopscotch_table.insert(18, "Gamma")
    hopscotch_table.insert(26, "Delta")

    # Show the hopscotch table.
    hopscotch_table.display()

    # Define search targets to query.
    targets = [2, 10, 18, 26, 99]

    for target in targets:
        result = hopscotch_table.lookup(target)
        status = f"Found -> '{result}'" if result is not None else "Not found."

        print(f"Querying key: {target:^2}: {status}.")

    hopscotch_table.insert(10, "Omega")

    print(f"Verification - Key 10 look yields: '{hopscotch_table.lookup(10)}'")

    # Delete an existing entry.
    hopscotch_table.erase(18)

    # Display the final table.
    hopscotch_table.display()

if __name__ == "__main__":
    main()