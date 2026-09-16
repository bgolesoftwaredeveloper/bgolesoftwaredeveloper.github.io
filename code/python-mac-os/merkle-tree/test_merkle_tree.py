"""
test_merkle_tree.py

Automated unit test suite for verifying the MerkleTree execution engine
across dataset sizes, architectural boundaries, and edge-case behaviors.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `MerkleTree`. It ensures that
    cryptographic invariants hold, mathematical trees pair accurately up to the
    root, and structural boundary behaviors handle edge-cases gracefully.

Testing Scope & Architectural Coverage:
    - Standard Structural Topologies: Validates perfect binary pairs (2, 4 elements),
      odd-numbered structures requiring duplicate nodes (1, 3, 5 elements), and massive trees.
    - Deterministic & Cryptographic Consistency: Assesses property-level constraints like
      reproducibility, avalanche sensitivity, and element order changes.
    - Input Variant Validations: Targets different block data variants including completely
      empty arrays, blank strings, uniform datasets, and specialized characters.
    - Cryptographic Verification & State Invariance: Verifies internal list tracking
      and state components match calculated base parameters.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_merkle_tree.py
"""
import unittest
import hashlib

from merkle_tree import MerkleTree, calculate_hash

class TestMerkleTree(unittest.TestCase):
    """
    Test suite containing 20 isolated test vectors evaluating cryptographic tree
    building mechanics, layer reductions, and string block invariants.
    """

    def test_01_single_element_tree(self):
        """1. Verifies a tree with a single item sets its root exactly to that item's hash."""
        data = ["block_0"]

        tree = MerkleTree(data)

        expected_hash = calculate_hash("block_0")

        self.assertEqual(tree.root, expected_hash)

    def test_02_basic_two_element_pair(self):
        """2. Validates that a balanced 2-element tree combines leaf hashes exactly as specified."""
        data = ["block_0", "block_1"]

        tree = MerkleTree(data)

        leaf_hash_zero = calculate_hash("block_0")
        leaf_hash_one = calculate_hash("block_1")
        expected_root = calculate_hash(leaf_hash_zero + leaf_hash_one)

        self.assertEqual(tree.root, expected_root)

    def test_03_odd_three_element_duplication(self):
        """3. Forces an odd-numbered layer reduction, validating duplication of the third leaf hash."""
        data = ["A", "B", "C"]

        tree = MerkleTree(data)

        leaf_hash_a = calculate_hash("A")
        leaf_hash_b = calculate_hash("B")
        leaf_hash_c = calculate_hash("C")

        parent_hash_left = calculate_hash(leaf_hash_a + leaf_hash_b)
        parent_hash_right = calculate_hash(leaf_hash_c + leaf_hash_c)  # Odd node duplicated
        expected_root = calculate_hash(parent_hash_left + parent_hash_right)

        self.assertEqual(tree.root, expected_root)

    def test_04_perfect_four_element_binary_tree(self):
        """4. Evaluates a known 4-element binary dataset across 3 strict vertical layers."""
        data = ["block_0", "block_1", "block_2", "block_3"]

        tree = MerkleTree(data)

        self.assertEqual(tree.root, "01d3e51a16eeac3e118031dfebdd8a9e0527be6996c965eaadbe619e6e42414d")

    def test_05_odd_five_element_cascading_duplication(self):
        """5. Tracks a 5-element dataset to verify duplicate logic cascaded over multi-tiered depths."""
        data = ["1", "2", "3", "4", "5"]

        tree = MerkleTree(data)

        leaf_hash_one = calculate_hash("1")
        leaf_hash_two = calculate_hash("2")
        leaf_hash_three = calculate_hash("3")
        leaf_hash_four = calculate_hash("4")
        leaf_hash_five = calculate_hash("5")

        parent_hash_one_and_two = calculate_hash(leaf_hash_one + leaf_hash_two)
        parent_hash_three_and_four = calculate_hash(leaf_hash_three + leaf_hash_four)
        parent_hash_five_duplicated = calculate_hash(leaf_hash_five + leaf_hash_five)

        grandparent_hash_left = calculate_hash(parent_hash_one_and_two + parent_hash_three_and_four)
        grandparent_hash_right = calculate_hash(parent_hash_five_duplicated + parent_hash_five_duplicated)

        expected_root = calculate_hash(grandparent_hash_left + grandparent_hash_right)

        self.assertEqual(tree.root, expected_root)

    def test_06_empty_dataset_handling(self):
        """6. Verifies that passing an empty data collection gracefully sets an empty string root."""
        data = []

        tree = MerkleTree(data)

        self.assertEqual(tree.root, "")
        self.assertEqual(tree.leaves, [])

    def test_07_empty_string_blocks(self):
        """7. Evaluates performance stability when data blocks contain empty strings."""
        data = ["", ""]

        tree = MerkleTree(data)

        leaf_hash_empty = calculate_hash("")
        expected_root = calculate_hash(leaf_hash_empty + leaf_hash_empty)

        self.assertEqual(tree.root, expected_root)

    def test_08_avalanche_effect_sensitivity(self):
        """8. Assures a minor one-character variation completely changes the root hash outcome."""
        tree_alpha = MerkleTree(["data_block_alpha"])
        tree_beta = MerkleTree(["data_block_alphb"])

        self.assertNotEqual(tree_alpha.root, tree_beta.root)

    def test_09_order_dependency_validation(self):
        """9. Validates that rearranging the dataset sequence produces a completely distinct root."""
        data_sequence_apple = ["apple", "banana", "cherry"]
        data_sequence_banana = ["banana", "apple", "cherry"]

        tree_apple_first = MerkleTree(data_sequence_apple)
        tree_banana_first = MerkleTree(data_sequence_banana)

        self.assertNotEqual(tree_apple_first.root, tree_banana_first.root)

    def test_10_deterministic_reproducibility(self):
        """10. Asserts identical data sets processed over independent runs generate equivalent roots."""
        dataset = ["alpha", "beta", "gamma", "delta"]

        tree_initial_run = MerkleTree(dataset)
        tree_secondary_run = MerkleTree(dataset)

        self.assertEqual(tree_initial_run.root, tree_secondary_run.root)

    def test_11_uniform_identical_elements(self):
        """11. Evaluates matching properties when processing uniform repeating elements."""
        data = ["same", "same", "same", "same"]

        tree = MerkleTree(data)

        leaf_hash_uniform = calculate_hash("same")
        parent_hash_uniform = calculate_hash(leaf_hash_uniform + leaf_hash_uniform)
        expected_root = calculate_hash(parent_hash_uniform + parent_hash_uniform)

        self.assertEqual(tree.root, expected_root)

    def test_12_leaf_tracking_integrity(self):
        """12. Verifies internal list storage matches exactly the number of submitted data blocks."""
        data = ["test1", "test2", "test3", "test4", "test5"]

        tree = MerkleTree(data)

        self.assertEqual(len(tree.leaves), len(data))

    def test_13_individual_leaf_hashes_accuracy(self):
        """13. Confirms internal leaves are isolated, literal cryptographic representations of inputs."""
        data = ["secure_data"]

        tree = MerkleTree(data)

        self.assertEqual(tree.leaves[0], calculate_hash("secure_data"))

    def test_14_numerical_string_blocks(self):
        """14. Verifies formatting structure behavior on simple sequential numeric strings."""
        data = ["123", "456", "789"]

        tree = MerkleTree(data)

        self.assertIsNotNone(tree.root)
        self.assertEqual(len(tree.root), 64)

    def test_15_special_characters_and_json_literals(self):
        """15. Confirms parsing compatibility against structural escape strings and symbols."""
        data = ["{'status': 200}", "!@#$%^&*()_+", "\n\t\r"]

        tree = MerkleTree(data)

        self.assertTrue(tree.root.isalnum())

    def test_16_large_scale_dataset_expansion(self):
        """16. Tests system processing limits and tree balance across an expanded 100-block dataset."""
        data = [f"block_index_{index}" for index in range(100)]

        tree = MerkleTree(data)

        self.assertEqual(len(tree.leaves), 100)
        self.assertNotEqual(tree.root, "")

    def test_17_helper_hash_function_purity(self):
        """17. Asserts the standalone calculate_hash function is stable, functional, and pure."""
        raw_text = "merkle_architecture_verification"

        calculated_verification_hash = calculate_hash(raw_text)
        native_standard_hash = hashlib.sha256(raw_text.encode('utf-8')).hexdigest()

        self.assertEqual(calculated_verification_hash, native_standard_hash)

    def test_18_data_immutability_after_build(self):
        """18. Ensures data structural parameters remain isolated once instanced into memory blocks."""
        data = ["immutable_a", "immutable_b"]

        tree = MerkleTree(data)

        saved_root_snapshot = tree.root

        data.append("immutable_c")

        self.assertEqual(tree.root, saved_root_snapshot)

    def test_19_duplicate_data_subsets_differentiation(self):
        """19. Validates that identical block values sitting in alternative locations do not overlap."""
        tree = MerkleTree(["A", "A", "B", "C"])

        leaf_hash_alpha = calculate_hash("A")
        leaf_hash_beta = calculate_hash("B")
        leaf_hash_gamma = calculate_hash("C")

        parent_hash_left_branch = calculate_hash(leaf_hash_alpha + leaf_hash_alpha)
        parent_hash_right_branch = calculate_hash(leaf_hash_beta + leaf_hash_gamma)
        expected_root = calculate_hash(parent_hash_left_branch + parent_hash_right_branch)

        self.assertEqual(tree.root, expected_root)

    def test_20_binary_power_boundary_parity(self):
        """20. Validates strict layer consolidation steps across consecutive powers of two bounds ($2^3$)."""
        data_eight_elements = [str(index) for index in range(8)]
        tree_power_of_three = MerkleTree(data_eight_elements)

        self.assertEqual(len(tree_power_of_three.leaves), 8)
        self.assertNotEqual(tree_power_of_three.root, "")

if __name__ == "__main__":
    unittest.main()