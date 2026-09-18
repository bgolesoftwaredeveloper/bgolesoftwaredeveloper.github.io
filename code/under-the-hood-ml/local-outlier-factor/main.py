import numpy

from local_outlier_factor import compute_local_outlier_factor

def main():
    # Set random seed for reproducibility.
    numpy.random.seed(42)

    # Generate a dummy dataset: 100 inlier points clustered around (0, 0).
    inliers = numpy.random.normal(loc=0.0, scale=1.0, size=(100, 2))

    # Generate 5 obvious outlier points located far from the cluster.
    outliers = numpy.random.uniform(low=10.0, high=15.0, size=(5, 2))

    # Combine inliers and outliers into a single dataset.
    dataset = numpy.vstack([inliers, outliers])

    # Specify the number of nearest neighbors to evaluate.
    nearest_neighbors_count = 10

    # Compute Local Outlier Factor (LOF) scores.
    scores = compute_local_outlier_factor(dataset, nearest_neighbors_count)

    # Output results
    print("LOF Scores Summary:")
    print(f"Mean Inlier Score: {numpy.mean(scores[:100]):.4f}")
    print(f"Outlier Scores: {scores[100:]}")

if __name__ == "__main__":
    main()