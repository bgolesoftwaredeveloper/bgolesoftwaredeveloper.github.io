"""
test_segment_tree.py

Automated unit test suite for verifying the structural lifecycle, partition integrity,
and sub-linear processing primitives of the custom Segment Tree class.

Overview:
    This module utilizes Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the custom tree entity: `__init__`, `_build`,
    `query`, and `update`. It ensures algorithmic precision across standard dataset arrays,
    mathematical edge cases, out-of-bounds safety guards, and extreme mutation scenarios.

Testing Scope & Architectural Coverage:
    - Initialization & Memory Allocation: Validates tree representation sizing multipliers,
      identity tracking states, and structural stability when processing null array vectors.
    - Range Sum Aggregation (Query): Verifies total interval bounds overlaps, exact single-element
      point queries, localized overlapping fractions, and structural safety boundaries.
    - Leaf & Layer Mutations (Update): Evaluates dynamic data injection steps, continuous linear
      value replacements, alternating zero balances, and localized bounds tracking.
    - Integration & Invariance: Confirms end-to-end multi-step update/query iteration loops,
      idempotency invariants, negative value offsets, and broad mathematical stress stability.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_segment_tree.py
"""
import unittest

from segment_tree import SegmentTree

class TestSegmentTree(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating core structural builders,
    sub-linear query optimizations, and cascading layer synchronization update sweeps.
    """

    def test_01_init_empty_array_handling(self):
        """1. Verifies that initializing with an empty array builds an empty tree tracking size zero."""
        data = []

        tree = SegmentTree(data)

        self.assertEqual(tree.size, 0)
        self.assertEqual(tree.tree, [])

    def test_02_init_single_element_leaf(self):
        """2. Validates tree allocation structural contents when parsing an atomic single-element array."""
        data = [42]

        tree = SegmentTree(data)

        self.assertEqual(tree.size, 1)
        self.assertEqual(tree.tree[0], 42)

    def test_03_init_tree_buffer_sizing_boundaries(self):
        """3. Confirms that internal tree lists allocate at worst a four-times padding scale factor."""
        data = [1, 2, 3, 4, 5]

        tree = SegmentTree(data)

        expected_buffer_allocation = 4 * len(data)

        self.assertEqual(len(tree.tree), expected_buffer_allocation)

    def test_04_build_cascading_root_accumulation(self):
        """4. Evaluates that the initialization structural build sums all child leaves at the tree root."""
        data = [1, 2, 3, 4, 5, 6]

        tree = SegmentTree(data)

        self.assertEqual(tree.tree[0], 21)

    def test_05_query_total_array_overlap(self):
        """5. Validates range queries spanning the absolute entire array scope from end to end."""
        data = [10, 20, 30, 40]

        tree = SegmentTree(data)

        computed_sum = tree.query(0, len(data) - 1)

        self.assertEqual(computed_sum, 100)

    def test_06_query_atomic_point_element(self):
        """6. Checks that query calls isolated to a singular index slice return exactly that element value."""
        data = [5, 8, 12, 3, 7]

        tree = SegmentTree(data)

        computed_sum = tree.query(2, 2)

        self.assertEqual(computed_sum, 12)

    def test_07_query_isolated_partial_overlap_left(self):
        """7. Verifies sub-range extraction alignment matching the left internal tree partition layout."""
        data = [1, 3, 5, 7, 9, 11]

        tree = SegmentTree(data)

        computed_sum = tree.query(0, 2)

        self.assertEqual(computed_sum, 9)

    def test_08_query_isolated_partial_overlap_right(self):
        """8. Verifies sub-range extraction alignment matching the right internal tree partition layout."""
        data = [1, 3, 5, 7, 9, 11]

        tree = SegmentTree(data)

        computed_sum = tree.query(3, 5)

        self.assertEqual(computed_sum, 27)

    def test_09_query_straddling_midpoint_intersection(self):
        """9. Validates subset extractions that cross directly over the primary middle split vector."""
        data = [2, 4, 6, 8, 10, 12]

        tree = SegmentTree(data)

        computed_sum = tree.query(1, 4)

        self.assertEqual(computed_sum, 28)

    def test_10_query_negative_out_of_bounds_guard(self):
        """10. Assures negative query parameter requests return zero identity flags without array faults."""
        data = [1, 2, 3, 4]

        tree = SegmentTree(data)

        computed_sum = tree.query(-3, 2)

        self.assertEqual(computed_sum, 0)

    def test_11_query_upper_out_of_bounds_guard(self):
        """11. Assures upper query parameter exceptions return zero identity flags without array faults."""
        data = [1, 2, 3, 4]

        tree = SegmentTree(data)

        computed_sum = tree.query(1, 15)

        self.assertEqual(computed_sum, 0)

    def test_12_query_inverted_boundary_range(self):
        """12. Verifies that inverted query limits (left bounds exceeding right bounds) default to zero returns."""
        data = [10, 20, 30, 40]

        tree = SegmentTree(data)

        computed_sum = tree.query(3, 1)

        self.assertEqual(computed_sum, 0)

    def test_13_update_atomic_leaf_synchronization(self):
        """13. Confirms single node modifications correctly reconstruct internal parent branches upward."""
        data = [1, 2, 3, 4]

        tree = SegmentTree(data)

        tree.update(2, 10)

        self.assertEqual(tree.data[2], 10)
        self.assertEqual(tree.query(2, 2), 10)
        self.assertEqual(tree.query(0, 3), 17)

    def test_14_update_out_of_bounds_low_fault_isolation(self):
        """14. Verifies negative structural index updates reject modification requests silently."""
        data = [5, 5, 5]
        tree = SegmentTree(data)

        tree.update(-1, 99)

        self.assertEqual(tree.query(0, 2), 15)

    def test_15_update_out_of_bounds_high_fault_isolation(self):
        """15. Verifies excessive structural index updates reject modification requests silently."""
        data = [5, 5, 5]
        tree = SegmentTree(data)

        tree.update(100, 99)

        self.assertEqual(tree.query(0, 2), 15)

    def test_16_update_zero_balance_annihilation(self):
        """16. Checks data transformations replacing positive nodes with zero-balance variables."""
        data = [10, 10, 10]
        tree = SegmentTree(data)

        tree.update(1, 0)

        self.assertEqual(tree.query(0, 2), 20)
        self.assertEqual(tree.query(1, 1), 0)

    def test_17_integration_complete_stretching_lifecycle(self):
        """17. Executes a full runtime validation cycle combining progressive mutations and dynamic querying."""
        data = [1, 1, 1, 1, 1, 1, 1, 1]

        tree = SegmentTree(data)

        tree.update(0, 5)
        tree.update(7, 5)
        tree.update(4, 10)

        self.assertEqual(tree.query(0, 3), 8)
        self.assertEqual(tree.query(4, 7), 17)
        self.assertEqual(tree.query(0, 7), 25)

    def test_18_integration_negative_value_offset_handling(self):
        """18. Confirms that tracking structures store and aggregate negative integer boundaries precisely."""
        data = [-5, -10, -15, -20]

        tree = SegmentTree(data)

        self.assertEqual(tree.query(0, 3), -50)

        tree.update(2, 15)  # [-5, -10, 15, -20]

        self.assertEqual(tree.query(1, 3), -15)

    def test_19_integration_update_idempotency_invariance(self):
        """19. Validates node calculation invariance by applying repeated identical update assignments."""
        data = [5, 10, 15]

        tree = SegmentTree(data)

        tree.update(1, 100)
        tree.update(1, 100)
        tree.update(1, 100)

        self.assertEqual(tree.query(0, 2), 120)

    def test_20_integration_sequential_replacement_stretching(self):
        """20. Tests operational resilience by sequentially updating every item inside the allocation structure."""
        data = [1, 2, 3, 4]

        tree = SegmentTree(data)

        for index in range(tree.size):
            tree.update(index, 10)

        self.assertEqual(tree.query(0, 3), 40)
        self.assertEqual(tree.tree[0], 40)

if __name__ == "__main__":
    unittest.main()