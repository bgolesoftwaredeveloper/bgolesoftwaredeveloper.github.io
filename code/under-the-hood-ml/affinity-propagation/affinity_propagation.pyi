import numpy
import numpy.typing as numpy_type

class AffinityPropagation:
    number_of_samples: int
    number_of_features: int
    maximum_iterations: int
    damping_factor: float
    preference_value: float

    def __init__(
        self, 
        maximum_iterations: int = 200, 
        damping_factor: float = 0.5, 
        preference_value: float = -50.0
    ) -> None: ...

    def fit(self, feature_matrix: numpy_type.NDArray[numpy.float64]) -> tuple[numpy_type.NDArray[numpy.int32], numpy_type.NDArray[numpy.int32]]: ...