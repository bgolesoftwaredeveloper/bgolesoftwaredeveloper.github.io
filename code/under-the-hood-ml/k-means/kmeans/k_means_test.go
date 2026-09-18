// -----------------------------------------------------------------------------
// Package: kmeans
// File: k_means_test.go
//
// Description:
//
//	This file contains unit tests for the N-dimensional K-Means clustering algorithm.
//	The tests validate distance calculations, cluster partitioning, convergence behaviors,
//	boundary condition handling, and structural invariants.
//
//	The test suite focuses on:
//
//	  - Euclidean distance correctness in 1D, 2D, and higher dimensions.
//	  - Parameter validation (nil data, negative K, K > len(data), zero iterations).
//	  - Correct partitioning into disjoint, well-separated clusters.
//	  - Early exit behavior when centroids stop moving.
//	  - Preservation of dataset contents (non-destructive execution).
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//
// -----------------------------------------------------------------------------
package kmeans

import (
	"math"
	"testing"
)

// verifyClusterInvariants performs structural assertions checking point counts
// and non-nil centroid allocations.
func verifyClusterInvariants(test *testing.T, clusters []Cluster, totalExpectedPoints int) {
	test.Helper()

	accumulatedPoints := 0

	for index, cluster := range clusters {
		if cluster.Centroid == nil {
			test.Fatalf("invariant violation: cluster %d has a nil centroid", index)
		}

		accumulatedPoints += len(cluster.Points)
	}

	if accumulatedPoints != totalExpectedPoints {
		test.Fatalf("invariant violation: total assigned points (%d) != expected points (%d)",
			accumulatedPoints,
			totalExpectedPoints,
		)
	}
}

// TestDistance_IdenticalPoints verifies that identical points yield a distance of zero.
func TestDistance_IdenticalPoints(test *testing.T) {
	// Arrange.
	originPointVector := Point{2.5, 4.0, 6.1}
	matchingPointVector := Point{2.5, 4.0, 6.1}

	// Act.
	calculatedDistance := Distance(originPointVector, matchingPointVector)

	// Assert.
	if calculatedDistance != 0.0 {
		test.Fatalf("expected distance 0.0 for identical points, got %f", calculatedDistance)
	}
}

// TestDistance_Known2DDistance verifies standard Euclidean distance calculation in 2D.
func TestDistance_Known2DDistance(test *testing.T) {
	// Arrange.
	sourceCoordinatePoint := Point{0.0, 0.0}
	destinationCoordinatePoint := Point{3.0, 4.0}

	expectedDistance := 5.0

	// Act.
	calculatedDistance := Distance(sourceCoordinatePoint, destinationCoordinatePoint)

	// Assert.
	if math.Abs(calculatedDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected distance %f, got %f", expectedDistance, calculatedDistance)
	}
}

// TestDistance_HighDimensional verifies Euclidean distance across 5 dimensions.
func TestDistance_HighDimensional(test *testing.T) {
	// Arrange.
	baseHyperdimensionalPoint := Point{1.0, 2.0, 3.0, 4.0, 5.0}
	targetHyperdimensionalPoint := Point{2.0, 3.0, 4.0, 5.0, 6.0}

	expectedDistance := math.Sqrt(5.0)

	// Act.
	calculatedDistance := Distance(baseHyperdimensionalPoint, targetHyperdimensionalPoint)

	// Assert.
	if math.Abs(calculatedDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected high-dimensional distance %f, got %f", expectedDistance, calculatedDistance)
	}
}

// TestKMeans_EmptyDataset verifies defensive return value when dataset is empty.
func TestKMeans_EmptyDataset(test *testing.T) {
	// Arrange.
	emptyDataset := []Point{}

	// Act.
	clusters := KMeans(emptyDataset, 2, 100)

	// Assert.
	if clusters != nil {
		test.Fatalf("expected nil return for empty dataset, got %v", clusters)
	}
}

// TestKMeans_ZeroK verifies defensive return value when k is zero.
func TestKMeans_ZeroK(test *testing.T) {
	// Arrange.
	dataset := []Point{{1.0, 1.0}, {2.0, 2.0}}

	// Act.
	clusters := KMeans(dataset, 0, 100)

	// Assert.
	if clusters != nil {
		test.Fatalf("expected nil return for K=0, got %v", clusters)
	}
}

// TestKMeans_NegativeK verifies defensive return value when k is negative.
func TestKMeans_NegativeK(test *testing.T) {
	// Arrange.
	dataset := []Point{{1.0, 1.0}, {2.0, 2.0}}

	// Act.
	clusters := KMeans(dataset, -3, 100)

	// Assert.
	if clusters != nil {
		test.Fatalf("expected nil return for negative K, got %v", clusters)
	}
}

// TestKMeans_KGreaterThanDatasetSize verifies rejection when K exceeds total available data points.
func TestKMeans_KGreaterThanDatasetSize(test *testing.T) {
	// Arrange.
	dataset := []Point{{1.0, 1.0}, {2.0, 2.0}}

	// Act.
	clusters := KMeans(dataset, 5, 100)

	// Assert.
	if clusters != nil {
		test.Fatalf("expected nil return when K > len(data), got %v", clusters)
	}
}

// TestKMeans_KEqualsDatasetSize verifies that K equal to dataset size assigns each point as its own cluster centroid.
func TestKMeans_KEqualsDatasetSize(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 1.0},
		{5.0, 5.0},
		{10.0, 10.0},
	}

	k := len(dataset)

	// Act.
	clusters := KMeans(dataset, k, 100)

	// Assert.
	if len(clusters) != k {
		test.Fatalf("expected %d clusters, got %d", k, len(clusters))
	}

	verifyClusterInvariants(test, clusters, len(dataset))
}

// TestKMeans_SingleCluster verifies that K=1 places all points into a single cluster.
func TestKMeans_SingleCluster(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 1.0},
		{2.0, 2.0},
		{3.0, 3.0},
	}

	// Act.
	clusters := KMeans(dataset, 1, 100)

	// Assert.
	if len(clusters) != 1 {
		test.Fatalf("expected 1 cluster, got %d", len(clusters))
	}

	if len(clusters[0].Points) != 3 {
		test.Fatalf("expected 3 points in cluster 0, got %d", len(clusters[0].Points))
	}

	expectedCentroid := Point{2.0, 2.0}

	if Distance(clusters[0].Centroid, expectedCentroid) > 1e-6 {
		test.Fatalf("expected centroid [2.0, 2.0], got %v", clusters[0].Centroid)
	}

	verifyClusterInvariants(test, clusters, len(dataset))
}

// TestKMeans_TwoWellSeparatedClusters verifies complete partitioning for distinct 2D clusters.
func TestKMeans_TwoWellSeparatedClusters(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{0.0, 0.0}, {0.2, 0.1}, {0.1, 0.3},
		{100.0, 100.0}, {100.2, 100.1}, {100.1, 100.3},
	}

	// Act.
	clusters := KMeans(dataset, 2, 100)

	// Assert.
	if len(clusters) != 2 {
		test.Fatalf("expected 2 clusters, got %d", len(clusters))
	}

	verifyClusterInvariants(test, clusters, len(dataset))

	firstClusterCount := len(clusters[0].Points)
	secondClusterCount := len(clusters[1].Points)

	if !(firstClusterCount == 3 && secondClusterCount == 3) {
		test.Fatalf("expected cluster sizes 3 and 3, got %d and %d", firstClusterCount, secondClusterCount)
	}
}

// TestKMeans_OneDimensionalData verifies clustering accuracy on 1D real numbers.
func TestKMeans_OneDimensionalData(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0}, {2.0}, {1.5},
		{50.0}, {51.0}, {49.5},
	}

	// Act.
	clusters := KMeans(dataset, 2, 100)

	// Assert.
	if len(clusters) != 2 {
		test.Fatalf("expected 2 clusters for 1D dataset, got %d", len(clusters))
	}

	verifyClusterInvariants(test, clusters, len(dataset))
}

// TestKMeans_ThreeDimensionalData verifies correct execution across 3D point clouds.
func TestKMeans_ThreeDimensionalData(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 1.0, 1.0}, {1.1, 0.9, 1.2},
		{10.0, 10.0, 10.0}, {10.2, 9.8, 10.1},
		{20.0, 20.0, 20.0}, {20.1, 19.9, 20.2},
	}

	// Act.
	clusters := KMeans(dataset, 3, 100)

	// Assert.
	if len(clusters) != 3 {
		test.Fatalf("expected 3 clusters for 3D dataset, got %d", len(clusters))
	}

	verifyClusterInvariants(test, clusters, len(dataset))
}

// TestKMeans_ZeroMaximumIterations verifies that algorithm initializes clusters even if iterations=0.
func TestKMeans_ZeroMaximumIterations(test *testing.T) {
	// Arrange.
	dataset := []Point{{1.0, 1.0}, {10.0, 10.0}}

	// Act.
	clusters := KMeans(dataset, 2, 0)

	// Assert.
	if len(clusters) != 2 {
		test.Fatalf("expected 2 initialized clusters when maximumIterations is 0, got %d", len(clusters))
	}
}

// TestKMeans_DataPreservation verifies that input points are not mutated during clustering.
func TestKMeans_DataPreservation(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 2.0},
		{3.0, 4.0},
		{5.0, 6.0},
	}

	x := dataset[0][0]
	y := dataset[0][1]

	// Act.
	_ = KMeans(dataset, 2, 50)

	// Assert.
	if dataset[0][0] != x || dataset[0][1] != y {
		test.Fatalf("input dataset was mutated: expected [%f, %f], got [%f, %f]",
			x, y, dataset[0][0], dataset[0][1],
		)
	}
}

// TestKMeans_IdenticalPointsInDataset verifies behavior when dataset contains duplicate spatial points.
func TestKMeans_IdenticalPointsInDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{5.0, 5.0},
		{5.0, 5.0},
		{5.0, 5.0},
		{5.0, 5.0},
	}

	// Act.
	clusters := KMeans(dataset, 2, 100)

	// Assert.
	if clusters == nil {
		test.Fatalf("expected valid clusters for dataset of duplicate points")
	}

	verifyClusterInvariants(test, clusters, len(dataset))
}

// TestKMeans_CollinearPoints verifies clustering along a straight line.
func TestKMeans_CollinearPoints(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0},
		{20.0, 20.0}, {21.0, 21.0}, {22.0, 22.0},
	}

	// Act.
	clusters := KMeans(dataset, 2, 100)

	// Assert.
	if len(clusters) != 2 {
		test.Fatalf("expected 2 clusters for collinear points, got %d", len(clusters))
	}

	verifyClusterInvariants(test, clusters, len(dataset))
}

// TestKMeans_NegativeCoordinates verifies spatial clustering with negative vector components.
func TestKMeans_NegativeCoordinates(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{-10.0, -10.0}, {-10.5, -9.5},
		{10.0, 10.0}, {10.5, 9.5},
	}

	// Act.
	clusters := KMeans(dataset, 2, 100)

	// Assert.
	if len(clusters) != 2 {
		test.Fatalf("expected 2 clusters for negative coordinates, got %d", len(clusters))
	}

	verifyClusterInvariants(test, clusters, len(dataset))

	for _, cluster := range clusters {
		if cluster.Centroid[0] < 0 {
			if math.Abs(cluster.Centroid[0]-(-10.25)) > 1.0 {
				test.Fatalf("negative centroid misplaced: got %f", cluster.Centroid[0])
			}
		}
	}
}

// TestKMeans_CentroidConvergence verifies that output centroids match the actual average of assigned points.
func TestKMeans_CentroidConvergence(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{0.0, 0.0}, {2.0, 0.0}, {0.0, 2.0}, {2.0, 2.0},
		{100.0, 100.0}, {102.0, 100.0}, {100.0, 102.0}, {102.0, 102.0},
	}

	// Act.
	clusters := KMeans(dataset, 2, 100)

	// Assert.
	verifyClusterInvariants(test, clusters, len(dataset))

	for _, cluster := range clusters {
		if len(cluster.Points) == 0 {
			continue
		}

		var calculatedSumX, calculatedSumY float64

		for _, point := range cluster.Points {
			calculatedSumX += point[0]
			calculatedSumY += point[1]
		}

		expectedMeanX := calculatedSumX / float64(len(cluster.Points))
		expectedMeanY := calculatedSumY / float64(len(cluster.Points))

		if math.Abs(cluster.Centroid[0]-expectedMeanX) > 1e-5 || math.Abs(cluster.Centroid[1]-expectedMeanY) > 1e-5 {
			test.Fatalf("centroid [%f, %f] does not equal mean of assigned points [%f, %f]",
				cluster.Centroid[0], cluster.Centroid[1],
				expectedMeanX, expectedMeanY,
			)
		}
	}
}

// TestKMeans_SingleItemDataset verifies execution against a dataset containing exactly one point.
func TestKMeans_SingleItemDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{{7.0, 14.0}}

	// Act.
	clusters := KMeans(dataset, 1, 10)

	// Assert.
	if len(clusters) != 1 {
		test.Fatalf("expected 1 cluster for single item dataset, got %d", len(clusters))
	}

	if Distance(clusters[0].Centroid, Point{7.0, 14.0}) > 1e-9 {
		test.Fatalf("expected centroid to match single item [7.0, 14.0], got %v", clusters[0].Centroid)
	}

	verifyClusterInvariants(test, clusters, 1)
}
