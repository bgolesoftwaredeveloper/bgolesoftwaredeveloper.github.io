// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This executable provides a demonstration entry point for the custom Multinomial
//	Naive Bayes text classifier implementation.
//
//	The primary purpose of this file is to showcase model initialization, multi-class
//	training, and target class inference on natural language feedback samples.
//
//	The demonstration workflow proceeds as follows:
//
//	  1. Instantiation — constructs a zero-dependency `Classifier` instance.
//	  2. Corpus Training — populates feature distributions across `positive`, `negative`,
//	     and `neutral` sentiment categories.
//	  3. Inference & Display — evaluates unseen phrases against the trained distribution,
//	     printing target predictions to standard output.
//
// Key Features & Design Principles:
//
//   - Multi-Class Support: Exercises classification across three distinct sentiment targets.
//   - Zero Allocation Wrappers: Feeds inline string primitives directly into classifier APIs.
//   - Formatted Diagnostics: Utilizes padded text alignment for clean CLI terminal output.
//
// Operational Limits & Bounds:
//
//   - Mock Dataset Scope: Serves as a localized functional test suite rather than a production model.
//
// Intended Use:
//
//	This main entry point serves as an example client for testing, verifying, and integrating
//	the `naivebayesclassifier` package into larger Go applications.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/naivebayesclassifier/naivebayesclassifier"
)

func main() {
	classifier := naivebayesclassifier.NewClassifier()

	// Positive data.
	classifier.Train("great quality product fast shipping highly recommended", "positive")
	classifier.Train("excellent customer service solved my issue quickly", "positive")
	classifier.Train("loved the experience fantastic quality and support", "positive")

	// Negative data.
	classifier.Train("terrible experience product arrived broken and late", "negative")
	classifier.Train("poor quality worst customer service ever", "negative")
	classifier.Train("broken item useless product very disappointed", "negative")

	// Neutral training data.
	classifier.Train("item received as ordered standard packaging", "neutral")
	classifier.Train("delivery took three days average quality", "neutral")
	classifier.Train("product matches description nothing special", "neutral")

	// Compare against phrases.
	phrases := []string{
		"fantastic customer service and fast delivery",
		"terrible quality product arrived broken",
		"item delivered on time standard product",
		"worst experience ever very disappointed",
	}

	fmt.Println("--- Sentiment Analysis Predictions ---")

	for _, phrase := range phrases {
		predictedClass := classifier.Predict(phrase)

		fmt.Printf("\tText: %-45q -> Category: %s\n", phrase, predictedClass)
	}
}
