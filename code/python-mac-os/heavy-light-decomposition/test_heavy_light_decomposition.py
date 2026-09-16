"""
test_heavy_light_decomposition.py

Automated unit test suite for verifying the Heavy-Light Decomposition (heavy_light_decomposition)
and Segment Tree data structures across diverse topological arrangements.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `HeavyLightDecomposition` and
    `SegmentTree` components. It ensures structural correctness, boundary safety,
    type validation compliance, and accurate path maximum aggregations.

Testing Scope & Architectural Coverage:
    - Defensive Input Validation: Assesses type checking and range limits on
      graph initialization, node indexing, and tree sizes.
    - Topology Variations: Evaluates line graphs, balanced trees, highly skewed
      structures, star graphs, and single-node trees.
    - heavy_light_decomposition Structural Invariants: Validates internal layout state properties,
      such as depths, subtree calculation mappings, and chain continuities.
    - Range & Path Maxima: Exercises combinations of isolated updates, cross-chain
      leapfrog queries, leaf-to-leaf traversals, and out-of-bounds validations.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_heavy_light_decomposition.py
"""
import unittest

from heavy_light_decomposition import SegmentTree, HeavyLightDecomposition

class TestHeavyLightDecomposition(unittest.TestCase):
    """
    Test suite containing 20 comprehensive, isolated test vectors evaluating
    the Heavy-Light Decomposition structural logic and path query integrations.
    """

    def test_01_segment_tree_invalid_size_type(self):
        """1. Verifies that SegmentTree raises a TypeError if initialized with a non-integer size."""
        with self.assertRaises(TypeError):
            SegmentTree("ten")

    def test_02_segment_tree_invalid_size_value(self):
        """2. Verifies that SegmentTree raises a ValueError if initialized with a size less than or equal to 0."""
        with self.assertRaises(ValueError):
            SegmentTree(0)

        with self.assertRaises(ValueError):
            SegmentTree(-5)

    def test_03_segment_tree_update_parameter_type_checks(self):
        """3. Assures that SegmentTree.update throws a TypeError if structural parameters are non-integers."""
        tree = SegmentTree(5)

        with self.assertRaises(TypeError):
            tree.update("one", 1, 5, 2, 10)

        with self.assertRaises(TypeError):
            tree.update(1, 1, 5, 2, "ten")

    def test_04_segment_tree_query_out_of_bounds(self):
        """4. Confirms SegmentTree.query returns float('-inf') if the query envelope misses the range boundary."""
        tree = SegmentTree(4)

        tree.update(1, 1, 4, 2, 42)

        result = tree.query(1, 1, 4, 5, 8)

        self.assertEqual(result, float('-inf'))

    def test_05_heavy_light_decomposition_invalid_node_count_type(self):
        """5. Validates that HeavyLightDecomposition raises a TypeError on non-integer node counts."""
        with self.assertRaises(TypeError):
            HeavyLightDecomposition("five", {1: []})

    def test_06_heavy_light_decomposition_invalid_node_count_value(self):
        """6. Validates that HeavyLightDecomposition raises a ValueError on invalid node scales."""
        with self.assertRaises(ValueError):
            HeavyLightDecomposition(0, {})

    def test_07_heavy_light_decomposition_invalid_adjacency_list_type(self):
        """7. Assures that HeavyLightDecomposition enforces a dictionary implementation type for adjacency logs."""
        with self.assertRaises(TypeError):
            HeavyLightDecomposition(5,[(1, 2), (2, 3)])

    def test_08_heavy_light_decomposition_missing_node_in_adjacency_list(self):
        """8. Validates that discovery steps catch graph architectural structural gaps and raise a ValueError."""
        incomplete_adjacency = {1: [2]}

        with self.assertRaises(ValueError):
            HeavyLightDecomposition(2, incomplete_adjacency)

    def test_09_single_node_tree_boundary(self):
        """9. Verifies correctness of depth, parent, and pathway computations on a single-node tree."""
        adjacency = {1: []}

        heavy_light_decomposition = HeavyLightDecomposition(1, adjacency)

        self.assertEqual(heavy_light_decomposition.parent[1], 0)
        self.assertEqual(heavy_light_decomposition.depth[1], 0)
        self.assertEqual(heavy_light_decomposition.subtree_size[1], 1)

        heavy_light_decomposition.update_value(1,99)

        self.assertEqual(heavy_light_decomposition.query_path_maximum(1, 1), 99)

    def test_10_linear_line_graph_chaining(self):
        """10. Exercises properties on a line tree graph where heavy paths should combine contiguously."""
        adjacency = {1: [2], 2: [1, 3], 3: [2]}

        heavy_light_decomposition = HeavyLightDecomposition(3, adjacency)

        self.assertEqual(heavy_light_decomposition.heavy_child[1], 2)
        self.assertEqual(heavy_light_decomposition.heavy_child[2], 3)
        self.assertEqual(heavy_light_decomposition.chain_head[3], 1)  # All nodes reside on the matching root head chain

    def test_11_balanced_binary_tree_traversal(self):
        """11. Evaluates heavy_light_decomposition over a symmetrical balanced layout checking subtree size measurements."""
        adjacency = {1: [2, 3], 2: [1], 3: [1]}

        heavy_light_decomposition = HeavyLightDecomposition(3, adjacency)

        self.assertEqual(heavy_light_decomposition.subtree_size[1], 3)
        self.assertEqual(heavy_light_decomposition.subtree_size[2], 1)
        self.assertEqual(heavy_light_decomposition.subtree_size[3], 1)

    def test_12_update_value_out_of_bounds(self):
        """12. Validates that update_value raises a ValueError for arbitrary outside nodes."""
        adjacency = {1: [2], 2: [1]}

        heavy_light_decomposition = HeavyLightDecomposition(2, adjacency)

        with self.assertRaises(ValueError):
            heavy_light_decomposition.update_value(3, 10)

        with self.assertRaises(ValueError):
            heavy_light_decomposition.update_value(0, 10)

    def test_13_update_value_type_check(self):
        """13. Assures update_value catches type-violating index descriptors."""
        adjacency = {1: [2], 2: [1]}

        heavy_light_decomposition = HeavyLightDecomposition(2, adjacency)

        with self.assertRaises(TypeError):
            heavy_light_decomposition.update_value("1",10)

    def test_14_query_path_invalid_node_types(self):
        """14. Checks type validation errors are thrown on invalid path node identifiers."""
        adjacency = {1: [2], 2: [1]}

        heavy_light_decomposition = HeavyLightDecomposition(2, adjacency)

        with self.assertRaises(TypeError):
            heavy_light_decomposition.query_path_maximum("1", end=2)

    def test_15_query_path_out_of_bounds_values(self):
        """15. Checks range verification limits inside path max engine execution paths."""
        adjacency = {1: [2], 2: [1]}

        heavy_light_decomposition = HeavyLightDecomposition(2, adjacency)

        with self.assertRaises(ValueError):
            heavy_light_decomposition.query_path_maximum(1, 99)

    def test_16_asymmetrical_star_graph_chains(self):
        """16. Evaluates a star layout pattern where one hub spawns multiple light branches."""
        adjacency = {1: [2, 3, 4], 2: [1], 3: [1], 4: [1]}

        heavy_light_decomposition = HeavyLightDecomposition(4, adjacency)

        elected_heavy = heavy_light_decomposition.heavy_child[1]

        self.assertTrue(elected_heavy in [2, 3, 4])

        for leaf in [2, 3, 4]:
            if leaf != elected_heavy:
                self.assertEqual(heavy_light_decomposition.chain_head[leaf], leaf)

    def test_17_cross_chain_leapfrog_maximum_query(self):
        """17. Assesses value calculations executing path leaps across independent head contexts."""
        # Tree Structure: 7 -> 4 -> 2 -> 1 <- 3 <- 6 <- 8
        adjacency = {
            1: [2, 3], 2: [1, 4], 3: [1, 6], 4: [2, 7],
            6: [3, 8], 7: [4], 8: [6]
        }

        heavy_light_decomposition = HeavyLightDecomposition(8, adjacency)

        heavy_light_decomposition.update_value(7, 10)
        heavy_light_decomposition.update_value(4, 85)
        heavy_light_decomposition.update_value(2, 20)
        heavy_light_decomposition.update_value(1, 5)
        heavy_light_decomposition.update_value(3, 90)
        heavy_light_decomposition.update_value(6, 40)
        heavy_light_decomposition.update_value(8, 15)

        self.assertEqual(heavy_light_decomposition.query_path_maximum(7, 8), 90)

    def test_18_isolated_path_independence_from_updates(self):
        """18. Verifies that updates outside a specific query track path do not influence calculations."""
        adjacency = {1: [2, 4], 2: [1, 3], 3: [2], 4: [1]}

        heavy_light_decomposition = HeavyLightDecomposition(4, adjacency)

        heavy_light_decomposition.update_value(3, 10)
        heavy_light_decomposition.update_value(2, 20)
        heavy_light_decomposition.update_value(1, 15)
        heavy_light_decomposition.update_value(4, 500)

        self.assertEqual(heavy_light_decomposition.query_path_maximum(3, 1), 20)

    def test_19_deeply_skewed_unbalanced_tree(self):
        """19. Validates robustness under highly skewed paths to ensure correct segment positioning."""
        adjacency = {1: [2], 2: [1, 3], 3: [2, 4], 4: [3, 5], 5: [4]}

        heavy_light_decomposition = HeavyLightDecomposition(5, adjacency)

        heavy_light_decomposition.update_value(1, -10)
        heavy_light_decomposition.update_value(5, 100)

        self.assertEqual(heavy_light_decomposition.query_path_maximum(1, 5), 100)

    def test_20_dynamic_alternating_updates_and_queries(self):
        """20. Assesses long-term consistency through sequences of overlapping value replacements."""
        adjacency = {1: [2], 2: [1, 3], 3: [2]}

        heavy_light_decomposition = HeavyLightDecomposition(3, adjacency)

        heavy_light_decomposition.update_value(1, 5)
        heavy_light_decomposition.update_value(2, 10)
        heavy_light_decomposition.update_value(3, 15)

        self.assertEqual(heavy_light_decomposition.query_path_maximum(1, 3), 15)

        heavy_light_decomposition.update_value(2, 50)

        self.assertEqual(heavy_light_decomposition.query_path_maximum(1, 3), 50)

        heavy_light_decomposition.update_value(2, 1)

        self.assertEqual(heavy_light_decomposition.query_path_maximum(1, 3), 15)

if __name__ == '__main__':
    unittest.main()