"""
main.py

An operational execution entry point demonstrating Heavy-Light Decomposition (HLD)
coupled with an underlying Segment Tree over an asymmetrical branching tree structure.
"""
from heavy_light_decomposition import HeavyLightDecomposition

def main():
    print("=" * 60)
    print("Initializing Heavy-Light Decomposition Execution Context")
    print("=" * 60)

    # Define an asymmetrical branching tree architecture with 9 nodes.
    number_of_nodes = 9

    adjacency_list = {
        1: [2, 3],
        2: [1, 4, 5],
        3: [1, 6],
        4: [2, 7],
        5: [2],
        6: [3, 8, 9],
        7: [4],
        8: [6],
        9: [6]
    }

    # Step 1: Instantiate the HLD framework.
    # This automatically invokes structural discovery and chain allocation passes.
    heavy_light_decomposition = HeavyLightDecomposition(number_of_nodes, adjacency_list)

    # Step 2: Seed the structural tree footprint with initial numerical node values.
    # Node values represent data payloads assigned directly to their positional index.
    node_values = {
        1: 15,
        2: 32,
        3: 8,
        4: 45,
        5: 12,
        6: 19,
        7: 2,
        8: 54,
        9: 21
    }

    print("\n[Action] Seeding active tree nodes with target value weights...")

    for node, value in node_values.items():
        heavy_light_decomposition.update_value(node, value)
        print(f"\t-> Node ({node}) initialized with value: {value}")

    print("\n" + "-" * 40)
    print("Executing Path Maximum Queries")
    print("-" * 40)

    maximum_branch = heavy_light_decomposition.query_path_maximum(7, 2)

    print(f"[Query] Maximum value on localized path 7 -> 2: {maximum_branch} (Expected: 45)")

    # Scenario B: Path query traversing completely across the root structure between different chains.
    # Path: 7 -> 4 -> 2 -> 1 -> 3 -> 6 -> 8 (Values: 2, 45, 32, 15, 8, 19, 54)
    maximum_branch = heavy_light_decomposition.query_path_maximum(7, 8)

    print(f"[Query] Maximum value on long cross-tree path 7 -> 8: {maximum_branch} (Expected: 54)")

    print("\n" + "-" * 40)
    print("Modifying Tree Node Context State (Point Updates)")
    print("-" * 40)

    # Scenario C: Dynamic optimization update.
    # We update Node 1 (the global root) to an extreme value of 99.
    print("[Update] Modifying root Node (1) value to 99...")

    heavy_light_decomposition.update_value(1,99)

    # Re-running the path check between 7 and 8. The path now includes the new highest value.
    max_branch_b_updated = heavy_light_decomposition.query_path_maximum(7, 8)

    print(f"[Query] Re-evaluating cross-tree path 7 -> 8: {max_branch_b_updated} (Expected: 99)")

    # Scenario D: Re-evaluating isolated branches to prove local updates do not leak context.
    # Path 5 -> 4 -> 7 (Values: 12, 45, 2) does not contain Node 1.
    maximum_branch = heavy_light_decomposition.query_path_maximum(5, 7)

    print(f"[Query] Maximum value on isolated branch path 5 -> 7: {maximum_branch} (Expected: 45)")

    print("\n" + "=" * 60)
    print("Demonstration Sequence Finalized Successfully")
    print("=" * 60)

if __name__ == "__main__":
    main()