// -----------------------------------------------------------------------------
// Package: randomforest
// File: random_forest_test.go
//
// Description:
//
//	This file contains unit tests for the Random Forest classification algorithm.
//	The test suite validates basic initialization, defensive input validation, decision tree recursion,
//	Gini impurity calculation, dataset splitting, bootstrap sampling, majority voting, and prediction accuracy.
//
//	The test suite focuses on:
//
//	  - Defensive validation against invalid initialization parameters and empty datasets.
//	  - Invariant checks for tree node construction and feature dimension alignment.
//	  - Exact mathematical correctness of Gini impurity and majority voting helper functions.
//	  - Split logic evaluation across multi-dimensional feature threshold boundaries.
//	  - Binary, multi-class, linearly separable, and overlapping classification accuracy.
//	  - Edge behavior with single-feature datasets, uniform labels, and subsample ratio limits.
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//
// -----------------------------------------------------------------------------
package randomforest

import (
	"math"
	"testing"
)

// verifyForestInvariants validates structure properties and parameter validity.
func verifyForestInvariants(test *testing.T, forest *RandomForest) {
	test.Helper()

	if forest == nil {
		test.Fatal("invariant violation: random forest instance is nil")
	}

	if forest.numberOfTrees <= 0 {
		test.Fatalf("invariant violation: number of trees must be positive (%d)", forest.numberOfTrees)
	}

	if forest.maximumDepth < 1 {
		test.Fatalf("invariant violation: maximum depth must be at least one (%d)", forest.maximumDepth)
	}

	if forest.subsampleRatio <= 0.0 || forest.subsampleRatio > 1.0 {
		test.Fatalf("invariant violation: subsample ratio out of range (%f)", forest.subsampleRatio)
	}
}

// TestNewForest_ValidParameters verifies initialization with valid arguments.
func TestNewForest_ValidParameters(test *testing.T) {
	// Arrange.
	numberOfTrees := 10
	maximumDepth := 5
	minimumSamples := 2
	subsampleRatio := 0.8

	// Act.
	forest, err := NewForest(numberOfTrees, maximumDepth, minimumSamples, subsampleRatio)

	// Assert.
	if err != nil {
		test.Fatalf("expected no error during forest creation, got %v", err)
	}

	verifyForestInvariants(test, forest)
}

// TestNewForest_ZeroTreesError verifies error when numberOfTrees is zero or negative.
func TestNewForest_ZeroTreesError(test *testing.T) {
	// Arrange & Act.
	forest, err := NewForest(0, 5, 2, 0.8)

	// Assert.
	if err == nil {
		test.Fatal("expected error for zero trees, got nil")
	}

	if forest != nil {
		test.Fatal("expected nil forest instance on error")
	}
}

// TestNewForest_InvalidDepthError verifies error when maximumDepth is less than one.
func TestNewForest_InvalidDepthError(test *testing.T) {
	// Arrange & Act.
	forest, err := NewForest(10, 0, 2, 0.8)

	// Assert.
	if err == nil {
		test.Fatal("expected error for maximum depth less than one, got nil")
	}

	if forest != nil {
		test.Fatal("expected nil forest instance on error")
	}
}

// TestNewForest_InvalidSubsampleRatioLow verifies error when subsampleRatio is zero.
func TestNewForest_InvalidSubsampleRatioLow(test *testing.T) {
	// Arrange & Act.
	forest, err := NewForest(10, 5, 2, 0.0)

	// Assert.
	if err == nil {
		test.Fatal("expected error for subsample ratio zero, got nil")
	}

	if forest != nil {
		test.Fatal("expected nil forest instance on error")
	}
}

// TestNewForest_InvalidSubsampleRatioHigh verifies error when subsampleRatio exceeds one.
func TestNewForest_InvalidSubsampleRatioHigh(test *testing.T) {
	// Arrange & Act.
	forest, err := NewForest(10, 5, 2, 1.5)

	// Assert.
	if err == nil {
		test.Fatal("expected error for subsample ratio greater than 1.0, got nil")
	}

	if forest != nil {
		test.Fatal("expected nil forest instance on error")
	}
}

// TestCalculateGiniInpurity_PureDataset verifies zero impurity for single-class dataset.
func TestCalculateGiniInpurity_PureDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{Label: 1, Features: []float64{1.0, 2.0}},
		{Label: 1, Features: []float64{2.0, 3.0}},
		{Label: 1, Features: []float64{3.0, 4.0}},
	}

	// Act.
	impurity := calculateGiniInpurity(dataset)

	// Assert.
	if impurity != 0.0 {
		test.Fatalf("expected Gini impurity 0.0 for pure dataset, got %f", impurity)
	}
}

// TestCalculateGiniInpurity_BalancedDataset verifies Gini impurity computation on equal distribution.
func TestCalculateGiniInpurity_BalancedDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{Label: 0, Features: []float64{1.0}},
		{Label: 1, Features: []float64{2.0}},
	}

	// Act.
	impurity := calculateGiniInpurity(dataset)

	// Assert.
	expectedImpurity := 0.5

	if math.Abs(impurity-expectedImpurity) > 1e-9 {
		test.Fatalf("expected Gini impurity %f, got %f", expectedImpurity, impurity)
	}
}

// TestCalculateGiniInpurity_EmptyDataset verifies zero impurity returned for empty dataset.
func TestCalculateGiniInpurity_EmptyDataset(test *testing.T) {
	// Arrange.
	var dataset []Point

	// Act.
	impurity := calculateGiniInpurity(dataset)

	// Assert.
	if impurity != 0.0 {
		test.Fatalf("expected 0.0 impurity for empty dataset, got %f", impurity)
	}
}

// TestRetrieveMajorityLabel_StandardDataset verifies selection of dominant class label.
func TestRetrieveMajorityLabel_StandardDataset(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{Label: 0, Features: []float64{1.0}},
		{Label: 1, Features: []float64{2.0}},
		{Label: 1, Features: []float64{3.0}},
	}

	// Act.
	majorityLabel := retrieveMajorityLabel(dataset)

	// Assert.
	if majorityLabel != 1 {
		test.Fatalf("expected majority label 1, got %d", majorityLabel)
	}
}

// TestRetrieveMajorityLabel_EmptyDataset verifies default zero return on empty slice.
func TestRetrieveMajorityLabel_EmptyDataset(test *testing.T) {
	// Arrange.
	var dataset []Point

	// Act.
	majorityLabel := retrieveMajorityLabel(dataset)

	// Assert.
	if majorityLabel != 0 {
		test.Fatalf("expected majority label 0 for empty dataset, got %d", majorityLabel)
	}
}

// TestSplitDataset_ValidSplit verifies dataset partitioning around threshold value.
func TestSplitDataset_ValidSplit(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{Label: 0, Features: []float64{1.0}},
		{Label: 0, Features: []float64{2.0}},
		{Label: 1, Features: []float64{5.0}},
		{Label: 1, Features: []float64{6.0}},
	}

	// Act.
	leftSet, rightSet := splitDataset(dataset, 0, 3.0)

	// Assert.
	if len(leftSet) != 2 {
		test.Fatalf("expected left split size 2, got %d", len(leftSet))
	}

	if len(rightSet) != 2 {
		test.Fatalf("expected right split size 2, got %d", len(rightSet))
	}
}

// TestFit_EmptyDatasetError verifies error when calling Fit on empty dataset.
func TestFit_EmptyDatasetError(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(5, 3, 2, 0.8)

	var dataset []Point

	// Act.
	err := forest.Fit(dataset)

	// Assert.
	if err == nil {
		test.Fatal("expected error when fitting empty dataset, got nil")
	}
}

// TestFit_EmptyFeaturesError verifies error when dataset points contain empty features.
func TestFit_EmptyFeaturesError(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(5, 3, 2, 0.8)

	dataset := []Point{
		{Label: 0, Features: []float64{}},
	}

	// Act.
	err := forest.Fit(dataset)

	// Assert.
	if err == nil {
		test.Fatal("expected error when fitting dataset with empty feature vectors, got nil")
	}
}

// TestFit_SuccessfulTraining verifies state and forest tree population after successful fit.
func TestFit_SuccessfulTraining(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(10, 5, 2, 0.8)

	dataset := []Point{
		{Label: 0, Features: []float64{1.0, 1.1}},
		{Label: 1, Features: []float64{9.0, 8.9}},
	}

	// Act.
	err := forest.Fit(dataset)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful fit, got error: %v", err)
	}

	if len(forest.trees) != 10 {
		test.Fatalf("expected 10 trained trees, got %d", len(forest.trees))
	}

	if forest.expectedDimension != 2 {
		test.Fatalf("expected model dimension 2, got %d", forest.expectedDimension)
	}
}

// TestPredict_UnfittedModelError verifies error when predicting on unfitted model instance.
func TestPredict_UnfittedModelError(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(5, 3, 2, 0.8)

	// Act.
	_, err := forest.Predict([]float64{1.0, 2.0})

	// Assert.
	if err == nil {
		test.Fatal("expected error when predicting with unfitted model, got nil")
	}
}

// TestPredict_DimensionMismatchError verifies error when evaluation features dimension disagrees.
func TestPredict_DimensionMismatchError(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(5, 3, 2, 0.8)

	dataset := []Point{
		{Label: 0, Features: []float64{1.0, 2.0}},
		{Label: 1, Features: []float64{8.0, 9.0}},
	}

	_ = forest.Fit(dataset)

	// Act.
	_, err := forest.Predict([]float64{1.0, 2.0, 3.0})

	// Assert.
	if err == nil {
		test.Fatal("expected error on feature dimension mismatch, got nil")
	}
}

// TestPredict_BinaryClassificationLinear verifies binary classification accuracy on separated clusters.
func TestPredict_BinaryClassificationLinear(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(15, 5, 2, 0.8)

	dataset := []Point{
		{Label: 0, Features: []float64{1.0, 1.0}},
		{Label: 0, Features: []float64{1.5, 2.0}},
		{Label: 0, Features: []float64{2.0, 1.5}},
		{Label: 1, Features: []float64{8.0, 8.0}},
		{Label: 1, Features: []float64{8.5, 9.0}},
		{Label: 1, Features: []float64{9.0, 8.5}},
	}

	_ = forest.Fit(dataset)

	// Act.
	labelZero, errZero := forest.Predict([]float64{1.2, 1.3})
	labelOne, errOne := forest.Predict([]float64{8.2, 8.7})

	// Assert.
	if errZero != nil || errOne != nil {
		test.Fatalf("unexpected prediction errors: %v, %v", errZero, errOne)
	}

	if labelZero != 0 {
		test.Fatalf("expected label 0, got %d", labelZero)
	}

	if labelOne != 1 {
		test.Fatalf("expected label 1, got %d", labelOne)
	}
}

// TestPredict_MultiClassClassification verifies classification accuracy for three distinct label clusters.
func TestPredict_MultiClassClassification(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(20, 5, 2, 0.9)

	dataset := []Point{
		{Label: 0, Features: []float64{1.0, 1.0}},
		{Label: 0, Features: []float64{1.2, 1.1}},
		{Label: 1, Features: []float64{10.0, 1.0}},
		{Label: 1, Features: []float64{10.2, 1.1}},
		{Label: 2, Features: []float64{5.0, 10.0}},
		{Label: 2, Features: []float64{5.1, 10.2}},
	}

	_ = forest.Fit(dataset)

	// Act.
	predictedLabel, err := forest.Predict([]float64{5.0, 9.8})

	// Assert.
	if err != nil {
		test.Fatalf("unexpected prediction error: %v", err)
	}

	if predictedLabel != 2 {
		test.Fatalf("expected predicted class label 2, got %d", predictedLabel)
	}
}

// TestPredict_SingleFeatureVector verifies binary classification on one-dimensional feature sets.
func TestPredict_SingleFeatureVector(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(10, 4, 2, 0.8)

	dataset := []Point{
		{Label: 0, Features: []float64{-10.0}},
		{Label: 0, Features: []float64{-5.0}},
		{Label: 1, Features: []float64{5.0}},
		{Label: 1, Features: []float64{10.0}},
	}

	_ = forest.Fit(dataset)

	// Act.
	predictedLabel, err := forest.Predict([]float64{7.5})

	// Assert.
	if err != nil {
		test.Fatalf("unexpected prediction error: %v", err)
	}

	if predictedLabel != 1 {
		test.Fatalf("expected label 1 for single-feature input, got %d", predictedLabel)
	}
}

// TestFit_UniformClassDataset verifies correct tree building when all dataset points share one label.
func TestFit_UniformClassDataset(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(5, 3, 2, 0.8)

	dataset := []Point{
		{Label: 5, Features: []float64{1.0, 2.0}},
		{Label: 5, Features: []float64{3.0, 4.0}},
		{Label: 5, Features: []float64{5.0, 6.0}},
	}

	// Act.
	err := forest.Fit(dataset)
	predictedLabel, predictionError := forest.Predict([]float64{2.5, 3.5})

	// Assert.
	if err != nil || predictionError != nil {
		test.Fatalf("unexpected errors in uniform dataset test: fit=%v, predict=%v", err, predictionError)
	}

	if predictedLabel != 5 {
		test.Fatalf("expected uniform label 5, got %d", predictedLabel)
	}
}

// TestPredict_NilNodeInTree verifies defensive handling when internal tree node traversal hits nil reference.
func TestPredict_NilNodeInTree(test *testing.T) {
	// Arrange.
	dummyTree := &tree{}

	// Act.
	predictedLabel := dummyTree.predict(nil, []float64{1.0, 2.0})

	// Assert.
	if predictedLabel != 0 {
		test.Fatalf("expected default label 0 for nil node traversal, got %d", predictedLabel)
	}
}

// TestPredict_OutOfBoundFeatureIndex verifies fallback leaf label return when node feature index exceeds input size.
func TestPredict_OutOfBoundFeatureIndex(test *testing.T) {
	// Arrange.
	dummyTree := &tree{}

	invalidNode := &node{
		label:        42,
		featureIndex: 10,
		isLeaf:       false,
	}

	// Act.
	predictedLabel := dummyTree.predict(invalidNode, []float64{1.0, 2.0})

	// Assert.
	if predictedLabel != 42 {
		test.Fatalf("expected fallback label 42 for out-of-bounds feature index, got %d", predictedLabel)
	}
}

// TestFit_MinimumSubsampleCount verifies tree construction when subsample count evaluates to zero or one.
func TestFit_MinimumSubsampleCount(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(5, 2, 2, 0.01)

	dataset := []Point{
		{Label: 1, Features: []float64{1.0, 2.0}},
		{Label: 2, Features: []float64{8.0, 9.0}},
	}

	// Act.
	err := forest.Fit(dataset)

	// Assert.
	if err != nil {
		test.Fatalf("expected fit success even with tiny subsample ratio, got error: %v", err)
	}
}

// TestFit_HighDimensionalFeatures verifies feature subsampling with high-dimensional feature vectors.
func TestFit_HighDimensionalFeatures(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(10, 5, 2, 0.8)

	featuresA := make([]float64, 100)
	featuresB := make([]float64, 100)

	for index := 0; index < 100; index++ {
		featuresA[index] = 1.0
		featuresB[index] = 10.0
	}

	dataset := []Point{
		{Label: 0, Features: featuresA},
		{Label: 1, Features: featuresB},
	}

	// Act.
	err := forest.Fit(dataset)
	predictedLabel, predictedError := forest.Predict(featuresA)

	// Assert.
	if err != nil || predictedError != nil {
		test.Fatalf("unexpected error during high dimension testing: fit=%v, predict=%v", err, predictedError)
	}

	if predictedLabel != 0 {
		test.Fatalf("expected label 0 for high-dimensional test point, got %d", predictedLabel)
	}
}

// TestPredict_DeterministicOutputs verifies identical predictions across multiple calls on same input.
func TestPredict_DeterministicOutputs(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(10, 5, 2, 0.8)

	dataset := []Point{
		{Label: 0, Features: []float64{1.0, 2.0}},
		{Label: 1, Features: []float64{8.0, 9.0}},
	}

	_ = forest.Fit(dataset)

	testFeatures := []float64{1.5, 2.5}

	// Act.
	firstPrediction, _ := forest.Predict(testFeatures)
	secondPrediction, _ := forest.Predict(testFeatures)

	// Assert.
	if firstPrediction != secondPrediction {
		test.Fatalf("expected deterministic predictions: got %d and %d", firstPrediction, secondPrediction)
	}
}

// TestBuildTree_UnsplitableDataset verifies leaf creation when dataset features contain identical values.
func TestBuildTree_UnsplitableDataset(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(5, 5, 2, 1.0)

	dataset := []Point{
		{Label: 0, Features: []float64{1.0, 1.0}},
		{Label: 1, Features: []float64{1.0, 1.0}},
	}

	// Act.
	err := forest.Fit(dataset)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful fit on identical feature dataset, got error: %v", err)
	}
}

// TestNewForest_SingleTreeConfiguration verifies forest operation with single decision tree.
func TestNewForest_SingleTreeConfiguration(test *testing.T) {
	// Arrange.
	forest, err := NewForest(1, 5, 2, 1.0)

	dataset := []Point{
		{Label: 0, Features: []float64{1.0}},
		{Label: 0, Features: []float64{1.2}},
		{Label: 0, Features: []float64{1.5}},
		{Label: 0, Features: []float64{2.0}},
		{Label: 1, Features: []float64{8.0}},
		{Label: 1, Features: []float64{8.5}},
		{Label: 1, Features: []float64{9.0}},
		{Label: 1, Features: []float64{10.0}},
	}

	// Act.
	fitError := forest.Fit(dataset)
	predictedLabel, predictionError := forest.Predict([]float64{9.5})

	// Assert.
	if err != nil || fitError != nil || predictionError != nil {
		test.Fatalf("unexpected error in single tree test: init=%v, fit=%v, predict=%v", err, fitError, predictionError)
	}

	if predictedLabel != 1 {
		test.Fatalf("expected label 1 from single-tree forest, got %d", predictedLabel)
	}
}

// TestSplitDataset_OutOfBoundFeature verifies slice assignment when feature index exceeds bounds.
func TestSplitDataset_OutOfBoundFeature(test *testing.T) {
	// Arrange.
	dataset := []Point{
		{Label: 0, Features: []float64{1.0}},
		{Label: 1, Features: []float64{2.0}},
	}

	// Act.
	leftSet, rightSet := splitDataset(dataset, 5, 1.5)

	// Assert.
	if len(leftSet) != 0 {
		test.Fatalf("expected empty left set for out-of-bounds feature split, got %d points", len(leftSet))
	}

	if len(rightSet) != 2 {
		test.Fatalf("expected all points in right set for out-of-bounds feature split, got %d points", len(rightSet))
	}
}

// TestPredict_NegativeFeatures verifies prediction with negative feature coordinates.
func TestPredict_NegativeFeatures(test *testing.T) {
	// Arrange.
	forest, _ := NewForest(10, 4, 2, 0.8)

	dataset := []Point{
		{Label: 0, Features: []float64{-10.0, -20.0}},
		{Label: 0, Features: []float64{-12.0, -18.0}},
		{Label: 1, Features: []float64{-1.0, -2.0}},
		{Label: 1, Features: []float64{-2.0, -1.0}},
	}

	_ = forest.Fit(dataset)

	// Act.
	predictedLabel, err := forest.Predict([]float64{-11.0, -19.0})

	// Assert.
	if err != nil {
		test.Fatalf("unexpected error predicting negative features: %v", err)
	}

	if predictedLabel != 0 {
		test.Fatalf("expected label 0 for negative feature sample, got %d", predictedLabel)
	}
}
