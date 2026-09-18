// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a clean, dependency-free executable example demonstrating
//	the usage of the ISOMAP (Isometric Feature Mapping) algorithm.
//
//	The program generates a synthetic 3D "S-Curve" manifold dataset and applies
//	the `isometricmapping` package to unroll and project the non-linear manifold
//	down into a 2D Euclidean coordinate space.
//
// Operational Flow:
//
//  1. Dataset Generation — constructs a 3D S-Curve manifold using parametric trigonometric functions.
//  2. Feature Embedding — executes the `isometricmapping.Fit` method using k-NN graph construction,
//     all-pairs shortest path search, and classical multidimensional scaling (MDS).
//  3. Results Display — outputs structural dimensions before and after reduction, along with sample
//     coordinates from the generated embedding space.
//
// Intended Use:
//
//	This main entrypoint serves as a validation harness and usage example for developers
//	integrating the ISOMAP manifold learning library into Go applications.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"log"
	"math"

	"github.com/bgolesoftwaredeveloper/isometricmapping/isometricmapping"
)

func main() {
	numberOfSamples := 100
	numberOfNeighbors := 8
	embeddingDimension := 2

	// Generate synthetic 3D S-Curve dataset.
	matrix := initializeSCurve(numberOfSamples)

	fmt.Printf("Input Data shape: %d samples x %d features\n", len(matrix), len(matrix[0]))

	// Execute ISOMAP dimensionality reduction.
	embedding, err := isometricmapping.Fit(matrix, numberOfNeighbors, embeddingDimension)

	if err != nil {
		log.Fatalf("Isomap failed: %v", err)
	}

	fmt.Printf("Output Data shape: %d samples x %d features\n\n", len(embedding), len(embedding[0]))
	fmt.Println("First 5 embedded points:")

	// Display sample embedded coordinate vectors.
	for rowIndex := 0; rowIndex < 5; rowIndex++ {
		fmt.Printf("\tPoint %d: [", rowIndex)

		for columnIndex := 0; columnIndex < embeddingDimension; columnIndex++ {
			fmt.Printf("%.4f", embedding[rowIndex][columnIndex])

			if columnIndex < embeddingDimension-1 {
				fmt.Print(", ")
			}
		}

		fmt.Println("]")
	}
}

// initializeSCurve generates a synthetic 3D manifold dataset shaped as an S-Curve curve.
func initializeSCurve(numberOfSamples int) [][]float64 {
	matrix := make([][]float64, numberOfSamples)

	// Sample 3D points along parametric manifold equations.
	for index := 0; index < numberOfSamples; index++ {
		progress := float64(index) / float64(numberOfSamples)
		angle := 3.0 * math.Pi * (progress - 0.5)

		xCoordinate := math.Sin(angle)
		yCoordinate := 2.0 * float64(index) / float64(numberOfSamples)

		var zCoordinate float64

		if angle < 0 {
			zCoordinate = 2.0 - math.Cos(angle)
		} else {
			zCoordinate = math.Cos(angle)
		}

		matrix[index] = []float64{xCoordinate, yCoordinate, zCoordinate}
	}

	return matrix
}
