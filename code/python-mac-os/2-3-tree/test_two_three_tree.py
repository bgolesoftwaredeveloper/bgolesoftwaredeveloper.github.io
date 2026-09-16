"""
test_two_three_tree.py

A comprehensive unit test suite containing 20 unique functional, boundary,
and invariant structural verification scenarios for the custom TwoThreeTree class.
"""
import unittest

from two_three_tree import TwoThreeTree, Node

class TestTwoThreeTree(unittest.TestCase):
    def setUp(self):
        """Set up an empty tree instance before executing each test case."""
        self.tree = TwoThreeTree()

    def test_01_initialization_empty_tree(self):
        """Ensure an initialized tree engine starts cleanly with a null root state."""
        self.assertIsNone(self.tree.root)

    def test_02_single_key_insertion(self):
        """Verify inserting a baseline scalar establishes a leaf root containing one key."""
        self.tree.insert(10)

        self.assertIsNotNone(self.tree.root)
        self.assertTrue(self.tree.root.is_leaf())
        self.assertEqual(self.tree.root.keys, [10])

    def test_03_duplicate_insertion_prevention(self):
        """Ensure inserting an identical key raises an invariance constraint ValueError."""
        self.tree.insert(42)

        with self.assertRaises(ValueError):
            self.tree.insert(42)

    def test_04_none_insertion_prohibition(self):
        """Ensure insertion engine rejects uninitialized or None type arguments outright."""
        with self.assertRaises(ValueError):
            self.tree.insert(None)

    def test_05_node_mutation_to_three_node(self):
        """Verify inserting a second sequential key mutates the root into a legal 3-node."""
        self.tree.insert(15)
        self.tree.insert(5)

        # A 3-node keeps keys sorted and has no children if it's a leaf layer.
        self.assertEqual(self.tree.root.keys, [5, 15])
        self.assertTrue(self.tree.root.is_leaf())

    def test_06_root_split_on_third_key(self):
        """Verify inserting a third element splits an unstable 4-node and increases global height."""
        self.tree.insert(10)
        self.tree.insert(20)
        self.tree.insert(15)

        self.assertEqual(self.tree.root.keys, [15])
        self.assertFalse(self.tree.root.is_leaf())
        self.assertEqual(len(self.tree.root.children), 2)
        self.assertEqual(self.tree.root.children[0].keys, [10])
        self.assertEqual(self.tree.root.children[1].keys, [20])

    def test_07_search_empty_tree(self):
        """Verify that looking up elements in an unpopulated hierarchy safely returns False."""
        self.assertFalse(self.tree.search(100))

    def test_08_search_non_existent_key(self):
        """Verify targeted branch traversal returns False when a value is absent from a populated tree."""
        for key in [50, 30, 70]:
            self.tree.insert(key)

        self.assertFalse(self.tree.search(99))

    def test_09_search_valid_keys(self):
        """Verify search tracking successfully locates all inserted elements across different branches."""
        keys = [25, 12, 38, 6, 18, 31, 45]

        for key in keys:
            self.tree.insert(key)

        for key in keys:
            self.assertTrue(self.tree.search(key), f"Key {key} was inserted but could not be located.")

    def test_10_sequential_ascending_insertion_balance(self):
        """Verify that a monotonically increasing sequence triggers chain splits and maintains balance."""
        ascending_sequence = list(range(1, 11))

        for key in ascending_sequence:
            self.tree.insert(key)

        for key in ascending_sequence:
            self.assertTrue(self.tree.search(key))

    def test_11_sequential_descending_insertion_balance(self):
        """Verify that a monotonically decreasing sequence splits left boundaries correctly without drift."""
        descending_sequence = list(range(10, 0, -1))

        for key in descending_sequence:
            self.tree.insert(key)

        for key in descending_sequence:
            self.assertTrue(self.tree.search(key))

    def test_12_internal_node_split_cascade(self):
        """Test intermediate structural parent split promotion when an internal child node saturates."""
        sequence = [20, 40, 10, 30, 50, 5, 15]

        for key in sequence:
            self.tree.insert(key)

        self.tree.insert(12)

        for key in sequence + [12]:
            self.assertTrue(self.tree.search(key))

    def test_13_perfect_height_invariance(self):
        """Manually verify calculated tree leaf nodes share a perfectly uniform global depth."""
        for key in [7, 14, 21, 5, 9, 18, 28, 35, 12]:
            self.tree.insert(key)

        # Use internal invariant validation method directly to check global depth consistency
        try:
            height = self.tree._validate_invariants(self.tree.root)

            self.assertGreater(height, 0)
        except ValueError as exception:
            self.fail(f"Tree structure drifted out of uniform depth bounds: {exception}")

    def test_14_node_is_leaf_property(self):
        """Verify Node structural classification flags correctly identify leaf vs internal states."""
        leaf_node = Node(5)

        self.assertTrue(leaf_node.is_leaf())

        child_node = Node(1)
        parent_node = Node(10, left = child_node)

        self.assertFalse(parent_node.is_leaf())

    def test_15_node_is_full_property(self):
        """Verify Node evaluation flags accurately identify illegal 4-node saturation boundaries."""
        node = Node(10)

        self.assertFalse(node.is_full())

        node.insert_key(20)

        self.assertFalse(node.is_full())

        node.insert_key(15)

        self.assertTrue(node.is_full())

    def test_16_string_keys_type_uniformity(self):
        """Verify that the engine natively handles alternate comparable types such as lexicographical text."""
        keys = ["delta", "alpha", "omega", "gamma", "beta"]

        for key in keys:
            self.tree.insert(key)

        for key in keys:
            self.assertTrue(self.tree.search(key))

    def test_17_negative_and_floating_point_keys(self):
        """Verify mathematical tracking bounds handle combinations of signed integers and floats."""
        mixed_numerical_keys = [-10, 3.14, -2.5, 0, 100.85]

        for key in mixed_numerical_keys:
            self.tree.insert(key)

        for key in mixed_numerical_keys:
            self.assertTrue(self.tree.search(key))

    def test_18_search_none_handling(self):
        """Ensure search traversal pipelines return False gracefully when handed None queries."""
        self.tree.insert(10)

        self.assertFalse(self.tree.search(None))

    def test_19_structural_child_ordering_invariance(self):
        """Verify parent pointer index tracking correlates correctly with key partition borders."""
        for key in [10, 20, 30]:
            self.tree.insert(key)

        self.assertLess(self.tree.root.children[0].keys[0], self.tree.root.keys[0])
        self.assertGreater(self.tree.root.children[1].keys[0], self.tree.root.keys[0])

    def test_20_massive_randomized_stride(self):
        """Execute a chaotic fuzz-style insertion sweep over a broad data stream to test structural stress."""
        import random

        random.seed(2026)

        large_dataset = list(set(random.randint(-5000, 5000) for _ in range(500)))

        for key in large_dataset:
            self.tree.insert(key)

        for key in large_dataset:
            self.assertTrue(self.tree.search(key))

if __name__ == "__main__":
    unittest.main()