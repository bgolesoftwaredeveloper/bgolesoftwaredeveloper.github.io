"""
hierarchical_clustering.pyx

A high-performance Cython implementation of Single-Linkage Hierarchical Agglomerative 
Clustering using C-level memoryviews, direct dynamic memory allocations, and GIL-free 
pairwise distance computations.

This engine computes dense pairwise Euclidean distance matrices and iteratively merges 
nearest clusters to produce a standard linkage matrix structure compliant with scipy.cluster.hierarchy.
"""
import numpy
cimport numpy

from libc.stdlib cimport malloc, free
from libc.math cimport sqrt

from distance cimport DistanceMatrix

cdef class HierarchicalClustering:
    """
    Cython implementation of Single-Linkage Hierarchical Clustering.
    """

    def __init__(self, int number_of_clusters=1):
        # 1. Validate target cluster hyperparameter bounds.
        if number_of_clusters < 1:
            raise ValueError("Number of cluster must be at least 1.")

        # 2. Initialize tracking attributes and default model flags.
        self.number_of_clusters = number_of_clusters
        self.number_of_samples = 0
        self.number_of_features = 0

        self.linkage_matrix = None
        
        self.is_fitted = False

    cdef void _validate_input(self, double[:, :] view):
        # 3. Assert memoryview dimensions satisfy non-empty and sample count bounds.
        if view.shape[0] < 1 or view.shape[1] < 1:
            raise ValueError("Input data matrix cannot be empty.")
        
        if view.shape[0] < self.number_of_clusters:
            raise ValueError("Number of samples cannot be smaller than requested number of clusters.")

    cdef void _c_compute_pairwise_distances(self, 
        double[:, :] feature_matrix, 
        DistanceMatrix distance_matrix,
        int sample_count,
        int feature_count
    ) noexcept nogil:
        cdef int reference_sample_index, comparison_sample_index, feature_dimension_index
        cdef double accumulated_difference, feature_difference

        # 4. Compute lower-triangle pairwise Euclidean distances across sample space in C.
        for reference_sample_index in range(sample_count):
            distance_matrix.setter(reference_sample_index, reference_sample_index, 0.0)

            for comparison_sample_index in range(reference_sample_index + 1, sample_count):
                accumulated_difference = 0.0

                for feature_dimension_index in range(feature_count):
                    feature_difference = (
                        feature_matrix[reference_sample_index, feature_dimension_index] - 
                        feature_matrix[comparison_sample_index, feature_dimension_index]
                    )

                    accumulated_difference += feature_difference * feature_difference
                
                accumulated_difference = sqrt(accumulated_difference)
                
                # 5. Populate symmetric distance matrix entries.
                distance_matrix.setter(reference_sample_index, comparison_sample_index, accumulated_difference)
                distance_matrix.setter(comparison_sample_index, reference_sample_index, accumulated_difference)

    cpdef void _compute_pairwise_distances(self, 
        double[:, :] feature_matrix, 
        DistanceMatrix distance_matrix
    ):
        cdef int sample_count = feature_matrix.shape[0]
        cdef int feature_count = feature_matrix.shape[1]
        
        # 6. Release Global Interpreter Lock (GIL) for parallelizable distance calculations.
        with nogil:
            self._c_compute_pairwise_distances(
                feature_matrix,
                distance_matrix,
                sample_count,
                feature_count
            )

    cdef void _agglomerate(self, DistanceMatrix distance_matrix):
        cdef int sample_count = self.number_of_samples
        cdef double[:, :] linkage_view = numpy.zeros((sample_count - 1, 4), dtype=numpy.float64)
        
        # 7. Dynamically allocate C-level buffers for active cluster tracking and member counts.
        cdef int* active_cluster_identifiers = <int*>malloc(sizeof(int) * sample_count)
        cdef int* cluster_member_counts = <int*>malloc(sizeof(int) * (2 * sample_count))

        if not active_cluster_identifiers:
            raise MemoryError("There is not enough memory to allocate to the active cluster identifiers.")

        if not cluster_member_counts:
            raise MemoryError("There is not enough memory to allocate the cluster member counts.")

        cdef int agglomeration_step_index, candidate_cluster_index, neighbor_cluster_index, unmerged_cluster_index
        cdef int primary_cluster_location, secondary_cluster_location
        cdef int primary_cluster_identifier, secondary_cluster_identifier, target_cluster_identifier
        cdef int active_cluster_count = sample_count
        cdef double minimum_pairwise_distance, evaluated_distance, updated_distance, alternative_distance
        cdef int generated_cluster_identifier = sample_count

        # 8. Verify C memory allocations succeeded.
        if not active_cluster_identifiers or not cluster_member_counts:
            if active_cluster_identifiers != NULL: 
                free(active_cluster_identifiers)

            if cluster_member_counts != NULL: 
                free(cluster_member_counts)
                
            raise MemoryError("Failed to allocate dynamic memory for cluster tracking.")

        # 9. Initialize singleton cluster state vectors.
        for unmerged_cluster_index in range(sample_count):
            active_cluster_identifiers[unmerged_cluster_index] = unmerged_cluster_index
            cluster_member_counts[unmerged_cluster_index] = 1

        try:
            # 10. Execute N-1 agglomerative merge iterations.
            for agglomeration_step_index in range(sample_count - 1):
                minimum_pairwise_distance = 1e308
                primary_cluster_location = -1
                secondary_cluster_location = -1

                # 11. Find closest pair of active clusters.
                for candidate_cluster_index in range(active_cluster_count):
                    for neighbor_cluster_index in range(candidate_cluster_index + 1, active_cluster_count):
                        evaluated_distance = distance_matrix.getter(
                            active_cluster_identifiers[candidate_cluster_index], 
                            active_cluster_identifiers[neighbor_cluster_index]
                        )

                        if evaluated_distance < minimum_pairwise_distance:
                            minimum_pairwise_distance = evaluated_distance
                            primary_cluster_location = candidate_cluster_index
                            secondary_cluster_location = neighbor_cluster_index

                primary_cluster_identifier = active_cluster_identifiers[primary_cluster_location]
                secondary_cluster_identifier = active_cluster_identifiers[secondary_cluster_location]

                # 12. Record merge metadata in linkage matrix view.
                linkage_view[agglomeration_step_index, 0] = <double> primary_cluster_identifier
                linkage_view[agglomeration_step_index, 1] = <double> secondary_cluster_identifier
                linkage_view[agglomeration_step_index, 2] = minimum_pairwise_distance
                
                cluster_member_counts[generated_cluster_identifier] = (
                    cluster_member_counts[primary_cluster_identifier] + 
                    cluster_member_counts[secondary_cluster_identifier]
                )

                linkage_view[agglomeration_step_index, 3] = <double> cluster_member_counts[generated_cluster_identifier]

                # 13. Update single-linkage minimum distances to new composite cluster.
                for unmerged_cluster_index in range(active_cluster_count):
                    if unmerged_cluster_index != primary_cluster_location and unmerged_cluster_index != secondary_cluster_location:
                        target_cluster_identifier = active_cluster_identifiers[unmerged_cluster_index]
                        
                        updated_distance = distance_matrix.getter(
                            primary_cluster_identifier, 
                            target_cluster_identifier
                        )

                        alternative_distance = distance_matrix.getter(
                            secondary_cluster_identifier, 
                            target_cluster_identifier
                        )
                        
                        if alternative_distance < updated_distance:
                            updated_distance = alternative_distance
                        
                        distance_matrix.setter(generated_cluster_identifier, target_cluster_identifier, updated_distance)
                        distance_matrix.setter(target_cluster_identifier, generated_cluster_identifier, updated_distance)

                # 14. Swap and shrink active cluster identifier table.
                active_cluster_identifiers[primary_cluster_location] = generated_cluster_identifier
                active_cluster_identifiers[secondary_cluster_location] = active_cluster_identifiers[active_cluster_count - 1]

                active_cluster_count -= 1
                generated_cluster_identifier += 1

            self.linkage_matrix = numpy.asarray(linkage_view)
        finally:
            # 15. Free dynamically allocated C pointers to prevent memory leaks.
            free(active_cluster_identifiers)
            free(cluster_member_counts)

    def fit(self, object dataset):
        # 16. Convert raw Python input to C-contiguous float64 NumPy array.
        cdef numpy.ndarray[double, ndim=2, cast=True] converted_array = numpy.ascontiguousarray(
            dataset, dtype=numpy.float64
        )

        cdef double[:, :] view = converted_array
        
        self._validate_input(view)

        self.number_of_samples = view.shape[0]
        self.number_of_features = view.shape[1]

        # 17. Allocate memory for upper bound cluster distance matrix.
        cdef DistanceMatrix distance_matrix = DistanceMatrix(2 * self.number_of_samples)
        
        # 18. Execute GIL-free pairwise distance calculations.
        with nogil:
            self._c_compute_pairwise_distances(
                view, 
                distance_matrix, 
                self.number_of_samples, 
                self.number_of_features
            )

        # 19. Run agglomerative clustering algorithm and mark model as fitted.
        self._agglomerate(distance_matrix)
        self.is_fitted = True

        return self

    def get_linkage_matrix(self):
        # 20. Enforce status check prior to returning computed results.
        if not self.is_fitted:
            raise RuntimeError("Model is not fitted yet. Call 'fit' prior to accessing results.")

        return self.linkage_matrix
