"""
test_decisiontree.py

A validation suite containing 20 unique unit tests designed to evaluate the
structural integrity, mathematical purity, and execution edge cases of the
ID3 decision tree induction engine.

Security Note:
    This test suite is isolated and runs within memory space using mock tabular data.
    It does not read or write persistent state parameters to the file system.
"""
import unittest
import math
import pandas as pd

from decisiontree import (
    DecisionTree,
    calculate_entropy,
    calculate_information_gain,
    build_tree
)

class TestDecisionTreeEngine(unittest.TestCase):
    """
    An execution matrix responsible for managing structural validation states,
    entropy limits, and verifying edge-case operations across ID3 pipelines.
    """

    def setUp(self):
        """
        Initializes repeatable structural data matrices prior to executing individual tests.
        """
        self.feature_space = ["Outlook", "Wind"]
        self.target_space = "Play"

    def test_01_node_default_initialization(self):
        """1. Verifies that a default node instantiates empty structural state registers."""
        node = DecisionTree()

        self.assertIsNone(node.feature)
        self.assertIsNone(node.value)
        self.assertIsNone(node.results)
        self.assertEqual(node.children, {})

    def test_02_node_leaf_state_assignment(self):
        """2. Validates that a terminal leaf node successfully binds class outcomes."""
        node = DecisionTree(results="Yes", value="Sunny")

        self.assertEqual(node.results, "Yes")
        self.assertEqual(node.value, "Sunny")
        self.assertEqual(node.children, {})

    def test_03_node_branch_registration(self):
        """3. Confirms child branches bind properly inside the internal node registry."""
        child_node = DecisionTree(results="No")
        root = DecisionTree(feature="Outlook", children={"Rain": child_node})

        self.assertIn("Rain", root.children)
        self.assertEqual(root.children["Rain"].results, "No")

    def test_04_entropy_absolute_homogeneity(self):
        """4. Verifies that a perfectly pure class series yields a Shannon scaler of 0.0."""
        pure_vector = pd.Series(["Yes", "Yes", "Yes", "Yes"])
        entropy = calculate_entropy(pure_vector)

        self.assertEqual(entropy, 0.0)

    def test_05_entropy_perfect_binary_split(self):
        """5. Verifies an exact 50/50 binary class distribution scales to a maximum of 1.0."""
        balanced_vector = pd.Series(["Yes", "No", "Yes", "No"])
        entropy = calculate_entropy(balanced_vector)

        self.assertAlmostEqual(entropy, 1.0, places=5)

    def test_06_entropy_unbalanced_distribution(self):
        """6. Evaluates mathematical precision of entropy against skewed distribution arrays."""
        skewed_vector = pd.Series(["Yes", "Yes", "Yes", "No"])
        expected_entropy = - (0.75 * math.log2(0.75) + 0.25 * math.log2(0.25))

        self.assertAlmostEqual(calculate_entropy(skewed_vector), expected_entropy, places=5)

    def test_07_entropy_empty_series(self):
        """7. Edge Case: Confirms empty array inputs return a safe baseline parameter of 0.0."""
        empty_vector = pd.Series([], dtype=object)

        self.assertEqual(calculate_entropy(empty_vector), 0.0)

    def test_08_entropy_single_element(self):
        """8. Validates that an array containing a lone element evaluates to an entropy of 0.0."""
        isolated_vector = pd.Series(["No"])

        self.assertEqual(calculate_entropy(isolated_vector), 0.0)

    def test_09_information_gain_perfect_split(self):
        """9. Verifies information gain equals 1.0 when a feature cleanly separates classes."""
        data_frame = pd.DataFrame({
            "Outlook": ["Sunny", "Sunny", "Rain", "Rain"],
            "Play": ["Yes", "Yes", "No", "No"]
        })

        gain = calculate_information_gain(data_frame, "Outlook", "Play")

        self.assertAlmostEqual(gain, 1.0, places=5)

    def test_10_information_gain_zero_utility(self):
        """10. Ensures a completely uninformative attribute yields an information gain of 0.0."""
        data_frame = pd.DataFrame({
            "Outlook": ["Sunny", "Sunny", "Sunny", "Sunny"],
            "Play": ["Yes", "No", "Yes", "No"]
        })

        gain = calculate_information_gain(data_frame, "Outlook", "Play")

        self.assertAlmostEqual(gain, 0.0, places=5)

    def test_11_information_gain_mixed_utility(self):
        """11. Evaluates partial informational metrics across standard variance reductions."""
        data_frame = pd.DataFrame({
            "Wind": ["Strong", "Strong", "Weak", "Weak"],
            "Play": ["No", "Yes", "Yes", "Yes"]
        })

        gain = calculate_information_gain(data_frame, "Wind", "Play")

        self.assertAlmostEqual(gain, 0.311278, places=5)

    def test_12_build_tree_base_case_pure(self):
        """12. Base Case 1: Assures building instantly drops to a leaf node if data is pure."""
        data_frame = pd.DataFrame({"Outlook": ["Sunny", "Rain"], "Play": ["Yes", "Yes"]})

        root = build_tree(data_frame, self.feature_space, self.target_space)

        self.assertIsNotNone(root.results)
        self.assertEqual(root.results, "Yes")
        self.assertEqual(root.children, {})

    def test_13_build_tree_base_case_empty_features(self):
        """13. Base Case 2: Resolves to majority vote if features run dry before purity."""
        data_frame = pd.DataFrame({"Play": ["No", "No", "Yes"]})

        root = build_tree(data_frame, features=[], target_name="Play")

        self.assertEqual(root.results, "No")

    def test_14_build_tree_structural_split_selection(self):
        """14. Verifies the algorithm splits on the column offering highest metric gains."""
        data_frame = pd.DataFrame({
            "Outlook": ["Sunny", "Sunny", "Rain", "Rain"],
            "Wind": ["Weak", "Strong", "Weak", "Strong"],
            "Play": ["Yes", "Yes", "No", "No"]
        })

        root = build_tree(data_frame, self.feature_space, self.target_space)

        self.assertEqual(root.feature, "Outlook")

    def test_15_build_tree_feature_deduplication(self):
        """15. Confirms selected attributes are purged from lower branching routines."""
        data_frame = pd.DataFrame({
            "Outlook": ["Sunny", "Sunny", "Rain", "Rain"],
            "Wind": ["Weak", "Strong", "Weak", "Strong"],
            "Play": ["Yes", "No", "Yes", "No"]
        })

        root = build_tree(data_frame, self.feature_space, self.target_space)

        for child in root.children.values():
            self.assertNotEqual(child.feature, "Outlook")

    def test_16_build_tree_unseen_parent_value_forwarding(self):
        """16. Confirms branching metadata flows down correctly to assign structural value attributes."""
        data_frame = pd.DataFrame({"Outlook": ["Overcast"], "Play": ["Yes"]})

        root = build_tree(data_frame, ["Outlook"], "Play", parent_value="RootCondition")

        self.assertEqual(root.value, "RootCondition")

    def test_17_build_tree_numerical_string_compatibility(self):
        """17. Checks that alphanumeric string representations of numerical vectors process accurately."""
        data_frame = pd.DataFrame({
            "Code": ["101", "101", "202", "202"],
            "Play": ["Yes", "Yes", "No", "No"]
        })

        root = build_tree(data_frame, ["Code"], "Play")

        self.assertEqual(root.feature, "Code")
        self.assertIn("101", root.children)

    def test_18_build_tree_highly_skewed_matrix(self):
        """18. Evaluates system behavior under heavily unbalanced asymmetric row entries."""
        data_frame = pd.DataFrame({
            "Outlook": ["Sunny"] * 99 + ["Rain"],
            "Play": ["Yes"] * 99 + ["No"]
        })

        root = build_tree(data_frame, ["Outlook"], "Play")

        self.assertEqual(root.feature, "Outlook")
        self.assertEqual(root.children["Sunny"].results, "Yes")
        self.assertEqual(root.children["Rain"].results, "No")

    def test_19_dataframe_missing_target_key_exception(self):
        """19. Verifies error metrics activate if the named target column key is missing."""
        data_frame = pd.DataFrame({"Outlook": ["Sunny"], "InvalidColumn": ["Yes"]})

        with self.assertRaises(KeyError):
            calculate_entropy(data_frame["Play"])

    def test_20_dataframe_missing_feature_key_exception(self):
        """20. Verifies execution fails explicitly if an invalid feature string name is evaluated."""
        data_frame = pd.DataFrame({"Play": ["Yes"]})

        with self.assertRaises(KeyError):
            calculate_information_gain(data_frame, "NonExistentFeature", "Play")

if __name__ == "__main__":
    unittest.main()