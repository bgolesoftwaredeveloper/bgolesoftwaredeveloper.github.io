// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file serves as the main entry point demonstrating how to train and execute predictions
//	using the custom Random Forest classification library.
//
//	The primary purpose of this application is to construct a labeled training dataset,
//	instantiate a configured Random Forest model, fit the model on sample feature vectors,
//	and infer the class label for an unseen feature point.
//
//	The execution flow follows three primary steps:
//
//	  1. Initialization — defines training points and configures model hyper-parameters.
//	  2. Training — fits the Random Forest ensemble on the provided dataset.
//	  3. Inference — predicts and prints the resulting target class label for test features.
//
// Key Features & Design Principles:
//
//   - Self-Contained Example: Provides a simple, working demonstration of the classification pipeline.
//   - Error Handling: Implements strict error logging for initialization, training, and inference.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"
	"log"

	"github.com/bgolesoftwaredeveloper/randomforest/randomforest"
)

func main() {
	// Define a training dataset containing feature vectors and corresponding binary class labels.
	dataset := []randomforest.Point{
		{Label: 0, Features: []float64{2.5, 1.5}},
		{Label: 0, Features: []float64{1.0, 1.0}},
		{Label: 0, Features: []float64{1.5, 2.0}},
		{Label: 1, Features: []float64{7.0, 8.0}},
		{Label: 1, Features: []float64{8.5, 9.0}},
		{Label: 1, Features: []float64{6.0, 7.5}},
	}

	// Instantiate a new Random Forest model with configured hyper-parameters.
	forest, err := randomforest.NewForest(10, 5, 2, 0.8)

	// Validate forest initialization step.
	if err != nil {
		log.Fatalf("failed to initialize random forest: %v", err)
	}

	// Fit the decision tree ensemble using the prepared training dataset.
	if err := forest.Fit(dataset); err != nil {
		log.Fatalf("failed to fit model: %v", err)
	}

	// Define test feature vector for target prediction evaluation.
	sampleFeatures := []float64{7.5, 8.2}
	predictedLabel, err := forest.Predict(sampleFeatures)

	// Validate prediction evaluation step.
	if err != nil {
		log.Fatalf("failed to make prediction: %v", err)
	}

	// Output predicted class label for evaluated test features.
	fmt.Printf("features: %v | predicted label: %d\n", sampleFeatures, predictedLabel)
}
