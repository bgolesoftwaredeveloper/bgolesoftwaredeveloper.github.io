"""
===============================================================================
Module: local_outlier_factor.pyx
Description: High-performance Cython implementation of the Local Outlier Factor
             (LOF) anomaly detection algorithm using typed C memoryviews and 
             optimized low-level loops.
===============================================================================
"""
import numpy
cimport numpy as cnumpy

from libc.math cimport sqrt

def calculate_euclidean_distance(double[:, :] matrix):
    """
    Computes pairwise Euclidean distances between all samples in a feature matrix.

    Parameters:
        matrix (double[:, :]): 2D Memoryview containing input dataset array.

    Returns:
        numpy.ndarray: 2D array of shape (number_of_samples, number_of_samples) 
                       containing symmetric pairwise distances.
    """
    # Defensive check on the matrix.
    if matrix is None:
        raise ValueError("Matrix cannot be None.")

    cdef int number_of_samples = matrix.shape[0]
    cdef int number_of_features = matrix.shape[1]

    if number_of_samples == 0:
        raise ValueError("Matrix must contain at least one sample.")

    if number_of_features == 0:
        raise ValueError("Matrix must contain at least one feature.")

    # Allocate zero-initialized matrix to hold distance calculations.
    cdef cnumpy.ndarray[cnumpy.float64_t, ndim=2] distances = numpy.zeros((number_of_samples, number_of_samples), dtype=numpy.float64)

    # Bind C-level memoryview for fast direct indexing without Python overhead.
    cdef double[:, :] distance_view = distances

    # Loop iteration and distance accumulation variables.
    cdef int outer_sample_index = 0
    cdef int inner_sample_index = 0
    cdef int feature_index = 0

    cdef double difference = 0.0
    cdef double squared_distance = 0.0

    # Calculate pairwise distances (upper triangular matrix optimization).
    for outer_sample_index in range(number_of_samples):
        for inner_sample_index in range(outer_sample_index + 1, number_of_samples):
            squared_distance = 0.0
            
            # Compute sum of square feature differneces.
            for feature_index in range(number_of_features):
                difference = matrix[outer_sample_index, feature_index] - matrix[inner_sample_index, feature_index]
                squared_distance += difference * difference

            # Compute square root and assign to symmetric entries.
            distance_view[outer_sample_index, inner_sample_index] = sqrt(squared_distance)
            distance_view[inner_sample_index, outer_sample_index] = distance_view[outer_sample_index, inner_sample_index]

    return distances

def compute_local_outlier_factor(double[:, :] dataset, int nearest_neighbors_count):
    """
    Computes Local Outlier Factor (LOF) anomaly scores for each sample in a dataset.

    Parameters:
        dataset (double[:, :]): 2D Memoryview array of shape (number_of_samples, number_of_features).
        nearest_neighbors_count (int): Number of k-nearest neighbors to consider for reachability distance.

    Returns:
        numpy.ndarray: 1D array of shape (number_of_samples,) containing LOF scores.
                       Values substantially greater than 1 indicate potential outliers.
    """
    # Defensive checks on the dataset.
    if dataset is None:
        raise ValueError("Dataset matrix cannot be None.")

    cdef int number_of_samples = dataset.shape[0]
    cdef int number_of_features = dataset.shape[1]

    if number_of_samples < 2:
        raise ValueError("Dataset matrix must contain at least 2 samples.")

    if number_of_features < 1:
        raise ValueError("Dataset matrix must contain at least 1 feature.")

    if nearest_neighbors_count < 1:
        raise ValueError(f"nearest_neighbors_count must be at least 1, got {nearest_neighbors_count}.")

    if nearest_neighbors_count >= number_of_samples:
        raise ValueError(f"nearest_neighbors_count ({nearest_neighbors_count}) must be strictly "
            f"less than the total number of samples ({number_of_samples}).")

    # Compute pairwise Euclidean distance matrix for all samples.
    cdef cnumpy.ndarray[cnumpy.float64_t, ndim=2] distances = calculate_euclidean_distance(dataset)

    # Allocate buffers for tracking k-nearest neighbor indices and k-distances.
    cdef cnumpy.ndarray[cnumpy.int64_t, ndim=2] neighbor_indices = numpy.zeros((number_of_samples, nearest_neighbors_count), dtype=numpy.int64)
    cdef cnumpy.ndarray[cnumpy.float64_t, ndim=1] distance_vector = numpy.zeros(number_of_samples, dtype=numpy.float64)

    cdef int current_sample_index = 0
    cdef int neighbor_rank = 0

    cdef cnumpy.ndarray[cnumpy.int64_t, ndim=1] sorted_indices

    # Identify k-nearest neighbors and determine k-distance for each sample.
    for current_sample_index in range(number_of_samples):
        # Sort distance indices to isolate nearest neighbor (excluding self at index 0).
        sorted_indices = numpy.argsort(distances[current_sample_index])
        neighbor_indices[current_sample_index] = sorted_indices[1:nearest_neighbors_count + 1]

        # Store the distance to the k-th nearest neighbor (k-distance).
        distance_vector[current_sample_index] = distances[current_sample_index, neighbor_indices[current_sample_index, nearest_neighbors_count - 1]]

    # Buffer for storing calculated local reachability densities.
    cdef cnumpy.ndarray[cnumpy.float64_t, ndim=1] local_reachability_density = numpy.zeros(number_of_samples, dtype=numpy.float64)

    cdef int target_neighbor_index = 0

    cdef double reachability_distance = 0.0
    cdef double reachability_distance_sum = 0.0

    # Calculate Local Reachability Density (LRD) for each sample.
    for current_sample_index in range(number_of_samples):
        reachability_distance_sum = 0.0

        # Accumulate reachability distances for all k-neighbors.
        for neighbor_rank in range(nearest_neighbors_count):
            target_neighbor_index = neighbor_indices[current_sample_index, neighbor_rank]
            reachability_distance = distances[current_sample_index, target_neighbor_index]

            if distance_vector[target_neighbor_index] > reachability_distance:
                reachability_distance = distance_vector[target_neighbor_index]

            reachability_distance_sum += reachability_distance

        # Compute inverse average reachability distance.
        if reachability_distance_sum > 0.0:
            local_reachability_density[current_sample_index] = nearest_neighbors_count / reachability_distance_sum
        else:
            local_reachability_density[current_sample_index] = 0.0

    # Buffer for final output anomaly scores.
    cdef cnumpy.ndarray[cnumpy.float64_t, ndim=1] local_outlier_factor_scores = numpy.zeros(number_of_samples, dtype=numpy.float64)

    cdef double neighbor_density_sum = 0.0

    # Calculate final Local Outlier Factor (LOF) score as average LRD ratio of neighbors.
    for current_sample_index in range(number_of_samples):
        neighbor_density_sum = 0.0
        
        # Sum local reachability densities of nearest neighbors.
        for neighbor_rank in range(nearest_neighbors_count):
            target_neighbor_index = neighbor_indices[current_sample_index, neighbor_rank]
            neighbor_density_sum += local_reachability_density[target_neighbor_index]

        # Compute ratio comparing local density against neighbor densities.
        if local_reachability_density[current_sample_index] > 0.0:
            local_outlier_factor_scores[current_sample_index] = (neighbor_density_sum / nearest_neighbors_count) / local_reachability_density[current_sample_index]
        else:
            local_outlier_factor_scores[current_sample_index] = 0.0

    return local_outlier_factor_scores