import numpy

from hierarchical_clustering import HierarchicalClustering

def generate_sample_dataset(number_of_samples: int = 10, number_of_features: int = 2) -> numpy.ndarray:
    """Generates a random 2D dataset for clustering validation."""
    numpy.random.seed(42)

    return numpy.random.rand(number_of_samples, number_of_features)

def print_section_header(title: str, width: int = 68) -> None:
    """Prints a styled section divider header."""
    print("\n" + "=" * width)
    print(f" {title.upper()} ".center(width, "="))
    print("=" * width)

def main():
    # 1. Configure input dimensions and console formatting options.
    console_width = 68

    number_of_samples = 8
    number_of_features = 2

    # 2. Generate a random synthetic matrix for feature data.
    dataset = generate_sample_dataset(
        number_of_samples=number_of_samples,
        number_of_features=number_of_features,
    )

    # 3. Print execution details and initial input feature samples.
    print_section_header("Hierarchical Clustering Execution Context", console_width)

    print(f"\n{'Input Samples:':<22} {number_of_samples}")
    print(f"{'Input Features:':<22} {number_of_features}")

    print("\n--- Raw Input Matrix ---")
    print(f"{'Sample ID':<12} | {'Feature 0':<15} | {'Feature 1':<15}")
    print("-" * console_width)

    for sample_index, sample_vector in enumerate(dataset):
        print(
            f"Sample {sample_index:<5} | {sample_vector[0]:<15.6f} | {sample_vector[1]:<15.6f}"
        )

    # 4. Initialize and fit the hierarchical clustering algorithm.
    requested_clusters = 2

    model = HierarchicalClustering(number_of_clusters=requested_clusters)

    model.fit(dataset)

    # 5. Output model status metadata populated during fit.
    print_section_header("Model Status & Metadata", console_width)

    print(f"{'Target Clusters:':<22} {model.number_of_clusters}")
    print(f"{'Processed Samples:':<22} {model.number_of_samples}")
    print(f"{'Processed Features:':<22} {model.number_of_features}")

    # 6. Retrieve and display step-by-step agglomerative merge records from linkage matrix.
    linkage_matrix = model.get_linkage_matrix()

    print_section_header("Agglomerative Linkage Matrix", console_width)

    print(
        f"{'Step':<6} | {'Cluster A':<11} | {'Cluster B':<11} | {'Distance':<14} | {'Size':<6}"
    )

    print("-" * console_width)

    for step_index, record in enumerate(linkage_matrix):
        cluster_first_identifier = int(record[0])
        cluster_second_identifier = int(record[1])

        merge_distance = record[2]

        accumulated_cluster_size = int(record[3])

        print(
            f"{step_index + 1:<6} | "
            f"{cluster_first_identifier:<11} | "
            f"{cluster_second_identifier:<11} | "
            f"{merge_distance:<14.6f} | "
            f"{accumulated_cluster_size:<6}"
        )

    print("=" * console_width + "\n")

if __name__ == "__main__":
    main()