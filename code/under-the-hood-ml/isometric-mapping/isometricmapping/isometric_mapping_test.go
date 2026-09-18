// -----------------------------------------------------------------------------
// Package: isometricmapping
// File: isometric_mapping_test.go
//
// Description:
//
//	This file contains 30 unit tests for the Isometric Mapping (ISOMAP) algorithm
//	implementation. The test suite validates matrix initialization, numerical algorithms
//	(Jacobi eigenvalue decomposition, Classical MDS), distance metrics (Euclidean, Dijkstra
//	shortest path geodesic), k-NN graph construction, and high-level manifold learning.
//
//	The test suite focuses on:
//
//	  - Basic matrix setup, identity generation, and invariant verification.
//	  - Defensive validation against invalid parameters, nil inputs, bounds violations, and disconnected graphs.
//	  - Numerical accuracy for Jacobi eigenvalue decomposition and orthogonal eigenvector unit normalization.
//	  - Classical Multidimensional Scaling (MDS) double-centering and spectral projection.
//	  - Single-source Dijkstra shortest path resolution and k-NN graph symmetry enforcing.
//	  - High-level ISOMAP embedding on standard synthetic manifolds (S-Curve, Swiss Roll, planar grids).
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//
// -----------------------------------------------------------------------------
package isometricmapping

import (
	"math"
	"sort"
	"testing"
)

// verifyMatrixShapeInvariants validates non-nil properties and dimension consistency of a matrix.
func verifyMatrixShapeInvariants(test *testing.T, matrix [][]float64, expectedRows int, expectedColumns int) {
	test.Helper()

	if expectedRows == 0 {
		if matrix != nil {
			test.Fatalf("invariant violation: expected nil matrix for 0 rows, got non-nil slice of length %d", len(matrix))
		}

		return
	}

	if matrix == nil {
		test.Fatalf("invariant violation: matrix instance is nil")
	}

	if len(matrix) != expectedRows {
		test.Fatalf("invariant violation: row count mismatch (expected %d, got %d)", expectedRows, len(matrix))
	}

	for rowIndex := 0; rowIndex < expectedRows; rowIndex++ {
		if matrix[rowIndex] == nil {
			test.Fatalf("invariant violation: row at index %d is nil", rowIndex)
		}

		if len(matrix[rowIndex]) != expectedColumns {
			test.Fatalf("invariant violation: column count mismatch at row %d (expected %d, got %d)",
				rowIndex, expectedColumns, len(matrix[rowIndex]))
		}
	}
}

// TestInitializeIdentityMatrix_StandardSize verifies standard identity matrix creation.
func TestInitializeIdentityMatrix_StandardSize(test *testing.T) {
	// Arrange.
	size := 3

	// Act.
	identityMatrix := initializeIdentityMatrix(size)

	// Assert.
	verifyMatrixShapeInvariants(test, identityMatrix, 3, 3)

	for rowIndex := 0; rowIndex < size; rowIndex++ {
		for columnIndex := 0; columnIndex < size; columnIndex++ {
			if rowIndex == columnIndex {
				if identityMatrix[rowIndex][columnIndex] != 1.0 {
					test.Fatalf("expected 1.0 on diagonal at (%d, %d), got %f",
						rowIndex, columnIndex, identityMatrix[rowIndex][columnIndex])
				}
			} else {
				if identityMatrix[rowIndex][columnIndex] != 0.0 {
					test.Fatalf("expected 0.0 off diagonal at (%d, %d), got %f",
						rowIndex, columnIndex, identityMatrix[rowIndex][columnIndex])
				}
			}
		}
	}
}

// TestInitializeIdentityMatrix_ZeroAndNegativeSize verifies defensive nil return on non-positive sizes.
func TestInitializeIdentityMatrix_ZeroAndNegativeSize(test *testing.T) {
	// Arrange & Act.
	zeroMatrix := initializeIdentityMatrix(0)
	negativeMatrix := initializeIdentityMatrix(-5)

	// Assert.
	if zeroMatrix != nil {
		test.Fatalf("expected nil for size 0, got %v", zeroMatrix)
	}

	if negativeMatrix != nil {
		test.Fatalf("expected nil for negative size, got %v", negativeMatrix)
	}
}

// TestJacobiEigenDecomposition_EmptyMatrix verifies defensive nil returns for empty input.
func TestJacobiEigenDecomposition_EmptyMatrix(test *testing.T) {
	// Arrange.
	emptyMatrix := [][]float64{}

	// Act.
	eigenValues, eigenVectors := jacobiEigenDecomposition(emptyMatrix)

	// Assert.
	if eigenValues != nil || eigenVectors != nil {
		test.Fatalf("expected nil values and vectors for empty matrix, got %v, %v", eigenValues, eigenVectors)
	}
}

// TestJacobiEigenDecomposition_SingleElementMatrix verifies trivial 1x1 decomposition.
func TestJacobiEigenDecomposition_SingleElementMatrix(test *testing.T) {
	// Arrange.
	singleMatrix := [][]float64{{42.0}}

	// Act.
	eigenValues, eigenVectors := jacobiEigenDecomposition(singleMatrix)

	// Assert.
	if len(eigenValues) != 1 || eigenValues[0] != 42.0 {
		test.Fatalf("expected eigenvalue [42.0], got %v", eigenValues)
	}

	verifyMatrixShapeInvariants(test, eigenVectors, 1, 1)

	if eigenVectors[0][0] != 1.0 {
		test.Fatalf("expected eigenvector [[1.0]], got %v", eigenVectors)
	}
}

// TestJacobiEigenDecomposition_DiagonalMatrix verifies extraction of exact diagonal values.
func TestJacobiEigenDecomposition_DiagonalMatrix(test *testing.T) {
	// Arrange.
	diagonalMatrix := [][]float64{
		{5.0, 0.0, 0.0},
		{0.0, 2.0, 0.0},
		{0.0, 0.0, 9.0},
	}

	// Act.
	eigenValues, eigenVectors := jacobiEigenDecomposition(diagonalMatrix)

	// Assert.
	verifyMatrixShapeInvariants(test, eigenVectors, 3, 3)

	sort.Float64s(eigenValues)

	expectedValues := []float64{2.0, 5.0, 9.0}

	for index := 0; index < 3; index++ {
		if math.Abs(eigenValues[index]-expectedValues[index]) > 1e-6 {
			test.Fatalf("expected eigenvalue %f at index %d, got %f", expectedValues[index], index, eigenValues[index])
		}
	}
}

// TestJacobiEigenDecomposition_SymmetricMatrix verifies decomposition accuracy on a symmetric 2x2 matrix.
func TestJacobiEigenDecomposition_SymmetricMatrix(test *testing.T) {
	// Arrange.
	symmetricMatrix := [][]float64{
		{2.0, 1.0},
		{1.0, 2.0},
	}

	// Act.
	eigenValues, eigenVectors := jacobiEigenDecomposition(symmetricMatrix)

	// Assert.
	verifyMatrixShapeInvariants(test, eigenVectors, 2, 2)

	sort.Float64s(eigenValues)

	expectedValues := []float64{1.0, 3.0}

	for index := 0; index < 2; index++ {
		if math.Abs(eigenValues[index]-expectedValues[index]) > 1e-6 {
			test.Fatalf("expected eigenvalue %f, got %f", expectedValues[index], eigenValues[index])
		}
	}
}

// TestJacobiEigenDecomposition_EigenvectorNormality verifies unit norm for resulting eigenvectors.
func TestJacobiEigenDecomposition_EigenvectorNormality(test *testing.T) {
	// Arrange.
	symmetricMatrix := [][]float64{
		{4.0, 1.0, 2.0},
		{1.0, 5.0, 3.0},
		{2.0, 3.0, 6.0},
	}

	// Act.
	_, eigenVectors := jacobiEigenDecomposition(symmetricMatrix)

	// Assert.
	for columnIndex := 0; columnIndex < 3; columnIndex++ {
		squaredSum := 0.0

		for rowIndex := 0; rowIndex < 3; rowIndex++ {
			value := eigenVectors[rowIndex][columnIndex]
			squaredSum += value * value
		}

		norm := math.Sqrt(squaredSum)

		if math.Abs(norm-1.0) > 1e-6 {
			test.Fatalf("expected column %d to have unit norm, got %f", columnIndex, norm)
		}
	}
}

// TestComputePairwiseEuclideanDistance_Validation verifies bounds and zero parameter handling.
func TestComputePairwiseEuclideanDistance_Validation(test *testing.T) {
	// Arrange.
	validMatrix := [][]float64{{1.0, 2.0}, {3.0, 4.0}}

	// Act & Assert.
	if computePairwiseEuclideanDistance(nil, 2, 2) != nil {
		test.Fatal("expected nil for nil input matrix")
	}

	if computePairwiseEuclideanDistance(validMatrix, 0, 2) != nil {
		test.Fatal("expected nil for sample count 0")
	}

	if computePairwiseEuclideanDistance(validMatrix, 2, 0) != nil {
		test.Fatal("expected nil for feature count 0")
	}

	if computePairwiseEuclideanDistance(validMatrix, 5, 2) != nil {
		test.Fatal("expected nil for sample count out of matrix length bounds")
	}
}

// TestComputePairwiseEuclideanDistance_KnownDistances verifies accurate distance calculations between 2D points.
func TestComputePairwiseEuclideanDistance_KnownDistances(test *testing.T) {
	// Arrange.
	points := [][]float64{
		{0.0, 0.0},
		{3.0, 0.0},
		{0.0, 4.0},
	}

	// Act.
	distances := computePairwiseEuclideanDistance(points, 3, 2)

	// Assert.
	verifyMatrixShapeInvariants(test, distances, 3, 3)

	if math.Abs(distances[0][1]-3.0) > 1e-6 {
		test.Fatalf("expected distance 3.0 between point 0 and 1, got %f", distances[0][1])
	}

	if math.Abs(distances[0][2]-4.0) > 1e-6 {
		test.Fatalf("expected distance 4.0 between point 0 and 2, got %f", distances[0][2])
	}

	if math.Abs(distances[1][2]-5.0) > 1e-6 {
		test.Fatalf("expected hypotenuse distance 5.0 between point 1 and 2, got %f", distances[1][2])
	}
}

// TestBuildKNearestNeighborGraph_Validation verifies defensive checks on neighbor graph options.
func TestBuildKNearestNeighborGraph_Validation(test *testing.T) {
	// Arrange.
	distances := [][]float64{
		{0.0, 1.0},
		{1.0, 0.0},
	}

	// Act & Assert.
	if buildKNearestNeighborGraph(nil, 2, 1) != nil {
		test.Fatal("expected nil for nil input distance matrix")
	}

	if buildKNearestNeighborGraph(distances, 2, 0) != nil {
		test.Fatal("expected nil for numberOfNeighbors < 1")
	}

	if buildKNearestNeighborGraph(distances, 0, 1) != nil {
		test.Fatal("expected nil for count = 0")
	}
}

// TestBuildKNearestNeighborGraph_SymmetryAndEdges verifies mutual and directed edge inclusion.
func TestBuildKNearestNeighborGraph_SymmetryAndEdges(test *testing.T) {
	// Arrange.
	distances := [][]float64{
		{0.0, 2.0, 10.0},
		{2.0, 0.0, 1.0},
		{10.0, 1.0, 0.0},
	}

	// Act.
	adjacency := buildKNearestNeighborGraph(distances, 3, 1)

	// Assert.
	verifyMatrixShapeInvariants(test, adjacency, 3, 3)

	if adjacency[0][1] != 2.0 || adjacency[1][0] != 2.0 {
		test.Fatalf("expected bidirectional edge between node 0 and node 1")
	}

	if !math.IsInf(adjacency[0][2], 1) {
		test.Fatalf("expected infinity for unselected distant edge (0,2), got %f", adjacency[0][2])
	}
}

// TestDijkstraSingleSource_Validation verifies bounds validation for Dijkstra single source calls.
func TestDijkstraSingleSource_Validation(test *testing.T) {
	// Arrange.
	adjacency := [][]float64{{0.0}}

	// Act & Assert.
	if dijkstraSingleSource(nil, 1, 0) != nil {
		test.Fatal("expected nil for nil adjacency input")
	}

	if dijkstraSingleSource(adjacency, 0, 0) != nil {
		test.Fatal("expected nil for count <= 0")
	}

	if dijkstraSingleSource(adjacency, 10, 0) != nil {
		test.Fatal("expected nil for count exceeding length")
	}
}

// TestDijkstraSingleSource_ShortestPaths verifies shortest path evaluation on a linear 3-node graph.
func TestDijkstraSingleSource_ShortestPaths(test *testing.T) {
	// Arrange.
	adjacency := [][]float64{
		{0.0, 2.0, math.Inf(1)},
		{2.0, 0.0, 3.0},
		{math.Inf(1), 3.0, 0.0},
	}

	// Act.
	distances := dijkstraSingleSource(adjacency, 3, 0)

	// Assert.
	if len(distances) != 3 {
		test.Fatalf("expected 3 path distance elements, got %d", len(distances))
	}

	if distances[0] != 0.0 {
		test.Fatalf("expected self distance 0.0, got %f", distances[0])
	}

	if distances[1] != 2.0 {
		test.Fatalf("expected direct neighbor distance 2.0, got %f", distances[1])
	}

	if distances[2] != 5.0 {
		test.Fatalf("expected indirect path distance 5.0 (2+3), got %f", distances[2])
	}
}

// TestComputeAllPairsGeodesicDistances_DisconnectedGraph verifies error handling on disconnected graph topologies.
func TestComputeAllPairsGeodesicDistances_DisconnectedGraph(test *testing.T) {
	// Arrange.
	disconnectedAdjacency := [][]float64{
		{0.0, math.Inf(1)},
		{math.Inf(1), 0.0},
	}

	// Act.
	distances, err := computeAllPairsGeodesicDistances(disconnectedAdjacency, 2)

	// Assert.
	if err == nil {
		test.Fatal("expected error for disconnected graph, got nil")
	}

	if distances != nil {
		test.Fatalf("expected nil distance matrix on error, got %v", distances)
	}
}

// TestComputeAllPairsGeodesicDistances_FullyConnectedGraph verifies all-pairs matrix computation on connected graphs.
func TestComputeAllPairsGeodesicDistances_FullyConnectedGraph(test *testing.T) {
	// Arrange.
	adjacency := [][]float64{
		{0.0, 1.0, math.Inf(1)},
		{1.0, 0.0, 2.0},
		{math.Inf(1), 2.0, 0.0},
	}

	// Act.
	geodesic, err := computeAllPairsGeodesicDistances(adjacency, 3)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error: %v", err)
	}

	verifyMatrixShapeInvariants(test, geodesic, 3, 3)

	if geodesic[0][2] != 3.0 {
		test.Fatalf("expected geodesic distance 3.0 between 0 and 2, got %f", geodesic[0][2])
	}
}

// TestClassicalMdsFromDistances_Validation verifies argument limits for Classical MDS.
func TestClassicalMdsFromDistances_Validation(test *testing.T) {
	// Arrange.
	distances := [][]float64{
		{0.0, 1.0},
		{1.0, 0.0},
	}

	// Act & Assert.
	if _, err := classicalMdsFromDistances(nil, 2, 1); err == nil {
		test.Fatal("expected error for nil distance matrix")
	}

	if _, err := classicalMdsFromDistances(distances, 0, 1); err == nil {
		test.Fatal("expected error for count <= 0")
	}

	if _, err := classicalMdsFromDistances(distances, 2, 0); err == nil {
		test.Fatal("expected error for embeddingDimension <= 0")
	}

	if _, err := classicalMdsFromDistances(distances, 2, 5); err == nil {
		test.Fatal("expected error for embeddingDimension > count")
	}
}

// TestClassicalMdsFromDistances_2DReconstruction verifies accurate 2D projection from exact Euclidean distances.
func TestClassicalMdsFromDistances_2DReconstruction(test *testing.T) {
	// Arrange.
	points := [][]float64{
		{0.0, 0.0},
		{1.0, 0.0},
		{0.0, 1.0},
		{1.0, 1.0},
	}

	euclidean := computePairwiseEuclideanDistance(points, 4, 2)

	// Act.
	embedding, err := classicalMdsFromDistances(euclidean, 4, 2)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected classical MDS error: %v", err)
	}

	verifyMatrixShapeInvariants(test, embedding, 4, 2)

	embeddedDistances := computePairwiseEuclideanDistance(embedding, 4, 2)

	for rowIndex := 0; rowIndex < 4; rowIndex++ {
		for columnIndex := 0; columnIndex < 4; columnIndex++ {
			originalDistance := euclidean[rowIndex][columnIndex]
			reconstructedDistance := embeddedDistances[rowIndex][columnIndex]

			if math.Abs(originalDistance-reconstructedDistance) > 1e-4 {
				test.Fatalf("distance mismatch between %d and %d: expected %f, got %f",
					rowIndex, columnIndex, originalDistance, reconstructedDistance)
			}
		}
	}
}

// TestFit_EmptyMatrix verifies error handling when calling Fit with an empty input matrix.
func TestFit_EmptyMatrix(test *testing.T) {
	// Arrange.
	emptyMatrix := [][]float64{}

	// Act.
	_, err := Fit(emptyMatrix, 2, 2)

	// Assert.
	if err == nil {
		test.Fatal("expected error for empty matrix input")
	}
}

// TestFit_InvalidNeighbors verifies error handling when neighbor count is invalid.
func TestFit_InvalidNeighbors(test *testing.T) {
	// Arrange.
	matrix := [][]float64{
		{1.0, 2.0},
		{3.0, 4.0},
		{5.0, 6.0},
	}

	// Act & Assert.
	if _, err := Fit(matrix, 0, 2); err == nil {
		test.Fatal("expected error for numberOfNeighbors < 1")
	}

	if _, err := Fit(matrix, 3, 2); err == nil {
		test.Fatal("expected error for numberOfNeighbors >= numberOfSamples")
	}
}

// TestFit_InvalidEmbeddingDimension verifies error handling when embedding dimension exceeds bounds.
func TestFit_InvalidEmbeddingDimension(test *testing.T) {
	// Arrange.
	matrix := [][]float64{
		{1.0, 2.0},
		{3.0, 4.0},
		{5.0, 6.0},
	}

	// Act & Assert.
	if _, err := Fit(matrix, 1, 0); err == nil {
		test.Fatal("expected error for embeddingDimension < 1")
	}

	if _, err := Fit(matrix, 1, 3); err == nil {
		test.Fatal("expected error for embeddingDimension >= numberOfSamples")
	}
}

// TestFit_EmptyRows verifies error handling for zero feature dimension rows.
func TestFit_EmptyRows(test *testing.T) {
	// Arrange.
	matrix := [][]float64{{}, {}}

	// Act.
	_, err := Fit(matrix, 1, 1)

	// Assert.
	if err == nil {
		test.Fatal("expected error for rows with zero features")
	}
}

// TestFit_InconsistentFeatureLengths verifies defensive detection of mismatched row dimensions.
func TestFit_InconsistentFeatureLengths(test *testing.T) {
	// Arrange.
	matrix := [][]float64{
		{1.0, 2.0, 3.0},
		{4.0, 5.0},
		{6.0, 7.0, 8.0},
	}

	// Act.
	_, err := Fit(matrix, 1, 1)

	// Assert.
	if err == nil {
		test.Fatal("expected error for inconsistent feature lengths across rows")
	}
}

// TestFit_PlanarLineEmbedding verifies dimensionality reduction on collinear 3D points.
func TestFit_PlanarLineEmbedding(test *testing.T) {
	// Arrange.
	linePoints := [][]float64{
		{0.0, 0.0, 0.0},
		{1.0, 1.0, 1.0},
		{2.0, 2.0, 2.0},
		{3.0, 3.0, 3.0},
		{4.0, 4.0, 4.0},
	}

	// Act.
	embedding, err := Fit(linePoints, 2, 1)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error fitting collinear points: %v", err)
	}

	verifyMatrixShapeInvariants(test, embedding, 5, 1)

	for index := 0; index < 4; index++ {
		if embedding[index][0] >= embedding[index+1][0] && embedding[index][0] <= embedding[index+1][0] {
			test.Fatalf("expected strictly ordered 1D projections along line manifold")
		}
	}
}

// TestFit_SCurveManifold verifies reduction of a standard 3D S-Curve manifold to 2D.
func TestFit_SCurveManifold(test *testing.T) {
	// Arrange.
	numberOfSamples := 30
	matrix := make([][]float64, numberOfSamples)

	for index := 0; index < numberOfSamples; index++ {
		progress := float64(index) / float64(numberOfSamples)
		angle := 3.0 * math.Pi * (progress - 0.5)

		xCoordinate := math.Sin(angle)
		yCoordinate := 2.0 * float64(index) / float64(numberOfSamples)
		zCoordinate := math.Cos(angle)

		matrix[index] = []float64{xCoordinate, yCoordinate, zCoordinate}
	}

	// Act.
	embedding, err := Fit(matrix, 4, 2)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error embedding S-Curve manifold: %v", err)
	}

	verifyMatrixShapeInvariants(test, embedding, numberOfSamples, 2)
}

// TestFit_CircleManifold verifies successful processing of a closed circular manifold.
func TestFit_CircleManifold(test *testing.T) {
	// Arrange.
	sampleCount := 20
	circlePoints := make([][]float64, sampleCount)

	for index := 0; index < sampleCount; index++ {
		angle := (2.0 * math.Pi * float64(index)) / float64(sampleCount)
		circlePoints[index] = []float64{math.Cos(angle), math.Sin(angle)}
	}

	// Act.
	embedding, err := Fit(circlePoints, 3, 2)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error embedding circle manifold: %v", err)
	}

	verifyMatrixShapeInvariants(test, embedding, sampleCount, 2)
}

// TestFit_HighDimensionalInput verifies ISOMAP reduction from 10D space down to 2D.
func TestFit_HighDimensionalInput(test *testing.T) {
	// Arrange.
	sampleCount := 15
	featureCount := 10
	highDimMatrix := make([][]float64, sampleCount)

	for rowIndex := 0; rowIndex < sampleCount; rowIndex++ {
		highDimMatrix[rowIndex] = make([]float64, featureCount)

		for columnIndex := 0; columnIndex < featureCount; columnIndex++ {
			highDimMatrix[rowIndex][columnIndex] = float64(rowIndex+1) * float64(columnIndex+1)
		}
	}

	// Act.
	embedding, err := Fit(highDimMatrix, 3, 2)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error fitting 10D input data: %v", err)
	}

	verifyMatrixShapeInvariants(test, embedding, sampleCount, 2)
}

// TestFit_DisconnectedGraphError verifies that Fit returns an error if k is too small to form a connected graph.
func TestFit_DisconnectedGraphError(test *testing.T) {
	// Arrange.
	clusters := [][]float64{
		{0.0, 0.0},
		{0.1, 0.1},
		{100.0, 100.0},
		{100.1, 100.1},
	}

	// Act.
	embedding, err := Fit(clusters, 1, 2)

	// Assert.
	if err == nil {
		test.Fatal("expected disconnected graph error when fitting isolated clusters with k=1")
	}

	if embedding != nil {
		test.Fatalf("expected nil embedding on graph disconnection error, got %v", embedding)
	}
}

// TestFit_IdentityTransformationDimensions verifies input-output dimension parity when embeddingDimension equals featureCount.
func TestFit_IdentityTransformationDimensions(test *testing.T) {
	// Arrange.
	points := [][]float64{
		{0.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0},
	}

	// Act.
	embedding, err := Fit(points, 2, 3)

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error fitting equal dimension embedding: %v", err)
	}

	verifyMatrixShapeInvariants(test, embedding, 4, 3)
}

// TestFit_Determinism verifies identical output matrices when running Fit repeatedly on identical inputs.
func TestFit_Determinism(test *testing.T) {
	// Arrange.
	points := [][]float64{
		{1.0, 3.0, 5.0},
		{2.0, 4.0, 6.0},
		{5.0, 1.0, 2.0},
		{3.0, 3.0, 3.0},
	}

	// Act.
	compareRun, err := Fit(points, 2, 2)
	againstRun, blunder := Fit(points, 2, 2)

	// Assert.
	if err != nil || blunder != nil {
		test.Fatalf("unexpected error during determinism runs: %v, %v", err, blunder)
	}

	for rowIndex := 0; rowIndex < 4; rowIndex++ {
		for columnIndex := 0; columnIndex < 2; columnIndex++ {
			if math.Abs(compareRun[rowIndex][columnIndex]-againstRun[rowIndex][columnIndex]) > 1e-12 {
				test.Fatalf("non-deterministic result at (%d, %d): %f vs %f",
					rowIndex, columnIndex, compareRun[rowIndex][columnIndex], againstRun[rowIndex][columnIndex])
			}
		}
	}
}
