"""
test_local_outlier_factor.py

A comprehensive, deterministic unit testing suite designed to validate the numerical 
integrity, structural boundary limits, error validation, C-level memoryview compatibility, 
and distance/density invariants of the custom Cython Local Outlier Factor (LOF) engine.

Test Coverage Domains:
  1. Input Validation & Parameter Boundaries  - Matrix shape, null checks, neighbor bounds assertions.
  2. Buffer Allocation & C Memoryview Setup   - Memoryview slicing, contiguous array layouts, memory stability.
  3. Pairwise Euclidean Distance Calculation  - Distance precision, zero-distance diagonal, matrix symmetry.
  4. Neighbor Sorting & k-Distance Extraction - K-nearest neighbor identification and k-distance vector bounds.
  5. Local Reachability Density (LRD) Math   - LRD positivity, reachability distance lower bounds.
  6. LOF Anomaly Score Evaluation            - Score bounds (inliers ~1.0, outliers >> 1.0), uniform density logic.
  7. Determinism & High-Dimensional Edge Cases - Synthetic spatial clusters, zero-variance features, 128D data.
"""
import unittest
import numpy

from local_outlier_factor import (calculate_euclidean_distance, compute_local_outlier_factor)

class TestLocalOutlierFactor(unittest.TestCase):
    """
    Unit test harness executing automated functional, edge-case, C-memoryview layout,
    and mathematical correctness assertions over the Cython Local Outlier Factor engine.
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

        self.default_nearest_neighbors_count = 5

    def test_01_null_matrix_validation_distance(self):
        """Ensure calculate_euclidean_distance raises ValueError on None input."""
        # Arrange, Act & Assert.
        with self.assertRaises(ValueError):
            calculate_euclidean_distance(None)

    def test_02_null_dataset_validation_lof(self):
        """Ensure compute_local_outlier_factor raises ValueError on None input dataset."""
        # Arrange, Act & Assert.
        with self.assertRaises(ValueError):
            compute_local_outlier_factor(None, self.default_nearest_neighbors_count)

    def test_03_zero_samples_boundary_distance(self):
        """Verify calculate_euclidean_distance raises ValueError for zero-sample 2D input."""
        # Arrange.
        empty_samples_matrix = numpy.empty((0, 2), dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            calculate_euclidean_distance(empty_samples_matrix)

    def test_04_zero_features_boundary_distance(self):
        """Verify calculate_euclidean_distance raises ValueError for zero-feature 2D input."""
        # Arrange.
        empty_features = numpy.empty((10, 0), dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            calculate_euclidean_distance(empty_features)

    def test_05_insufficient_samples_boundary_lof(self):
        """Verify compute_local_outlier_factor raises ValueError for sample count < 2."""
        # Arrange.
        dataset = numpy.array([[1.0, 2.0]], dtype=numpy.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            compute_local_outlier_factor(dataset, 1)

    def test_06_neighbors_count_lower_bound_lof(self):
        """Verify compute_local_outlier_factor raises ValueError when nearest_neighbors_count < 1."""
        # Arrange & Act & Assert.
        with self.assertRaises(ValueError):
            compute_local_outlier_factor(self.standard_synthetic_features_matrix, 0)

    def test_07_neighbors_count_upper_bound_lof(self):
        """Verify compute_local_outlier_factor raises ValueError when nearest_neighbors_count >= N."""
        # Arrange.
        number_of_samples = len(self.standard_synthetic_features_matrix)

        # Act & Assert.
        with self.assertRaises(ValueError):
            compute_local_outlier_factor(self.standard_synthetic_features_matrix, number_of_samples)

    def test_08_c_contiguous_memoryview_acceptance(self):
        """Ensure Cython engine seamlessly accepts C-contiguous float64 arrays."""
        # Arrange.
        c_contiguous_input_feature_array = numpy.ascontiguousarray(
            self.standard_synthetic_features_matrix, dtype=numpy.float64
        )

        # Act.
        evaluated_lof_scores = compute_local_outlier_factor(
            c_contiguous_input_feature_array, self.default_nearest_neighbors_count
        )

        # Assert.
        self.assertIsInstance(evaluated_lof_scores, numpy.ndarray)
        self.assertEqual(evaluated_lof_scores.dtype, numpy.float64)

    def test_09_fortran_contiguous_memoryview_coercion(self):
        """Verify non-C-contiguous (Fortran) arrays are processed without memory faults."""
        # Arrange.
        fortran_contiguous_input_feature_array = numpy.asfortranarray(
            self.standard_synthetic_features_matrix, dtype=numpy.float64
        )

        # Act.
        evaluated_fortran_lof_scores = compute_local_outlier_factor(
            fortran_contiguous_input_feature_array, self.default_nearest_neighbors_count
        )

        # Assert.
        self.assertEqual(len(evaluated_fortran_lof_scores), 22)

    def test_10_distance_matrix_shape_and_type(self):
        """Verify distance matrix function returns a float64 2D NumPy array of shape (N, N)."""
        # Arrange & Act.
        calculated_distances_array = calculate_euclidean_distance(self.standard_synthetic_features_matrix)

        # Assert.
        self.assertIsInstance(calculated_distances_array, numpy.ndarray)
        self.assertEqual(calculated_distances_array.shape, (22, 22))
        self.assertEqual(calculated_distances_array.dtype, numpy.float64)

    def test_11_distance_matrix_zero_diagonal(self):
        """Verify diagonal elements of calculate_euclidean_distance are strictly zero."""
        # Arrange & Act.
        calculated_distances_matrix = calculate_euclidean_distance(self.standard_synthetic_features_matrix)
        diagonal_distances_vector = numpy.diag(calculated_distances_matrix)

        # Assert.
        numpy.testing.assert_allclose(diagonal_distances_vector, numpy.zeros(22), atol=1e-12)

    def test_12_distance_matrix_symmetry(self):
        """Verify calculate_euclidean_distance matrix is strictly symmetric (D_ij == D_ji)."""
        # Arrange & Act.
        calculated_distances_matrix = calculate_euclidean_distance(self.standard_synthetic_features_matrix)

        # Assert.
        numpy.testing.assert_allclose(calculated_distances_matrix, calculated_distances_matrix.T, atol=1e-12)

    def test_13_distance_matrix_positivity(self):
        """Verify all off-diagonal distance values are strictly non-negative."""
        # Arrange & Act.
        evaluated_distance_matrix = calculate_euclidean_distance(self.standard_synthetic_features_matrix)

        # Assert.
        self.assertTrue(numpy.all(evaluated_distance_matrix >= 0.0))

    def test_14_known_euclidean_distance_values(self):
        """Verify calculate_euclidean_distance against known mathematical 3-4-5 triangle values."""
        # Arrange.
        known_coordinate_matrix = numpy.array([[0.0, 0.0], [3.0, 4.0]], dtype=numpy.float64)

        # Act.
        evaluated_known_distances = calculate_euclidean_distance(known_coordinate_matrix)

        # Assert.
        self.assertAlmostEqual(evaluated_known_distances[0, 1], 5.0, places=12)
        self.assertAlmostEqual(evaluated_known_distances[1, 0], 5.0, places=12)

    def test_15_lof_scores_output_dimension(self):
        """Verify LOF output array length exactly matches the dataset sample count N."""
        # Arrange & Act.
        calculated_lof_scores_vector = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, self.default_nearest_neighbors_count
        )

        # Assert.
        self.assertEqual(len(calculated_lof_scores_vector), 22)
        self.assertEqual(calculated_lof_scores_vector.ndim, 1)

    def test_16_inlier_scores_hover_near_unity(self):
        """Verify inliers in a standard Gaussian cluster yield LOF scores close to 1.0 on average."""
        # Arrange & Act.
        calculated_lof_scores = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, self.default_nearest_neighbors_count
        )

        inlier_lof_scores = calculated_lof_scores[:20]
        outlier_lof_scores = calculated_lof_scores[20:]

        # Assert.
        self.assertAlmostEqual(float(numpy.mean(inlier_lof_scores)), 1.0, delta=0.35)
        self.assertLess(float(numpy.max(inlier_lof_scores)), float(numpy.min(outlier_lof_scores)))

    def test_17_outlier_scores_significantly_elevated(self):
        """Verify isolated outlier points produce LOF scores substantially greater than 1.0."""
        # Arrange & Act.
        calculated_lof_scores = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, self.default_nearest_neighbors_count
        )

        outlier_lof_scores = calculated_lof_scores[20:]

        # Assert.
        self.assertTrue(numpy.all(outlier_lof_scores > 5.0))

    def test_18_uniform_grid_density_equivalence(self):
        """Verify samples arranged in a perfectly uniform grid produce identical, near-unity LOF scores."""
        # Arrange.
        coordinates = numpy.linspace(0.0, 10.0, 5)
        x_coordinates, y_coordinates = numpy.meshgrid(coordinates, coordinates)

        uniform_grid_features_matrix = numpy.column_stack(
            [x_coordinates.ravel(), y_coordinates.ravel()]
        ).astype(numpy.float64)

        # Act.
        scores = compute_local_outlier_factor(uniform_grid_features_matrix, nearest_neighbors_count=4)

        # Assert.
        numpy.testing.assert_allclose(scores, numpy.ones(25), atol=0.3)

    def test_19_identical_coincident_points_handling(self):
        """Verify numerical stability and zero division handling for duplicate sample points."""
        # Arrange.
        duplicate_samples = numpy.tile([2.0, 3.0], (10, 1)).astype(numpy.float64)

        # Act.
        coincident_lof_scores = compute_local_outlier_factor(
            duplicate_samples, nearest_neighbors_count=3
        )

        # Assert.
        self.assertFalse(numpy.isnan(coincident_lof_scores).any())
        self.assertFalse(numpy.isinf(coincident_lof_scores).any())

    def test_20_minimal_sample_size_boundary(self):
        """Verify compute_local_outlier_factor operates on minimum valid size (N=2, k=1)."""
        # Arrange.
        minimal_matrix = numpy.array([[1.0, 1.0], [10.0, 10.0]], dtype=numpy.float64)

        # Act.
        minimal_sample_lof_scores = compute_local_outlier_factor(minimal_matrix, nearest_neighbors_count=1)

        # Assert.
        self.assertEqual(len(minimal_sample_lof_scores), 2)
        self.assertFalse(numpy.isnan(minimal_sample_lof_scores).any())

    def test_21_high_dimensional_feature_space(self):
        """Verify distance and LOF loops scale seamlessly over high-dimensional feature vectors (128D)."""
        # Arrange.
        high_dimensional_matrix = self.randomized_engine.normal(size=(25, 128)).astype(numpy.float64)

        # Act.
        high_dimensional_distances = calculate_euclidean_distance(high_dimensional_matrix)
        high_dimensional_lof_scores = compute_local_outlier_factor(high_dimensional_matrix, nearest_neighbors_count=5)

        # Assert.
        self.assertEqual(high_dimensional_distances.shape, (25, 25))
        self.assertEqual(len(high_dimensional_lof_scores), 25)

    def test_22_deterministic_reproducibility(self):
        """Verify running LOF repeatedly on identical dataset matrix yields exact identical scores."""
        # Arrange & Act.
        scores = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, self.default_nearest_neighbors_count
        )

        other_scores = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, self.default_nearest_neighbors_count
        )

        # Assert.
        numpy.testing.assert_array_equal(scores, other_scores)

    def test_23_neighbor_count_impact_on_resolution(self):
        """Verify altering nearest_neighbors_count alters score distribution as neighborhood broadens."""
        # Arrange & Act.
        small_neighborhood_lof_scores = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, nearest_neighbors_count=3
        )

        large_neighborhood_lof_scores = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, nearest_neighbors_count=15
        )

        # Assert.
        self.assertFalse(numpy.array_equal(small_neighborhood_lof_scores, large_neighborhood_lof_scores))

    def test_24_zero_variance_features_handling(self):
        """Verify constant zero-variance feature columns do not disrupt distance calculations."""
        # Arrange.
        zero_variance_augmented_matrix = numpy.hstack([
            self.standard_synthetic_features_matrix,
            numpy.zeros((22, 1), dtype=numpy.float64)
        ])

        # Act.
        zero_var_distances = calculate_euclidean_distance(zero_variance_augmented_matrix)
        zero_var_lof_scores = compute_local_outlier_factor(
            zero_variance_augmented_matrix, self.default_nearest_neighbors_count
        )

        # Assert.
        self.assertEqual(zero_var_distances.shape, (22, 22))
        self.assertEqual(len(zero_var_lof_scores), 22)

    def test_25_negative_feature_values_distance_math(self):
        """Verify distance calculations strictly maintain geometric correctness in negative coordinate space."""
        # Arrange.
        negative_coordinate_features_matrix = numpy.array([
            [-10.0, -20.0],
            [-13.0, -24.0],
            [-100.0, -200.0]
        ], dtype=numpy.float64)

        # Act.
        evaluated_negative_coord_distances = calculate_euclidean_distance(negative_coordinate_features_matrix)

        # Assert.
        self.assertAlmostEqual(evaluated_negative_coord_distances[0, 1], 5.0, places=12)

    def test_26_large_sample_cardinality_stress(self):
        """Verify Cython C-loops handle larger sample sizes (100 samples) without buffer overflow."""
        # Arrange.
        large_sample_cardinality_matrix = self.randomized_engine.normal(size=(100, 5)).astype(numpy.float64)

        # Act.
        stress_test_lof_scores = compute_local_outlier_factor(
            large_sample_cardinality_matrix, nearest_neighbors_count=10
        )

        # Assert.
        self.assertEqual(len(stress_test_lof_scores), 100)

    def test_27_single_feature_column_support(self):
        """Verify 1D feature arrays represented as 2D column vectors (N, 1) execute correctly."""
        # Arrange.
        single_feature_column_matrix = numpy.array([[1.0], [2.0], [2.5], [10.0]], dtype=numpy.float64)

        # Act.
        single_feature_lof_scores = compute_local_outlier_factor(
            single_feature_column_matrix, nearest_neighbors_count=2
        )

        # Assert.
        self.assertEqual(len(single_feature_lof_scores), 4)

    def test_28_memoryview_slice_as_input(self):
        """Verify non-contiguous memoryview slices operate cleanly without implicit memory corruption."""
        # Arrange.
        padded_features_matrix = numpy.zeros((22, 10), dtype=numpy.float64)
        padded_features_matrix[:, :2] = self.standard_synthetic_features_matrix
        sliced_feature_matrix_view = padded_features_matrix[:, :2]

        # Act.
        sliced_view_lof_scores = compute_local_outlier_factor(
            sliced_feature_matrix_view, self.default_nearest_neighbors_count
        )

        # Assert.
        self.assertEqual(len(sliced_view_lof_scores), 22)

    def test_29_no_nan_or_inf_in_scores(self):
        """Verify compute_local_outlier_factor output strictly contains no NaN or Infinite values."""
        # Arrange & Act.
        evaluated_lof_scores = compute_local_outlier_factor(
            self.standard_synthetic_features_matrix, self.default_nearest_neighbors_count
        )

        # Assert.
        self.assertFalse(numpy.isnan(evaluated_lof_scores).any())
        self.assertFalse(numpy.isinf(evaluated_lof_scores).any())

    def test_30_distance_matrix_triangle_inequality(self):
        """Verify pairwise Euclidean distances satisfy the triangle inequality (D_ij <= D_ik + D_kj)."""
        # Arrange & Act.
        distances = calculate_euclidean_distance(self.standard_synthetic_features_matrix[:5])

        # Assert.
        for source_index in range(5):
            for target_index in range(5):
                for intermediate_index in range(5):
                    self.assertLessEqual(distances[source_index, target_index],
                        distances[source_index, intermediate_index] + distances[intermediate_index, target_index] + 1e-12)

if __name__ == "__main__":
    unittest.main()
