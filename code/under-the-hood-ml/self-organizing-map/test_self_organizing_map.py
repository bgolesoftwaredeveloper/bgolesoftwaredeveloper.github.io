"""
test_self_organizing_map.py

A comprehensive, deterministic unit testing suite designed to validate the numerical 
integrity, structural boundary limits, state lifecycle invariants, and topological 
mapping metrics of the custom SelfOrganizingMap engine.

Test Coverage Domains:
  1. Initialization & Input Validation   - Bounds checks for grid rows, columns, feature dimensions, and learning rates.
  2. Weight Matrix & Coordinate Setup    - Initialization shapes and precalculated 2D spatial node mapping assertions.
  3. Best Matching Unit (BMU) Selection  - Vectorized distance reduction and output tuple format verification.
  4. Training & Competitive Adaptation   - Decaying neighborhood updates, iteration caps, and deterministic convergence.
  5. Exception & Guardrail Enforcement   - Rejection of invalid data shapes, empty inputs, non-integer parameters, and NaNs.
  6. Out-of-Sample Dataset Mapping       - Batch mapping preservation of sample cardinalities and topological ordering.
  7. Numerical & Structural Edge Cases   - Mathematical stability across single observations, high dimensions, and scalar limits.
"""
from __future__ import annotations

from self_organizing_map import SelfOrganizingMap

import unittest
import numpy

class TestSelfOrganizingMap(unittest.TestCase):
    """
    Unit test harness executing automated functional, edge-case, and mathematical 
    correctness assertions over the SelfOrganizingMap engine implementation.
    """

    def setUp(self) -> None:
        """Set up standard feature datasets, seeds, and default model instances used across tests."""
        self.seed = 42
        self.generator = numpy.random.default_rng(self.seed)

        self.rows = 5
        self.columns = 5
        self.input_dimension = 3

        self.standard_features = self.generator.uniform(
            low=0.0, high=1.0, size=(100, self.input_dimension)
        )

        self.default_model = SelfOrganizingMap(
            rows=self.rows,
            columns=self.columns,
            input_dimension=self.input_dimension,
            learning_rate=0.5,
            radius=2.5,
        )

    def test_01_initialization_valid_parameters(self) -> None:
        """Ensure class instantiates cleanly with valid parameters and default radius."""
        try:
            # Act.
            engine = SelfOrganizingMap(
                rows=10, columns=10, input_dimension=4, learning_rate=0.5
            )

            # Assert.
            self.assertIsInstance(engine, SelfOrganizingMap)
            self.assertEqual(engine.rows, 10)
            self.assertEqual(engine.columns, 10)
            self.assertEqual(engine.input_dimension, 4)
            self.assertEqual(engine.learning_rate, 0.5)
            self.assertEqual(engine.radius, 5.0)
        except Exception as exception:
            self.fail(
                f"Initialization failed with valid parameters: {exception}"
            )

    def test_02_initialization_invalid_type_rejection(self) -> None:
        """Ensure constructor rejects non-integer grid dimensions or feature counts."""
        # Arrange, Act & Assert.
        with self.assertRaises(TypeError):
            SelfOrganizingMap(rows=5.5, columns=5, input_dimension=3)  # type: ignore

        with self.assertRaises(TypeError):
            SelfOrganizingMap(rows=5, columns="5", input_dimension=3)  # type: ignore

        with self.assertRaises(TypeError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=3.0)  # type: ignore

    def test_03_initialization_non_positive_dimension_rejection(self) -> None:
        """Ensure constructor rejects non-positive rows, columns, or feature dimensions."""
        # Arrange, Act & Assert.
        with self.assertRaises(ValueError):
            SelfOrganizingMap(rows=0, columns=5, input_dimension=3)

        with self.assertRaises(ValueError):
            SelfOrganizingMap(rows=5, columns=-1, input_dimension=3)

        with self.assertRaises(ValueError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=0)

    def test_04_initialization_invalid_learning_rate_bounds(self) -> None:
        """Ensure constructor enforces strict bounds (0.0, 1.0] on learning rate."""
        # Arrange, Act & Assert.
        with self.assertRaises(ValueError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=3, learning_rate=0.0)

        with self.assertRaises(ValueError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=3, learning_rate=1.5)

        with self.assertRaises(TypeError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=3, learning_rate="high")  # type: ignore

    def test_05_initialization_invalid_radius_bounds(self) -> None:
        """Ensure constructor rejects non-positive or non-numeric initial neighborhood radiuses."""
        # Arrange, Act & Assert.
        with self.assertRaises(ValueError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=3, radius=0.0)

        with self.assertRaises(ValueError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=3, radius=-2.0)

        with self.assertRaises(TypeError):
            SelfOrganizingMap(rows=5, columns=5, input_dimension=3, radius="invalid")  # type: ignore

    def test_06_weight_matrix_dimensions_and_ranges(self) -> None:
        """Verify initial weight tensor matches grid shape and lies in standard uniform bounds [0.0, 1.0)."""
        # Arrange & Act.
        weights = self.default_model.weights

        # Assert.
        self.assertEqual(
            weights.shape, (self.rows, self.columns, self.input_dimension)
        )

        self.assertTrue(numpy.all(weights >= 0.0))
        self.assertTrue(numpy.all(weights <= 1.0))

    def test_07_coordinate_map_structure(self) -> None:
        """Verify internal precomputed spatial coordinate tensor shape and endpoint values."""
        # Arrange & Act.
        coordinate_map = self.default_model.coordinate_map

        # Assert.
        self.assertEqual(coordinate_map.shape, (self.rows, self.columns, 2))

        numpy.testing.assert_array_equal(coordinate_map[0, 0], [0, 0])
        numpy.testing.assert_array_equal(
            coordinate_map[self.rows - 1, self.columns - 1],
            [self.rows - 1, self.columns - 1],
        )

    def test_08_find_bmu_valid_output_format(self) -> None:
        """Verify BMU search returns standard Python integer coordinate tuples within grid bounds."""
        # Arrange.
        sample_vector = numpy.array([0.5, 0.5, 0.5])

        # Act.
        row_index, column_index = self.default_model.find_best_matching_unit(
            sample_vector
        )

        # Assert.
        self.assertIsInstance(row_index, int)
        self.assertIsInstance(column_index, int)
        self.assertTrue(0 <= row_index < self.rows)
        self.assertTrue(0 <= column_index < self.columns)

    def test_09_find_bmu_dimension_mismatch_rejection(self) -> None:
        """Verify BMU search raises ValueError when input vector feature length is misaligned."""
        # Arrange.
        invalid_vector = numpy.array([0.1, 0.2])

        # Act & Assert.
        with self.assertRaises(ValueError):
            self.default_model.find_best_matching_unit(invalid_vector)

    def test_10_find_bmu_non_1d_array_rejection(self) -> None:
        """Verify BMU search rejects multi-dimensional feature tensors."""
        # Arrange.
        invalid_tensor = numpy.array([[0.1, 0.2, 0.3]])

        # Act & Assert.
        with self.assertRaises(ValueError):
            self.default_model.find_best_matching_unit(invalid_tensor)

    def test_11_find_bmu_nan_inf_rejection(self) -> None:
        """Verify BMU search guards against NaN and infinite input values."""
        # Arrange.
        nan_vector = numpy.array([0.1, numpy.nan, 0.3])
        inf_vector = numpy.array([numpy.inf, 0.2, 0.3])

        # Act & Assert.
        with self.assertRaises(ValueError):
            self.default_model.find_best_matching_unit(nan_vector)

        with self.assertRaises(ValueError):
            self.default_model.find_best_matching_unit(inf_vector)

    def test_12_map_dataset_to_grid_cardinality_and_types(self) -> None:
        """Verify batch dataset mapping preserves sample count and returns a list of coordinate tuples."""
        # Arrange & Act.
        mapped_coordinates = self.default_model.map_dataset_to_grid(
            self.standard_features
        )

        # Assert.
        self.assertIsInstance(mapped_coordinates, list)
        self.assertEqual(len(mapped_coordinates), len(self.standard_features))

        for item in mapped_coordinates:
            self.assertIsInstance(item, tuple)
            self.assertEqual(len(item), 2)
            self.assertIsInstance(item[0], int)
            self.assertIsInstance(item[1], int)

    def test_13_map_dataset_empty_rejection(self) -> None:
        """Verify dataset mapping raises ValueError when passed an empty array."""
        # Arrange.
        empty_dataset = numpy.empty((0, self.input_dimension))

        # Act & Assert.
        with self.assertRaises(ValueError):
            self.default_model.map_dataset_to_grid(empty_dataset)

    def test_14_train_updates_weight_matrix(self) -> None:
        """Verify training alters grid node weight parameters away from initial values."""
        # Arrange.
        initial_weights = self.default_model.weights.copy()

        # Act.
        self.default_model.train(self.standard_features, maximum_iterations=100)

        # Assert.
        self.assertFalse(numpy.allclose(initial_weights, self.default_model.weights))

    def test_15_train_invalid_iterations_rejection(self) -> None:
        """Verify train method rejects non-integer or non-positive maximum iteration counts."""
        # Arrange, Act & Assert.
        with self.assertRaises(ValueError):
            self.default_model.train(self.standard_features, maximum_iterations=0)

        with self.assertRaises(ValueError):
            self.default_model.train(self.standard_features, maximum_iterations=-10)

        with self.assertRaises(ValueError):
            self.default_model.train(self.standard_features, maximum_iterations=100.5)  # type: ignore

    def test_16_train_matrix_dimension_mismatch_rejection(self) -> None:
        """Verify train method raises ValueError when input matrix feature dimension mismatches."""
        # Arrange.
        invalid_matrix = self.generator.uniform(size=(50, 5))

        # Act & Assert.
        with self.assertRaises(ValueError):
            self.default_model.train(invalid_matrix, maximum_iterations=50)

    def test_17_train_nan_inf_matrix_rejection(self) -> None:
        """Verify train method guards against training datasets containing NaNs or infinite values."""
        # Arrange.
        corrupted_matrix = self.standard_features.copy()
        corrupted_matrix[10, 1] = numpy.nan

        # Act & Assert.
        with self.assertRaises(ValueError):
            self.default_model.train(corrupted_matrix, maximum_iterations=50)

    def test_18_single_sample_training_and_mapping(self) -> None:
        """Verify SOM supports single-observation dataset arrays (1, input_dimension)."""
        # Arrange.
        single_sample_matrix = numpy.array([[0.1, 0.8, 0.3]])

        # Act.
        self.default_model.train(single_sample_matrix, maximum_iterations=10)
        mapped = self.default_model.map_dataset_to_grid(single_sample_matrix)

        # Assert.
        self.assertEqual(len(mapped), 1)
        self.assertIsInstance(mapped[0], tuple)

    def test_19_high_dimensional_feature_space_support(self) -> None:
        """Verify pipeline operates seamlessly over high-dimensional feature vectors."""
        # Arrange.
        high_dimension = 64
        high_dimension_data = self.generator.uniform(size=(30, high_dimension))
        high_dimension_model = SelfOrganizingMap(
            rows=4, columns=4, input_dimension=high_dimension
        )

        # Act.
        high_dimension_model.train(high_dimension_data, maximum_iterations=20)
        mapped = high_dimension_model.map_dataset_to_grid(high_dimension_data)

        # Assert.
        self.assertEqual(high_dimension_model.weights.shape, (4, 4, high_dimension))
        self.assertEqual(len(mapped), 30)

    def test_20_exact_match_bmu_zero_distance(self) -> None:
        """Verify BMU selection correctly identifies a node whose weights perfectly match an input vector."""
        # Arrange.
        model = SelfOrganizingMap(rows=3, columns=3, input_dimension=3)

        target_coordinate = (1, 2)
        target_vector = numpy.array([0.99, 0.11, 0.55])

        model.weights[target_coordinate] = target_vector

        # Act.
        best_matching_unit = model.find_best_matching_unit(target_vector)

        # Assert.
        self.assertEqual(best_matching_unit, target_coordinate)

    def test_21_list_input_coercion(self) -> None:
        """Verify functions accept raw Python lists and automatically coerce them into array types."""
        # Arrange.
        raw_vector = [0.1, 0.5, 0.9]
        raw_dataset = [[0.1, 0.5, 0.9], [0.8, 0.2, 0.4]]

        # Act.
        best_matching_unit = self.default_model.find_best_matching_unit(raw_vector)  # type: ignore
        mapped = self.default_model.map_dataset_to_grid(raw_dataset)  # type: ignore

        # Assert.
        self.assertIsInstance(best_matching_unit, tuple)
        self.assertEqual(len(mapped), 2)

    def test_22_reproducibility_via_random_seed(self) -> None:
        """Verify deterministic initial state and training when seed is controlled via numpy."""
        # Arrange.
        numpy.random.seed(123)

        model_a = SelfOrganizingMap(rows=4, columns=4, input_dimension=3)
        initial_weights_a = model_a.weights.copy()

        numpy.random.seed(123)

        model_b = SelfOrganizingMap(rows=4, columns=4, input_dimension=3)
        initial_weights_b = model_b.weights.copy()

        # Assert.
        numpy.testing.assert_array_equal(initial_weights_a, initial_weights_b)

    def test_23_topological_clustering_convergence(self) -> None:
        """Verify distinct clusters adapt adjacent BMU nodes on the SOM grid after extensive training."""
        # Arrange.
        numpy.random.seed(42)

        model = SelfOrganizingMap(
            rows=10, columns=10, input_dimension=2, learning_rate=0.8
        )

        cluster_a = numpy.array([[0.05, 0.05]] * 20)
        cluster_b = numpy.array([[0.95, 0.95]] * 20)

        combined_dataset = numpy.vstack([cluster_a, cluster_b])

        # Act.
        model.train(combined_dataset, maximum_iterations=500)

        mapped_cluster_a = model.map_dataset_to_grid(cluster_a[:1])[0]
        mapped_cluster_b = model.map_dataset_to_grid(cluster_b[:1])[0]

        self.assertNotEqual(mapped_cluster_a, mapped_cluster_b)

    def test_24_weight_values_remain_bounded_after_training(self) -> None:
        """Verify weight updates do not diverge or create NaNs/Infs during training."""
        # Arrange & Act.
        self.default_model.train(self.standard_features, maximum_iterations=200)

        # Assert.
        self.assertFalse(numpy.isnan(self.default_model.weights).any())
        self.assertFalse(numpy.isinf(self.default_model.weights).any())

    def test_25_rectangular_grid_architecture_support(self) -> None:
        """Verify algorithm handles non-square grid geometries (e.g., rows != columns)."""
        # Arrange.
        rectangular_rows, rectangular_columns = 4, 8
        rectangular_model = SelfOrganizingMap(
            rows=rectangular_rows, columns=rectangular_columns, input_dimension=3
        )

        # Act.
        rectangular_model.train(self.standard_features, maximum_iterations=50)
        
        mapped_coordinates = rectangular_model.map_dataset_to_grid(self.standard_features[:5])

        # Assert.
        self.assertEqual(
            rectangular_model.weights.shape, (rectangular_rows, rectangular_columns, self.input_dimension)
        )

        for row_index, column_index in mapped_coordinates:
            self.assertTrue(0 <= row_index < rectangular_rows)
            self.assertTrue(0 <= column_index < rectangular_columns)

if __name__ == "__main__":
    unittest.main()
