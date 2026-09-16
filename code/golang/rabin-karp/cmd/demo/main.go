// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a functional demonstration of the Rabin-Karp
//	multi-pattern search implementation.
//
//	It simulates a real-world scenario by searching for multiple keywords
//	within a block of text, highlighting the algorithm's ability to locate
//	overlapping or repeated patterns in a single pass.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/rabin-karp/rabinkarp"
)

func main() {
	// Define a sample text body to be searched.
	text := "The fox is clever, but the fox is also very fast. The fox jumps."

	// Define a list of patterns to search for.
	// Note: In this implementation, all patterns must share the same length.
	targets := []string{"fox", "the"}

	// Execute the multi-pattern search.
	// This performs a single pass over the sampleText using rolling hashes.
	matches := rabinkarp.MultiPatternSearch(text, targets)

	// Display the results of the search
	fmt.Printf("Input text: %s\n", text)
	fmt.Printf("Searching for: %v\n\n", targets)

	if len(matches) == 0 {
		fmt.Println("No matches fouund.")

		return
	}

	fmt.Printf("Search successful! Found %d matches:\n", len(matches))
	fmt.Println("-------------------------------------------------")

	// Iterate through the results and print the index and identified pattern.
	for _, match := range matches {
		fmt.Printf("At Index %2d: Found pattern '%s'\n", match.Index, match.Pattern)
	}

	fmt.Println("-------------------------------------------------")

	// Additional demonstration: Search for something that doesn't exist.
	missingTarget := []string{"cat"}
	noResults := rabinkarp.MultiPatternSearch(text, missingTarget)

	if len(noResults) == 0 {
		fmt.Printf("\nVerified: Search for '%v' returned zero results.\n", missingTarget)
	}
}
