"""
self_organizing_map.py

A custom vector implementation of Kohonen Self-Organizing Maps (SOM).

This module provides an unsupervised competitive learning model for mapping 
high-dimensional continuous feature spaces onto low-dimensional (2D) discrete lattice grids.
Neighborhood functions and topological learning rates decay dynamically over training iterations,
preserving structural spatial relationships between observations.

Core Operational Capabilities:
  - Vectorized multidimensional Euclidean distance evaluation for Best Matching Unit (BMU) selection.
  - Spatial coordinate mapping and distance calculations across 2D grid node architectures.
  - Exponentially decaying Gaussian neighborhood functions and adaptive learning rates.
  - Out-of-sample mapping of continuous feature vectors to discrete grid coordinates.
"""
from __future__ import annotations

import numpy

class SelfOrganizingMap:
    """
    An engine responsible for managing grid architecture, mapping input patterns 
    to topological nodes, and updating node weights via competitive learning.
    """

    def __init__(self, rows: int, columns: int, input_dimension: int, 
                 learning_rate: float = 0.5, radius: float | None = None):
        """
        Initializes the SOM grid dimensions, learning parameters, weight vectors, and spatial map.

        Args:
            rows (int): Total number of discrete nodes along the lattice height axis.
            columns (int): Total number of discrete nodes along the lattice width axis.
            input_dimension (int): Feature vector dimensionality expected in training samples.
            learning_rate (float): Initial competitive learning rate (0.0, 1.0]. Defaults to 0.5.
            radius (float, optional): Initial neighborhood Gaussian spread radius factor. 
                Defaults to half of the maximum grid dimension.

        Raises:
            TypeError: If row, column, or feature dimension values are not integers.
            ValueError: If structural parameters, learning rates, or radiuses violate bounds.
        """
        if (not isinstance(rows, int) or not isinstance(columns, int) or not isinstance(input_dimension, int)):
            raise TypeError("Grid rows, columns, and input dimension must be integers.")

        if rows <= 0 or columns <= 0 or input_dimension <= 0:
            raise ValueError(
                "Grid rows, columns, and input dimension must be positive integers strictly greater than 0."
            )

        if not isinstance(learning_rate, (float, int)):
            raise TypeError("Learning rate must be a float or integer.")

        if not (0.0 < learning_rate <= 1.0):
            raise ValueError(
                f"Learning rate must be in the range (0.0, 1.0]. Got {learning_rate}."
            )

        self.rows = rows
        self.columns = columns
        self.input_dimension = input_dimension
        self.learning_rate = float(learning_rate)

        if radius is None:
            self.radius = max(rows, columns) / 2.0
        else:
            if not isinstance(radius, (int, float)):
                raise TypeError("Radius must be a float or integer.")

            if radius <= 0:
                raise ValueError(
                    f"Initial neighborhood radius must be greater than 0. Got {radius}."
                )

            self.radius = float(radius)

        # Initialize weight matrix with normalized random continuous variables [0.0, 1.0).
        self.weights = numpy.random.rand(rows, columns, input_dimension)

        # Precompute explicit 2D spatial coordinates for all grid nodes to enable vectorized distance checks.
        row_indices, column_indices = numpy.indices((rows, columns))
        self.coordinate_map = numpy.stack(
            (row_indices, column_indices), axis=-1
        )

    def find_best_matching_unit(self, vector: numpy.ndarray) -> tuple[int, int]:
        """
        Locates the 2D grid coordinates (row, column) of the neuron closest to an input vector.

        Args:
            vector (numpy.ndarray): Single observation feature vector of shape (input_dimension,).

        Returns:
            tuple[int, int]: Integer grid coordinates representing the winning node (BMU).

        Raises:
            ValueError: If vector shape fails feature dimension checks or contains non-numeric values.
        """
        if not isinstance(vector, numpy.ndarray):
            vector = numpy.asarray(vector)

        if vector.ndim != 1:
            raise ValueError(
                f"Input vector must be a 1D array. Got shape {vector.shape}."
            )

        if vector.shape[0] != self.input_dimension:
            raise ValueError(
                f"Input vector dimension mismatch. Expected shape ({self.input_dimension},), got {vector.shape}."
            )

        if numpy.isnan(vector).any() or numpy.isinf(vector).any():
            raise ValueError("Input vector contains NaN or infinite values.")

        # Compute Euclidean distance across feature dimensions for every node.
        euclidean_distances = numpy.linalg.norm(
            self.weights - vector, axis=-1
        )

        # Unflatten index location yielding the minimum distance coordinate.
        unflattened_best_matching_unit_index = numpy.argmin(
            euclidean_distances
        )

        best_matching_unit_row_index, best_matching_unit_column_index = (
            numpy.unravel_index(
                unflattened_best_matching_unit_index, (self.rows, self.columns)
            )
        )

        return (
            int(best_matching_unit_row_index),
            int(best_matching_unit_column_index),
        )

    def map_dataset_to_grid(self, dataset: numpy.ndarray) -> list[tuple[int, int]]:
        """
        Projects an evaluation dataset to winning node BMU coordinates across the topological grid.

        Args:
            dataset (numpy.ndarray): Feature dataset array of shape (n_samples, input_dimension).

        Returns:
            list[tuple[int, int]]: Mapped grid coordinate tuples for each sample vector.

        Raises:
            ValueError: If the dataset is empty, misaligned in feature count, or improperly shaped.
        """
        if not isinstance(dataset, numpy.ndarray):
            dataset = numpy.asarray(dataset)

        if dataset.ndim != 2:
            raise ValueError(
                f"Input dataset must be a 2D array of shape (samples, features). Got shape {dataset.shape}."
            )

        if dataset.shape[0] == 0:
            raise ValueError("Evaluation dataset cannot be empty.")

        if dataset.shape[1] != self.input_dimension:
            raise ValueError(
                f"Dataset feature dimension mismatch. Expected {self.input_dimension} features, got {dataset.shape[1]}."
            )

        mapped_coordinates = []

        for vector in dataset:
            best_matching_unit_coordinates = self.find_best_matching_unit(
                vector
            )
            mapped_coordinates.append(best_matching_unit_coordinates)

        return mapped_coordinates

    def train(self, matrix: numpy.ndarray, maximum_iterations: int) -> None:
        """
        Trains the competitive network over iterations via decaying neighborhood adaptation loops.

        Args:
            matrix (numpy.ndarray): Training observation dataset array of shape (n_samples, input_dimension).
            maximum_iterations (int): Total training pass count.

        Raises:
            ValueError: If input matrix formatting, sample counts, or iteration integers are invalid.
        """
        if not isinstance(matrix, numpy.ndarray):
            matrix = numpy.asarray(matrix)

        if matrix.ndim != 2:
            raise ValueError(
                f"Training matrix must be a 2D array of shape (samples, features). Got shape {matrix.shape}."
            )

        if matrix.shape[0] == 0:
            raise ValueError("Training matrix must contain at least 1 sample.")

        if matrix.shape[1] != self.input_dimension:
            raise ValueError(
                f"Training data feature dimension mismatch. Expected {self.input_dimension} features, got {matrix.shape[1]}."
            )

        if numpy.isnan(matrix).any() or numpy.isinf(matrix).any():
            raise ValueError("Training matrix contains NaN or infinite values.")

        if (not isinstance(maximum_iterations, int) or maximum_iterations <= 0):
            raise ValueError(
                f"Maximum iterations must be a positive integer strictly greater than zero. Got {maximum_iterations}."
            )

        # Decay scale time constant derived from maximum allowable training loops.
        time_decay_constant = maximum_iterations / numpy.log(self.radius)

        for iteration in range(maximum_iterations):
            # Calculate iteration-decayed neighborhood radius and learning rate factors.
            radius = self.radius * numpy.exp(-iteration / time_decay_constant)
            learning_rate = self.learning_rate * numpy.exp(
                -iteration / maximum_iterations
            )

            # Sample a random observation vector from the training matrix.
            random_sample_index = numpy.random.randint(0, matrix.shape[0])
            selected_sample = matrix[random_sample_index]

            # Identify the Best Matching Unit (BMU) for the selected sample vector.
            (
                best_matching_unit_row,
                best_matching_unit_column,
            ) = self.find_best_matching_unit(selected_sample)

            best_matching_unit_coordinate = numpy.array(
                [best_matching_unit_row, best_matching_unit_column]
            )

            # Evaluate spatial grid distances from all nodes to the target BMU coordinate.
            spatial_distance_to_best_matching_unit = numpy.linalg.norm(
                self.coordinate_map - best_matching_unit_coordinate, axis=-1
            )

            # Evaluate Gaussian neighborhood influence factors centered on the target BMU.
            neighborhood_influence = numpy.exp(
                -(spatial_distance_to_best_matching_unit**2)
                / (2 * (radius**2))
            )

            # Expand influence matrix dimensions to broadcast across weight feature length.
            neighborhood_influence_broadcasted = numpy.expand_dims(
                neighborhood_influence, axis=-1
            )

            # Apply gradient weight update step towards the selected training vector.
            weight_adjustments = (
                learning_rate
                * neighborhood_influence_broadcasted
                * (selected_sample - self.weights)
            )

            self.weights += weight_adjustments