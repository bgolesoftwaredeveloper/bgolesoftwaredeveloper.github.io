// -----------------------------------------------------------------------------
// Package: kmeans
// File: k_means.go
//
// Description:
//
//	This file provides a clean, dependency-free implementation of the K-Means
//	unsupervised clustering algorithm for N-dimensional vector data.
//
//	The primary purpose of this algorithm is to partition a set of spatial points
//	into K distinct clusters, where each point belongs to the cluster with the
//	nearest mean (centroid), serving as a fundamental primitive for data quantization,
//	pattern recognition, and feature grouping.
//
//	The algorithm operates iteratively using standard Expectation-Maximization (EM) steps:
//
//	  1. Initialization — selects K unique initial centroids using a uniform random permutation.
//	  2. Assignment (Expectation) — assigns each data point to its closest centroid via Euclidean distance.
//	  3. Update (Maximization) — recalculates cluster centroids based on the mean of assigned points.
//
// Key Features & Design Principles:
//
//   - Dimension-Agnostic: Point abstractions support arbitrary N-dimensional real-valued spaces.
//   - Self-Contained Math: Implements Euclidean distance metrics using standard library primitives.
//   - Early Convergence: Monitors centroid drift (epsilon threshold) to exit loop execution early.
//   - Non-Destructive Allocations: Preserves original input vector slices without in-place modification.
//
// Operational Limits & Bounds:
//
//   - Convergence Criteria: Stops when all centroid position shifts fall below 1e-6.
//   - Bound Invariant: K must be positive and strictly less than or equal to the total data size.
//
// Intended Use:
//
//	This package is intended as a core machine learning building block for zero-dependency
//	Go applications requiring spatial clustering, vector quantization, or data grouping.
//
// -----------------------------------------------------------------------------
package kmeans

import (
	"math"
	"math/rand"
)

// Point represents an N-dimensional numerical vector.
type Point []float64

// Cluster encapsulates a central mean vector (Centroid) alongside
// the collection of spatial points currently assigned to it.
type Cluster struct {
	Centroid Point
	Points   []Point
}

// Distance computes the Euclidean distance (L2 norm) between two N-dimensional points.
func Distance(compare, against Point) float64 {
	var sum float64

	for index := range compare {
		difference := compare[index] - against[index]

		sum += difference * difference
	}

	return math.Sqrt(sum)
}

// KMeans partitions an N-dimensional dataset into k distinct clusters over a maximum
// number of iterations using Lloyd's algorithm. Returns nil if parameters are invalid.
func KMeans(data []Point, k int, maximumIterations int) []Cluster {
	// Guard against empty datasets, non-positive K values, or requesting more clusters than available data points.
	if len(data) == 0 || k <= 0 || k > len(data) {
		return nil
	}

	dimensions := len(data[0])

	clusters := make([]Cluster, k)

	// Generate a random permutation of indices to seed distinct initial centroids.
	permutation := rand.Perm(len(data))

	// Initialize K clusters with distinct randomly selected points from the dataset.
	for index := 0; index < k; index++ {
		clusters[index] = Cluster{
			Centroid: make(Point, dimensions),
		}

		copy(clusters[index].Centroid, data[permutation[index]])
	}

	for iteration := 0; iteration < maximumIterations; iteration++ {
		// Reset assigned point sets for all clusters prior to reassignment step.
		for index := range clusters {
			clusters[index].Points = nil
		}

		// Assignment Step: Associate each point with its nearest centroid.
		for _, point := range data {
			minimumDistance := math.MaxFloat64
			bestClusterIndex := 0

			for clusterIndex, cluster := range clusters {
				distance := Distance(point, cluster.Centroid)

				if distance < minimumDistance {
					minimumDistance = distance
					bestClusterIndex = clusterIndex
				}
			}

			clusters[bestClusterIndex].Points = append(clusters[bestClusterIndex].Points, point)
		}

		shifted := false

		// Update Step: Recalculate centroids as the component-wise mean of assigned points.
		for index := range clusters {
			pointCount := len(clusters[index].Points)

			// Skip recalculation if a cluster contains no assigned points to prevent division by zero.
			if pointCount == 0 {
				continue
			}

			newCentroid := make(Point, dimensions)

			// Accumulate dimension sums across all points assigned to this cluster.
			for _, point := range clusters[index].Points {
				for dimension := 0; dimension < dimensions; dimension++ {
					newCentroid[dimension] += point[dimension]
				}
			}

			// Compute average position for each dimension.
			for dimension := 0; dimension < dimensions; dimension++ {
				newCentroid[dimension] /= float64(pointCount)
			}

			// Convergence check: Verify whether centroid moved beyond tolerance threshold.
			if Distance(clusters[index].Centroid, newCentroid) > 1e-6 {
				shifted = true
				clusters[index].Centroid = newCentroid
			}
		}

		// Halt execution early if no centroids moved significantly during this iteration.
		if !shifted {
			break
		}
	}

	return clusters
}
