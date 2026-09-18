"""
main.py

An execution harness designed to validate and test the manual Fuzzy C-Means
soft clustering engine implementation.

This script verifies multidimensional cluster fitting pipelines, ensures spatial
centroid convergence, evaluates fuzzy partition quality metrics, and tests soft
membership predictions against unseen spatial query coordinates.
"""
from fuzzy_c_means import FuzzyCMeans

import numpy

def synthetic_clusters(samples: int = 100, seed: int = 42) -> numpy.ndarray:
    """
    Generates synthetic 2D Gaussian clusters to simulate multidimensional observation spaces.

    Args:
        samples (int): The total number of individual points to sample per cluster blob.
        seed (int): The seed used to initialize the NumPy pseudo-random number generator.

    Returns:
        numpy.ndarray: A combined feature matrix of shape (samples * 3, 2).
    """
    generator = numpy.random.default_rng(seed)

    primary_cluster = generator.normal(loc=[3.0, 3.0], scale=0.8, size=(samples, 2))
    secondary_cluster = generator.normal(loc=[-3.0, -3.0], scale=0.8, size=(samples, 2))
    tertiary_cluster = generator.normal(loc=[3.0, -3.0], scale=0.8, size=(samples, 2))

    return numpy.vstack([primary_cluster, secondary_cluster, tertiary_cluster])

def main():
    print("=" * 60)
    print("FUZZY C-MEANS CLUSTERING ENGINE DEMONSTRATION")
    print("=" * 60)

    # 1. Synthesize multidimensional dataset.
    training_features = synthetic_clusters(samples=100, seed=42)

    print(f"\n[+] Generated training matrix of shape: {training_features.shape}")

    # 2. Instantiate and fit Fuzzy C-Means model.
    model = FuzzyCMeans(
        number_of_clusters=3,
        fuzziness_degree=2.0,
        maximum_iterations=300,
        convergence_tolerance=1e-5,
    )

    print("\n[+] Fitting model parameters...")

    model.fit(training_features, state=42)

    print(f"\t-> Convergence achieved in {model.count} iterations.")

    # 3. Output learned cluster centroids.
    print("\n[+] Calculated Cluster Centroids:")
    for cluster_index, centroid in enumerate(model.centers):
        print(f"\tCluster {cluster_index + 1}: [{centroid[0]:.4f}, {centroid[1]:.4f}]")

    # 4. Evaluate quality metrics.
    fuzzy_coefficient = model.compute_fuzzy_coefficient()
    entropy = model.compute_entropy()

    print("\n[+] Partition Quality Metrics:")
    print(f"\t-> Fuzzy Partition Coefficient (FPC): {fuzzy_coefficient:.4f} (Higher is better, max 1.0)")
    print(f"\t-> Partition Entropy (PE): {entropy:.4f} (Lower is better, min 0.0)")

    # 5. Predict soft memberships for new unseen coordinates.
    unseen_observations = numpy.array(
        [
            [3.1, 2.9],
            [-2.8, -3.2],
            [0.0, 0.0],
        ]
    )

    print("\n[+] Evaluating Unseen Observations via predict():")

    predicted_memberships = model.predict(unseen_observations)

    for sample_index, sample_coordinates in enumerate(unseen_observations):
        memberships = predicted_memberships[:, sample_index]
        formatted_memberships = ", ".join([f"C{index+1}: {degree*100:.1f}%" for index, degree in enumerate(memberships)])

        print(f"\tPoint [{sample_coordinates[0]:4.1f}, {sample_coordinates[1]:4.1f}] => {formatted_memberships}")

    print("\n" + "=" * 60)

if __name__ == "__main__":
    main()