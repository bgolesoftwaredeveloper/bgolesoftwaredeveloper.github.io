"""
knapsack_01.py

0/1 Knapsack Problem (Dynamic Programming Implementation)

This module provides an implementation of the classic 0/1 Knapsack problem using
a bottom-up Dynamic Programming (DP) approach. It maximizes total reward given
a collection of items, each with a specific cost and reward, without exceeding
a specified maximum capacity constraint.
"""

def get_maximum_value(capacity: int, costs: list[int], rewards: list[int]) -> int:
    """
    Computes the maximum total reward achievable within a given cost capacity.

    This function builds a 2D look-up table where each cell represents the optimal
    reward for a subset of items and a restricted capacity. It evaluates whether
    including or excluding each item yields a higher total reward.

    Args:
        capacity (int): The maximum allowable total cost constraint (e.g., maximum weight or budget).
        costs (list[int]): A list representing the cost of each individual item.
        rewards (list[int]): A list representing the reward or value of each individual item.

    Returns:
        int: The maximum total reward achievable that satisfies the capacity constraint.
    """
    item_count = len(rewards)

    # Initialize a 2D table of size (item_count + 1) x (capacity + 1) with zeros.
    table = [[0 for _ in range(capacity + 1)] for _ in range(item_count + 1)]

    # Build the table bottom-up.
    for item_index in range(1, item_count + 1):
        # Retrieve the attributes of teh current item (adjusted for 0-based input list indexing).
        current_cost = costs[item_index - 1]
        current_reward = rewards[item_index - 1]

        for current_capacity in range(1, capacity + 1):
            # Option 1: Exclude the item. Inherit the optimal reward from the row above.
            reward_excluding_item = table[item_index - 1][current_capacity]

            # Option 2: Include the item (only viable if its cost fits the current capacity).
            if current_cost <= current_capacity:
                remaining_capacity = current_capacity - current_cost

                # Combine the current item's reward with the optimal reward of the remaining capacity.
                reward_including_item = current_reward + table[item_index - 1][remaining_capacity]
            else:
                # The item cannot fit; its inclusion value defaults.
                reward_including_item = 0

            # Store the highest value strategy for the current subproblem call.
            table[item_index][current_capacity] = max(reward_excluding_item, reward_including_item)

    # The bottom-right cell contains the solution for all items at maximum capacity.
    return table[item_count][capacity]