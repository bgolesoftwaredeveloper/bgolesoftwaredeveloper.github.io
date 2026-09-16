"""
main.py

Knapsack Optimization Runner

This module demonstrates the application of a generalized 0/1 Knapsack
Dynamic Programming algorithm. It establishes a test scenario using
project resource constraints (budgets and expected payouts) to show how
the algorithm determines the optimal distribution of resources.
"""
from knapsack_01 import get_maximum_value

def main():
    # Define the parameters to work with.
    capacity = 50
    item_costs = [10, 20, 30]
    item_rewards = [60, 100, 120]

    # Header.
    print("=========================================")
    print("   Running Knapsack Optimization Engine  ")
    print("=========================================\n")

    # Configuration values.
    print(f"Max Capacity Allowable : {capacity}.")
    print(f"Item Cost Registry : {item_costs}.")
    print(f"Item Reward Profile : {item_rewards}.\n")

    print("Calculating optimal allocation...")

    # Execute Knapsack 0/1 algorithm.
    maximum_achievable_value = get_maximum_value(capacity, item_costs, item_rewards)

    # Summary.
    print(f"Maximum total value achieved: {maximum_achievable_value}.")

if __name__ == "__main__":
    main()