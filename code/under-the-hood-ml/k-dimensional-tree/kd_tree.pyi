import numpy
import numpy.typing as numpy_type

from typing import Tuple

class KDTree:
    def __init__(self, points: numpy_type.NDArray[numpy.float64]) -> None: ...
    def nearest_neighbor(self, point: numpy_type.NDArray[numpy.float64]) -> Tuple[int, float]: ...
