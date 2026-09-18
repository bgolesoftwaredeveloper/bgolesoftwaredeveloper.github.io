// -----------------------------------------------------------------------------
// Package: randomforest
// File: random_forest.go
//
// Description:
//
//	This file provides a clean, dependency-free implementation of the Random Forest
//	classification algorithm for tabular and multi-dimensional feature data.
//
//	The primary purpose of this classifier is to construct an ensemble of decision trees
//	trained on bootstrapped subsamples of dataset points with random feature selection.
//	Each individual decision tree evaluates optimal binary split thresholds using Gini impurity.
//
//	The algorithm operates in two main phases:
//
//	  1. Fitting — generates specified decision trees by drawing random sub-samples
//	     and constructing binary decision trees up to a maximum configured depth.
//	  2. Prediction — aggregates individual predictions from all trees in the ensemble
//	     and selects the final class label via majority voting.
//
// Key Features & Design Principles:
//
//   - Random Feature Subsampling: Selects a subset of features (sqrt(N)) at each split node
//     to decorrelate decision trees within the forest.
//   - Bootstrap Aggregating (Bagging): Trains individual trees on randomly drawn subsamples
//     to enhance model variance reduction and generalization.
//   - Defensive Input Validation: Ensures sample sizes, dimensions, and hyperparameters meet
//     strict criteria during initialization and inference.
//   - Self-Contained Implementation: Relies exclusively on Go standard library primitives
//     (`math`, `math/rand/v2`, `sort`, `errors`).
//
// Operational Limits & Bounds:
//
//   - Feature Dimension Consistency: All input points must maintain uniform feature lengths.
//   - Ensemble Storage: Memory consumption scales linearly with the number of trees and tree depth.
//
// Intended Use:
//
//	This package serves as a core machine learning building block for zero-dependency
//	Go applications requiring robust non-linear multi-class classification.
//
// -----------------------------------------------------------------------------
package randomforest

import (
	"errors"
	"math"
	"math/rand/v2"
	"sort"
)

// node represents a single structural node or leaf within a decision tree.
type node struct {
	label        int
	featureIndex int
	threshold    float64
	left         *node
	right        *node
	isLeaf       bool
}

// tree holds configurations, randomness provider, and root reference for an individual decision tree.
type tree struct {
	root                   *node
	maximumDepth           int
	minimumSamples         int
	numberOfFeaturesToPick int
	numberOfFeatures       *node
	randomEngine           *rand.Rand
}

// Point represents a multi-dimensional feature vector paired with an integer target class label.
type Point struct {
	Label    int
	Features []float64
}

// RandomForest manages the ensemble parameters, dimensionality metadata, and underlying decision trees.
type RandomForest struct {
	numberOfTrees     int
	maximumDepth      int
	minimumSamples    int
	subsampleRatio    float64
	expectedDimension int
	trees             []*tree
}

// calculateGiniInpurity computes the Gini impurity score for a collection of data points.
func calculateGiniInpurity(dataset []Point) float64 {
	// Return zero impurity if dataset is empty.
	if len(dataset) == 0 {
		return 0.0
	}

	counts := make(map[int]int)

	// Count occurrences of each class label in the dataset.
	for _, point := range dataset {
		counts[point.Label]++
	}

	impurity := 1.0
	total := float64(len(dataset))

	// Compute target Gini impurity score by subtracting squared class probabilities.
	for _, count := range counts {
		probability := float64(count) / total
		impurity -= probability * probability
	}

	return impurity
}

// retrieveMajorityLabel identifies and returns the most frequent class label in a dataset segment.
func retrieveMajorityLabel(dataset []Point) int {
	// Fallback to default class if empty set is provided.
	if len(dataset) == 0 {
		return 0
	}

	counts := make(map[int]int)
	majorityLabel := dataset[0].Label

	maximumCount := 0

	// Aggregate occurrences and track majority label concurrently.
	for _, point := range dataset {
		counts[point.Label]++

		if counts[point.Label] > maximumCount {
			maximumCount = counts[point.Label]
			majorityLabel = point.Label
		}
	}

	return majorityLabel
}

// splitDataset partitions a dataset into left and right subsets based on a feature index and split threshold.
func splitDataset(dataset []Point, featureIndex int, threshold float64) ([]Point, []Point) {
	var left []Point
	var right []Point

	// Distribute points according to target threshold boundary.
	for _, point := range dataset {
		if featureIndex < len(point.Features) && point.Features[featureIndex] <= threshold {
			left = append(left, point)
		} else {
			right = append(right, point)
		}
	}

	return left, right
}

// buildTree recursively constructs decision tree nodes by selecting feature splits that minimize Gini impurity.
func (tree *tree) buildTree(dataset []Point, depth int) *node {
	// Guard against empty input data.
	if len(dataset) == 0 {
		return nil
	}

	// Terminate recursion and create leaf node if max depth, min samples, or zero impurity condition is reached.
	if depth >= tree.maximumDepth || len(dataset) < tree.minimumSamples || calculateGiniInpurity(dataset) == 0 {
		return &node{
			label:  retrieveMajorityLabel(dataset),
			isLeaf: true,
		}
	}

	// Randomly permute and select subset of available feature indices.
	totalFeatures := len(dataset[0].Features)
	featureIndices := tree.randomEngine.Perm(totalFeatures)[:tree.numberOfFeaturesToPick]

	lowestGiniInpurity := math.MaxFloat64

	selectedFeatureIndex := -1
	selectedThreshold := 0.0

	var selectedLeft []Point
	var selectedRight []Point

	// Evaluate candidate feature split thresholds.
	for _, featureIndex := range featureIndices {
		featureValues := make([]float64, len(dataset))

		// Extract values for the selected feature across all samples.
		for index, point := range dataset {
			featureValues[index] = point.Features[featureIndex]
		}

		// Sort values to evaluate adjacent threshold midpoints.
		sort.Float64s(featureValues)

		// Check candidate split midpoints between adjacent sorted features.
		for index := 0; index < len(featureValues)-1; index++ {
			threshold := (featureValues[index] + featureValues[index+1]) / 2.0
			leftSet, rightSet := splitDataset(dataset, featureIndex, threshold)

			// Skip invalid splits that fail to divide dataset samples.
			if len(leftSet) == 0 || len(rightSet) == 0 {
				continue
			}

			// Calculate weighted Gini impurity for the proposed split.
			leftWeight := float64(len(leftSet)) / float64(len(dataset))
			rightWeight := float64(len(rightSet)) / float64(len(dataset))

			splitGiniInpurity := leftWeight*calculateGiniInpurity(leftSet) + rightWeight*calculateGiniInpurity(rightSet)

			// Retain split configuration yielding lowest overall Gini impurity score.
			if splitGiniInpurity < lowestGiniInpurity {
				lowestGiniInpurity = splitGiniInpurity

				selectedFeatureIndex = featureIndex
				selectedThreshold = threshold

				selectedLeft = leftSet
				selectedRight = rightSet
			}
		}
	}

	// Fallback to leaf node creation if no valid split point was discovered.
	if selectedFeatureIndex == -1 {
		return &node{
			label:  retrieveMajorityLabel(dataset),
			isLeaf: true,
		}
	}

	// Recursively construct left and right child subtrees.
	leftChild := tree.buildTree(selectedLeft, depth+1)
	rightChild := tree.buildTree(selectedRight, depth+1)

	// Construct internal node containing optimal decision split parameters.
	return &node{
		featureIndex: selectedFeatureIndex,
		threshold:    selectedThreshold,
		left:         leftChild,
		right:        rightChild,
		isLeaf:       false,
	}
}

// predict recursively traverses a decision tree structure to infer a class label for a feature vector.
func (tree *tree) predict(node *node, features []float64) int {
	// Guard against nil node references.
	if node == nil {
		return 0
	}

	// Return assigned label upon reaching leaf node.
	if node.isLeaf {
		return node.label
	}

	// Return label if requested feature index exceeds provided feature vector dimensions.
	if node.featureIndex >= len(features) {
		return node.label
	}

	// Recursively traverse left or right branch based on decision threshold.
	if features[node.featureIndex] <= node.threshold {
		return tree.predict(node.left, features)
	}

	return tree.predict(node.right, features)
}

// NewForest validates configuration values and initializes an un-fitted RandomForest structure.
func NewForest(numberOfTrees, maximumDepth, minimumSamples int, subsampleRatio float64) (*RandomForest, error) {
	// Validate total count of trees.
	if numberOfTrees <= 0 {
		return nil, errors.New("number of trees must be greater than 0")
	}

	// Validate tree maximum depth bound.
	if maximumDepth < 1 {
		return nil, errors.New("maximum depth must be at least one")
	}

	// Validate subsample ratio range constraints.
	if subsampleRatio <= 0.0 || subsampleRatio > 1.0 {
		return nil, errors.New("subsample ratio must be in the range (0.0, 1.0]")
	}

	// Return initialized instance.
	return &RandomForest{
		numberOfTrees:  numberOfTrees,
		maximumDepth:   maximumDepth,
		minimumSamples: minimumSamples,
		subsampleRatio: subsampleRatio,
	}, nil
}

// Fit constructs the decision tree ensemble by building individual trees on random subsamples.
func (forest *RandomForest) Fit(dataset []Point) error {
	// Ensure dataset contains training points.
	if len(dataset) == 0 {
		return errors.New("dataset cannot be empty")
	}

	// Ensure training point features are not empty.
	totalDimension := len(dataset[0].Features)

	if totalDimension == 0 {
		return errors.New("dataset features cannot be empty")
	}

	// Store target input feature dimensions for future validation checks.
	forest.expectedDimension = totalDimension

	// Determine feature sub-sampling count using standard square root rule.
	numberOfFeaturesToPick := int(math.Sqrt(float64(totalDimension)))

	if numberOfFeaturesToPick < 1 {
		numberOfFeaturesToPick = 1
	}

	// Compute training subsample count per tree.
	sampleCount := int(float64(len(dataset)) * forest.subsampleRatio)

	if sampleCount < 1 {
		sampleCount = 1
	}

	// Allocate ensemble slice holding decision tree instances.
	forest.trees = make([]*tree, forest.numberOfTrees)

	// Construct each decision tree instance using bootstrapped sample sets.
	for index := 0; index < forest.numberOfTrees; index++ {
		// Initialize isolated random source engine per tree to maintain deterministic independence.
		randomEngine := rand.New(rand.NewPCG(uint64(index), uint64(index+1)))

		subsample := make([]Point, sampleCount)

		// Draw random samples with replacement.
		for sampleIndex := 0; sampleIndex < sampleCount; sampleIndex++ {
			randomIndex := randomEngine.IntN(len(dataset))
			subsample[sampleIndex] = dataset[randomIndex]
		}

		// Initialize individual decision tree instance.
		tree := &tree{
			maximumDepth:           forest.maximumDepth,
			minimumSamples:         forest.minimumSamples,
			numberOfFeaturesToPick: numberOfFeaturesToPick,
			randomEngine:           randomEngine,
		}

		// Build recursive binary tree structure and record root node.
		tree.root = tree.buildTree(subsample, 0)
		forest.trees[index] = tree
	}

	return nil
}

// Predict gathers inferences from all constructed trees and yields the class receiving the majority vote.
func (forest *RandomForest) Predict(features []float64) (int, error) {
	// Ensure model was trained prior to running predictions.
	if len(forest.trees) == 0 {
		return 0, errors.New("model is not fitted yet")
	}

	// Validate target input feature length against stored model dimension.
	if len(features) != forest.expectedDimension {
		return 0, errors.New("feature dimension mismatch")
	}

	labelCounts := make(map[int]int)

	majorityCount := 0
	predictedLabel := 0

	// Aggregate predictions across all trained decision trees.
	for _, tree := range forest.trees {
		label := tree.predict(tree.root, features)
		labelCounts[label]++

		// Track and update leading class majority vote count.
		if labelCounts[label] > majorityCount {
			majorityCount = labelCounts[label]
			predictedLabel = label
		}
	}

	return predictedLabel, nil
}
