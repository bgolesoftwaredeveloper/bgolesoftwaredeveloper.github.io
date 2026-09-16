"""
main.py

An operational execution script that serves as a demonstration harness for the
K-Dimensional Tree (K-D Tree) spatial-partitioning structure.

This module initializes a 2D coordinate network modeling localized fulfillment
and logistics nodes. It handles sequential data insertion, triggers structural
hierarchy visualizations, and executes multi-query Nearest Neighbor Searches (NNS).
By isolating target coordinates, it validates the spatial pruning mechanics and
bounding-box intersection logic inherent to optimized k-dimensional lookups.

Dependencies:
    k_dimensional.py -> Requires an accessible KDTree class implementation exposing
                        'insert', 'nearest_neighbor', and 'display' interfaces.
"""
from k_dimensional import KDTree

def main():
    # 1. Initialize a 2-Dimensional spatial index structure.
    spatial_index = KDTree(k=2)

    # 2. Define a dataset representing a localized fulfillment grid.
    logistics_nodes = [
        ((50.0, 50.0), "Central Matrix Hub"),
        ((25.0, 30.0), "Western Outpost A"),
        ((75.0, 70.0), "Eastern Logistics Center"),
        ((10.0, 15.0), "Coastal Drone Port"),
        ((35.0, 85.0), "Northern Fulfillment Pod"),
        ((60.0, 20.0), "Southern Freight Yard"),
        ((80.0, 45.0), "Atlantic Transit Node"),
    ]

    print("Initializing Simulation Dataset...")
    for coordinates, identifier in logistics_nodes:
        print(f"  -> Registering spatial vertex: {coordinates} associated with '{identifier}'")
        spatial_index.insert(coordinates, identifier)

    print("\nData loading phase complete.\n")

    # 3. Print the internal configuration mapping
    spatial_index.display()
    print()

    # 4. Perform distinct Nearest Neighbor Search tests.
    query_points = [
        (12.0, 18.0),
        (40.0, 75.0),
        (55.0, 45.0),
    ]

    print("=" * 60)
    print("Executing Nearest Neighbor Spatial Queries".center(60))
    print("=" * 60)

    for step, target in enumerate(query_points, start=1):
        print(f"\nQuery #{step}: Locating closest point proxy to target: {target}")

        search_result = spatial_index.nearest_neighbor(target)

        if search_result:
            closest_coord, asset_name, distance = search_result

            print(f"\t[Match Found]")
            print(f"\tIdentifier  : {asset_name}")
            print(f"\tCoordinates : {closest_coord}")
            print(f"\tTrue Scalar Distance : {distance:.4f} units")
        else:
            print("  [Error] Spatial lookup yielded no viable targets.")

    print("\n" + "=" * 60)
    print("-- Simulation Complete --".center(60))
    print("=" * 60)

if __name__ == "__main__":
    main()