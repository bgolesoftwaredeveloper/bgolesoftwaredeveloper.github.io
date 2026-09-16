"""
test_knapsack.py

Automated unit test suite for verifying the 0/1 Knapsack optimization algorithm
across diverse capacity profiles, item asset distributions, and edge-case boundaries.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `get_maximum_value` engine.
    It ensures that bottom-up dynamic programming states compute correctly,
    capacity thresholds restrict choices accurately, and items are cleanly
    included or excluded according to the 0/1 rule.

Testing Scope & Architectural Coverage:
    - Standard Optimization Layouts: Validates classic textbook item arrays with
      known maximum reward benchmarks.
    - Minimal Inputs & Zero Boundaries: Assesses empty structures, absolute zero
      capacities, and free items.
    - Capacity Obstacles: Verifies proper state behavior when items cannot fit
      due to extreme costs or fractional margin mismatches.
    - Distribution Profiles: Evaluates uniform profiles, highly dominant items,
      redundant identical duplicates, and vast value-to-cost ratios.
    - Scale & Edge Constraints: Targets processing limits with massive resource
      over-provisions or exact fit scenarios.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_knapsack.py
"""
import unittest

from knapsack_01 import get_maximum_value

class TestKnapsack(unittest.TestCase):
    """
    Test suite containing 20 comprehensive, isolated test vectors evaluating
    the dynamic programming matrix lookup and maximum value selections.
    """

    def test_01_classic_textbook_scenario(self):
        """1. Standard 4-item problem with diverse capacities and clear optimal layout."""
        capacity = 5
        costs = [1, 2, 3, 4]
        rewards = [10, 20, 30, 40]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 50)

    def test_02_zero_knapsack_capacity(self):
        """2. Knapsack capacity is strictly zero; no item can be taken."""
        capacity = 0
        costs = [1, 2, 3]
        rewards = [10, 20, 30]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 0)

    def test_03_empty_item_lists(self):
        """3. No items are available to select from; reward should be zero."""
        capacity = 10
        costs = []
        rewards = []

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 0)

    def test_04_all_items_exceed_capacity(self):
        """4. Every single item has a cost greater than the total allowable capacity."""
        capacity = 4
        costs = [5, 10, 15]
        rewards = [100, 200, 300]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 0)

    def test_05_all_items_fit_simultaneously(self):
        """5. Total capacity exceeds the combined cost sum of all individual items."""
        capacity = 50
        costs = [5, 10, 15]
        rewards = [10, 20, 30]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 60)

    def test_06_exact_fit_capacity(self):
        """6. The total cost of the items exactly hits the capacity limit."""
        capacity = 10
        costs = [2, 3, 5]
        rewards = [5, 10, 20]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 35)

    def test_07_single_item_fits_exactly(self):
        """7. A single item profile whose cost perfectly consumes the budget."""
        capacity = 7
        costs = [7]
        rewards = [100]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 100)

    def test_08_single_item_too_heavy(self):
        """8. A single item profile whose cost is just over the capacity limit."""
        capacity = 5
        costs = [6]
        rewards = [500]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 0)

    def test_09_fractional_knapsack_counter_test(self):
        """9. Verifies the 0/1 constraint where taking parts of high-ratio items is impossible."""
        capacity = 10
        costs = [6, 5, 5]
        rewards = [60, 40, 40]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 80)

    def test_10_items_with_zero_cost_free_items(self):
        """10. Items that add value but consume zero capacity allocation."""
        capacity = 5
        costs = [0, 0, 3]
        rewards = [15, 25, 10]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 50)

    def test_11_items_with_zero_reward_worthless(self):
        """11. Items that consume space but carry no asset value."""
        capacity = 10
        costs = [2, 4, 3]
        rewards = [0, 0, 0]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 0)

    def test_12_highly_dominant_item(self):
        """12. One item holds massive value compared to others but requires strict sacrifice."""
        capacity = 10
        costs = [1, 1, 1, 10]
        rewards = [2, 2, 2, 100]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 100)

    def test_13_identical_duplicate_items(self):
        """13. Multiple completely identical items to ensure matrix states evaluate redundancies cleanly."""
        capacity = 6
        costs = [3, 3, 3]
        rewards = [10, 10, 10]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 20)

    def test_14_large_cost_and_value_differentials(self):
        """14. Small and huge variables combined within the dynamic allocation array."""
        capacity = 1000
        costs = [999, 1, 500]
        rewards = [10000, 10, 5]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 10010)

    def test_15_low_value_high_cost_outliers(self):
        """15. Bad trade-off profiles that should be systematically ignored by the engine."""
        capacity = 10
        costs = [10, 2, 2]
        rewards = [1, 20, 20]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 40)

    def test_16_uniform_costs_and_rewards(self):
        """16. All items have completely uniform costs and rewards; reduces to count constraint."""
        capacity = 3
        costs = [1, 1, 1, 1, 1]
        rewards = [5, 5, 5, 5, 5]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 15)

    def test_17_capacity_smaller_than_any_cost(self):
        """17. Knapsack has positive volume, but it's smaller than the smallest item cost."""
        capacity = 2
        costs = [3, 4, 5]
        rewards = [10, 20, 30]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 0)

    def test_18_ascending_cost_efficiency_profile(self):
        """18. Items are ordered such that later choices are increasingly cost-efficient."""
        capacity = 5
        costs = [4, 3, 2]
        rewards = [4, 6, 10]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 16)

    def test_19_descending_cost_efficiency_profile(self):
        """19. Items are ordered such that earlier choices are increasingly cost-efficient."""
        capacity = 5
        costs = [2, 3, 4]
        rewards = [10, 6, 4]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 16)

    def test_20_massive_capacity_granular_items(self):
        """20. Large capacity with multiple highly granular tracking points."""
        capacity = 100
        costs = [20, 30, 40, 50]
        rewards = [40, 90, 120, 130]

        result = get_maximum_value(capacity, costs, rewards)

        self.assertEqual(result, 260)

if __name__ == "__main__":
    unittest.main()