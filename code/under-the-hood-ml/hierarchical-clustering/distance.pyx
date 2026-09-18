"""
===============================================================================
Module: distance.pyx
Description: High-performance Cython implementation of a 2D distance matrix
             using raw C-level dynamic memory allocation for high execution
             speed and minimal memory overhead.
===============================================================================
"""
from libc.stdlib cimport malloc, free

cdef class DistanceMatrix:
    """
    Flat contiguous 2D Distance Matrix implemented in Cython.

    Manages a dynamically allocated double-precision floating-point array in C memory,
    providing fast element lookup and modification without Python GIL overhead.
    """

    def __cinit__(self, int size):
        """
        Allocates heap memory for an N x N distance matrix upon C-level initialization.

        Parameters:
            size (int): Dimension of the square matrix (must be greater than zero).

        Raises:
            ValueError: If size is less than or equal to zero.
            MemoryError: If dynamic memory allocation via malloc fails.
        """
        if size <= 0:
            raise ValueError("Size of distance matrix must be greater than zero.")

        self.size = size
        self._matrix = <double*>malloc(sizeof(double) * size * size)

        if not self._matrix:
            raise MemoryError("Failed to allocate memory for distance matrix.")

    cdef inline double getter(self, int row_index, int column_index) noexcept nogil:
        """
        Retrieves a distance value from the raw memory buffer at the specified coordinates.

        Parameters:
            row_index (int): Row index in the square matrix.
            column_index (int): Column index in the square matrix.

        Returns:
            double: Distance value stored at the given matrix position.
        """
        return self._matrix[row_index * self.size + column_index]

    cdef inline void setter(self, int row_index, int column_index, double value) noexcept nogil:
        """
        Stores a distance value into the raw memory buffer at the specified coordinates.

        Parameters:
            row_index (int): Row index in the square matrix.
            column_index (int): Column index in the square matrix.
            value (double): Distance value to store.
        """
        self._matrix[row_index * self.size + column_index] = value

    def get_element(self, int row_index, int column_index):
        """
        Provides safe Python-accessible retrieval of a matrix element with boundary checking.

        Parameters:
            row_index (int): Row index in the square matrix.
            column_index (int): Column index in the square matrix.

        Returns:
            float: Distance value stored at the target matrix position.

        Raises:
            IndexError: If row or column indices exceed matrix boundaries.
        """
        if row_index < 0 or row_index >= self.size or column_index < 0 or column_index >= self.size:
            raise IndexError("Matrix index out of bounds.")

        return self.getter(row_index, column_index)

    def set_element(self, int row_index, int column_index, double value):
        """
        Provides safe Python-accessible modification of a matrix element with boundary checking.

        Parameters:
            row_index (int): Row index in the square matrix.
            column_index (int): Column index in the square matrix.
            value (double): Distance value to store into the matrix.

        Raises:
            IndexError: If row or column indices exceed matrix boundaries.
        """
        if row_index < 0 or row_index >= self.size or column_index < 0 or column_index >= self.size:
            raise IndexError("Matrix index out of bounds.")

        self.setter(row_index, column_index, value)

    def __dealloc__(self):
        """
        Frees dynamically allocated C memory when the instance is garbage collected.
        """
        if self._matrix != NULL:
            free(self._matrix)

            self._matrix = NULL
