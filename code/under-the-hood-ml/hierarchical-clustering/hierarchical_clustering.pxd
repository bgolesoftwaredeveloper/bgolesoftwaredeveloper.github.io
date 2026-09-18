from distance cimport DistanceMatrix

cdef class HierarchicalClustering:
    cdef public int number_of_clusters
    cdef public int number_of_samples
    cdef public int number_of_features

    cdef readonly object linkage_matrix
    cdef readonly bint is_fitted

    cdef void _validate_input(self, double[:, :] view)
    
    cdef void _c_compute_pairwise_distances(self,
        double[:, :] feature_matrix,
        DistanceMatrix distance_matrix,
        int sample_count,
        int feature_count
    ) noexcept nogil

    cpdef void _compute_pairwise_distances(self,
        double[:, :] feature_matrix,
        DistanceMatrix distance_matrix
    )

    cdef void _agglomerate(self, DistanceMatrix distance_matrix)
