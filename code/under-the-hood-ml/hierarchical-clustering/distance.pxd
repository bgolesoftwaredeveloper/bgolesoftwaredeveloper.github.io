cdef class DistanceMatrix:
    cdef double* _matrix
    cdef readonly int size

    cdef inline double getter(self, int row_index, int column_index) noexcept nogil
    cdef inline void setter(self, int row_index, int column_index, double value) noexcept nogil