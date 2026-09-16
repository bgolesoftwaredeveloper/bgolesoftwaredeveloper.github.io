// -----------------------------------------------------------------------------
// Package: boyermoore
// File: boyer_moore_test.go
//
// Description:
//
//		This file defines unit tests for the Boyer–Moore string searching
//		implementation. The tests validate correctness across common,
//		edge-case, and boundary scenarios, ensuring that both preprocessing
//		heuristics and the main search routine behave as expected.
//
//		The test cases focus on verifying:
//	  - Correct detection of multiple pattern matches.
//	  - Proper handling of empty patterns.
//	  - Safe behavior when the pattern exceeds text length.
//	  - Accurate byte-offset reporting.
//
// Notes:
//   - All indices are validated as byte offsets.
//   - Tests use Arrange / Act / Assert structure for clarity.
//   - The testing parameter is explicitly named `test`.
//
// -----------------------------------------------------------------------------
package boyermoore

import (
	"reflect"
	"testing"
)

// TestBoyerMoore_MultipleMatchesFound verifies that the algorithm correctly
// identifies all occurrences of a pattern that appears multiple times within
// the input text.
func TestBoyerMoore_MultipleMatchesFound(test *testing.T) {
	// Arrange.
	inputText := "ABAAABCDABCDAAAABCD"
	searchPattern := "ABCD"
	expectedMatchStartIndices := []int{4, 8, 15}

	// Act.
	actualMatchStartIndices := BoyerMoore(inputText, searchPattern)

	// Assert.
	if !reflect.DeepEqual(actualMatchStartIndices, expectedMatchStartIndices) {
		test.Fatalf("unexpected match indices.\nexpected: %v\ngot:      %v",
			expectedMatchStartIndices,
			actualMatchStartIndices,
		)
	}
}

// TestBoyerMoore_NoMatchesFound verifies that the algorithm returns an empty
// result slice when the pattern does not occur in the text.
func TestBoyerMoore_NoMatchesFound(test *testing.T) {
	// Arrange.
	inputText := "THIS IS A SIMPLE STRING"
	searchPattern := "NOTPRESENT"
	expectedMatchStartIndices := []int{}

	// Act.
	actualMatchStartIndices := BoyerMoore(inputText, searchPattern)

	// Assert.
	if !reflect.DeepEqual(actualMatchStartIndices, expectedMatchStartIndices) {
		test.Fatalf("unexpected match indices.\nexpected: %v\ngot:      %v",
			expectedMatchStartIndices,
			actualMatchStartIndices,
		)
	}
}

// TestBoyerMoore_EmptyPattern verifies that an empty pattern produces a match
// at every possible byte boundary within the text.
func TestBoyerMoore_EmptyPattern(test *testing.T) {
	// Arrange.
	inputText := "ABC"
	searchPattern := ""
	expectedMatchStartIndices := []int{0, 1, 2, 3}

	// Act.
	actualMatchStartIndices := BoyerMoore(inputText, searchPattern)

	// Assert.
	if !reflect.DeepEqual(actualMatchStartIndices, expectedMatchStartIndices) {
		test.Fatalf("unexpected match indices for empty pattern.\nexpected: %v\ngot:      %v",
			expectedMatchStartIndices,
			actualMatchStartIndices,
		)
	}
}

// TestBoyerMoore_PatternLongerThanText verifies that the algorithm safely
// returns nil when the pattern length exceeds the text length.
func TestBoyerMoore_PatternLongerThanText(test *testing.T) {
	// Arrange.
	inputText := "ABC"
	searchPattern := "ABCDEFG"

	// Act.
	actualMatchStartIndices := BoyerMoore(inputText, searchPattern)

	// Assert.
	if actualMatchStartIndices != nil {
		test.Fatalf("expected nil result when pattern is longer than text, got: %v",
			actualMatchStartIndices,
		)
	}
}

// TestBuildLastOccurenceTable_BasicValidation verifies that the last-occurrence
// table correctly records the final index of each byte present in the pattern.
func TestBuildLastOccurenceTable_BasicValidation(test *testing.T) {
	// Arrange.
	patternBytes := []byte("ABCA")

	expectedLastIndexForA := 3
	expectedLastIndexForB := 1
	expectedLastIndexForC := 2

	// Act.
	lastOccurenceTable := BuildLastOccurenceTable(patternBytes)

	// Assert.
	if lastOccurenceTable['A'] != expectedLastIndexForA {
		test.Fatalf("unexpected last occurrence for 'A': expected %d, got %d",
			expectedLastIndexForA,
			lastOccurenceTable['A'],
		)
	}

	if lastOccurenceTable['B'] != expectedLastIndexForB {
		test.Fatalf("unexpected last occurrence for 'B': expected %d, got %d",
			expectedLastIndexForB,
			lastOccurenceTable['B'],
		)
	}

	if lastOccurenceTable['C'] != expectedLastIndexForC {
		test.Fatalf("unexpected last occurrence for 'C': expected %d, got %d",
			expectedLastIndexForC,
			lastOccurenceTable['C'],
		)
	}
}

// TestBuildGoodSuffixTable_LengthValidation verifies that the good-suffix
// shift table has the correct length relative to the pattern size.
func TestBuildGoodSuffixTable_LengthValidation(test *testing.T) {
	// Arrange.
	patternBytes := []byte("ABCD")
	expectedTableLength := len(patternBytes) + 1

	// Act.
	goodSuffixTable := BuildGoodSuffixTable(patternBytes)

	// Assert.
	if len(goodSuffixTable) != expectedTableLength {
		test.Fatalf("unexpected good-suffix table length.\nexpected: %d\ngot:      %d",
			expectedTableLength,
			len(goodSuffixTable),
		)
	}
}
