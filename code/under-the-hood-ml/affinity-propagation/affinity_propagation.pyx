"""
===============================================================================
Module: affinity_propagation.pyx
Description: High-performance Cython implementation of the Affinity Propagation
             clustering algorithm using typed C memoryviews and fast, low-level
             loops.
===============================================================================
"""
import numpy
cimport numpy as cnumpy

cdef class AffinityPropagation:
    """
    Affinity Propagation Clustering algorithm implemented in Cython.

    Performs clustering by passing messages (responsibilities and availabilities)
    between data points until a high-quality set of exemplars and corresponding
    clusters emerges.

    Attributes:
        maximum_iterations (int): Upper bound on the number of message-passing loops.
        damping_factor (float): Factor in [0.5, 1.0) applied to numerical updates to avoid numerical oscillations.
        preference_value (float): Input preference value assigned to the matrix diagonal (higher values produce more clusters).
        number_of_samples (int): Total number of samples in the fitted dataset.
        number_of_features (int): Total number of feature columns per sample.
    """
    cdef public int maximum_iterations
    cdef public double damping_factor
    cdef public double preference_value

    cdef public int number_of_samples
    cdef public int number_of_features

    cdef public double[:, :] features
    cdef public double[:, :] similarities
    cdef public double[:, :] responsibilities
    cdef public double[:, :] availabilities

    def __init__(self, int maximum_iterations=200, double damping_factor=0.5, double preference_value=-50.0):
        """
        Initializes the AffinityPropagation hyper-parameters.

        Parameters:
            maximum_iterations (int): Maximum number of iterations to execute during optimization.
            damping_factor (float): Damping coefficient to control numerical stability.
            preference_value (float): Shared preference value for all potential exemplars.
        """
        self.maximum_iterations = maximum_iterations
        self.damping_factor = damping_factor
        self.preference_value = preference_value

    cpdef void _initialize_buffers(self, double[:, :] features):
        """
        Allocates and sets up C-level memory view buffers for matrices.

        Parameters:
            features (double[:, :]): 2D Memoryview containing input dataset array.
        """
        self.features = features
        self.number_of_samples = features.shape[0]
        self.number_of_features = features.shape[1]

        # Allocate zero-initialized memory buffers.
        self.similarities = numpy.zeros((self.number_of_samples, self.number_of_samples), dtype=numpy.float64)
        self.responsibilities = numpy.zeros((self.number_of_samples, self.number_of_samples), dtype=numpy.float64)
        self.availabilities = numpy.zeros((self.number_of_samples, self.number_of_samples), dtype=numpy.float64)

    cpdef void _compute_similarity_matrix(self):
        """
        Computes the pairwise negative squared Euclidean distance matrix S, 
        populating the diagonal with the preference value.
        """
        cdef int sample_index, k, feature_index
        cdef double feature_difference, squared_distance

        for sample_index in range(self.number_of_samples):
            for k in range(self.number_of_samples):
                if sample_index == k:
                    self.similarities[sample_index, k] = self.preference_value
                else:
                    squared_distance = 0.0

                    for feature_index in range(self.number_of_features):
                        feature_difference = self.features[sample_index, feature_index] - self.features[k, feature_index]
                        squared_distance += feature_difference * feature_difference

                    self.similarities[sample_index, k] = -squared_distance

    cpdef void _update_responsibilities(self, double[:, :] response):
        """
        Updates the responsibility matrix R based on current availabilities and similarities, 
        applying the specified damping factor.

        Parameters:
            response (double[:, :]): Temporary 2D memoryview buffer for raw update values.
        """
        cdef int sample_index, k, other_k
        cdef double maximum_other_value, candidate_value

        for sample_index in range(self.number_of_samples):
            for k in range(self.number_of_samples):
                maximum_other_value = -1e300
                
                for other_k in range(self.number_of_samples):
                    if other_k != k:
                        candidate_value = (self.availabilities[sample_index, other_k] + 
                                           self.similarities[sample_index, other_k])

                        if candidate_value > maximum_other_value:
                            maximum_other_value = candidate_value
                            
                response[sample_index, k] = self.similarities[sample_index, k] - maximum_other_value

        # Apply damping factor.
        for sample_index in range(self.number_of_samples):
            for k in range(self.number_of_samples):
                self.responsibilities[sample_index, k] = (
                    self.damping_factor * self.responsibilities[sample_index, k] +
                    (1.0 - self.damping_factor) * response[sample_index, k]
                )

    cpdef void _update_availabilities(self, double[:, :] availabilities):
        """
        Updates the availability matrix A based on updated responsibilities, 
        applying the specified damping factor.

        Parameters:
            availabilities (double[:, :]): Temporary 2D memoryview buffer for raw update values.
        """
        cdef int sample_index, k
        cdef double positive_response_sum, current_position, response_contribution, candidate_value

        for k in range(self.number_of_samples):
            # Sum positive responsibilities sent to exemplar candidate k.
            positive_response_sum = 0.0

            for sample_index in range(self.number_of_samples):
                if sample_index != k and self.responsibilities[sample_index, k] > 0:
                    positive_response_sum += self.responsibilities[sample_index, k]

            for sample_index in range(self.number_of_samples):
                if sample_index == k:
                    # Diagonal update.
                    availabilities[k, k] = positive_response_sum
                else:
                    # Off-diagonal update.
                    current_position = self.responsibilities[sample_index, k]
                    response_contribution = current_position if current_position > 0 else 0.0
                    
                    candidate_value = self.responsibilities[k, k] + (positive_response_sum - response_contribution)
                    availabilities[sample_index, k] = candidate_value if candidate_value < 0 else 0.0

        # Apply damping factor.
        for sample_index in range(self.number_of_samples):
            for k in range(self.number_of_samples):
                self.availabilities[sample_index, k] = (
                    self.damping_factor * self.availabilities[sample_index, k] +
                    (1.0 - self.damping_factor) * availabilities[sample_index, k]
                )

    cpdef tuple _extract_clusters(self):
        """
        Identifies exemplars from matrix sum diagonals R + A > 0 and assigns each 
        sample point to its nearest selected exemplar.

        Returns:
            tuple: (exemplar_indices, cluster_assignments) containing 1D NumPy arrays.
        """
        cdef int sample_index, exemplar_index, best_exemplar
        cdef double maximum_affinity, current_affinity

        # Identify points where R(k,k) + A(k,k) > 0 as exemplars.
        exemplar_indices = []

        for sample_index in range(self.number_of_samples):
            if (self.responsibilities[sample_index, sample_index] + self.availabilities[sample_index, sample_index]) > 0:
                exemplar_indices.append(sample_index)

        # Assign remaining points to their closest exemplar based on similarity.
        cdef cnumpy.ndarray[cnumpy.int32_t, ndim=1] cluster_assignments = numpy.zeros(self.number_of_samples, dtype=numpy.int32)

        cdef int[:] assignments_view = cluster_assignments

        for sample_index in range(self.number_of_samples):
            maximum_affinity = -1e300
            best_exemplar = -1

            for exemplar_index in exemplar_indices:
                current_affinity = self.similarities[sample_index, exemplar_index]

                if current_affinity > maximum_affinity:
                    maximum_affinity = current_affinity
                    best_exemplar = exemplar_index

            assignments_view[sample_index] = best_exemplar

        return numpy.array(exemplar_indices, dtype=numpy.int32), cluster_assignments

    cpdef tuple fit(self, double[:, :] features):
        """
        Runs the full Affinity Propagation message passing algorithm on the provided dataset.

        Parameters:
            features (double[:, :]): 2D Memoryview array of shape (number_of_samples, number_of_features).

        Returns:
            tuple: (exemplar_indices, cluster_assignments) NumPy integer arrays representing cluster centers and predictions.
        """
        cdef int iteration

        # 1. Setup buffers and compute matrix.
        self._initialize_buffers(features)
        self._compute_similarity_matrix()

        # Temporary iteration buffers.
        response = numpy.zeros((self.number_of_samples, self.number_of_samples), dtype=numpy.float64)
        availabilities = numpy.zeros((self.number_of_samples, self.number_of_samples), dtype=numpy.float64)

        # 2. Iterative message passing loops.
        for iteration in range(self.maximum_iterations):
            self._update_responsibilities(response)
            self._update_availabilities(availabilities)

        # 3. Resolve exemplars and assignments.
        return self._extract_clusters()