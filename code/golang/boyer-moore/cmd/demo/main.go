// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a simple executable demonstration of the Boyer–Moore
//	string searching algorithm. It shows how to invoke the BoyerMoore function
//	from the boyermoore package and interpret the returned match indices.
//
//	The demo intentionally uses small, readable example strings to make the
//	algorithm’s behavior easy to verify by inspection.
//
// Notes:
//   - Output indices are byte offsets within the input text.
//   - This demo does not perform Unicode rune indexing.
//   - The program is intended for illustrative and validation purposes only.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/boyer-moore/boyermoore"
)

func main() {
	// Define the input text to be searched.
	text := "ABAAABCDABCDAAAABCD"

	// Define the pattern to locate within the text.
	pattern := "ABCD"

	// Execute the Boyer–Moore search algorithm.
	matchStartIndices := boyermoore.BoyerMoore(text, pattern)

	// Display the input values for clarity.
	fmt.Println("Text:", text)
	fmt.Println("Pattern:", pattern)
	fmt.Println()

	// Report the search results.
	if len(matchStartIndices) == 0 {
		fmt.Println("No matches found.")

		return
	}

	fmt.Println("Pattern found at byte indices:")

	for _, index := range matchStartIndices {
		// Print each match location on its own line for readability.
		fmt.Printf("  - %d\n", index)
	}
}
