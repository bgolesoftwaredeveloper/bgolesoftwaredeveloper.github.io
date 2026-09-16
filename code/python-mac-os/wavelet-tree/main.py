"""
main.py

An execution entry point and structural demonstration harness designed to validate
the class-based Wavelet Tree implementation.

This script handles mock dataset generation, orchestrates structural initialization,
and executes sample sub-linear range queries to verify alphabet partitioning mechanics
and rank tracking operations across discrete data limits.
"""
from wavelet_tree import WaveletTree

def main():
    # Initialize a integer sequence representing a discrete alphabet payload.
    sequence = [3, 1, 4, 1, 5, 9, 2, 6, 5, 3]

    # Header.
    print("=" * 60)
    print("Wavelet Tree Implementation".center(60))
    print("=" * 60)

    # Instantiate the tree.
    tree = WaveletTree(sequence)

    # Define parameters.
    symbol = 5
    upper_bound_index = 9

    # Calculate the rank.
    calculated_rank = tree.rank(upper_bound_index, symbol)

    print()
    print(f"Execution result: The symbol '{symbol}' appears exactly {calculated_rank} times.")

if __name__ == "__main__":
    main()