"""
fuzzy_c_means.py

A custom implementation of the Fuzzy C-Means (FCM) soft-clustering engine.

This module provides a vectorized algorithm for partitioning multidimensional feature 
spaces into overlapping, probabilistic cluster distributions. Unlike rigid k-means 
partitioning, FCM allows data points to maintain varying degrees of membership across 
multiple cluster centroids, optimized via iterative expectation-maximization loops.

Core Operational Capabilities:
  - Vectorized Euclidean distance tensor calculations across arbitrary feature dimensions.
  - Exponential membership weighting governed by a user-configured fuzziness parameter (m > 1).
  - Automated calculation of validation metrics including Fuzzy Partition Coefficient (FPC) 
    and Partition Entropy (PE).
  - Out-of-sample soft membership inference for unseen spatial query coordinates.
"""
from __future__ import annotations

import numpy

class FuzzyCMeans:
    """
    An engine responsible for managing structural cluster centers, calculating soft partition 
    membership matrices, and evaluating clustering entropy across numerical feature spaces.
    """

    def __init__(self, number_of_clusters: int = 3, 
        fuzziness_degree: float = 2.0, 
        maximum_iterations: int = 300, 
        convergence_tolerance: float = 1e-5):
        """
        Initializes the fuzzy clustering engine with target structural constraints.

        Args:
            number_of_clusters (int): The total number of operational centroids (c) to fit.
            fuzziness_degree (float): The fuzziness weighting exponent (m > 1).
            maximum_iterations (int): The maximum allowable optimization loop passes.
            convergence_tolerance (float): Stopping criteria matrix change threshold.

        Raises:
            ValueError: If the fuzziness degree exponent does not strictly exceed 1.0.
        """
        if fuzziness_degree <= 1.0:
            raise ValueError("Fuzziness weighting exponent 'm' must be strictly greater than 1.0.")

        self.number_of_clusters = number_of_clusters
        self.fuzziness_degree = fuzziness_degree
        self.maximum_iterations = maximum_iterations
        self.convergence_tolerance = convergence_tolerance

        # Internal operation states.
        self.centers = None
        self.memberships = None
        self.count = 0

    def _initialize_membership_matrix(self, count: int, generator: numpy.random.Generator) -> numpy.ndarray:
        """
        Derives the initial soft membership distribution using standard uniform random initialization.

        Args:
            count (int): Total number of discrete data samples.
            generator (np.random.Generator): State engine for deterministic sampling.

        Returns:
            np.ndarray: A normalized membership matrix of shape (c, n_samples).
        """
        # Draw uniform stochastic values.
        unnormalized_memberships = generator.uniform(size=(self.number_of_clusters, count))

        # Enforce column-wise probability sum constraint.
        return unnormalized_memberships / numpy.sum(unnormalized_memberships, axis=0, keepdims=True)

    def _compute_cluster_centers(self, features: numpy.ndarray, memberships: numpy.ndarray) -> numpy.ndarray:
        """
        Calculates fuzzy-weighted centroids using exponential membership weighting (U^m).

        Args:
            features (numpy.ndarray): Target multidimensional data array.
            memberships (numpy.ndarray): Active soft membership probability array.

        Returns:
            numpy.ndarray: Updated centroid spatial coordinates of shape (c, n_features).
        """
        # Ensure feature coordinates maintain a 2D matrix shape.
        features_2d = numpy.atleast_2d(features)

        # Apply exponent to memberships.
        weighted_memberships = memberships ** self.fuzziness_degree

        # Sum exponentiated weights per cluster across samples to establish denominator scalar.
        center_denominators = numpy.sum(weighted_memberships, axis=1, keepdims=True)

        # Floor denominator to epsilon to avoid zero-division errors.
        epsilon = 1e-12

        bounded_denominators = numpy.maximum(center_denominators, epsilon)

        # Pre-normalize weights per cluster row.
        normalized_weights = weighted_memberships / bounded_denominators

        return numpy.dot(normalized_weights, features_2d)

    def _compute_membership_matrix(self, features: numpy.ndarray, centers: numpy.ndarray) -> numpy.ndarray:
        """
        Updates membership degrees using vectorized distance metrics over finite fields.

        Args:
            features (numpy.ndarray): Target multidimensional data array.
            centers (numpy.ndarray): Calculated structural centroid coordinates.

        Returns:
            numpy.ndarray: Updated fuzzy partition membership matrix of shape (c, n_samples).
        """
        features_2d = numpy.atleast_2d(features)

        # Compute 3D broadcasted pairwise Euclidean distances between features and centers.
        distances = numpy.linalg.norm(features_2d[numpy.newaxis, :, :] - centers[:, numpy.newaxis, :], axis=2)

        # Identify coincident query points that match a cluster center coordinate.
        coincident_mask = distances < 1e-10

        if numpy.any(coincident_mask):
            # Directly allocate probability 1.0 to coincident centroid(s) and 0.0 elsewhere.
            memberships = numpy.zeros_like(distances)
            coincident_counts = numpy.sum(coincident_mask, axis=0, keepdims=True)

            memberships[coincident_mask] = 1.0
            memberships /= numpy.maximum(coincident_counts, 1.0)

            return memberships

        # FCM exponent ratio scaling parameter.
        fuzziness_exponent_scaling = 2.0 / (self.fuzziness_degree - 1.0)

        # Vectorized ratio calculation.
        distance_ratios = distances[:, numpy.newaxis, :] / distances[numpy.newaxis, :, :]
        scaled_ratios = distance_ratios ** fuzziness_exponent_scaling

        # Sum ratios across alternative cluster indices k to yield denominator sums.
        summed_ratios = numpy.sum(scaled_ratios, axis=1)

        return 1.0 / summed_ratios

    def compute_fuzzy_coefficient(self) -> float:
        """
        Calculates the Fuzzy Partition Coefficient (FPC) evaluating partition clarity.

        Returns:
            float: FPC metric scalar value in range [1/c, 1.0]. Higher values indicate clean separation.
        """
        if self.memberships is None:
            raise RuntimeError("Model engine must be fitted prior to evaluating metrics.")

        observation_count = self.memberships.shape[1]

        # Calculate sum of all squared memberships.
        squared_membership_trace = numpy.trace(numpy.dot(self.memberships, self.memberships.T))

        return float(squared_membership_trace / observation_count)

    def compute_entropy(self) -> float:
        """
        Calculates the Partition Entropy (PE) measuring cluster overlap ambiguity.

        Returns:
            float: PE metric scalar value in range [0, log(c)]. Lower values indicate less uncertainty.
        """
        if self.memberships is None:
            raise RuntimeError("Model engine must be fitted prior to evaluating metrics.")

        epsilon = 1e-12
        observation_count = self.memberships.shape[1]

        entropy_sum = numpy.sum(self.memberships * numpy.log(self.memberships + epsilon))

        return float(-entropy_sum / observation_count)

    def fit(self, features: numpy.ndarray, state: int | None = None) -> "FuzzyCMeans":
        """
        Executes optimization iterations to converge cluster centers and memberships.

        Args:
            features (numpy.ndarray): The raw numerical observation matrix (n_samples, n_features).
            state (int, optional): Random generator seed for reproducibility.

        Returns:
            FuzzyCMeans: The fitted model engine instance.
        """
        # Coerce inputs to float64 contiguous arrays and ensure 2D feature matrix representation.
        coerced_features = numpy.asarray(features, dtype=numpy.float64)
        coerced_features = numpy.atleast_2d(coerced_features)
        observation_count, _ = coerced_features.shape

        # Initialize a NumPy random number generator for reproducible sampling.
        generator = numpy.random.default_rng(state)

        # Derive initial random normalized soft membership distribution matrix.
        self.memberships = self._initialize_membership_matrix(observation_count, generator)

        # Execute expectation-maximization optimization loop.
        for iteration in range(self.maximum_iterations):
            previous_memberships = self.memberships.copy()

            # Step 1: Update cluster centroids based on current soft memberships.
            self.centers = self._compute_cluster_centers(coerced_features, self.memberships)

            # Step 2: Re-evaluate soft memberships based on updated centroid spatial coordinates.
            self.memberships = self._compute_membership_matrix(coerced_features, self.centers)

            # Measure matrix movement magnitude via Frobenius norm.
            delta = numpy.linalg.norm(self.memberships - previous_memberships)

            # Halt optimization early if membership matrix delta satisfies convergence threshold.
            if delta < self.convergence_tolerance:
                self.count = iteration + 1
                break
        else:
            # Loop completed without reaching tolerance threshold; set execution count to maximum.
            self.count = self.maximum_iterations

        return self

    def predict(self, features: numpy.ndarray) -> numpy.ndarray:
        """
        Evaluates soft cluster membership distributions for unseen input datasets.

        Args:
            features (numpy.ndarray): Target numerical observations array.

        Returns:
            numpy.ndarray: Membership probability matrix of shape (c, n_samples).

        Raises:
            RuntimeError: If called before model execution and fitting.
        """
        if self.centers is None:
            raise RuntimeError("Model engine must be fitted prior to predicting membership.")

        # Cast query data and project through fitted membership distance calculation.
        coerced_features = numpy.asarray(features, dtype=numpy.float64)
        coerced_features = numpy.atleast_2d(coerced_features)

        return self._compute_membership_matrix(coerced_features, self.centers)