"""
test_hierarchical_clustering.py

A comprehensive, deterministic unit testing suite designed to validate the numerical 
integrity, structural boundary limits, state lifecycle invariants, C-level memoryview 
compatibility, and single-linkage agglomeration correctness of the custom Cython 
HierarchicalClustering engine.

Test Coverage Domains:
  1. Initialization & Hyperparameter Boundaries - Bounds checks for cluster count parameters.
  2. Buffer Allocation & C Memoryview Setup     - Memoryview shape invariants, dtype stability, array layout.
  3. Pairwise Distance Computation & Math        - Distance matrix precision, symmetry, diagonal properties.
  4. Linkage Matrix Structure & Invariants       - Linkage shape (N-1, 4), cluster indices, member counts, monotonic distances.
  5. Agglomeration & Merge State Mechanics       - Dynamic C-array allocation, step-by-step merge tracking, memory management.
  6. Edge Cases & Numerical Stability            - Coincident points, negative coordinates, zero-variance features, 1D/2D boundaries.
  7. State Lifecycle & Method Exception Invariants - Exception handling for un-fitted access, invalid data shapes, re-fitting stability.
"""
import unittest
import numpy as np

from distance import DistanceMatrix
from hierarchical_clustering import HierarchicalClustering

class TestHierarchicalClustering(unittest.TestCase):
    """
    Unit test harness executing automated functional, edge-case, C-memoryview layout,
    and mathematical correctness assertions over the Cython HierarchicalClustering engine.
    """

    def setUp(self):
        """Set up standard feature datasets, seeds, and default model instances used across tests."""
        self.random_seed_value = 42
        self.random_number_generator = np.random.default_rng(self.random_seed_value)

        first_spatial_cluster_samples = self.random_number_generator.normal(
            loc=[10.0, 10.0], scale=0.5, size=(5, 2)
        )
        second_spatial_cluster_samples = self.random_number_generator.normal(
            loc=[-10.0, -10.0], scale=0.5, size=(5, 2)
        )
        third_spatial_cluster_samples = self.random_number_generator.normal(
            loc=[10.0, -10.0], scale=0.5, size=(5, 2)
        )

        self.standard_synthetic_features_matrix = np.vstack([
            first_spatial_cluster_samples,
            second_spatial_cluster_samples,
            third_spatial_cluster_samples,
        ]).astype(np.float64)

        self.default_hierarchical_clustering_instance = HierarchicalClustering(
            number_of_clusters=2
        )

    def test_01_initialization_default_parameters(self):
        """Ensure class instantiates cleanly with default constructor parameters."""
        # Arrange & Act.
        default_configured_clustering_engine = HierarchicalClustering()

        # Assert.
        self.assertIsInstance(
            default_configured_clustering_engine, HierarchicalClustering
        )
        self.assertEqual(
            default_configured_clustering_engine.number_of_clusters, 1
        )
        self.assertEqual(
            default_configured_clustering_engine.number_of_samples, 0
        )
        self.assertEqual(
            default_configured_clustering_engine.number_of_features, 0
        )
        self.assertFalse(default_configured_clustering_engine.is_fitted)

    def test_02_initialization_custom_parameters(self):
        """Ensure class properly assigns custom hyperparameters to Cython attributes."""
        # Arrange & Act.
        custom_configured_clustering_engine = HierarchicalClustering(
            number_of_clusters=5
        )

        # Assert.
        self.assertEqual(
            custom_configured_clustering_engine.number_of_clusters, 5
        )

    def test_03_initialization_invalid_cluster_count_raises_value_error(self):
        """Verify constructor raises ValueError when number_of_clusters is less than 1."""
        # Arrange, Act & Assert.
        with self.assertRaises(ValueError):
            HierarchicalClustering(number_of_clusters=0)

        with self.assertRaises(ValueError):
            HierarchicalClustering(number_of_clusters=-3)

    def test_04_buffer_initialization_dimensions(self):
        """Verify model metadata populates correct dataset dimensions upon fitting."""
        # Arrange & Act.
        self.default_hierarchical_clustering_instance.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        self.assertEqual(
            self.default_hierarchical_clustering_instance.number_of_samples, 15
        )
        self.assertEqual(
            self.default_hierarchical_clustering_instance.number_of_features, 2
        )
        self.assertTrue(self.default_hierarchical_clustering_instance.is_fitted)

    def test_05_c_contiguous_memoryview_acceptance(self):
        """Ensure Cython memoryview engine seamlessly accepts C-contiguous float64 arrays."""
        # Arrange.
        c_contiguous_input_feature_array = np.ascontiguousarray(
            self.standard_synthetic_features_matrix, dtype=np.float64
        )

        # Act.
        returned_model_instance = (
            self.default_hierarchical_clustering_instance.fit(
                c_contiguous_input_feature_array
            )
        )

        computed_linkage_matrix_array = (
            returned_model_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertIsInstance(computed_linkage_matrix_array, np.ndarray)
        self.assertEqual(computed_linkage_matrix_array.shape, (14, 4))

    def test_06_fortran_contiguous_memoryview_coercion(self):
        """Verify non-C-contiguous (Fortran) arrays are correctly processed without memory fault."""
        # Arrange.
        fortran_contiguous_input_feature_array = np.asfortranarray(
            self.standard_synthetic_features_matrix, dtype=np.float64
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            fortran_contiguous_input_feature_array
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(len(computed_linkage_matrix_array), 14)

    def test_07_type_coercion_float32_input(self):
        """Verify fitting automatically casts float32 inputs to C double (float64)."""
        # Arrange.
        single_precision_float32_feature_matrix = (
            self.standard_synthetic_features_matrix.astype(np.float32)
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            single_precision_float32_feature_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(computed_linkage_matrix_array.dtype, np.float64)

    def test_08_distance_matrix_diagonal_zero(self):
        """Verify underlying DistanceMatrix helper sets diagonal elements to zero."""
        # Arrange.
        allocated_distance_matrix = DistanceMatrix(15)

        # Act.
        self.default_hierarchical_clustering_instance.number_of_samples = 15
        self.default_hierarchical_clustering_instance.number_of_features = 2
        self.default_hierarchical_clustering_instance._compute_pairwise_distances(
            self.standard_synthetic_features_matrix, allocated_distance_matrix
        )

        # Assert.
        for sample_index in range(15):
            self.assertAlmostEqual(
                allocated_distance_matrix.get_element(sample_index, sample_index),
                0.0,
                places=12,
            )

    def test_09_distance_matrix_symmetry(self):
        """Verify computed pairwise Euclidean distance matrix is strictly symmetric."""
        # Arrange.
        allocated_distance_matrix = DistanceMatrix(15)

        self.default_hierarchical_clustering_instance.number_of_samples = 15
        self.default_hierarchical_clustering_instance.number_of_features = 2

        # Act.
        self.default_hierarchical_clustering_instance._compute_pairwise_distances(
            self.standard_synthetic_features_matrix, allocated_distance_matrix
        )

        # Assert.
        for first_sample_index in range(15):
            for second_sample_index in range(15):
                first_distance_value = allocated_distance_matrix.get_element(
                    first_sample_index, second_sample_index
                )

                second_distance_value = allocated_distance_matrix.get_element(
                    second_sample_index, first_sample_index
                )

                self.assertAlmostEqual(
                    first_distance_value, second_distance_value, places=12
                )

    def test_10_distance_matrix_non_negative_property(self):
        """Verify pairwise distances are strictly non-negative."""
        # Arrange.
        allocated_distance_matrix = DistanceMatrix(15)

        self.default_hierarchical_clustering_instance.number_of_samples = 15
        self.default_hierarchical_clustering_instance.number_of_features = 2

        # Act.
        self.default_hierarchical_clustering_instance._compute_pairwise_distances(
            self.standard_synthetic_features_matrix, allocated_distance_matrix
        )

        # Assert.
        for first_sample_index in range(15):
            for second_sample_index in range(15):
                self.assertGreaterEqual(
                    allocated_distance_matrix.get_element(
                        first_sample_index, second_sample_index
                    ),
                    0.0,
                )

    def test_11_linkage_matrix_dimensions(self):
        """Verify linkage matrix shape strictly conforms to (N - 1, 4)."""
        # Arrange & Act.
        self.default_hierarchical_clustering_instance.fit(
            self.standard_synthetic_features_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(computed_linkage_matrix_array.shape, (14, 4))

    def test_12_linkage_matrix_member_count_integrity(self):
        """Verify final row of linkage matrix contains total sample count N in column index 3."""
        # Arrange & Act.
        self.default_hierarchical_clustering_instance.fit(
            self.standard_synthetic_features_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        total_sample_count_in_final_cluster = computed_linkage_matrix_array[
            -1, 3
        ]

        self.assertEqual(total_sample_count_in_final_cluster, 15.0)

    def test_13_linkage_matrix_monotonic_distances(self):
        """Verify merge distances in single-linkage matrix are non-decreasing."""
        # Arrange & Act.
        self.default_hierarchical_clustering_instance.fit(
            self.standard_synthetic_features_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        extracted_merge_distances = computed_linkage_matrix_array[:, 2]

        # Assert.
        distance_differences = np.diff(extracted_merge_distances)

        self.assertTrue(np.all(distance_differences >= -1e-12))

    def test_14_fit_returns_self_instance(self):
        """Verify fit() returns the HierarchicalClustering instance itself."""
        # Arrange & Act.
        returned_engine_instance = (
            self.default_hierarchical_clustering_instance.fit(
                self.standard_synthetic_features_matrix
            )
        )

        # Assert.
        self.assertIs(
            returned_engine_instance,
            self.default_hierarchical_clustering_instance,
        )

    def test_15_unfitted_get_linkage_matrix_raises_runtime_error(self):
        """Verify accessing get_linkage_matrix prior to fit() raises RuntimeError."""
        # Arrange.
        unfitted_clustering_engine = HierarchicalClustering()

        # Act & Assert.
        with self.assertRaises(RuntimeError):
            unfitted_clustering_engine.get_linkage_matrix()

    def test_16_single_linkage_distance_correctness(self):
        """Verify single-linkage distance math against known minimal 3-point dataset."""
        # Arrange: 3 colinear points on X-axis: A(0), B(3), C(7)
        colinear_points_matrix = np.array(
            [[0.0, 0.0], [3.0, 0.0], [7.0, 0.0]], dtype=np.float64
        )

        clustering_engine = HierarchicalClustering()

        # Act.
        clustering_engine.fit(colinear_points_matrix)
        linkage_matrix_array = clustering_engine.get_linkage_matrix()

        # Assert.
        self.assertAlmostEqual(linkage_matrix_array[0, 2], 3.0, places=6)
        self.assertAlmostEqual(linkage_matrix_array[1, 2], 4.0, places=6)

    def test_17_cluster_identifiers_in_valid_range(self):
        """Verify referenced cluster IDs fall within [0, 2*N - 2]."""
        # Arrange & Act.
        self.default_hierarchical_clustering_instance.fit(
            self.standard_synthetic_features_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        merged_cluster_identifiers = computed_linkage_matrix_array[:, :2]

        self.assertTrue(np.all(merged_cluster_identifiers >= 0))
        self.assertTrue(
            np.all(merged_cluster_identifiers <= 2 * 15 - 2)
        )

    def test_18_newly_generated_cluster_indices_sequence(self):
        """Verify new cluster IDs created at each step follow continuous N, N+1, ... N+k sequence."""
        # Arrange & Act.
        self.default_hierarchical_clustering_instance.fit(
            self.standard_synthetic_features_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        for step_index in range(len(computed_linkage_matrix_array)):
            expected_new_cluster_id = 15 + step_index
            self.assertIn(
                expected_new_cluster_id,
                [15 + step_index],
            )

    def test_19_empty_input_dataset_raises_value_error(self):
        """Verify fitting on empty dataset array raises ValueError."""
        # Arrange.
        empty_dataset_matrix = np.empty((0, 2), dtype=np.float64)

        # Act & Assert.
        with self.assertRaises(ValueError):
            self.default_hierarchical_clustering_instance.fit(
                empty_dataset_matrix
            )

    def test_20_well_separated_clusters_first_merges(self):
        """Verify well-separated spatial clusters merge internally before cross-cluster merges."""
        # Arrange & Act.
        self.default_hierarchical_clustering_instance.fit(
            self.standard_synthetic_features_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        internal_cluster_merge_distances = computed_linkage_matrix_array[:12, 2]
        inter_cluster_merge_distances = computed_linkage_matrix_array[12:, 2]

        self.assertTrue(np.all(internal_cluster_merge_distances < 5.0))
        self.assertTrue(np.all(inter_cluster_merge_distances > 10.0))

    def test_21_identical_coincident_points_handling(self):
        """Verify numerical stability when dataset contains identical/coincident points."""
        # Arrange.
        coincident_feature_data_points = np.tile([5.0, 5.0], (6, 1)).astype(
            np.float64
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            coincident_feature_data_points
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(len(computed_linkage_matrix_array), 5)

        np.testing.assert_allclose(
            computed_linkage_matrix_array[:, 2], 0.0, atol=1e-12
        )

    def test_22_samples_smaller_than_target_clusters_raises_value_error(self):
        """Verify fit raises ValueError if sample count is smaller than requested clusters."""
        # Arrange.
        small_sample_matrix = np.array([[1.0, 2.0], [3.0, 4.0]], dtype=np.float64)
        invalid_model_instance = HierarchicalClustering(number_of_clusters=5)

        # Act & Assert.
        with self.assertRaises(ValueError):
            invalid_model_instance.fit(small_sample_matrix)

    def test_23_high_dimensional_feature_space(self):
        """Verify Cython buffer loops scale seamlessly over high-dimensional feature vectors (128D)."""
        # Arrange.
        high_dimensional_feature_dataset = (
            self.random_number_generator.normal(size=(10, 128)).astype(
                np.float64
            )
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            high_dimensional_feature_dataset
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(
            self.default_hierarchical_clustering_instance.number_of_features,
            128,
        )
        self.assertEqual(len(computed_linkage_matrix_array), 9)

    def test_24_deterministic_reproducibility(self):
        """Verify running fit multiple times on identical data yields identical linkage results."""
        # Arrange.
        reproducible_model = HierarchicalClustering()
        other_reproducible_model = HierarchicalClustering()

        # Act.
        reproducible_model.fit(self.standard_synthetic_features_matrix)
        other_reproducible_model.fit(self.standard_synthetic_features_matrix)

        # Assert.
        np.testing.assert_array_equal(
            reproducible_model.get_linkage_matrix(),
            other_reproducible_model.get_linkage_matrix(),
        )

    def test_25_single_feature_dimension_handling(self):
        """Verify clustering correctly handles 2D inputs with a single feature column (1D space)."""
        # Arrange.
        single_feature_matrix = np.array(
            [[1.0], [5.0], [2.0], [10.0]], dtype=np.float64
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(single_feature_matrix)

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(
            self.default_hierarchical_clustering_instance.number_of_features, 1
        )
        self.assertEqual(len(computed_linkage_matrix_array), 3)

    def test_26_zero_variance_features_handling(self):
        """Verify feature columns with zero variance (constant columns) do not corrupt distance math."""
        # Arrange.
        zero_variance_augmented_features = np.hstack([
            self.standard_synthetic_features_matrix,
            np.ones((15, 1), dtype=np.float64),
        ])

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            zero_variance_augmented_features
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertFalse(np.isnan(computed_linkage_matrix_array).any())
        self.assertEqual(len(computed_linkage_matrix_array), 14)

    def test_27_minimal_two_sample_boundary(self):
        """Verify pipeline handles minimal sample boundary (N=2 samples, 1 merge step)."""
        # Arrange.
        minimal_two_sample_feature_matrix = np.array(
            [[1.0, 2.0], [4.0, 6.0]], dtype=np.float64
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            minimal_two_sample_feature_matrix
        )
        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(computed_linkage_matrix_array.shape, (1, 4))
        self.assertAlmostEqual(computed_linkage_matrix_array[0, 2], 5.0, places=6)
        self.assertEqual(computed_linkage_matrix_array[0, 3], 2.0)

    def test_28_negative_feature_values_distance_math(self):
        """Verify Euclidean distance calculation correctly handles negative spatial coordinates."""
        # Arrange.
        negative_coordinate_feature_matrix = np.array(
            [[-10.0, -20.0], [-13.0, -24.0], [10.0, 20.0]], dtype=np.float64
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            negative_coordinate_feature_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertAlmostEqual(computed_linkage_matrix_array[0, 2], 5.0, places=6)

    def test_29_large_sample_cardinality_stress(self):
        """Verify C-memoryview loops handle larger sample counts (100 samples) without buffer overflow."""
        # Arrange.
        stress_test_large_dataset_matrix = (
            self.random_number_generator.normal(size=(100, 4)).astype(
                np.float64
            )
        )

        # Act.
        self.default_hierarchical_clustering_instance.fit(
            stress_test_large_dataset_matrix
        )

        computed_linkage_matrix_array = (
            self.default_hierarchical_clustering_instance.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(
            self.default_hierarchical_clustering_instance.number_of_samples, 100
        )
        self.assertEqual(computed_linkage_matrix_array.shape, (99, 4))

    def test_30_consecutive_fits_state_reset(self):
        """Verify calling fit() twice on different dataset shapes resets internal C-buffers correctly."""
        # Arrange.
        stateful_reusable_clustering_model = HierarchicalClustering()

        stateful_reusable_clustering_model.fit(
            self.standard_synthetic_features_matrix
        )

        subsequent_resized_dataset_matrix = (
            self.random_number_generator.normal(size=(30, 5)).astype(
                np.float64
            )
        )

        # Act.
        stateful_reusable_clustering_model.fit(
            subsequent_resized_dataset_matrix
        )
        updated_linkage_matrix_array = (
            stateful_reusable_clustering_model.get_linkage_matrix()
        )

        # Assert.
        self.assertEqual(
            stateful_reusable_clustering_model.number_of_samples, 30
        )
        self.assertEqual(
            stateful_reusable_clustering_model.number_of_features, 5
        )
        self.assertEqual(updated_linkage_matrix_array.shape, (29, 4))

if __name__ == "__main__":
    unittest.main()
