// -----------------------------------------------------------------------------
// Package: isometricmapping
// File: isometric_mapping.go
//
// Description:
//
//	This file provides a clean, dependency-free implementation of the ISOMAP
//	(Isometric Feature Mapping) algorithm for non-linear dimensionality reduction.
//
//	The primary purpose of ISOMAP is to map high-dimensional data into a lower-dimensional
//	embedding while preserving the intrinsic geodesic (shortest-path) distances between all
//	pairs of points on an underlying manifold.
//
//	The algorithm operates in four sequential phases:
//
//	  1. Pairwise Distance Calculation — computes Euclidean distances between all input samples.
//	  2. k-NN Graph Construction — builds an adjacency matrix connecting each point to its k-nearest neighbors.
//	  3. Shortest Path Computation — applies Dijkstra's algorithm across all source pairs to approximate
//	     geodesic distances on the manifold.
//	  4. Classical Multidimensional Scaling (MDS) — performs Jacobi eigenvalue decomposition on double-centered
//	     geodesic squared-distance matrices to extract the low-dimensional embedding coordinates.
//
// Key Features & Design Principles:
//
//   - Manifold Preservation: Preserves non-linear geometry by computing graph-based geodesic paths.
//   - Numerical Stability: Utilizes the Jacobi rotation method for symmetric eigenvalue decomposition.
//   - Robust Input Validation: Performs thorough defensive boundary checks against non-finite values,
//     empty inputs, shape mismatches, and disconnected graph components.
//   - Self-Contained Implementation: Relies exclusively on standard library primitives (`math`, `sort`, `fmt`, `errors`).
//
// Operational Limits & Bounds:
//
//   - Graph Connectivity: Requires that the choice of `k` (neighbors) yields a fully connected graph;
//     otherwise, an error is returned due to infinite geodesic distances.
//   - Eigenvalue Availability: Requires at least `embeddingDimension` positive eigenvalues to construct
//     the requested embedding projection.
//
// Intended Use:
//
//	This package serves as a core machine learning building block for zero-dependency
//	Go applications requiring non-linear dimensionality reduction, manifold learning, or visual feature extraction.
//
// -----------------------------------------------------------------------------
package isometricmapping

import (
	"errors"
	"fmt"
	"math"
	"sort"
)

// candidate represents a candidate neighbor point with its original index and calculated distance.
type candidate struct {
	index    int
	distance float64
}

// initializeIdentityMatrix generates and returns an identity matrix of size `size x size`.
func initializeIdentityMatrix(size int) [][]float64 {
	if size <= 0 {
		return nil
	}

	matrix := make([][]float64, size)

	for rowIndex := 0; rowIndex < size; rowIndex++ {
		matrix[rowIndex] = make([]float64, size)
		matrix[rowIndex][rowIndex] = 1.0
	}

	return matrix
}

// jacobiEigenDecomposition computes the eigenvalues and eigenvectors of a real symmetric matrix
// using iterative Jacobi Givens rotations.
func jacobiEigenDecomposition(matrix [][]float64) ([]float64, [][]float64) {
	if len(matrix) == 0 {
		return nil, nil
	}

	size := len(matrix)

	if size == 1 {
		return []float64{matrix[0][0]}, initializeIdentityMatrix(1)
	}

	workingMatrix := make([][]float64, size)

	for rowIndex := 0; rowIndex < size; rowIndex++ {
		workingMatrix[rowIndex] = make([]float64, size)

		copy(workingMatrix[rowIndex], matrix[rowIndex])
	}

	eigenVectorMatrix := initializeIdentityMatrix(size)

	const maximumIterations = 200
	const convergenceTolerance = 1e-12

	// Perform iterative Jacobi rotations to reduce off-diagonal elements to zero.
	for iteration := 0; iteration < maximumIterations; iteration++ {
		pivotRowIndex := 0
		pivotColumnIndex := 1

		maximumOffDiagonalValue := 0.0

		// Identify the off-diagonal entry with the largest magnitude.
		for rowIndex := 0; rowIndex < size; rowIndex++ {
			for columnIndex := rowIndex + 1; columnIndex < size; columnIndex++ {
				absoluteValue := math.Abs(workingMatrix[rowIndex][columnIndex])

				if absoluteValue > maximumOffDiagonalValue {
					maximumOffDiagonalValue = absoluteValue

					pivotRowIndex = rowIndex
					pivotColumnIndex = columnIndex
				}
			}
		}

		// Terminate if off-diagonal entries fall below convergence threshold.
		if maximumOffDiagonalValue < convergenceTolerance {
			break
		}

		diagonalRowValue := workingMatrix[pivotRowIndex][pivotRowIndex]
		diagonalColumnValue := workingMatrix[pivotColumnIndex][pivotColumnIndex]

		offDiagonal := workingMatrix[pivotRowIndex][pivotColumnIndex]

		if math.Abs(offDiagonal) < 1e-15 {
			continue
		}

		// Compute Givens rotation parameters (cosine and sine).
		halfRotatingAngleTangentRatio := (diagonalColumnValue - diagonalRowValue) / (2.0 * offDiagonal)

		sign := 1.0

		if halfRotatingAngleTangentRatio < 0 {
			sign = -1.0
		}

		jacobiRotationHalfAngleTangent := sign / (math.Abs(halfRotatingAngleTangentRatio) +
			math.Sqrt(1.0+halfRotatingAngleTangentRatio*halfRotatingAngleTangentRatio))

		cosine := 1.0 / math.Sqrt(1.0+jacobiRotationHalfAngleTangent*jacobiRotationHalfAngleTangent)
		sine := jacobiRotationHalfAngleTangent * cosine

		// Apply rotation transformation to working matrix.
		for rowIndex := 0; rowIndex < size; rowIndex++ {
			if rowIndex == pivotRowIndex || rowIndex == pivotColumnIndex {
				continue
			}

			rowValue := workingMatrix[rowIndex][pivotRowIndex]
			columnValue := workingMatrix[rowIndex][pivotColumnIndex]

			workingMatrix[rowIndex][pivotRowIndex] = cosine*rowValue - sine*columnValue
			workingMatrix[rowIndex][pivotColumnIndex] = sine*rowValue + cosine*columnValue

			workingMatrix[pivotRowIndex][rowIndex] = workingMatrix[rowIndex][pivotRowIndex]
			workingMatrix[pivotColumnIndex][rowIndex] = workingMatrix[rowIndex][pivotColumnIndex]
		}

		workingMatrix[pivotRowIndex][pivotRowIndex] = cosine*cosine*diagonalRowValue -
			2.0*sine*cosine*offDiagonal +
			sine*sine*diagonalColumnValue

		workingMatrix[pivotColumnIndex][pivotColumnIndex] = sine*sine*diagonalRowValue +
			2.0*sine*cosine*offDiagonal +
			cosine*cosine*diagonalColumnValue

		workingMatrix[pivotRowIndex][pivotColumnIndex] = 0.0
		workingMatrix[pivotColumnIndex][pivotRowIndex] = 0.0

		// Update eigenvector basis matrix.
		for rowIndex := 0; rowIndex < size; rowIndex++ {
			previousEigenvectorRow := eigenVectorMatrix[rowIndex][pivotRowIndex]
			previousEigenvectorColumn := eigenVectorMatrix[rowIndex][pivotColumnIndex]

			eigenVectorMatrix[rowIndex][pivotRowIndex] = cosine*previousEigenvectorRow - sine*previousEigenvectorColumn
			eigenVectorMatrix[rowIndex][pivotColumnIndex] = sine*previousEigenvectorRow + cosine*previousEigenvectorColumn
		}
	}

	eigenValues := make([]float64, size)

	for diagonalIndex := 0; diagonalIndex < size; diagonalIndex++ {
		eigenValues[diagonalIndex] = workingMatrix[diagonalIndex][diagonalIndex]
	}

	// Normalize eigenvector columns to unit norm.
	for column := 0; column < size; column++ {
		normal := 0.0

		for rowIndex := 0; rowIndex < size; rowIndex++ {
			value := eigenVectorMatrix[rowIndex][column]
			normal += value * value
		}

		normal = math.Sqrt(normal)

		if normal > 0.0 {
			for row := 0; row < size; row++ {
				eigenVectorMatrix[row][column] /= normal
			}
		}
	}

	return eigenValues, eigenVectorMatrix
}

// classicalMdsFromDistances applies Classical Multidimensional Scaling (MDS) on a pairwise
// distance matrix to extract low-dimensional target coordinates.
func classicalMdsFromDistances(matrix [][]float64, count int, embeddingDimension int) ([][]float64, error) {
	if len(matrix) == 0 {
		return nil, errors.New("distance matrix cannot be nil or empty")
	}

	if count <= 0 || count > len(matrix) {
		return nil, errors.New("count is out of valid bounds")
	}

	if embeddingDimension <= 0 || embeddingDimension > count {
		return nil, errors.New("embedding dimension is out of valid bounds")
	}

	// Compute squared pairwise distances.
	distances := make([][]float64, count)

	for rowIndex := 0; rowIndex < count; rowIndex++ {
		distances[rowIndex] = make([]float64, count)

		for columnIndex := 0; columnIndex < count; columnIndex++ {
			distance := matrix[rowIndex][columnIndex]
			distances[rowIndex][columnIndex] = distance * distance
		}
	}

	// Calculate row, column, and global means for double-centering.
	rowMeans := make([]float64, count)
	columnMeans := make([]float64, count)
	grandMean := 0.0

	for rowIndex := 0; rowIndex < count; rowIndex++ {
		sum := 0.0

		for columnIndex := 0; columnIndex < count; columnIndex++ {
			sum += distances[rowIndex][columnIndex]
		}

		rowMeans[rowIndex] = sum / float64(count)
		grandMean += rowMeans[rowIndex]
	}

	grandMean /= float64(count)

	for columnIndex := 0; columnIndex < count; columnIndex++ {
		sum := 0.0

		for rowIndex := 0; rowIndex < count; rowIndex++ {
			sum += distances[rowIndex][columnIndex]
		}

		columnMeans[columnIndex] = sum / float64(count)
	}

	// Perform double-centering to construct the centered Gram matrix.
	gramMatrix := make([][]float64, count)

	for rowIndex := 0; rowIndex < count; rowIndex++ {
		gramMatrix[rowIndex] = make([]float64, count)

		for columnIndex := 0; columnIndex < count; columnIndex++ {
			gramMatrix[rowIndex][columnIndex] = -0.5 * (distances[rowIndex][columnIndex] -
				rowMeans[rowIndex] -
				columnMeans[columnIndex] +
				grandMean)
		}
	}

	// Extract spectrum via Jacobi eigenvalue decomposition.
	values, vectors := jacobiEigenDecomposition(gramMatrix)

	if values == nil || vectors == nil {
		return nil, errors.New("eigenvalue decomposition failed due to invalid matrix")
	}

	// Sort eigenvalue indices in descending order.
	indices := make([]int, len(values))

	for index := range indices {
		indices[index] = index
	}

	sort.Slice(indices, func(leftIndex, rightIndex int) bool {
		return values[indices[leftIndex]] > values[indices[rightIndex]]
	})

	embedding := make([][]float64, count)

	for rowIndex := 0; rowIndex < count; rowIndex++ {
		embedding[rowIndex] = make([]float64, embeddingDimension)
	}

	component := 0

	// Project samples onto leading eigenvectors scaled by positive singular values.
	for _, eigenIndex := range indices {
		eigenValue := values[eigenIndex]

		if eigenValue <= 0.0 {
			continue
		}

		scale := math.Sqrt(eigenValue)

		for sampleIndex := 0; sampleIndex < count; sampleIndex++ {
			embedding[sampleIndex][component] = vectors[sampleIndex][eigenIndex] * scale
		}

		component++

		if component == embeddingDimension {
			break
		}
	}

	if component < embeddingDimension {
		return nil, errors.New("not enough positive eigenvalues to construct the requested embedding")
	}

	return embedding, nil
}

// computePairwiseEuclideanDistance builds an N x N symmetric matrix containing Euclidean distances
// between all sample vectors.
func computePairwiseEuclideanDistance(matrix [][]float64, count int, featureCount int) [][]float64 {
	length := len(matrix)

	if length == 0 || count <= 0 || count > length || featureCount <= 0 {
		return nil
	}

	distances := make([][]float64, count)

	for rowIndex := 0; rowIndex < count; rowIndex++ {
		distances[rowIndex] = make([]float64, count)
	}

	for index := 0; index < count; index++ {
		for next := index + 1; next < count; next++ {
			squareDifference := 0.0

			for featureIndex := 0; featureIndex < featureCount; featureIndex++ {
				difference := matrix[index][featureIndex] - matrix[next][featureIndex]

				squareDifference += difference * difference
			}

			distance := math.Sqrt(squareDifference)

			distances[index][next] = distance
			distances[next][index] = distance
		}
	}

	return distances
}

// dijkstraSingleSource evaluates single-source shortest paths from a given source node across
// an weighted adjacency graph.
func dijkstraSingleSource(adjacency [][]float64, count int, index int) []float64 {
	length := len(adjacency)

	if length == 0 || count <= 0 || count > length {
		return nil
	}

	distances := make([]float64, count)
	visited := make([]bool, count)

	for sampleIndex := 0; sampleIndex < count; sampleIndex++ {
		distances[sampleIndex] = math.Inf(1)
	}

	distances[index] = 0.0

	for iteration := 0; iteration < count; iteration++ {
		closestUnvisitedIndex := -1
		smallestDistance := math.Inf(1)

		// Select the unvisited node with minimum tentative distance.
		for candidateIndex := 0; candidateIndex < count; candidateIndex++ {
			if !visited[candidateIndex] && distances[candidateIndex] < smallestDistance {
				smallestDistance = distances[candidateIndex]
				closestUnvisitedIndex = candidateIndex
			}
		}

		if closestUnvisitedIndex == -1 {
			break
		}

		visited[closestUnvisitedIndex] = true

		// Relax outgoing neighbor edges.
		for neighborIndex := 0; neighborIndex < count; neighborIndex++ {
			edgeWeight := adjacency[closestUnvisitedIndex][neighborIndex]

			if math.IsNaN(edgeWeight) || math.IsInf(edgeWeight, 1) {
				continue
			}

			candidateDistance := distances[closestUnvisitedIndex] + edgeWeight

			if candidateDistance < distances[neighborIndex] {
				distances[neighborIndex] = candidateDistance
			}
		}
	}

	return distances
}

// computeAllPairsGeodesicDistances constructs an all-pairs shortest path distance matrix
// using repeated single-source Dijkstra executions.
func computeAllPairsGeodesicDistances(adjacency [][]float64, count int) ([][]float64, error) {
	length := len(adjacency)

	if length == 0 {
		return nil, errors.New("adjacency matrix cannot be nil or empty")
	}

	if count <= 0 || count > length {
		return nil, errors.New("count is out of valid bounds")
	}

	for index := 0; index < count; index++ {
		if adjacency[index] == nil || len(adjacency[index]) < count {
			return nil, fmt.Errorf("failed to compute single source Dijkstra from index %d", index)
		}
	}

	distances := make([][]float64, count)

	// Compute shortest paths from each sample point as source.
	for sourceIndex := 0; sourceIndex < count; sourceIndex++ {
		distancesFromSource := dijkstraSingleSource(adjacency, count, sourceIndex)
		distances[sourceIndex] = distancesFromSource
	}

	// Verify complete graph connectivity.
	for rowIndex := 0; rowIndex < count; rowIndex++ {
		for columnIndex := 0; columnIndex < count; columnIndex++ {
			if math.IsInf(distances[rowIndex][columnIndex], 1) {
				return nil, errors.New("the k-nearest-neighbor graph is disconnected")
			}
		}
	}

	return distances, nil
}

// buildKNearestNeighborGraph creates a weighted neighborhood graph connecting each point
// to its k-nearest neighbors.
func buildKNearestNeighborGraph(matrix [][]float64, count int, numberOfNeighbors int) [][]float64 {
	length := len(matrix)

	if length == 0 || count <= 0 || count > length || numberOfNeighbors < 1 {
		return nil
	}

	if numberOfNeighbors > count-1 {
		numberOfNeighbors = count - 1
	}

	for index := 0; index < count; index++ {
		if len(matrix[index]) < count {
			return nil
		}
	}

	adjacency := make([][]float64, count)

	// Initialize matrix with zeros on diagonal and positive infinity elsewhere.
	for rowIndex := 0; rowIndex < count; rowIndex++ {
		adjacency[rowIndex] = make([]float64, count)

		for columnIndex := 0; columnIndex < count; columnIndex++ {
			if rowIndex == columnIndex {
				adjacency[rowIndex][columnIndex] = 0.0
			} else {
				adjacency[rowIndex][columnIndex] = math.Inf(1)
			}
		}
	}

	// Determine nearest neighbors and set symmetric graph edges.
	for rowIndex := 0; rowIndex < count; rowIndex++ {
		neighborCandidates := make([]candidate, 0, count-1)

		for columnIndex := 0; columnIndex < count; columnIndex++ {
			if columnIndex == rowIndex {
				continue
			}

			neighborCandidates = append(neighborCandidates, candidate{
				index:    columnIndex,
				distance: matrix[rowIndex][columnIndex],
			})
		}

		sort.Slice(neighborCandidates, func(compare, against int) bool {
			return neighborCandidates[compare].distance < neighborCandidates[against].distance
		})

		for rankIndex := 0; rankIndex < numberOfNeighbors; rankIndex++ {
			neighborIndex := neighborCandidates[rankIndex].index
			neighborDistance := neighborCandidates[rankIndex].distance

			adjacency[rowIndex][neighborIndex] = neighborDistance
			adjacency[neighborIndex][rowIndex] = math.Min(adjacency[neighborIndex][rowIndex], neighborDistance)
		}
	}

	return adjacency
}

// Fit executes the ISOMAP algorithm on a high-dimensional matrix, embedding data samples into
// a target coordinate space while preserving manifold geodesic distances.
func Fit(matrix [][]float64, numberOfNeighbors int, embeddingDimension int) ([][]float64, error) {
	numberOfSamples := len(matrix)

	if numberOfSamples == 0 {
		return nil, errors.New("input matrix cannot be empty")
	}

	if numberOfNeighbors < 1 || numberOfNeighbors >= numberOfSamples {
		return nil, errors.New("neighborCount out of valid range")
	}

	if embeddingDimension < 1 || embeddingDimension >= numberOfSamples {
		return nil, errors.New("embeddingDimension out of valid range")
	}

	if len(matrix[0]) == 0 {
		return nil, errors.New("input matrix rows cannot be empty")
	}

	featureCount := len(matrix[0])

	for index := 1; index < numberOfSamples; index++ {
		if len(matrix[index]) != featureCount {
			return nil, fmt.Errorf("row at index %d has inconsistent feature length", index)
		}
	}

	// Step 1: Compute pairwise Euclidean distances in input feature space.
	euclideanDistances := computePairwiseEuclideanDistance(matrix, numberOfSamples, featureCount)

	if len(euclideanDistances) == 0 {
		return nil, errors.New("failed to compute pairwise euclidean distances due to invalid inputs")
	}

	// Step 2: Construct k-nearest-neighbor neighborhood graph.
	adjacencyMatrix := buildKNearestNeighborGraph(euclideanDistances, numberOfSamples, numberOfNeighbors)

	if len(adjacencyMatrix) == 0 {
		return nil, errors.New("failed to build k-nearest-neighbor graph due to invalid inputs")
	}

	// Step 3: Compute all-pairs shortest paths over neighborhood graph to model geodesic distance.
	geodesicDistances, err := computeAllPairsGeodesicDistances(adjacencyMatrix, numberOfSamples)

	if err != nil {
		return nil, err
	}

	// Step 4: Apply Classical Multidimensional Scaling to extract coordinate embedding.
	return classicalMdsFromDistances(geodesicDistances, numberOfSamples, embeddingDimension)
}
