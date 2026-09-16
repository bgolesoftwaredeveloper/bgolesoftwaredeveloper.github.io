"""
test_kd_tree.py

Automated unit test suite for verifying the K-Dimensional Tree (K-D Tree) implementation
across diverse spatial dimensions, geometric boundaries, and branch pruning limits.

Overview:
    This module uses Python's built-in `unittest` framework to execute a matrix
    of 20 unique testing scenarios against the `KDTree` engineering architecture. It
    ensures that tree instantiation, spatial partitioning, point overrides, node branch
    linkages, nearest neighbor lookups, and hypersphere-hyperplane boundary pruning
    all maintain structural integrity.

Testing Scope & Architectural Coverage:
    - Core Initialization & Guards: Assesses structural invariant exceptions, dimension
      matching boundary constraints, and empty state safety returns.
    - CRUD & Payload Updates: Verifies standard spatial point placement, path routing
      along cycling axes, and in-place metadata updates for duplicate coordinates.
    - Geometric Distance Proximity: Exercises math verification across single and
      multi-dimensional coordinate frames.
    - Backtracking & Pruning Logic: Validates nearest neighbor lookups that trigger
      hypersphere boundary overlaps, as well as scenarios where entire branches are
      successfully pruned out for optimization.

Execution Instructions:
    Run the complete testing matrix natively from the shell or command line via:

        python -m unittest test_kd_tree.py
"""
import unittest

from k_dimensional import Node, KDTree

class TestKDTree(unittest.TestCase):
    """
    Test suite containing 20 comprehensive, isolated test vectors evaluating
    the K-Dimensional Tree spatial partitioning and nearest neighbor optimization logic.
    """

    def test_01_initial_empty_state(self):
        """1. Verifies that an initialized K-D Tree contains an empty root and correct dimensions."""
        tree = KDTree(k=3)

        self.assertIsNone(tree.root)
        self.assertEqual(tree.k, 3)

    def test_02_invalid_dimension_guards(self):
        """2. Validates that instantiating a tree with invalid dimensions throws standard exceptions."""
        # Check type restrictions on k parameter
        with self.assertRaises(TypeError):
            KDTree(k="InvalidType")

        with self.assertRaises(ValueError):
            KDTree(k=0)

    def test_03_node_payload_type_safety(self):
        """3. Confirms that explicit parameter validations raise TypeErrors on bad inputs."""
        # Test bad coordinate type format
        with self.assertRaises(TypeError):
            Node(point="NotATuple", value="Payload")

        with self.assertRaises(TypeError):
            Node(point=(1.0, "InvalidCoordinate"), value="Payload")

        with self.assertRaises(TypeError):
            Node(point=(1.0, 2.0), value=12345)

    def test_04_basic_insertion_and_root_linkage(self):
        """4. Validates that the first inserted coordinate correctly becomes the tree root."""
        tree = KDTree(k=2)

        target_point = (50.0, 50.0)

        tree.insert(target_point, "Root Asset")

        self.assertIsNotNone(tree.root)
        self.assertEqual(tree.root.point, target_point)
        self.assertEqual(tree.root.value, "Root Asset")
        self.assertIsNone(tree.root.left)
        self.assertIsNone(tree.root.right)

    def test_05_insertion_dimension_mismatch_guard(self):
        """5. Assures that inserting a point violating the tree's dimension limit throws a ValueError."""
        tree = KDTree(k=2)

        with self.assertRaises(ValueError):
            tree.insert((10.0, 20.0, 30.0), "Malformed Point")

    def test_06_axis_0_left_right_branching(self):
        """6. Checks that insertion paths follow left/right constraints on the primary Axis 0."""
        tree = KDTree(k=2)

        tree.insert((50.0, 50.0), "Root")
        tree.insert((25.0, 60.0), "Left Leaf")
        tree.insert((75.0, 40.0), "Right Leaf")

        self.assertEqual(tree.root.left.value, "Left Leaf")
        self.assertEqual(tree.root.right.value, "Right Leaf")

    def test_07_axis_1_alternating_depth_branching(self):
        """7. Checks that insertion depth correctly cycles splitting axes to Axis 1."""
        tree = KDTree(k=2)

        tree.insert((50.0, 50.0), "Root (Split Axis 0)")
        tree.insert((25.0, 30.0), "Left Subtree Root (Split Axis 1)")

        tree.insert((35.0, 85.0), "Target Node")

        self.assertEqual(tree.root.left.right.value, "Target Node")

    def test_08_duplicate_point_payload_overwrite(self):
        """8. Validates that inserting an existing duplicate coordinate updates its value string in-place."""
        tree = KDTree(k=2)

        point = (10.0, 20.0)

        tree.insert(point, "Initial Value")
        tree.insert(point, "Updated Value")

        self.assertEqual(tree.root.value, "Updated Value")
        self.assertIsNone(tree.root.left)
        self.assertIsNone(tree.root.right)

    def test_09_euclidean_distance_math_accuracy(self):
        """9. Verifies calculation accuracy for the internal multi-dimensional Euclidean math helper."""
        tree = KDTree(k=3)

        p1 = (1.0, 2.0, 3.0)
        p2 = (4.0, 6.0, 8.0)

        calculated_distance = tree._euclidean_distance(p1, p2)
        expected_distance = 7.0710678118654755

        self.assertAlmostEqual(calculated_distance, expected_distance, places=6)

    def test_10_nearest_neighbor_empty_tree(self):
        """10. Verifies that querying an empty tree safely returns None."""
        tree = KDTree(k=2)

        self.assertIsNone(tree.nearest_neighbor((10.0, 10.0)))

    def test_11_nearest_neighbor_exact_match(self):
        """11. Validates that querying a point matching an existing vertex returns a zero distance scalar."""
        tree = KDTree(k=2)

        target_point = (14.0, 92.0)

        tree.insert((50.0, 50.0), "Root")
        tree.insert(target_point, "Target Station")

        point, value, distance = tree.nearest_neighbor(target_point)

        self.assertEqual(point, target_point)
        self.assertEqual(value, "Target Station")
        self.assertEqual(distance, 0.0)

    def test_12_nearest_neighbor_dimension_mismatch_guard(self):
        """12. Assures that a proximity search with an invalid target coordinate length raises a ValueError."""
        tree = KDTree(k=2)

        tree.insert((50.0, 50.0), "Root")

        with self.assertRaises(ValueError):
            tree.nearest_neighbor((10.0,))

    def test_13_nearest_neighbor_standard_2d_lookup(self):
        """13. Exercises a standard lookup test determining spatial proximity on a small 2D dataset."""
        tree = KDTree(k=2)

        tree.insert((50.0, 50.0), "Hub")
        tree.insert((10.0, 15.0), "Coastal Node")
        tree.insert((80.0, 85.0), "Mountain Node")

        point, value, distance = tree.nearest_neighbor((12.0, 17.0))

        self.assertEqual(value, "Coastal Node")
        self.assertEqual(point, (10.0, 15.0))

    def test_14_nearest_neighbor_with_hyperplane_pruning(self):
        """14. Verifies the optimization phase where search paths successfully prune away unneeded subtrees."""
        tree = KDTree(k=2)

        tree.insert((50.0, 50.0), "Root")
        tree.insert((20.0, 20.0), "Far Left")
        tree.insert((60.0, 60.0), "Near Right")

        point, value, distance = tree.nearest_neighbor((58.0, 58.0))

        self.assertEqual(value, "Near Right")

    def test_15_nearest_neighbor_with_backtracking_search(self):
        """15. Validates search paths that cannot be pruned, forcing traversal across alternative branches."""
        tree = KDTree(k=2)

        tree.insert((50.0, 100.0), "Root")
        tree.insert((49.0, 49.0), "Hidden Close Left")
        tree.insert((52.0, 90.0), "Distant Right")

        point, value, distance = tree.nearest_neighbor((51.0, 49.0))

        self.assertEqual(value, "Hidden Close Left")
        self.assertEqual(point, (49.0, 49.0))

    def test_16_higher_dimensionality_matrix(self):
        """16. Confirms tree structural scaling stability using high-dimensional (5D) space coordinates."""
        tree = KDTree(k=5)

        p1 = (1.0, 2.0, 3.0, 4.0, 5.0)
        p2 = (10.0, 20.0, 30.0, 40.0, 50.0)

        tree.insert(p1, "5D Base Node")
        tree.insert(p2, "5D Remote Node")

        point, value, distance = tree.nearest_neighbor((1.1, 2.1, 3.1, 4.1, 5.1))

        self.assertEqual(value, "5D Base Node")
        self.assertEqual(point, p1)

    def test_17_negative_coordinate_handling(self):
        """17. Assures structural precision and geometric lookups operate accurately using negative coordinate systems."""
        tree = KDTree(k=2)

        tree.insert((-50.0, -50.0), "Negative Center")
        tree.insert((-10.0, -15.0), "Quadrant Two")

        point, value, distance = tree.nearest_neighbor((-12.0, -14.0))

        self.assertEqual(value, "Quadrant Two")
        self.assertEqual(point, (-10.0, -15.0))

    def test_18_monotonically_increasing_line_imbalance(self):
        """18. Tests handling of an imbalanced structural arrangement generated via linearly increasing points."""
        tree = KDTree(k=2)

        for index in range(1, 6):
            tree.insert((float(index), float(index)), f"Node_{index}")

        point, value, distance = tree.nearest_neighbor((1.2, 0.8))

        self.assertEqual(value, "Node_1")
        self.assertEqual(point, (1.0, 1.0))

    def test_19_large_scale_stress_packing(self):
        """19. Evaluates lookup performance and depth transitions under a moderately high structural load."""
        tree = KDTree(k=2)

        for x in range(10):
            for y in range(10):
                tree.insert((float(x), float(y)), f"Grid_{x}_{y}")

        point, value, distance = tree.nearest_neighbor((4.2, 7.8))

        self.assertEqual(value, "Grid_4_8")
        self.assertEqual(point, (4.0, 8.0))

    def test_20_axis_cycling_boundary_wrap(self):
        """20. Confirms tree layers cycle through all k axes back to Axis 0 when depth exceeds dimensionality."""
        # 1-Dimensional spatial boundary context
        tree_1d = KDTree(k=1)

        tree_1d.insert((10.0,), "Root_Depth_0_Axis_0")
        tree_1d.insert((5.0,), "Leaf_Depth_1_Axis_0")

        self.assertEqual(tree_1d.root.left.point, (5.0,))

        tree_3d = KDTree(k=3)

        tree_3d.insert((10.0, 10.0, 10.0), "Root_D0_A0")
        tree_3d.insert((5.0, 20.0, 20.0), "Node_D1_A1")
        tree_3d.insert((5.0, 5.0, 20.0), "Node_D2_A2")
        tree_3d.insert((5.0, 5.0, 5.0), "Node_D3_A0")

        self.assertEqual(tree_3d.root.left.left.left.point, (5.0, 5.0, 5.0))

if __name__ == '__main__':
    unittest.main()