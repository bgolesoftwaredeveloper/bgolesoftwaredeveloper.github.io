"""
===============================================================================
Module: kd_tree.pyx
Description: High-performance Cython implementation of a K-Dimensional Tree
             (KD-Tree) spatial partitioning structure, utilizing C memoryviews,
             raw pointer allocations, and lock-free C-level recursion for
             fast exact nearest neighbor lookups.
===============================================================================
"""
from libc.stdlib cimport malloc, free
from libc.math cimport sqrt, INFINITY

# Represents an internal node or leaf within the spatial KD-Tree.
cdef struct Node:
    size_t index
    size_t left_child
    size_t right_child
    size_t split_dimension

# Tracks state of the current closest point discovered during spatial traversal.
cdef struct Neighbor:
    size_t index
    double distance_squared

cdef class KDTree:
    """
    K-Dimensional Tree spatial partitioning structure implemented in Cython.

    Organizes spatial feature coordinates into a binary search tree using cyclic
    dimension median splits via quickselect, enabling fast O(log N) nearest neighbor 
    queries in low-to-medium dimensional metric spaces.

    Attributes:
        points (double[:, :]): 2D Memoryview pointing to input coordinate dataset.
        root (size_t): Pointer array index referencing the root node.
        count (size_t): Internal counter tracking total allocated node structures.
        nodes (Node*): Raw C heap pointer allocating array of Node structures.
        indices (size_t*): Heap buffer managing spatial point index permutations.
        number_of_points (size_t): Total count of spatial coordinate samples (rows).
        number_of_dimensions (size_t): Total count of spatial feature dimensions (columns).
    """
    cdef double[:, :] points

    cdef size_t root
    cdef size_t count

    cdef Node* nodes
    cdef size_t* indices

    cdef size_t number_of_points
    cdef size_t number_of_dimensions

    def __cinit__(self, points):
        """
        Validates input arguments, allocates raw heap buffers, and constructs the tree.

        Parameters:
            points (object): 2D floating-point array-like object containing dataset coordinates.

        Raises:
            ValueError: If input is None, non-2D, or contains zero samples/dimensions.
            MemoryError: If allocation of native C buffers fails.
        """
        self.nodes = NULL
        self.indices = NULL

        # 1. Validate object presence and type coercions.
        if points is None:
            raise ValueError("Input points matrix cannot be None.")

        try:
            self.points = points
        except (TypeError, ValueError):
            raise ValueError("Input must be a 2D array-like object of floating-point values.")

        if self.points.ndim != 2:
            raise ValueError("Point array must be 2 dimensional.")

        self.number_of_points = <size_t>self.points.shape[0]
        self.number_of_dimensions = <size_t>self.points.shape[1]

        # 2. Assert spatial matrix boundary bounds.
        if self.number_of_points == 0:
            raise ValueError("Input points dataset cannot be empty (0 samples).")

        if self.number_of_dimensions == 0:
            raise ValueError("Input points dataset cannot have 0 feature dimensions.")

        self.count = 0
        
        # 3. Heap-allocate contiguous memory buffers.
        self.nodes = <Node*> malloc(self.number_of_points * sizeof(Node))
        self.indices = <size_t*> malloc(self.number_of_points * sizeof(size_t))

        if self.nodes == NULL or self.indices == NULL:
            if self.nodes != NULL:
                free(self.nodes)
            if self.indices != NULL:
                free(self.indices)
            raise MemoryError("Failed to allocate raw C buffers for KD-Tree.")
        
        # Populate initial index permutor buffer.
        for index in range(self.number_of_points):
            self.indices[index] = index

        # 4. Recursively build tree topology starting at root depth 0.
        self.root = self._build_tree(0, self.number_of_points, 0)

    cdef double _euclidean_distance(self, double[:] target, size_t pt_idx) noexcept nogil:
        """
        Computes squared Euclidean distance between query target vector and a dataset point.

        Parameters:
            target (double[:]): 1D memoryview holding the query coordinates.
            pt_idx (size_t): Row index of candidate dataset point.

        Returns:
            double: Calculated squared Euclidean metric.
        """
        cdef size_t dimension
        cdef double distance = 0.0
        cdef double difference = 0.0
        
        for dimension in range(self.number_of_dimensions):
            difference = target[dimension] - self.points[pt_idx, dimension]
            distance += difference * difference

        return distance

    cdef size_t _partition(self, size_t left, size_t right, size_t dimension) noexcept nogil:
        """
        Lomuto partitioning scheme operating over spatial index array along target dimension.

        Parameters:
            left (size_t): Inclusive left bound index.
            right (size_t): Inclusive right bound index (pivot location).
            dimension (size_t): Axis dimension evaluated for ordering.

        Returns:
            size_t: Final sorted position index of the pivot element.
        """
        cdef size_t index
        cdef size_t store_index = left

        cdef double pivot_value = self.points[self.indices[right], dimension]

        for index in range(left, right):
            if self.points[self.indices[index], dimension] < pivot_value:
                self.indices[store_index], self.indices[index] = self.indices[index], self.indices[store_index]
                store_index += 1
                
        self.indices[store_index], self.indices[right] = self.indices[right], self.indices[store_index]

        return store_index

    cdef void _quickselect(self, size_t left, size_t right, size_t k, size_t dimension) noexcept nogil:
        """
        In-place selection algorithm finding target k-th median position in O(N) average time.

        Parameters:
            left (size_t): Inclusive left subarray boundary.
            right (size_t): Inclusive right subarray boundary.
            k (size_t): Target median element index to isolate.
            dimension (size_t): Axis dimension evaluated for spatial ordering.
        """
        cdef size_t pivot

        while left < right:
            pivot = self._partition(left, right, dimension)

            if pivot == k:
                return
            elif pivot < k:
                left = pivot + 1
            else:
                if pivot == 0:
                    break
                right = pivot - 1

    cdef size_t _build_tree(self, size_t start, size_t end, size_t depth) noexcept nogil:
        """
        Recursively constructs spatial subtrees by partitioning along median split dimensions.

        Parameters:
            start (size_t): Starting sample offset index.
            end (size_t): Ending sample offset boundary index.
            depth (size_t): Current recursion depth level.

        Returns:
            size_t: Node array memory location index (<size_t>-1 if empty range).
        """
        if start >= end:
            return <size_t>-1

        cdef size_t split_dimension = depth % self.number_of_dimensions
        cdef size_t middle = start + (end - start) // 2

        self._quickselect(start, end - 1, middle, split_dimension)

        cdef size_t current_index = self.count
        self.count += 1

        self.nodes[current_index].index = self.indices[middle]
        self.nodes[current_index].split_dimension = split_dimension
        self.nodes[current_index].left_child = self._build_tree(start, middle, depth + 1)
        self.nodes[current_index].right_child = self._build_tree(middle + 1, end, depth + 1)

        return current_index

    cdef void _search_nearest(self, size_t index, double[:] target, Neighbor* best) noexcept nogil:
        """
        Traverses tree branches and prunes unpromising search spaces via hypersphere bounding.

        Parameters:
            index (size_t): Current node array pointer index.
            target (double[:]): 1D target point coordinate memoryview.
            best (Neighbor*): Pointer to mutable struct holding nearest candidate state.
        """
        if index == <size_t>-1:
            return

        cdef Node* current = &self.nodes[index]
        cdef double distance_squared = self._euclidean_distance(target, current.index)

        # Update candidate best match if closer distance discovered.
        if distance_squared < best.distance_squared:
            best.distance_squared = distance_squared
            best.index = current.index

        cdef size_t dimension = current.split_dimension
        cdef double difference = target[dimension] - self.points[current.index, dimension]
        
        cdef size_t near_child = current.left_child if difference <= 0 else current.right_child
        cdef size_t far_child = current.right_child if difference <= 0 else current.left_child

        # 1. Search primary promising half space first.
        self._search_nearest(near_child, target, best)

        # 2. Prune and skip far branch if hyperplane offset exceeds closest radius.
        if (difference * difference) < best.distance_squared:
            self._search_nearest(far_child, target, best)

    def nearest_neighbor(self, point):
        """
        Queries the KD-Tree structure to return the closest point index and Euclidean distance.

        Parameters:
            point (object): 1D array-like query point matching dataset dimensionality.

        Returns:
            tuple: (best_index, euclidean_distance) containing integer index and float distance.

        Raises:
            ValueError: If query is None, incorrectly shaped, or dimension mismatched.
            RuntimeError: If tree is unbuilt or search fails to locate valid point.
        """
        if point is None:
            raise ValueError("Query point cannot be None.")

        cdef double[:] target_view
        try:
            target_view = point
        except (TypeError, ValueError):
            raise ValueError("Query point must be a 1D array-like object of floating-point values.")

        if <size_t> target_view.shape[0] != self.number_of_dimensions:
            raise ValueError(
                f"Dimensionality mismatch: Expected query point of shape ({self.number_of_dimensions},), "
                f"got ({target_view.shape[0]},)."
            )

        if self.count == 0 or self.root == <size_t>-1:
            raise RuntimeError("KDTree is empty or failed to build.")

        cdef Neighbor best
        best.index = <size_t>-1
        best.distance_squared = INFINITY

        # Release Python GIL during native C lookup execution.
        with nogil:
            self._search_nearest(self.root, target_view, &best)

        if best.index == <size_t>-1:
            raise RuntimeError("Nearest neighbor search failed to locate a valid point.")

        return best.index, sqrt(best.distance_squared)

    def __dealloc__(self):
        """
        Releases raw heap allocated memory structures upon object garbage collection.
        """
        if self.nodes != NULL:
            free(self.nodes)
            self.nodes = NULL
            
        if self.indices != NULL:
            free(self.indices)
            self.indices = NULL