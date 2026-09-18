// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This executable provides a demonstration entry point for the custom Principal Component Analysis
//	(PCA) package using Gonum linear algebra primitives.
//
//	The primary purpose of this file is to showcase model initialization, fitting against a sample
//	dataset matrix, performing dimensionality reduction transformation, and outputting formatted matrix state.
//
//	The demonstration workflow proceeds as follows:
//
//	  1. Dataset Preparation — constructs a high-dimensional 5x3 sample dense matrix.
//	  2. Model Fitting — initializes the PCA model targeting 2 principal components and learns
//	     the empirical feature means and principal directions.
//	  3. Subspace Transformation — projects the original 3D dataset into a reduced 2D subspace.
//	  4. Output & Formatting — renders key parameters, matrix dimensions, projection directions,
//	     and transformed output data using custom terminal formatting helpers.
//
// Key Features & Design Principles:
//
//   - Dimensionality Reduction Pipeline: Demonstrates full end-to-end model workflow from fit to transform.
//   - Explicit Error Handling: Log-fatal error checks ensure early exits on matrix dimensions or execution failures.
//   - Formatted Diagnostics: Utilizes custom tabular output functions for clean matrix inspection.
//
// Operational Limits & Bounds:
//
//   - Mock Dataset Scope: Serves as a localized functional demonstration rather than a production data pipeline.
//
// Intended Use:
//
//	This main entry point serves as an example client for testing, verifying, and integrating
//	the `principalcomponentanalysis` package into larger Go applications.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"log"

	"github.com/bgolesoftwaredeveloper/principalcomponentanalysis/principalcomponentanalysis"
	"gonum.org/v1/gonum/mat"
)

func main() {
	// Raw feature values for dataset matrix.
	dataset := []float64{
		2.5, 2.4, 0.5,
		0.5, 0.7, 0.1,
		2.2, 2.9, 0.8,
		1.9, 2.2, 0.4,
		3.1, 3.0, 0.9,
	}

	// Construct 5x3 dataset matrix.
	sampleMatrix := mat.NewDense(5, 3, dataset)

	// Initialize PCA model targeting 2 components.
	model, err := principalcomponentanalysis.NewPrincipalComponentAnalysis(2)

	// Handle initialization error.
	if err != nil {
		log.Fatalf("Initialization failed: %v", err)
	}

	// Fit PCA model on sample dataset.
	if err := model.Fit(sampleMatrix); err != nil {
		log.Fatalf("Fit failed: %v", err)
	}

	// Transform sample dataset into reduced subspace.
	transformedMatrix, err := model.Transform(sampleMatrix)

	// Handle transformation error.
	if err != nil {
		log.Fatalf("Transform failed: %v", err)
	}

	// Extract dimension metadata from original and reduced matrices.
	rows, columns := sampleMatrix.Dims()
	transformedRows, transformedColumns := transformedMatrix.Dims()

	// Print model metadata overview.
	fmt.Println("=== PCA Demonstration ===")
	fmt.Printf("Model Is Fitted: %t\n", model.IsFitted())
	fmt.Printf("Configured Target Components: %d\n", model.ComponentCount())
	fmt.Printf("Feature Mean Vector: %.4f\n", model.FeatureMeanVector())
	fmt.Println()

	// Print dimension metadata overview.
	fmt.Printf("Original Data Dimensions: %d x %d\n", rows, columns)
	fmt.Printf("Transformed Data Dimensions: %d x %d\n", transformedRows, transformedColumns)
	fmt.Println()

	// Output principal component directions.
	fmt.Println("Projection matrix (Principal Components):")

	// Format projection matrix string.
	formattedProjection, err := principalcomponentanalysis.FormatMatrixOutput(model.ProjectionMatrix(), 4)

	// Handle projection formatting error.
	if err != nil {
		log.Fatalf("failed to format projection matrix %v", err)
	}

	fmt.Print(formattedProjection)
	fmt.Println()

	// Output transformed reduced matrix.
	fmt.Println("Transformed result matrix (5x2):")

	// Format transformed matrix string.
	formattedTransformed, err := principalcomponentanalysis.FormatMatrixOutput(transformedMatrix, 4)

	// Handle transformed matrix formatting error.
	if err != nil {
		log.Fatalf("failed to format transformed matrix %v", err)
	}

	fmt.Print(formattedTransformed)
}
