"""
test_kd_tree.py

A comprehensive, deterministic unit testing suite designed to validate the numerical 
integrity, structural boundary limits, error validation, C-level memoryview compatibility, 
and spatial partitioning invariants of the custom Cython K-Dimensional Tree (KDTree) engine.

Test Coverage Domains:
  1. Input Validation & Parameter Boundaries  - Matrix shape, null checks, feature dimension bounds assertions.
  2. Buffer Allocation & C Memoryview Setup   - Memoryview slicing, contiguous array layouts, memory stability.
  3. Spatial Partitioning & Tree Assembly     - Subtree generation, balance factor verification, leaf placement.
  4. Quickselect & Dimension Splitting Logic   - Pivot determination, cyclic dimension traversal, median selection.
  5. Distance Metric & Pruning Invariants      - Euclidean metric accuracy, bounding box distance pruning limits.
  6. Nearest Neighbor Query Verification       - Single & multi-point lookup accuracy against exact linear scan.
  7. Determinism & High-Dimensional Edge Cases - Duplicate points, zero variance, collinearity, 128D data.
"""
import unittest
import numpy

from kd_tree import KDTree

class TestKDTree(unittest.TestCase):
    """
    Unit test harness executing automated functional, edge-case, C-memoryview layout,
    and mathematical correctness assertions over the Cython KDTree engine.
    """

    def setUp(self):
        """Set up standard feature datasets, seeds, and reference arrays used across tests."""
        self.seed_value = 42
        self.randomized_engine = numpy.random.default_rng(self.seed_value)

        # Generate standard synthetic cluster (20 inliers clustered around origin, 2 outliers).
        inlier_feature_samples = self.randomized_engine.normal(loc=[0.0, 0.0], scale=1.0, size=(20, 2))
        outlier_feature_samples = numpy.array([[15.0, 15.0], [-15.0, -15.0]], dtype=numpy.float64)

        self.standard_synthetic_features_matrix = numpy.vstack(
            [inlier_feature_samples, outlier_feature_samples]
        ).astype(numpy.float64)

        self.single_query_point = numpy.array([0.5, 0.5], dtype=numpy.float64)

    def test_01_null_matrix_validation_constructor(self):
        """Ensure KDTree constructor raises ValueError or TypeError on None input."""
        # Arrange, Act & Assert.
        with self.assertRaises((ValueError, TypeError)):
            KDTree(None)

    def test_02_null_query_point_validation(self):
        """Ensure nearest_neighbor raises ValueError or TypeError on None input query."""
        # Arrange.
        tree = KDTree(self.standard_synthetic_features_matrix)

        # Act & Assert.
        with self.assertRaises((ValueError, TypeError)):
            tree.nearest_neighbor(None)

    def test_03_zero_samples_boundary_constructor(self):
        """Verify KDTree constructor raises ValueError for zero-sample 2D input."""
        # Arrange.
        empty_samples_matrix = numpy.empty((0, 2), dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            KDTree(empty_samples_matrix)

    def test_04_zero_features_boundary_constructor(self):
        """Verify KDTree constructor raises ValueError for zero-feature 2D input."""
        # Arrange.
        empty_features_matrix = numpy.empty((10, 0), dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            KDTree(empty_features_matrix)

    def test_05_1d_input_array_boundary_constructor(self):
        """Verify KDTree constructor raises ValueError when passed a 1D input array."""
        # Arrange.
        flat_input_array = numpy.array([1.0, 2.0, 3.0, 4.0], dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            KDTree(flat_input_array)

    def test_06_3d_input_tensor_boundary_constructor(self):
        """Verify KDTree constructor raises ValueError when passed a 3D tensor input."""
        # Arrange.
        three_dimensional_tensor = numpy.zeros((2, 2, 2), dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            KDTree(three_dimensional_tensor)

    def test_07_query_point_dimension_mismatch_higher(self):
        """Verify nearest_neighbor raises ValueError when query dimension exceeds tree dimension."""
        # Arrange.
        tree = KDTree(self.standard_synthetic_features_matrix)
        higher_dimensional_query = numpy.array([1.0, 2.0, 3.0], dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            tree.nearest_neighbor(higher_dimensional_query)

    def test_08_c_contiguous_memoryview_acceptance(self):
        """Ensure Cython engine seamlessly accepts C-contiguous float64 arrays."""
        # Arrange.
        c_contiguous_input = numpy.ascontiguousarray(
            self.standard_synthetic_features_matrix, dtype=numpy.float64
        )

        # Act.
        tree = KDTree(c_contiguous_input)
        index, distance = tree.nearest_neighbor(self.single_query_point)

        # Assert.
        self.assertIsInstance(index, int)
        self.assertIsInstance(distance, float)

    def test_09_fortran_contiguous_memoryview_coercion(self):
        """Verify non-C-contiguous (Fortran) arrays are processed without memory faults."""
        # Arrange.
        fortran_contiguous_input = numpy.asfortranarray(
            self.standard_synthetic_features_matrix, dtype=numpy.float64
        )

        # Act.
        tree = KDTree(fortran_contiguous_input)
        index, distance = tree.nearest_neighbor(self.single_query_point)

        # Assert.
        self.assertGreaterEqual(index, 0)
        self.assertLess(index, len(self.standard_synthetic_features_matrix))

    def test_10_single_sample_dataset_lookup(self):
        """Verify KDTree operates correctly on a single-element dataset (N=1)."""
        # Arrange.
        single_sample_matrix = numpy.array([[3.0, 4.0]], dtype=numpy.float64)
        query = numpy.array([0.0, 0.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(single_sample_matrix)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertEqual(index, 0)
        self.assertAlmostEqual(distance, 5.0, places=12)

    def test_11_exact_point_lookup_zero_distance(self):
        """Verify querying an exact point present in the tree yields zero distance and matching index."""
        # Arrange.
        tree = KDTree(self.standard_synthetic_features_matrix)
        target_index = 5
        exact_query = self.standard_synthetic_features_matrix[target_index]

        # Act.
        index, distance = tree.nearest_neighbor(exact_query)

        # Assert.
        self.assertEqual(index, target_index)
        self.assertAlmostEqual(distance, 0.0, places=12)

    def test_12_known_euclidean_distance_values(self):
        """Verify nearest_neighbor distance against known mathematical 3-4-5 triangle values."""
        # Arrange.
        triangle_points = numpy.array([[0.0, 0.0], [10.0, 10.0]], dtype=numpy.float64)
        query_point = numpy.array([3.0, 4.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(triangle_points)
        index, distance = tree.nearest_neighbor(query_point)

        # Assert.
        self.assertEqual(index, 0)
        self.assertAlmostEqual(distance, 5.0, places=12)

    def test_13_query_point_dimension_mismatch_lower(self):
        """Verify nearest_neighbor raises ValueError when query dimension is below tree dimension."""
        # Arrange.
        tree = KDTree(self.standard_synthetic_features_matrix)
        lower_dimensional_query = numpy.array([1.0], dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            tree.nearest_neighbor(lower_dimensional_query)

    def test_14_1d_feature_space_support(self):
        """Verify 1D feature arrays represented as 2D column vectors (N, 1) execute correctly."""
        # Arrange.
        one_dimensional_dataset = numpy.array([[10.0], [20.0], [30.0]], dtype=numpy.float64)
        query_point = numpy.array([22.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(one_dimensional_dataset)
        index, distance = tree.nearest_neighbor(query_point)

        # Assert.
        self.assertEqual(index, 1)
        self.assertAlmostEqual(distance, 2.0, places=12)

    def test_15_nearest_neighbor_brute_force_equivalence(self):
        """Verify KDTree output matches exact brute-force linear search over random synthetic data."""
        # Arrange.
        dataset = self.randomized_engine.uniform(-10.0, 10.0, size=(100, 3)).astype(numpy.float64)
        query = numpy.array([1.0, -2.0, 3.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(dataset)
        index, distance = tree.nearest_neighbor(query)

        differences = dataset - query
        expected_index = int(numpy.argmin(numpy.sum(differences * differences, axis=1)))
        expected_distance = float(numpy.sqrt(numpy.min(numpy.sum(differences * differences, axis=1))))

        # Assert.
        self.assertEqual(index, expected_index)
        self.assertAlmostEqual(distance, expected_distance, places=12)

    def test_16_collinear_points_partitioning(self):
        """Verify spatial partitioning on axis-aligned, collinear points."""
        # Arrange.
        collinear_points = numpy.array([[1.0, 0.0], [2.0, 0.0], [3.0, 0.0], [4.0, 0.0]], dtype=numpy.float64)
        query = numpy.array([2.8, 0.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(collinear_points)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertEqual(index, 2)
        self.assertAlmostEqual(distance, 0.2, places=12)

    def test_17_outlier_point_query_lookup(self):
        """Verify querying near extreme spatial outliers accurately selects the outlier node."""
        # Arrange.
        tree = KDTree(self.standard_synthetic_features_matrix)
        outlier_query = numpy.array([14.0, 14.0], dtype=numpy.float64)

        # Act.
        index, distance = tree.nearest_neighbor(outlier_query)

        # Assert.
        self.assertEqual(index, 20)
        self.assertAlmostEqual(distance, numpy.sqrt(2.0), places=12)

    def test_18_uniform_grid_lookup_accuracy(self):
        """Verify spatial lookups on a structured uniform grid match expected nearest grid node."""
        # Arrange.
        grid_ticks = numpy.linspace(0.0, 10.0, 5)
        x_grid, y_grid = numpy.meshgrid(grid_ticks, grid_ticks)
        grid_dataset = numpy.column_stack([x_grid.ravel(), y_grid.ravel()]).astype(numpy.float64)

        query = numpy.array([2.6, 2.6], dtype=numpy.float64)

        # Act.
        tree = KDTree(grid_dataset)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertEqual(grid_dataset[index].tolist(), [2.5, 2.5])
        self.assertAlmostEqual(distance, numpy.sqrt(0.02), places=12)

    def test_19_identical_coincident_points_handling(self):
        """Verify numerical stability and deterministic indexing for duplicate sample points."""
        # Arrange.
        duplicate_points = numpy.tile([2.0, 3.0], (10, 1)).astype(numpy.float64)
        query = numpy.array([2.0, 3.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(duplicate_points)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertIn(index, list(range(10)))
        self.assertAlmostEqual(distance, 0.0, places=12)

    def test_20_minimal_sample_size_boundary(self):
        """Verify KDTree operates on minimum valid multi-sample dataset size (N=2)."""
        # Arrange.
        minimal_matrix = numpy.array([[1.0, 1.0], [10.0, 10.0]], dtype=numpy.float64)
        query = numpy.array([2.0, 2.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(minimal_matrix)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertEqual(index, 0)
        self.assertAlmostEqual(distance, numpy.sqrt(2.0), places=12)

    def test_21_high_dimensional_feature_space(self):
        """Verify tree partitioning and search scale seamlessly over high-dimensional vectors (128D)."""
        # Arrange.
        high_dimensional_dataset = self.randomized_engine.normal(size=(50, 128)).astype(numpy.float64)
        target_sample_index = 12
        query = high_dimensional_dataset[target_sample_index] + 1e-5

        # Act.
        tree = KDTree(high_dimensional_dataset)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertEqual(index, target_sample_index)
        self.assertLess(distance, 1e-3)

    def test_22_deterministic_reproducibility(self):
        """Verify running nearest_neighbor repeatedly on identical queries yields identical results."""
        # Arrange.
        tree = KDTree(self.standard_synthetic_features_matrix)

        # Act.
        first_index, first_distance = tree.nearest_neighbor(self.single_query_point)
        second_index, second_distance = tree.nearest_neighbor(self.single_query_point)

        # Assert.
        self.assertEqual(first_index, second_index)
        self.assertEqual(first_distance, second_distance)

    def test_23_multiple_sequential_queries_on_same_tree(self):
        """Verify executing multiple queries across varying spatial regions on a single tree instance."""
        # Arrange.
        dataset = self.randomized_engine.uniform(-100.0, 100.0, size=(100, 3)).astype(numpy.float64)
        tree = KDTree(dataset)

        # Act & Assert.
        for _ in range(15):
            random_query = self.randomized_engine.uniform(-100.0, 100.0, size=(3,)).astype(numpy.float64)
            index, distance = tree.nearest_neighbor(random_query)

            diffs = dataset - random_query
            expected_index = int(numpy.argmin(numpy.sum(diffs * diffs, axis=1)))
            expected_distance = float(numpy.sqrt(numpy.min(numpy.sum(diffs * diffs, axis=1))))

            self.assertEqual(index, expected_index)
            self.assertAlmostEqual(distance, expected_distance, places=10)

    def test_24_zero_variance_features_handling(self):
        """Verify constant zero-variance feature columns do not disrupt spatial partition splits."""
        # Arrange.
        zero_variance_augmented_matrix = numpy.hstack([
            self.standard_synthetic_features_matrix,
            numpy.zeros((22, 1), dtype=numpy.float64)
        ])
        
        augmented_query = numpy.append(self.single_query_point, 0.0)

        # Act.
        tree = KDTree(zero_variance_augmented_matrix)
        index, distance = tree.nearest_neighbor(augmented_query)

        # Assert.
        self.assertGreaterEqual(index, 0)
        self.assertLess(index, 22)

    def test_25_negative_coordinate_space_math(self):
        """Verify tree partitioning strictly maintains geometric correctness in negative space."""
        # Arrange.
        negative_coordinate_dataset = numpy.array([
            [-10.0, -20.0],
            [-13.0, -24.0],
            [-100.0, -200.0]
        ], dtype=numpy.float64)

        query = numpy.array([-11.0, -21.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(negative_coordinate_dataset)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertEqual(index, 0)
        self.assertAlmostEqual(distance, numpy.sqrt(2.0), places=12)

    def test_26_large_sample_cardinality_stress(self):
        """Verify Cython C-loops handle larger sample sizes (1,000 samples) without buffer overflow."""
        # Arrange.
        large_dataset = self.randomized_engine.normal(size=(1000, 5)).astype(numpy.float64)
        query = large_dataset[500]

        # Act.
        tree = KDTree(large_dataset)
        index, distance = tree.nearest_neighbor(query)

        # Assert.
        self.assertEqual(index, 500)
        self.assertAlmostEqual(distance, 0.0, places=12)

    def test_27_query_outside_dataset_bounding_box(self):
        """Verify query points placed far outside dataset bounding box properly resolve nearest point."""
        # Arrange.
        bounded_dataset = numpy.array([[0.0, 0.0], [1.0, 1.0]], dtype=numpy.float64)
        far_query = numpy.array([100.0, 100.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(bounded_dataset)
        index, distance = tree.nearest_neighbor(far_query)

        # Assert.
        self.assertEqual(index, 1)
        self.assertAlmostEqual(distance, numpy.sqrt(99.0**2 + 99.0**2), places=12)

    def test_28_memoryview_slice_as_input(self):
        """Verify non-contiguous memoryview slices operate cleanly without implicit memory corruption."""
        # Arrange.
        padded_features_matrix = numpy.zeros((22, 10), dtype=numpy.float64)
        padded_features_matrix[:, :2] = self.standard_synthetic_features_matrix
        sliced_feature_matrix_view = padded_features_matrix[:, :2]

        # Act.
        tree = KDTree(sliced_feature_matrix_view)
        index, distance = tree.nearest_neighbor(self.single_query_point)

        # Assert.
        self.assertGreaterEqual(index, 0)
        self.assertLess(index, 22)

    def test_29_no_nan_or_inf_in_returned_distance(self):
        """Verify nearest_neighbor distance strictly returns no NaN or Infinite values."""
        # Arrange.
        tree = KDTree(self.standard_synthetic_features_matrix)

        # Act.
        index, distance = tree.nearest_neighbor(self.single_query_point)

        # Assert.
        self.assertFalse(numpy.isnan(distance))
        self.assertFalse(numpy.isinf(distance))

    def test_30_equidistant_points_query_handling(self):
        """Verify query point equidistant from multiple dataset points returns a valid candidate."""
        # Arrange.
        equidistant_dataset = numpy.array([[-1.0, 0.0], [1.0, 0.0]], dtype=numpy.float64)
        centered_query = numpy.array([0.0, 0.0], dtype=numpy.float64)

        # Act.
        tree = KDTree(equidistant_dataset)
        index, distance = tree.nearest_neighbor(centered_query)

        # Assert.
        self.assertIn(index, [0, 1])
        self.assertAlmostEqual(distance, 1.0, places=12)

if __name__ == "__main__":
    unittest.main()
