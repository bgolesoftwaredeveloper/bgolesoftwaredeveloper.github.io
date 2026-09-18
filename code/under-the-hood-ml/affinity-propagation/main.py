import numpy

from affinity_propagation import AffinityPropagation

def print_section_header(title: str, console_width: int = 68) -> None:
    """Prints a styled section divider header."""
    print("\n" + "=" * console_width)
    print(f" {title.upper()} ".center(console_width, "="))
    print("=" * console_width)

def main():
    console_width = 68

    # 1. Initialize the algorithm.
    affinity = AffinityPropagation(
        maximum_iterations=100,
        damping_factor=0.5,
        preference_value=-10.0,
    )

    # 2. Create synthetic dataset with 3 distinct clusters (15 samples, 2 features).
    numpy.random.seed(42)

    first_cluster_samples = numpy.random.randn(5, 2) + numpy.array([10.0, 10.0])
    second_cluster_samples = numpy.random.randn(5, 2) + numpy.array([-10.0, -10.0])
    third_cluster_samples = numpy.random.randn(5, 2) + numpy.array([10.0, -10.0])

    data = numpy.vstack((
        first_cluster_samples, 
        second_cluster_samples, 
        third_cluster_samples
    )).astype(numpy.float64)

    # 3. Fit the model on our data.
    exemplars, labels = affinity.fit(data)

    # 4. Access and print public class attributes populated during fit.
    print_section_header("Affinity Propagation Execution Context", console_width)

    print(f"\n{'Processed Samples:':<22} {affinity.number_of_samples}")
    print(f"{'Processed Features:':<22} {affinity.number_of_features}")
    print(f"{'Maximum Iterations:':<22} {affinity.maximum_iterations}")
    print(f"{'Damping Factor:':<22} {affinity.damping_factor}")
    print(f"{'Preference Value:':<22} {affinity.preference_value}")

    # 5. Output exemplar indices representing selected cluster centers.
    print_section_header("Cluster Exemplars & Summary", console_width)

    formatted_exemplars_list = ", ".join(str(exemplar_index) for exemplar_index in exemplars)
    print(f"\n{'Exemplar Indices:':<22} [{formatted_exemplars_list}]")
    print(f"{'Total Clusters Found:':<22} {len(exemplars)}")

    # 6. Display step-by-step sample assignments with spatial coordinates.
    print_section_header("Sample Assignments & Spatial Mapping", console_width)

    print(f"{'Sample ID':<10} | {'Feature 0':<12} | {'Feature 1':<12} | {'Assigned Exemplar':<18}")
    print("-" * console_width)

    for sample_index, assigned_exemplar_index in enumerate(labels):
        feature_zero_value = data[sample_index][0]
        feature_one_value = data[sample_index][1]

        print(
            f"Sample {sample_index:<3} | "
            f"{feature_zero_value:<12.6f} | "
            f"{feature_one_value:<12.6f} | "
            f"Exemplar {assigned_exemplar_index:<9}"
        )

    print("=" * console_width + "\n")

if __name__ == "__main__":
    main()
