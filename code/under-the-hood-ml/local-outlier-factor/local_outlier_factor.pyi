import numpy
import numpy.typing as numpy_type

def calculate_euclidean_distance(
    matrix: numpy_type.NDArray[numpy.float64],
) -> numpy_type.NDArray[numpy.float64]: ...

def compute_local_outlier_factor(
    dataset: numpy_type.NDArray[numpy.float64],
    nearest_neighbors_count: int,
) -> numpy_type.NDArray[numpy.float64]: ...