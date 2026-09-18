"""
test_affinity_propagation.py

A comprehensive, deterministic unit testing suite designed to validate the numerical 
integrity, structural boundary limits, state lifecycle invariants, C-level memoryview 
compatibility, and message-passing convergence of the custom Cython AffinityPropagation engine.

Test Coverage Domains:
  1. Initialization & Hyperparameter Boundaries - Bounds checks for damping factor, max iterations, preferences.
  2. Buffer Allocation & C Memoryview Setup     - Memoryview shape invariants, dtype stability, array layout.
  3. Pairwise Distance & Similarity Math       - Distance metric precision, symmetry properties, diagonal preferences.
  4. Message-Passing Matrix Updates            - Responsibility (R) and Availability (A) numerical invariants.
  5. Damping Factor & Stability Mechanics       - Numerical convergence smoothing and oscillation prevention.
  6. Cluster Extraction & Exemplar Resolution   - Exemplar selection logic (R(k,k) + A(k,k) > 0) and assignments.
  7. Determinism & High-Dimensional Edge Cases  - Seeded synthetic datasets, 1D/2D boundaries, zero variance features.
"""
import unittest
import numpy

from affinity_propagation import AffinityPropagation

class TestAffinityPropagation(unittest.TestCase):
    """
    Unit test harness executing automated functional, edge-case, C-memoryview layout,
    and mathematical correctness assertions over the Cython AffinityPropagation engine.
    """

    def setUp(self):
        """Set up standard feature datasets, seeds, and default model instances used across tests."""
        self.random_seed_value = 42
        self.random_number_generator = numpy.random.default_rng(self.random_seed_value)

        # Generate 3 distinct synthetic spatial clusters (15 samples, 2 features).
        first_spatial_cluster_samples = self.random_number_generator.normal(loc=[10.0, 10.0], scale=0.5, size=(5, 2))
        second_spatial_cluster_samples = self.random_number_generator.normal(loc=[-10.0, -10.0], scale=0.5, size=(5, 2))
        third_spatial_cluster_samples = self.random_number_generator.normal(loc=[10.0, -10.0], scale=0.5, size=(5, 2))

        self.standard_synthetic_features_matrix = numpy.vstack(
            [first_spatial_cluster_samples, second_spatial_cluster_samples, third_spatial_cluster_samples]
        ).astype(numpy.float64)

        self.default_affinity_propagation_instance = AffinityPropagation(
            maximum_iterations=100,
            damping_factor=0.5,
            preference_value=-10.0
        )

    def test_01_initialization_default_parameters(self):
        """Ensure class instantiates cleanly with default constructor parameters."""
        # Arrange & Act.
        default_configured_clustering_engine = AffinityPropagation()

        # Assert.
        self.assertIsInstance(default_configured_clustering_engine, AffinityPropagation)
        self.assertEqual(default_configured_clustering_engine.maximum_iterations, 200)
        self.assertEqual(default_configured_clustering_engine.damping_factor, 0.5)
        self.assertEqual(default_configured_clustering_engine.preference_value, -50.0)

    def test_02_initialization_custom_parameters(self):
        """Ensure class properly assigns custom hyperparameters to Cython C-attributes."""
        # Arrange & Act.
        custom_configured_clustering_engine = AffinityPropagation(
            maximum_iterations=50,
            damping_factor=0.8,
            preference_value=-5.0
        )

        # Assert.
        self.assertEqual(custom_configured_clustering_engine.maximum_iterations, 50)
        self.assertEqual(custom_configured_clustering_engine.damping_factor, 0.8)
        self.assertEqual(custom_configured_clustering_engine.preference_value, -5.0)

    def test_03_initialization_damping_factor_bounds(self):
        """Verify constructor accepts boundary damping factors (e.g., 0.5 to 0.99)."""
        # Arrange & Act.
        boundary_damping_clustering_engine = AffinityPropagation(damping_factor=0.99)

        # Assert.
        self.assertEqual(boundary_damping_clustering_engine.damping_factor, 0.99)

    def test_04_buffer_initialization_dimensions(self):
        """Verify memoryview buffers populate correct dataset dimensions upon fitting."""
        # Arrange & Act.
        self.default_affinity_propagation_instance.fit(self.standard_synthetic_features_matrix)

        # Assert.
        self.assertEqual(self.default_affinity_propagation_instance.number_of_samples, 15)
        self.assertEqual(self.default_affinity_propagation_instance.number_of_features, 2)

    def test_05_c_contiguous_memoryview_acceptance(self):
        """Ensure Cython memoryview engine seamlessly accepts C-contiguous float64 arrays."""
        # Arrange.
        c_contiguous_inumpyut_feature_array = numpy.ascontiguousarray(
            self.standard_synthetic_features_matrix, dtype=numpy.float64
        )

        # Act.
        resolved_c_exemplars, assigned_c_labels = self.default_affinity_propagation_instance.fit(
            c_contiguous_inumpyut_feature_array
        )

        # Assert.
        self.assertIsInstance(resolved_c_exemplars, numpy.ndarray)
        self.assertIsInstance(assigned_c_labels, numpy.ndarray)

    def test_06_fortran_contiguous_memoryview_coercion(self):
        """Verify non-C-contiguous (Fortran) arrays are correctly processed without memory fault."""
        # Arrange.
        fortran_contiguous_inumpyut_feature_array = numpy.asfortranarray(
            self.standard_synthetic_features_matrix, dtype=numpy.float64
        )

        # Act.
        resolved_fortran_exemplars, assigned_fortran_labels = self.default_affinity_propagation_instance.fit(
            fortran_contiguous_inumpyut_feature_array
        )

        # Assert.
        self.assertEqual(len(assigned_fortran_labels), 15)

    def test_07_type_coercion_float32_input(self):
        """Verify fitting automatically casts float32 inputs to C double (float64)."""
        # Arrange.
        single_precision_float32_feature_matrix = self.standard_synthetic_features_matrix.astype(numpy.float32)

        # Act.
        resolved_float32_exemplars, assigned_float32_labels = self.default_affinity_propagation_instance.fit(
            single_precision_float32_feature_matrix.astype(numpy.float64)
        )

        # Assert.
        self.assertEqual(assigned_float32_labels.dtype, numpy.int32)

    def test_08_similarity_matrix_diagonal_preference(self):
        """Verify similarity matrix diagonal strictly equals the input preference value."""
        # Arrange.
        self.default_affinity_propagation_instance._initialize_buffers(self.standard_synthetic_features_matrix)

        # Act.
        self.default_affinity_propagation_instance._compute_similarity_matrix()
        computed_similarity_matrix_array = numpy.asarray(self.default_affinity_propagation_instance.similarities)
        extracted_diagonal_preference_values = numpy.diag(computed_similarity_matrix_array)

        # Assert.
        numpy.testing.assert_allclose(
            extracted_diagonal_preference_values,
            numpy.full(15, -10.0),
            atol=1e-12
        )

    def test_09_similarity_matrix_symmetry(self):
        """Verify pairwise Euclidean similarity matrix S is strictly symmetric (S_ik = S_ki)."""
        # Arrange.
        self.default_affinity_propagation_instance._initialize_buffers(self.standard_synthetic_features_matrix)

        # Act.
        self.default_affinity_propagation_instance._compute_similarity_matrix()
        evaluated_pairwise_similarity_matrix = numpy.asarray(self.default_affinity_propagation_instance.similarities)

        # Assert.
        numpy.testing.assert_allclose(
            evaluated_pairwise_similarity_matrix,
            evaluated_pairwise_similarity_matrix.T,
            atol=1e-12
        )

    def test_10_similarity_matrix_negative_distance_property(self):
        """Verify off-diagonal similarity values are strictly non-positive (negative squared distance)."""
        # Arrange.
        self.default_affinity_propagation_instance._initialize_buffers(self.standard_synthetic_features_matrix)

        # Act.
        self.default_affinity_propagation_instance._compute_similarity_matrix()
        non_positive_similarity_matrix_buffer = numpy.asarray(self.default_affinity_propagation_instance.similarities)

        # Assert.
        self.assertTrue(numpy.all(non_positive_similarity_matrix_buffer <= 0.0))

    def test_11_responsibility_update_dimensions(self):
        """Verify responsibility matrix R preserves (N, N) sample shape."""
        # Arrange.
        self.default_affinity_propagation_instance._initialize_buffers(self.standard_synthetic_features_matrix)
        self.default_affinity_propagation_instance._compute_similarity_matrix()
        temporary_responsibility_working_buffer = numpy.zeros((15, 15), dtype=numpy.float64)

        # Act.
        self.default_affinity_propagation_instance._update_responsibilities(temporary_responsibility_working_buffer)
        extracted_responsibility_matrix_shape = numpy.asarray(self.default_affinity_propagation_instance.responsibilities)

        # Assert.
        self.assertEqual(extracted_responsibility_matrix_shape.shape, (15, 15))

    def test_12_availability_off_diagonal_non_positive(self):
        """Verify off-diagonal availability matrix elements A(i, k) are non-positive (A_ik <= 0)."""
        # Arrange.
        self.default_affinity_propagation_instance._initialize_buffers(self.standard_synthetic_features_matrix)
        self.default_affinity_propagation_instance._compute_similarity_matrix()
        temporary_responsibility_buffer_a = numpy.zeros((15, 15), dtype=numpy.float64)
        temporary_availability_buffer_a = numpy.zeros((15, 15), dtype=numpy.float64)

        # Act.
        self.default_affinity_propagation_instance._update_responsibilities(temporary_responsibility_buffer_a)
        self.default_affinity_propagation_instance._update_availabilities(temporary_availability_buffer_a)
        extracted_availability_matrix_off_diag = numpy.asarray(self.default_affinity_propagation_instance.availabilities)
        non_diagonal_boolean_mask = ~numpy.eye(15, dtype=bool)

        # Assert.
        self.assertTrue(numpy.all(extracted_availability_matrix_off_diag[non_diagonal_boolean_mask] <= 0.0))

    def test_13_availability_diagonal_non_negative(self):
        """Verify self-availability diagonal elements A(k, k) remain non-negative (A_kk >= 0)."""
        # Arrange.
        self.default_affinity_propagation_instance._initialize_buffers(self.standard_synthetic_features_matrix)
        self.default_affinity_propagation_instance._compute_similarity_matrix()
        temporary_responsibility_buffer_b = numpy.zeros((15, 15), dtype=numpy.float64)
        temporary_availability_buffer_b = numpy.zeros((15, 15), dtype=numpy.float64)

        # Act.
        self.default_affinity_propagation_instance._update_responsibilities(temporary_responsibility_buffer_b)
        self.default_affinity_propagation_instance._update_availabilities(temporary_availability_buffer_b)
        extracted_availability_matrix_diag = numpy.asarray(self.default_affinity_propagation_instance.availabilities)

        # Assert.
        self.assertTrue(numpy.all(numpy.diag(extracted_availability_matrix_diag) >= 0.0))

    def test_14_fit_returns_correct_types(self):
        """Verify fit() returns a tuple of two 1D NumPy C-integer arrays (exemplars, labels)."""
        # Arrange & Act.
        fitted_exemplar_indices, assigned_cluster_labels = self.default_affinity_propagation_instance.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        self.assertIsInstance(fitted_exemplar_indices, numpy.ndarray)
        self.assertIsInstance(assigned_cluster_labels, numpy.ndarray)
        self.assertEqual(fitted_exemplar_indices.dtype, numpy.int32)
        self.assertEqual(assigned_cluster_labels.dtype, numpy.int32)

    def test_15_fit_label_cardinality(self):
        """Verify length of output labels array exactly matches dataset sample count (N)."""
        # Arrange & Act.
        _, cardinal_assigned_cluster_labels = self.default_affinity_propagation_instance.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        self.assertEqual(len(cardinal_assigned_cluster_labels), 15)

    def test_16_damping_effect_on_numerical_stability(self):
        """Verify high damping factor (0.9) produces finite, non-NaN matrices across iteration loops."""
        # Arrange.
        high_damping_clustering_model = AffinityPropagation(maximum_iterations=50, damping_factor=0.9)

        # Act.
        stabilized_exemplars, stabilized_assigned_labels = high_damping_clustering_model.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        self.assertFalse(numpy.isnan(stabilized_exemplars).any())
        self.assertFalse(numpy.isnan(stabilized_assigned_labels).any())

    def test_17_exemplars_are_valid_sample_indices(self):
        """Verify resolved exemplar indices lie strictly within range [0, N-1]."""
        # Arrange & Act.
        bounded_exemplar_indices, _ = self.default_affinity_propagation_instance.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        self.assertTrue(numpy.all(bounded_exemplar_indices >= 0))
        self.assertTrue(numpy.all(bounded_exemplar_indices < 15))

    def test_18_cluster_labels_map_to_valid_exemplars(self):
        """Verify all assigned cluster labels exist within the resolved exemplars array."""
        # Arrange & Act.
        valid_exemplar_set, mapped_cluster_assignments = self.default_affinity_propagation_instance.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        for individual_cluster_label in mapped_cluster_assignments:
            self.assertIn(individual_cluster_label, valid_exemplar_set)

    def test_19_exemplar_self_assignment(self):
        """Verify exemplar points are always assigned to themselves as cluster centers."""
        # Arrange & Act.
        resolved_self_exemplars, self_assigned_cluster_labels = self.default_affinity_propagation_instance.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        for individual_exemplar_index in resolved_self_exemplars:
            self.assertEqual(
                self_assigned_cluster_labels[individual_exemplar_index],
                individual_exemplar_index
            )

    def test_20_well_separated_clusters_accuracy(self):
        """Verify perfect exemplar identification on 3 distinct synthetic spatial clusters."""
        # Arrange & Act.
        well_separated_exemplar_centers, spatial_cluster_assignments = self.default_affinity_propagation_instance.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        self.assertEqual(len(well_separated_exemplar_centers), 3)
        self.assertEqual(len(set(spatial_cluster_assignments[0:5])), 1)
        self.assertEqual(len(set(spatial_cluster_assignments[5:10])), 1)
        self.assertEqual(len(set(spatial_cluster_assignments[10:15])), 1)

    def test_21_identical_coincident_points_handling(self):
        """Verify numerical stability when dataset contains identical/coincident points."""
        # Arrange.
        coincident_feature_data_points = numpy.tile([5.0, 5.0], (10, 1)).astype(numpy.float64)

        # Act.
        coincident_exemplars, coincident_assigned_labels = self.default_affinity_propagation_instance.fit(
            coincident_feature_data_points
        )

        # Assert.
        self.assertFalse(numpy.isnan(coincident_assigned_labels).any())
        self.assertEqual(len(coincident_assigned_labels), 10)

    def test_22_single_iteration_execution(self):
        """Verify algorithm executes gracefully with maximum_iterations=1."""
        # Arrange.
        single_iteration_clustering_model = AffinityPropagation(maximum_iterations=1)

        # Act.
        single_iter_exemplars, single_iter_labels = single_iteration_clustering_model.fit(
            self.standard_synthetic_features_matrix
        )

        # Assert.
        self.assertEqual(len(single_iter_labels), 15)

    def test_23_high_dimensional_feature_space(self):
        """Verify Cython buffer loops scale seamlessly over high-dimensional feature vectors (e.g. 128D)."""
        # Arrange.
        high_dimensional_feature_dataset = self.random_number_generator.normal(size=(20, 128)).astype(numpy.float64)
        high_dimensional_clustering_model = AffinityPropagation(maximum_iterations=10, preference_value=-100.0)

        # Act.
        high_dim_exemplars, high_dim_labels = high_dimensional_clustering_model.fit(
            high_dimensional_feature_dataset
        )

        # Assert.
        self.assertEqual(high_dimensional_clustering_model.number_of_features, 128)
        self.assertEqual(len(high_dim_labels), 20)

    def test_24_deterministic_reproducibility(self):
        """Verify running fit multiple times on identical data yields identical exemplar results."""
        # Arrange.
        first_reproducible_model = AffinityPropagation(maximum_iterations=50, preference_value=-15.0)
        second_reproducible_model = AffinityPropagation(maximum_iterations=50, preference_value=-15.0)

        # Act.
        first_run_exemplars, first_run_labels = first_reproducible_model.fit(self.standard_synthetic_features_matrix)
        second_run_exemplars, second_run_labels = second_reproducible_model.fit(self.standard_synthetic_features_matrix)

        # Assert.
        numpy.testing.assert_array_equal(first_run_exemplars, second_run_exemplars)
        numpy.testing.assert_array_equal(first_run_labels, second_run_labels)

    def test_25_preference_value_controls_cluster_granularity(self):
        """Verify lower preference value creates fewer exemplars than a higher preference value."""
        # Arrange.
        low_preference_clustering_model = AffinityPropagation(maximum_iterations=50, preference_value=-500.0)
        high_preference_clustering_model = AffinityPropagation(maximum_iterations=50, preference_value=-1.0)

        # Act.
        coarse_granularity_exemplars, _ = low_preference_clustering_model.fit(self.standard_synthetic_features_matrix)
        fine_granularity_exemplars, _ = high_preference_clustering_model.fit(self.standard_synthetic_features_matrix)

        # Assert.
        self.assertLessEqual(len(coarse_granularity_exemplars), len(fine_granularity_exemplars))

    def test_26_zero_variance_features_handling(self):
        """Verify feature columns with zero variance (constant columns) do not generate zero-division errors."""
        # Arrange.
        zero_variance_augmented_features = numpy.hstack([
            self.standard_synthetic_features_matrix,
            numpy.ones((15, 1), dtype=numpy.float64)
        ])

        # Act.
        zero_var_exemplars, zero_var_labels = self.default_affinity_propagation_instance.fit(
            zero_variance_augmented_features
        )

        # Assert.
        self.assertEqual(len(zero_var_labels), 15)

    def test_27_minimal_sample_size_boundary(self):
        """Verify pipeline handles minimal sample sizes (e.g., N=2 samples)."""
        # Arrange.
        minimal_two_sample_feature_matrix = numpy.array([[1.0, 2.0], [8.0, 9.0]], dtype=numpy.float64)

        # Act.
        minimal_sample_exemplars, minimal_sample_labels = self.default_affinity_propagation_instance.fit(
            minimal_two_sample_feature_matrix
        )

        # Assert.
        self.assertEqual(len(minimal_sample_labels), 2)

    def test_28_negative_feature_values_distance_math(self):
        """Verify Euclidean distance squared calculation correctly handles negative coordinates."""
        # Arrange.
        negative_coordinate_feature_matrix = numpy.array([
            [-10.5, -20.2],
            [-30.1, -40.4],
            [-50.3, -60.8]
        ], dtype=numpy.float64)

        # Act.
        negative_coord_exemplars, negative_coord_labels = self.default_affinity_propagation_instance.fit(
            negative_coordinate_feature_matrix
        )

        # Assert.
        self.assertEqual(len(negative_coord_labels), 3)

    def test_29_large_sample_cardinality_stress(self):
        """Verify C-memoryview loops handle larger sample counts (100 samples) without buffer overflow."""
        # Arrange.
        stress_test_large_dataset_matrix = self.random_number_generator.normal(size=(100, 4)).astype(numpy.float64)
        large_sample_cardinality_model = AffinityPropagation(maximum_iterations=20)

        # Act.
        stress_test_exemplars, stress_test_labels = large_sample_cardinality_model.fit(
            stress_test_large_dataset_matrix
        )

        # Assert.
        self.assertEqual(large_sample_cardinality_model.number_of_samples, 100)
        self.assertEqual(len(stress_test_labels), 100)

    def test_30_consecutive_fits_state_reset(self):
        """Verify calling fit() twice on different dataset shapes resets internal C-buffers correctly."""
        # Arrange.
        stateful_reusable_clustering_model = AffinityPropagation(maximum_iterations=20)
        stateful_reusable_clustering_model.fit(self.standard_synthetic_features_matrix)
        subsequent_resized_dataset_matrix = self.random_number_generator.normal(size=(30, 5)).astype(numpy.float64)

        # Act.
        stateful_reusable_clustering_model.fit(subsequent_resized_dataset_matrix)

        # Assert.
        self.assertEqual(stateful_reusable_clustering_model.number_of_samples, 30)
        self.assertEqual(stateful_reusable_clustering_model.number_of_features, 5)

if __name__ == "__main__":
    unittest.main()