// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This program demonstrates the usage and execution of the N-dimensional K-Means
//	clustering algorithm provided by the kmeans package.
//
//	The example constructs a synthetic 2D spatial dataset composed of distinct coordinate
//	clusters, configures model hyperparameters, and executes iterative expectation-maximization
//	to group the points and compute converged centroids.
//
//	The goal is to demonstrate practical applications of the K-Means structure:
//
//	  - Synthesizing vector data points across multidimensional spatial domains.
//	  - Invoking unsupervised cluster partitioning with bounds and convergence parameters.
//	  - Inspecting output structures including cluster centroids and assigned point sets.
//	  - Validating convergence outcomes and handling boundary error states.
//
// Notes:
//   - Initial centroids are randomly sampled using internal pseudo-random permutations.
//   - Input datasets and output cluster memberships are preserved without side effects.
//   - Point slices are printed with 4-decimal precision for centroids and 2-decimal for values.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/kmeans/kmeans"
)

func main() {
	// Synthesize an N-dimensional (2D) dataset featuring two distinct spatial clusters.
	dataset := []kmeans.Point{
		{1.0, 1.2},
		{1.5, 1.8},
		{1.2, 0.8},
		{2.0, 1.5},

		{9.5, 10.2},
		{10.0, 9.8},
		{10.5, 11.0},
		{9.8, 10.5},
	}

	// Define parameters for the K-Means operation.
	k := 2
	maximumIterations := 100

	// Output baseline algorithm configuration details.
	fmt.Println("Running K-means Clustering...")
	fmt.Printf("Dataset size %d points | K: %d | MaximumIterations: %d\n\n", len(dataset), k, maximumIterations)

	// Execute K-Means algorithm.
	clusters := kmeans.KMeans(dataset, k, maximumIterations)

	// Verify successful model execution and parameters.
	if clusters == nil {
		fmt.Println("Failed to execute K-Means: Invalid parameters or empty dataset.")

		return
	}

	// Iterate through calculated clusters and render centroids along with point memberships.
	for index, cluster := range clusters {
		fmt.Printf("--- Cluster %d ----\n", index+1)
		fmt.Printf("Centroid: [%.4f, %.4f]\n", cluster.Centroid[0], cluster.Centroid[1])
		fmt.Printf("Assigned Points (%d):\n", len(cluster.Points))

		// Print each point belonging to the current cluster partition.
		for _, point := range cluster.Points {
			fmt.Printf("\t-> [%.2f, %.2f]\n", point[0], point[1])
		}

		fmt.Println()
	}
}
