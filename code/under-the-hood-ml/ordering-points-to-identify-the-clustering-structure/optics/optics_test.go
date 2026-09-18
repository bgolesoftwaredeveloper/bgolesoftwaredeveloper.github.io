// -----------------------------------------------------------------------------
// Package: optics
// File: optics_test.go
//
// Description:
//
//	This file contains unit tests for the OPTICS (Ordering Points To Identify
//	the Clustering Structure) density-based clustering algorithm.
//	The tests validate distance metrics, core distance evaluations, seed queue
//	updates, execution ordering, density boundary transitions, and structural invariants.
//
//	The test suite focuses on:
//
//	  - Euclidean distance correctness across 1D, 2D, and higher dimensions.
//	  - Core distance calculations and +Inf threshold handling.
//	  - Priority seed queue update logic and ordering mechanics.
//	  - Disjoint cluster traversal, noise identification, and reachability spikes.
//	  - Structural invariants (complete point visiting and zero duplicate inclusions).
//	  - Edge conditions (zero parameters, zero distance, dense grids, sub-epsilon boundaries).
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//
// -----------------------------------------------------------------------------
package optics

import (
	"math"
	"testing"
)

// verifyOpticsInvariants performs structural assertions checking that all original data
// points are included in the output ordered set exactly once and marked as visited.
func verifyOpticsInvariants(test *testing.T, algorithm *Optics, totalExpectedPoints int) {
	test.Helper()

	if len(algorithm.OrderedSet) != totalExpectedPoints {
		test.Fatalf("invariant violation: total ordered points (%d) != expected points (%d)",
			len(algorithm.OrderedSet),
			totalExpectedPoints,
		)
	}

	indexOccurrenceTracker := make(map[int]bool)

	for position, node := range algorithm.OrderedSet {
		if node == nil {
			test.Fatalf("invariant violation: node at position %d in OrderedSet is nil", position)
		}

		if !node.Visited {
			test.Fatalf("invariant violation: node at index %d in OrderedSet was not marked visited", node.Index)
		}

		if indexOccurrenceTracker[node.Index] {
			test.Fatalf("invariant violation: duplicate point index %d present in OrderedSet", node.Index)
		}

		indexOccurrenceTracker[node.Index] = true
	}
}

// TestDistance_IdenticalPoints verifies that identical points yield a distance of zero.
func TestDistance_IdenticalPoints(test *testing.T) {
	// Arrange.
	originPointVector := Point{2.5, 4.0, 6.1}
	matchingPointVector := Point{2.5, 4.0, 6.1}

	// Act.
	calculatedDistance := originPointVector.Distance(matchingPointVector)

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
	calculatedDistance := sourceCoordinatePoint.Distance(destinationCoordinatePoint)

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
	calculatedDistance := baseHyperdimensionalPoint.Distance(targetHyperdimensionalPoint)

	// Assert.
	if math.Abs(calculatedDistance-expectedDistance) > 1e-9 {
		test.Fatalf("expected high-dimensional distance %f, got %f", expectedDistance, calculatedDistance)
	}
}

// TestDistance_ZeroVectorDistance verifies distance computation between zero vectors.
func TestDistance_ZeroVectorDistance(test *testing.T) {
	// Arrange.
	zeroPointA := Point{0.0, 0.0, 0.0}
	zeroPointB := Point{0.0, 0.0, 0.0}

	// Act.
	calculatedDistance := zeroPointA.Distance(zeroPointB)

	// Assert.
	if calculatedDistance != 0.0 {
		test.Fatalf("expected distance 0.0 for zero vectors, got %f", calculatedDistance)
	}
}

// TestDistance_SymmetricProperty verifies that distance(A, B) equals distance(B, A).
func TestDistance_SymmetricProperty(test *testing.T) {
	// Arrange.
	point := Point{1.5, -3.2, 4.8}
	other := Point{-2.1, 7.4, 0.3}

	// Act.
	distancePointToOther := point.Distance(other)
	distanceOtherToPoint := other.Distance(point)

	// Assert.
	if math.Abs(distancePointToOther-distanceOtherToPoint) > 1e-9 {
		test.Fatalf("distance metric asymmetric: AB=%f, BA=%f", distancePointToOther, distanceOtherToPoint)
	}
}

// TestOptics_EmptyDataset verifies defensive behavior when dataset is empty.
func TestOptics_EmptyDataset(test *testing.T) {
	// Arrange.
	emptyDataset := []Point{}

	// Act.
	algorithm := NewOptics(emptyDataset, 10.0, 3)
	algorithm.Run()

	// Assert.
	if len(algorithm.OrderedSet) != 0 {
		test.Fatalf("expected empty OrderedSet for empty dataset, got %d items", len(algorithm.OrderedSet))
	}
}

// TestOptics_SingleItemDataset verifies execution against a dataset containing exactly one point.
func TestOptics_SingleItemDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{{7.0, 14.0}}

	// Act.
	algorithm := NewOptics(dataset, 5.0, 1)
	algorithm.Run()

	// Assert.
	if len(algorithm.OrderedSet) != 1 {
		test.Fatalf("expected 1 ordered node for single item dataset, got %d", len(algorithm.OrderedSet))
	}

	if algorithm.OrderedSet[0].CoreDistance != 0.0 {
		test.Fatalf("expected core distance 0.0 for single item with minimumPoints=1, got %f", algorithm.OrderedSet[0].CoreDistance)
	}

	verifyOpticsInvariants(test, algorithm, 1)
}

// TestOptics_DataPreservation verifies that input data points are not mutated during optics run.
func TestOptics_DataPreservation(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 2.0},
		{3.0, 4.0},
		{5.0, 6.0},
	}

	initialXCoordinate := dataset[0][0]
	initialYCoordinate := dataset[0][1]

	algorithm := NewOptics(dataset, 10.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	if dataset[0][0] != initialXCoordinate || dataset[0][1] != initialYCoordinate {
		test.Fatalf("input dataset was mutated: expected [%f, %f], got [%f, %f]",
			initialXCoordinate, initialYCoordinate, dataset[0][0], dataset[0][1],
		)
	}
}

// TestOptics_NodeIndexPreservation verifies that Node structures preserve original slice indices.
func TestOptics_NodeIndexPreservation(test *testing.T) {
	// Arrange.
	dataset := []Point{{10.0, 10.0}, {20.0, 20.0}, {30.0, 30.0}}

	// Act.
	algorithm := NewOptics(dataset, 50.0, 2)

	// Assert.
	for expectedIndex, node := range algorithm.Nodes {
		if node.Index != expectedIndex {
			test.Fatalf("expected node index %d, got %d", expectedIndex, node.Index)
		}
	}
}

// TestOptics_CalculateCoreDistanceSufficient verifies correct core distance evaluation when neighbor threshold is met.
func TestOptics_CalculateCoreDistanceSufficient(test *testing.T) {
	// Arrange.
	algorithm := NewOptics([]Point{}, 10.0, 3)
	neighborDistances := []float64{0.0, 0.5, 1.2, 3.4}

	expectedCoreDistance := 1.2

	// Act.
	evaluatedCoreDistance := algorithm.calculateCoreDistance(neighborDistances)

	// Assert.
	if evaluatedCoreDistance != expectedCoreDistance {
		test.Fatalf("expected core distance %f, got %f", expectedCoreDistance, evaluatedCoreDistance)
	}
}

// TestOptics_CalculateCoreDistanceInsufficient verifies core distance yields +Inf when neighbors fall below minimumPoints.
func TestOptics_CalculateCoreDistanceInsufficient(test *testing.T) {
	// Arrange.
	algorithm := NewOptics([]Point{}, 10.0, 4)
	neighborDistances := []float64{0.0, 0.5, 1.2}

	// Act.
	evaluatedCoreDistance := algorithm.calculateCoreDistance(neighborDistances)

	// Assert.
	if !math.IsInf(evaluatedCoreDistance, 1) {
		test.Fatalf("expected +Inf core distance for insufficient neighbors, got %f", evaluatedCoreDistance)
	}
}

// TestOptics_CalculateCoreDistanceExactMinPts verifies core distance when distance count equals minimumPoints.
func TestOptics_CalculateCoreDistanceExactMinPts(test *testing.T) {
	// Arrange.
	algorithm := NewOptics([]Point{}, 10.0, 3)
	neighborDistances := []float64{2.5, 0.0, 1.1}

	expectedCoreDistance := 2.5

	// Act.
	evaluatedCoreDistance := algorithm.calculateCoreDistance(neighborDistances)

	// Assert.
	if evaluatedCoreDistance != expectedCoreDistance {
		test.Fatalf("expected core distance %f, got %f", expectedCoreDistance, evaluatedCoreDistance)
	}
}

// TestOptics_MinimumPointsExceedsDataset verifies handling when minimumPoints exceeds total dataset length.
func TestOptics_MinimumPointsExceedsDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{{1.0, 1.0}, {2.0, 2.0}}

	algorithm := NewOptics(dataset, 10.0, 5)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	for index, node := range algorithm.OrderedSet {
		if !math.IsInf(node.CoreDistance, 1) {
			test.Fatalf("expected +Inf core distance for node at index %d when minimumPoints > dataset size, got %f", index, node.CoreDistance)
		}
	}
}

// TestOptics_UpdateNewSeedInsertion verifies newly discovered unvisited neighbors are assigned reachability and queued.
func TestOptics_UpdateNewSeedInsertion(test *testing.T) {
	// Arrange.
	dataset := []Point{{0.0, 0.0}, {2.0, 0.0}}
	algorithm := NewOptics(dataset, 5.0, 2)

	centerNode := algorithm.Nodes[0]
	centerNode.CoreDistance = 1.0

	neighborNode := algorithm.Nodes[1]
	neighbors := []*Node{neighborNode}

	var seeds []*Node

	// Act.
	updatedSeeds := algorithm.update(neighbors, centerNode, seeds)

	// Assert.
	if len(updatedSeeds) != 1 {
		test.Fatalf("expected 1 seed in queue, got %d", len(updatedSeeds))
	}

	expectedReachabilityDistance := 2.0

	if neighborNode.ReachabilityDistance != expectedReachabilityDistance {
		test.Fatalf("expected reachability distance %f, got %f", expectedReachabilityDistance, neighborNode.ReachabilityDistance)
	}
}

// TestOptics_UpdateDecreaseReachabilityDistance verifies reachability distance is updated down when a shorter path is found.
func TestOptics_UpdateDecreaseReachabilityDistance(test *testing.T) {
	// Arrange.
	dataset := []Point{{0.0, 0.0}, {5.0, 0.0}, {2.0, 0.0}}
	algorithm := NewOptics(dataset, 10.0, 2)

	targetNode := algorithm.Nodes[2]
	targetNode.ReachabilityDistance = 8.0

	centerNode := algorithm.Nodes[0]
	centerNode.CoreDistance = 1.0

	neighbors := []*Node{targetNode}
	seeds := []*Node{targetNode}

	// Act.
	_ = algorithm.update(neighbors, centerNode, seeds)

	// Assert.
	expectedNewReachability := 2.0

	if targetNode.ReachabilityDistance != expectedNewReachability {
		test.Fatalf("expected reachability distance decreased to %f, got %f", expectedNewReachability, targetNode.ReachabilityDistance)
	}
}

// TestOptics_UpdateIgnoreVisitedNeighbors verifies that visited neighbors are excluded from seed queue updates.
func TestOptics_UpdateIgnoreVisitedNeighbors(test *testing.T) {
	// Arrange.
	dataset := []Point{{0.0, 0.0}, {1.0, 0.0}}
	algorithm := NewOptics(dataset, 5.0, 2)

	algorithm.Nodes[1].Visited = true

	centerNode := algorithm.Nodes[0]
	centerNode.CoreDistance = 0.5

	// Act.
	updatedSeeds := algorithm.update([]*Node{algorithm.Nodes[1]}, centerNode, nil)

	// Assert.
	if len(updatedSeeds) != 0 {
		test.Fatalf("expected visited neighbor to be skipped, but seed queue length is %d", len(updatedSeeds))
	}
}

// TestOptics_UpdateReachabilityBoundByCoreDistance verifies reachability is set to core distance when spatial distance is smaller.
func TestOptics_UpdateReachabilityBoundByCoreDistance(test *testing.T) {
	// Arrange.
	dataset := []Point{{0.0, 0.0}, {0.5, 0.0}}
	algorithm := NewOptics(dataset, 5.0, 2)

	centerNode := algorithm.Nodes[0]
	centerNode.CoreDistance = 3.0

	neighborNode := algorithm.Nodes[1]

	// Act.
	_ = algorithm.update([]*Node{neighborNode}, centerNode, nil)

	// Assert.
	if neighborNode.ReachabilityDistance != 3.0 {
		test.Fatalf("expected reachability bound by core distance 3.0, got %f", neighborNode.ReachabilityDistance)
	}
}

// TestOptics_TwoWellSeparatedClusters verifies complete execution and reachability spike across 2D clusters.
func TestOptics_TwoWellSeparatedClusters(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{0.0, 0.0}, {0.2, 0.1}, {0.1, 0.3},
		{100.0, 100.0}, {100.2, 100.1}, {100.1, 100.3},
	}

	algorithm := NewOptics(dataset, 150.0, 3)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	transitionNode := algorithm.OrderedSet[3]

	if transitionNode.ReachabilityDistance < 90.0 {
		test.Fatalf("expected large reachability transition distance (>90.0), got %f", transitionNode.ReachabilityDistance)
	}
}

// TestOptics_OneDimensionalData verifies ordering and reachability on 1D real numbers.
func TestOptics_OneDimensionalData(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0}, {1.1}, {1.2},
		{50.0}, {50.1}, {50.2},
	}

	algorithm := NewOptics(dataset, 100.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))
}

// TestOptics_ThreeDimensionalData verifies execution consistency across 3D point clouds.
func TestOptics_ThreeDimensionalData(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 1.0, 1.0}, {1.1, 0.9, 1.2},
		{10.0, 10.0, 10.0}, {10.2, 9.8, 10.1},
		{20.0, 20.0, 20.0}, {20.1, 19.9, 20.2},
	}

	algorithm := NewOptics(dataset, 30.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))
}

// TestOptics_IdenticalPointsInDataset verifies core distance behavior when identical spatial points are present.
func TestOptics_IdenticalPointsInDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{5.0, 5.0},
		{5.0, 5.0},
		{5.0, 5.0},
		{5.0, 5.0},
	}

	algorithm := NewOptics(dataset, 2.0, 3)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	for index, node := range algorithm.OrderedSet {
		if node.CoreDistance != 0.0 {
			test.Fatalf("expected core distance 0.0 for duplicate point at index %d, got %f", index, node.CoreDistance)
		}
	}
}

// TestOptics_CollinearPoints verifies traversal along points distributed on a straight line.
func TestOptics_CollinearPoints(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0},
		{20.0, 20.0}, {21.0, 21.0}, {22.0, 22.0},
	}

	algorithm := NewOptics(dataset, 30.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))
}

// TestOptics_NegativeCoordinates verifies clustering with negative spatial components.
func TestOptics_NegativeCoordinates(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{-10.0, -10.0}, {-10.5, -9.5},
		{10.0, 10.0}, {10.5, 9.5},
	}

	algorithm := NewOptics(dataset, 30.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))
}

// TestOptics_NoisePointUnreachable verifies isolated outlier point retains infinite reachability.
func TestOptics_NoisePointUnreachable(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{0.0, 0.0}, {0.1, 0.0}, {0.0, 0.1},
		{500.0, 500.0},
	}

	algorithm := NewOptics(dataset, 5.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	noiseNode := algorithm.Nodes[3]

	if !math.IsInf(noiseNode.ReachabilityDistance, 1) {
		test.Fatalf("expected noise node reachability to remain +Inf, got %f", noiseNode.ReachabilityDistance)
	}
}

// TestOptics_GridTopology verifies execution on a structured 2x2 spatial grid.
func TestOptics_GridTopology(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{0.0, 0.0}, {0.0, 1.0},
		{1.0, 0.0}, {1.0, 1.0},
	}

	algorithm := NewOptics(dataset, 2.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))
}

// TestOptics_ZeroMaximumEpsilon verifies behavior when search radius epsilon is zero.
func TestOptics_ZeroMaximumEpsilon(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{1.0, 1.0},
		{2.0, 2.0},
	}

	algorithm := NewOptics(dataset, 0.0, 1)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	for index, node := range algorithm.OrderedSet {
		if node.CoreDistance != 0.0 {
			test.Fatalf("expected core distance 0.0 for node %d under epsilon=0 and minPts=1, got %f", index, node.CoreDistance)
		}
	}
}

// TestOptics_RestrictiveEpsilonIsolatesPoints verifies points out of epsilon radius are treated as separate noise clusters.
func TestOptics_RestrictiveEpsilonIsolatesPoints(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{0.0, 0.0}, {0.5, 0.0},
		{10.0, 0.0}, {10.5, 0.0},
	}

	algorithm := NewOptics(dataset, 2.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	secondClusterStartNode := algorithm.OrderedSet[2]

	if !math.IsInf(secondClusterStartNode.ReachabilityDistance, 1) {
		test.Fatalf("expected +Inf reachability when transitioning across epsilon cutoff, got %f", secondClusterStartNode.ReachabilityDistance)
	}
}

// TestOptics_MinPtsOneAssignsZeroCoreDistance verifies that MinimumPoints=1 assigns 0 core distance to all nodes.
func TestOptics_MinPtsOneAssignsZeroCoreDistance(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{10.0, 10.0},
		{20.0, 20.0},
	}

	algorithm := NewOptics(dataset, 100.0, 1)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	for index, node := range algorithm.OrderedSet {
		if node.CoreDistance != 0.0 {
			test.Fatalf("expected core distance 0.0 for node %d when minPts=1, got %f", index, node.CoreDistance)
		}
	}
}

// TestOptics_SubEpsilonBoundaryNeighborSearch verifies neighbor detection right on the epsilon boundary.
func TestOptics_SubEpsilonBoundaryNeighborSearch(test *testing.T) {
	// Arrange.
	dataset := []Point{{0.0, 0.0}, {3.0, 4.0}}

	algorithm := NewOptics(dataset, 5.0, 2)

	// Act.
	neighbors, distances := algorithm.getNeighbors(algorithm.Nodes[0])

	// Assert.
	if len(neighbors) != 2 {
		test.Fatalf("expected 2 neighbors at exact epsilon boundary 5.0, got %d", len(neighbors))
	}

	if math.Abs(distances[1]-5.0) > 1e-9 {
		test.Fatalf("expected neighbor distance 5.0, got %f", distances[1])
	}
}

// TestOptics_FirstProcessedNodeHasInfiniteReachability verifies that the seed node initiating execution has +Inf reachability.
func TestOptics_FirstProcessedNodeHasInfiniteReachability(test *testing.T) {
	// Arrange.
	dataset := []Point{{1.0, 1.0}, {1.1, 1.1}, {1.2, 1.2}}

	algorithm := NewOptics(dataset, 10.0, 2)

	// Act.
	algorithm.Run()

	// Assert.
	verifyOpticsInvariants(test, algorithm, len(dataset))

	initialNode := algorithm.OrderedSet[0]

	if !math.IsInf(initialNode.ReachabilityDistance, 1) {
		test.Fatalf("expected initial node reachability distance to be +Inf, got %f", initialNode.ReachabilityDistance)
	}
}

// TestOptics_UnprocessedNodeReachabilityDefault verifies initial state of unvisited nodes prior to run.
func TestOptics_UnprocessedNodeReachabilityDefault(test *testing.T) {
	// Arrange.
	dataset := []Point{{1.0, 2.0}, {3.0, 4.0}}

	// Act.
	algorithm := NewOptics(dataset, 10.0, 2)

	// Assert.
	for index, node := range algorithm.Nodes {
		if node.Visited {
			test.Fatalf("node at index %d should not be marked visited before Run()", index)
		}

		if !math.IsInf(node.CoreDistance, 1) || !math.IsInf(node.ReachabilityDistance, 1) {
			test.Fatalf("node at index %d should initialize distances to +Inf", index)
		}
	}
}
