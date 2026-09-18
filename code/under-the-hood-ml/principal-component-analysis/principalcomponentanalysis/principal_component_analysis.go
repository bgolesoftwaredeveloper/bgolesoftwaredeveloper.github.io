// -----------------------------------------------------------------------------
// Package: principalcomponentanalysis
// File: principal_component_analysis.go
//
// Description:
//
//	This file provides a production-grade, encapsulated Go implementation of the
//	Principal Component Analysis (PCA) algorithm using high-performance linear
//	algebra primitives from Gonum (`gonum.org/v1/gonum/mat`).
//
//	The primary purpose of this package is to perform linear dimensionality reduction
//	by projecting high-dimensional feature spaces onto lower-dimensional orthogonal
//	subspaces while retaining maximum variance. It computes column-wise empirical means,
//	centers the dataset around zero, and executes Singular Value Decomposition (SVD)
//	to extract the target principal component directions.
//
//	The algorithm operates in two main phases:
//
//	  1. Fitting (`Fit`) — validates input dimensions, calculates feature mean vectors,
//	     centers data, performs thin SVD decomposition to isolate right singular vectors
//	     (principal axes), and slices the top target component directions into a projection matrix.
//	  2. Transformation (`Transform`) — verifies matrix dimensional compatibility with
//	     fitted state, subtracts learned feature means from new input observations,
//	     and projects centered data onto the stored principal axes via matrix multiplication.
//
// Key Features & Design Principles:
//
//   - Encapsulated Architecture: All internal model state fields (`componentCount`,
//     `meanVector`, `projectedMatrix`, `isFitted`) remain unexported to guarantee strict state protection.
//   - Defensive Data Access: Public getters return copies of internal slices and matrices
//     to prevent callers from modifying model state post-fitting.
//   - Rigorous Dimensional Validation: Explicit validation guards against `nil` inputs,
//     empty matrices, insufficient sample sizes, and component target mismatches.
//   - SVD Numerical Stability: Utilizes thin right singular vector decomposition (`mat.SVDThinV`)
//     to ensure fast and stable extraction of principal directions.
//   - Output Formatting Utilities: Provides tabular helper functions for terminal matrix output.
//
// Operational Limits & Bounds:
//
//   - Minimum Sample Requirement: Requires at least 2 observations during fitting to compute
//     meaningful feature means and variance vectors.
//   - Memory Bounds: Dense matrix allocations scale with dataset dimensions (rows x columns).
//
// Intended Use:
//
//	This package serves as a reliable machine learning utility for dimensionality
//	reduction, feature extraction, data compression, and visual data analysis in Go projects.
//
// -----------------------------------------------------------------------------
package principalcomponentanalysis

import (
	"errors"
	"fmt"

	"gonum.org/v1/gonum/mat"
)

// PrincipalComponentAnalysis manages target component parameters, feature mean vectors,
// projection matrices, and fitted status state for linear dimensionality reduction.
type PrincipalComponentAnalysis struct {
	componentCount  int
	meanVector      []float64
	projectedMatrix *mat.Dense
	isFitted        bool
}

// validateFitDimensions checks matrix non-nilness, non-emptiness, sample minimums, and component bounds.
func (pca *PrincipalComponentAnalysis) validateFitDimensions(matrix mat.Matrix) (int, int, error) {
	// Reject empty pointers before accessing matrix dimensions.
	if matrix == nil {
		return 0, 0, errors.New("matrix cannot be nil")
	}

	sampleCount, featureCount := matrix.Dims()

	// Ensure the dataset contains non-zero dimensions.
	if sampleCount == 0 || featureCount == 0 {
		return 0, 0, errors.New("matrix must not be empty")
	}

	// Require a minimum sample size to perform variance estimations.
	if sampleCount < 2 {
		return 0, 0, errors.New("at least 2 samples are required to compute covariance and principal components")
	}

	// Guard against requesting more components than input features.
	if pca.componentCount > featureCount {
		return 0, 0, fmt.Errorf("component count (%d) exceeds available feature count (%d)",
			pca.componentCount, featureCount)
	}

	return sampleCount, featureCount, nil
}

// validateTransformDimensions checks input matrix validity and verifies feature dimension parity with fitted state.
func (pca *PrincipalComponentAnalysis) validateTransformDimensions(matrix mat.Matrix) (int, int, error) {
	// Check matrix reference before evaluation.
	if matrix == nil {
		return 0, 0, errors.New("matrix cannot be null")
	}

	sampleCount, featureCount := matrix.Dims()

	// Ensure input matrix is non-empty.
	if sampleCount == 0 {
		return 0, 0, errors.New("matrix must not be empty")
	}

	// Verify input feature count matches the fitted model mean vector length.
	if featureCount != len(pca.meanVector) {
		return 0, 0, fmt.Errorf("feature dimension mismatch: expected %d features, got %d",
			len(pca.meanVector), featureCount)
	}

	return sampleCount, featureCount, nil
}

// computeFeatureMeans calculates column-wise arithmetic averages across all matrix samples.
func (pca *PrincipalComponentAnalysis) computeFeatureMeans(matrix mat.Matrix, sampleCount, featureCount int) []float64 {
	featureMeanVector := make([]float64, featureCount)

	// Iterate over each feature column to aggregate totals.
	for featureIndex := 0; featureIndex < featureCount; featureIndex++ {
		var featureSum float64

		// Accumulate sample values across current feature column.
		for sampleIndex := 0; sampleIndex < sampleCount; sampleIndex++ {
			featureSum += matrix.At(sampleIndex, featureIndex)
		}

		// Compute empirical mean for current feature index.
		featureMeanVector[featureIndex] = featureSum / float64(sampleCount)
	}

	return featureMeanVector
}

// createMeanCenteredMatrix subtracts calculated feature means from input matrix elements.
func (pca *PrincipalComponentAnalysis) createMeanCenteredMatrix(matrix mat.Matrix, sampleCount, featureCount int) *mat.Dense {
	centeredMatrix := mat.NewDense(sampleCount, featureCount, nil)

	// Center each element relative to its feature mean.
	for sampleIndex := 0; sampleIndex < sampleCount; sampleIndex++ {
		for featureIndex := 0; featureIndex < featureCount; featureIndex++ {
			// Subtract empirical mean from corresponding matrix cell value.
			centeredValue := matrix.At(sampleIndex, featureIndex) - pca.meanVector[featureIndex]
			centeredMatrix.Set(sampleIndex, featureIndex, centeredValue)
		}
	}

	return centeredMatrix
}

// computeRightSingularVectors factors mean-centered matrix using thin SVD to obtain principal directions.
func (pca *PrincipalComponentAnalysis) computeRightSingularVectors(centeredMatrix *mat.Dense) (*mat.Dense, error) {
	var singularValueDecomposition mat.SVD

	// Perform SVD factorization returning thin right singular vectors.
	factorizationSuccess := singularValueDecomposition.Factorize(centeredMatrix, mat.SVDThinV)

	// Verify numerical convergence of SVD algorithm.
	if !factorizationSuccess {
		return nil, errors.New("singular value decomposition factorization failed to converge")
	}

	var rightSingularVectors mat.Dense

	// Copy extracted V vectors into explicit dense matrix target.
	singularValueDecomposition.VTo(&rightSingularVectors)

	return &rightSingularVectors, nil
}

// ComponentCount returns the configured target number of principal components.
func (pca *PrincipalComponentAnalysis) ComponentCount() int {
	return pca.componentCount
}

// IsFitted reports whether the model has successfully completed PCA fitting.
func (pca *PrincipalComponentAnalysis) IsFitted() bool {
	return pca.isFitted
}

// FeatureMeanVector returns a defensive copy of the calculated empirical feature means.
func (pca *PrincipalComponentAnalysis) FeatureMeanVector() []float64 {
	// Guard against un-fitted access.
	if !pca.isFitted {
		return nil
	}

	// Create independent slice to prevent external modification.
	meanVectorCopy := make([]float64, len(pca.meanVector))

	copy(meanVectorCopy, pca.meanVector)

	return meanVectorCopy
}

// ProjectionMatrix returns a defensive copy of the fitted principal component projection matrix.
func (pca *PrincipalComponentAnalysis) ProjectionMatrix() *mat.Dense {
	// Guard against un-fitted state or uninitialized matrix.
	if !pca.isFitted || pca.projectedMatrix == nil {
		return nil
	}

	// Perform deep matrix copy to enforce encapsulation.
	return mat.DenseCopyOf(pca.projectedMatrix)
}

// NewPrincipalComponentAnalysis initializes and returns a ready-to-use PCA instance.
func NewPrincipalComponentAnalysis(componentCount int) (*PrincipalComponentAnalysis, error) {
	// Enforce positive target component requirement.
	if componentCount <= 0 {
		return nil, errors.New("component count must be a positive integer.")
	}

	return &PrincipalComponentAnalysis{
		componentCount: componentCount,
		isFitted:       false,
	}, nil
}

// Fit computes empirical means, centers input matrix data, and extracts principal component axes.
func (pca *PrincipalComponentAnalysis) Fit(matrix mat.Matrix) error {
	// Validate matrix structural bounds prior to computation.
	sampleCount, featureCount, err := pca.validateFitDimensions(matrix)

	if err != nil {
		return fmt.Errorf("fit validation failed %w", err)
	}

	// Compute empirical feature means.
	pca.meanVector = pca.computeFeatureMeans(matrix, sampleCount, featureCount)

	// Construct zero-centered data representation.
	centeredDataMatrix := pca.createMeanCenteredMatrix(matrix, sampleCount, featureCount)

	// Decompose centered dataset via singular value decomposition.
	rightSingularVectors, err := pca.computeRightSingularVectors(centeredDataMatrix)

	if err != nil {
		return fmt.Errorf("decomposition failed: %w", err)
	}

	// Extract top K principal directions from right singular vectors.
	componentSlice := rightSingularVectors.Slice(0, featureCount, 0, pca.componentCount)

	// Persist projection matrix copy and update fitted status indicator.
	pca.projectedMatrix = mat.DenseCopyOf(componentSlice)
	pca.isFitted = true

	return nil
}

// Transform projects input feature data into the reduced principal component subspace.
func (pca *PrincipalComponentAnalysis) Transform(matrix mat.Matrix) (*mat.Dense, error) {
	// Ensure model was fitted prior to transform attempt.
	if !pca.isFitted {
		return nil, errors.New("cannot transform data: model has not been fitted")
	}

	// Validate input matrix dimensions against learned model state.
	sampleCount, featureCount, err := pca.validateTransformDimensions(matrix)

	if err != nil {
		return nil, fmt.Errorf("transform validation failed %w", err)
	}

	// Subtract learned feature means from transform dataset.
	centeredDataMatrix := pca.createMeanCenteredMatrix(matrix, sampleCount, featureCount)

	var transformedResultMatrix mat.Dense

	// Project centered matrix onto stored component directions via matrix multiplication.
	transformedResultMatrix.Mul(centeredDataMatrix, pca.projectedMatrix)

	return &transformedResultMatrix, nil
}

// FormatMatrixOutput formats matrix elements into a tabbed bracketed string with custom decimal precision.
func FormatMatrixOutput(matrix mat.Matrix, precision int) (string, error) {
	// Check matrix instance validity.
	if matrix == nil {
		return "", errors.New("matrix was not supplied")
	}

	// Validate numerical precision threshold.
	if precision < 0 {
		return "", errors.New("precision cannot be negative")
	}

	rowCount, columnCount := matrix.Dims()

	var result string

	// Construct dynamic floating point formatting specifier.
	formatSpecifier := fmt.Sprintf("%%.%df", precision)

	// Render matrix elements row by row.
	for rowIndex := 0; rowIndex < rowCount; rowIndex++ {
		result += "\t["

		for columnIndex := 0; columnIndex < columnCount; columnIndex++ {
			result += fmt.Sprintf(formatSpecifier, matrix.At(rowIndex, columnIndex))

			// Append comma separator between matrix columns.
			if columnIndex < columnCount-1 {
				result += ", "
			}
		}

		result += "]\n"
	}

	return result, nil
}
