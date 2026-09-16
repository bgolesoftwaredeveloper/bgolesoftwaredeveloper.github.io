// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates usage of the farthestpairofpoints package.
//	It constructs a small set of two-dimensional points, invokes the
//	farthest-pair computation, and prints the resulting maximum distance.
//
//	The purpose of this file is purely demonstrative. It shows how the
//	public API behaves under valid input conditions and illustrates the
//	defensive guarantees provided by the underlying implementation.
//
// Notes:
//   - All numeric inputs are finite and well-formed.
//   - The example avoids edge cases to keep the control flow explicit.
//   - The farthest-pair computation is treated as a pure function.
//   - No internal helpers or invariants are accessed directly.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/farthest-pair-of-points/farthestpairofpoints"
)

func main() {
	// -------------------------------------------------------------------------
	// Arrange: Construct a collection of two-dimensional points.
	//
	// Invariant:
	//   - Each point contains finite X and Y coordinates.
	//   - The slice contains more than two points, ensuring at least one pair.
	// -------------------------------------------------------------------------
	pointCollection := []farthestpairofpoints.Point{
		{X: 0.0, Y: 0.0},
		{X: 1.0, Y: 1.0},
		{X: -2.0, Y: 3.0},
		{X: 4.0, Y: -1.0},
		{X: -5.0, Y: -4.0},
	}

	// -------------------------------------------------------------------------
	// Act: Invoke the farthest-pair computation.
	//
	// Guarantees:
	//   - The function will not panic.
	//   - Invalid input would yield a neutral distance of zero.
	//   - For valid input, the maximum Euclidean distance is returned.
	// -------------------------------------------------------------------------
	farthestDistance := farthestpairofpoints.FindFarthestPairDistance(pointCollection)

	// -------------------------------------------------------------------------
	// Assert / Observe: Output the result.
	//
	// Interpretation:
	//   - The returned value represents the greatest distance between any
	//     unordered pair of points in the collection.
	//   - The exact pair is abstracted away by the API.
	// -------------------------------------------------------------------------
	fmt.Printf("Farthest pair distance: %.6f\n", farthestDistance)
}
