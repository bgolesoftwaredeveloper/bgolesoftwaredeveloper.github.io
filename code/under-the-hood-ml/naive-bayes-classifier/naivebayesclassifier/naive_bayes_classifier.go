// -----------------------------------------------------------------------------
// Package: naivebayesclassifier
// File: naive_bayes_classifier.go
//
// Description:
//
//	This file provides a clean, dependency-free implementation of the Multinomial
//	Naive Bayes algorithm for text classification and categorical analysis.
//
//	The primary purpose of this classifier is to assign categorical labels to input text
//	documents by applying Bayes' theorem with the assumption of strong independence between
//	individual features (tokens). It computes prior probabilities from target class frequencies
//	and conditional feature likelihoods from token distributions.
//
//	The algorithm operates in two main phases:
//
//	  1. Training — updates class occurrence counters, tracks per-class word frequency distributions,
//	     and dynamically expands the globally tracked vocabulary set.
//	  2. Prediction — calculates log-posterior likelihoods across all known classes using Laplace
//	     (add-one) smoothing to select the class maximizing conditional probability.
//
// Key Features & Design Principles:
//
//   - Log Probability Space: Operates entirely in log-space (`math.Log`) to prevent floating-point
//     underflow during multiplication of long sequences of conditional probabilities.
//   - Zero-Probability Mitigation: Incorporates Laplace (add-one) smoothing to handle unseen
//     vocabulary tokens gracefully during inference.
//   - Tokenization Standard: Uses whitespace splitting and lowercasing for generalized feature extraction.
//   - Self-Contained Implementation: Relies exclusively on standard library primitives (`math` and `strings`).
//
// Operational Limits & Bounds:
//
//   - Vocabulary Growth: Memory allocation scales dynamically with unique token entries across all trained samples.
//   - Class Scoring: Returned results reflect relative maximum log-posterior likelihoods.
//
// Intended Use:
//
//	This package serves as a core machine learning building block for zero-dependency
//	Go applications requiring fast text classification, such as spam filtering or sentiment analysis.
//
// -----------------------------------------------------------------------------
package naivebayesclassifier

import (
	"math"
	"strings"
)

// Classifier manages probability distributions, term frequencies,
// target class counts, and vocabulary metadata required for inference.
type Classifier struct {
	classes    map[string]int
	features   map[string]map[string]int
	classSize  map[string]int
	vocabulary map[string]bool
	documents  int
}

// tokenize splits input text into lowercased tokens using standard whitespace separation.
func tokenize(text string) []string {
	cleanText := strings.TrimSpace(text)

	if cleanText == "" {
		return nil
	}

	return strings.Fields(strings.ToLower(cleanText))
}

// NewClassifier initializes and returns a ready-to-use Classifier instance.
func NewClassifier() *Classifier {
	return &Classifier{
		classes:    make(map[string]int),
		features:   make(map[string]map[string]int),
		classSize:  make(map[string]int),
		vocabulary: make(map[string]bool),
	}
}

// Train updates the internal frequency distributions and vocabulary counters with a labeled document.
func (classifier *Classifier) Train(text, class string) {
	// Guard against nil receiver.
	if classifier == nil {
		return
	}

	targetClass := strings.TrimSpace(class)

	tokens := tokenize(text)

	// Ignore invalid calls with empty class names or empty token sets.
	if targetClass == "" || len(tokens) == 0 {
		return
	}

	// Lazy initialization check in case instance was constructed.
	if classifier.classes == nil {
		classifier.classes = make(map[string]int)
	}

	if classifier.features == nil {
		classifier.features = make(map[string]map[string]int)
	}

	if classifier.classSize == nil {
		classifier.classSize = make(map[string]int)
	}

	if classifier.vocabulary == nil {
		classifier.vocabulary = make(map[string]bool)
	}

	classifier.classes[targetClass]++
	classifier.documents++

	// Lazily initialize feature count map for newly observed classes.
	if _, ok := classifier.features[targetClass]; !ok {
		classifier.features[class] = make(map[string]int)
	}

	// Update word occurrence counters for the target class and expand global vocabulary tracking.
	for _, word := range tokens {
		classifier.features[targetClass][word]++
		classifier.classSize[targetClass]++
		classifier.vocabulary[word] = true
	}
}

// Predict evaluates input text and returns the target class label with the highest log-posterior score.
func (classifier *Classifier) Predict(text string) string {
	// Guard against nil receiver or untrained classifier.
	if classifier == nil || classifier.documents == 0 || len(classifier.classes) == 0 {
		return ""
	}

	tokens := tokenize(text)

	// Return empty result if input contains no valid tokens.
	if len(tokens) == 0 {
		return ""
	}

	vocabularySize := float64(len(classifier.vocabulary))

	bestClass := ""
	maximumScore := -math.MaxFloat64

	// Evaluate log-posterior probabilities for each candidate class.
	for class, count := range classifier.classes {
		// Calculate log prior probability: log(P(Class))
		prior := float64(count) / float64(classifier.documents)
		score := math.Log(prior)

		// Accumulate log conditional likelihoods: log(P(Word | Class))
		for _, word := range tokens {
			wordCount := float64(classifier.features[class][word])
			totalWords := float64(classifier.classSize[class])

			// Apply Laplace (add-one) smoothing to prevent zero-probability evaluation.
			probability := (wordCount + 1.0) / (totalWords + vocabularySize)
			score += math.Log(probability)
		}

		// Select class that yields the maximum log-posterior likelihood.
		if score > maximumScore {
			maximumScore = score
			bestClass = class
		}
	}

	return bestClass
}
