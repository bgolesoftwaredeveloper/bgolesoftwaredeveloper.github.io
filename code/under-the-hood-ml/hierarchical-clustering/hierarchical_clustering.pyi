import numpy
import numpy.typing as npType

from typing import Any, Optional

class HierarchicalClustering:
    number_of_clusters: int
    number_of_samples: int
    number_of_features: int

    linkage_matrix: Optional[npType.NDArray[numpy.float64]]

    def __init__(self, number_of_clusters: int = 1) -> None: ...
    def fit(self, dataset: Any) -> HierarchicalClustering: ...
    def get_linkage_matrix(self) -> npType.NDArray[numpy.float64]: ...
