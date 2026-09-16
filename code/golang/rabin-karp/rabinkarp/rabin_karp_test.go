// -----------------------------------------------------------------------------
// Package: rabinkarp
// File: rabinkarp_test.go
//
// Description:
//
//	This file contains a comprehensive suite of 15 unique unit tests for the
//	MultiPatternSearch function.
//
//	The tests are designed to validate the robustness of the rolling hash
//	implementation across various edge cases, including overlapping patterns,
//	multi-byte Unicode characters, empty inputs, and hash collision scenarios.
//
//	Each test follows a strict Arrange-Act-Assert pattern to ensure clarity
//	and maintainability of the testing logic.
//
// -----------------------------------------------------------------------------
package rabinkarp

import (
	"reflect"
	"testing"
)

// TestStandardSearch verifies that the algorithm can find multiple distinct
// patterns in a simple sentence.
func TestStandardSearch(test *testing.T) {
	// Arrange.
	body := "the quick brown fox"
	patterns := []string{"the", "fox"}
	expectedResults := []Result{
		{Index: 0, Pattern: "the"},
		{Index: 16, Pattern: "fox"},
	}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if !reflect.DeepEqual(actualResults, expectedResults) {
		test.Errorf("Standard search failed. Expected %v, got %v", expectedResults, actualResults)
	}
}

// TestOverlappingPatterns ensures the algorithm identifies patterns that
// share characters (e.g., "aba" in "ababa").
func TestOverlappingPatterns(test *testing.T) {
	// Arrange.
	body := "ababa"
	patterns := []string{"aba"}
	expectedResults := []Result{
		{Index: 0, Pattern: "aba"},
		{Index: 2, Pattern: "aba"},
	}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if len(actualResults) != len(expectedResults) {
		test.Errorf("Overlapping search failed. Expected %d matches, got %d", len(expectedResults), len(actualResults))
	}
}

// TestEmptyTextBody checks that the function returns nil when the text
// body to search is an empty string.
func TestEmptyTextBody(test *testing.T) {
	// Arrange.
	body := ""
	patterns := []string{"pattern"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if actualResults != nil {
		test.Error("Expected nil result for empty body.")
	}
}

// TestEmptyPatternsList checks that the function returns nil when no
// search patterns are provided in the slice.
func TestEmptyPatternsList(test *testing.T) {
	// Arrange.
	body := "some text"
	var patterns []string

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if actualResults != nil {
		test.Error("Expected nil result for empty patterns list.")
	}
}

// TestPatternLongerThanText ensures the algorithm handles cases where
// the search term is impossible to find due to length constraints.
func TestPatternLongerThanText(test *testing.T) {
	// Arrange.
	body := "short"
	patterns := []string{"longerthanbody"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if actualResults != nil {
		test.Error("Expected nil result when pattern is longer than body.")
	}
}

// TestCaseSensitivity verifies that capitalization differences result
// in distinct hash values.
func TestCaseSensitivity(test *testing.T) {
	// Arrange.
	body := "Apple apple APPLE"
	patterns := []string{"apple"}
	expectedResults := []Result{{Index: 6, Pattern: "apple"}}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if !reflect.DeepEqual(actualResults, expectedResults) {
		test.Errorf("Case sensitivity failed. Got %v", actualResults)
	}
}

// TestUnicodeCharacterSupport validates that the algorithm correctly
// handles multi-byte UTF-8 characters such as emojis.
func TestUnicodeCharacterSupport(test *testing.T) {
	// Arrange.
	body := "hello 🌍 world 🌍"
	patterns := []string{"🌍"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	expectedCount := 2

	if len(actualResults) != expectedCount {
		test.Errorf("Unicode search failed. Expected %d emojis, found %d", expectedCount, len(actualResults))
	}
}

// TestDuplicatePatternsInInput ensures that if the same pattern is
// provided twice in the input slice, it records both matches at each index.
func TestDuplicatePatternsInInput(test *testing.T) {
	// Arrange.
	body := "gold"
	patterns := []string{"gold", "gold"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	expectedCount := 2

	if len(actualResults) != expectedCount {
		test.Errorf("Duplicate input patterns should yield duplicate results. Got %d", len(actualResults))
	}
}

// TestNoMatchFound verifies the behavior when the patterns are
// completely absent from the text body.
func TestNoMatchFound(test *testing.T) {
	// Arrange.
	body := "abcdefg"
	patterns := []string{"xyz12"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if len(actualResults) != 0 {
		test.Error("Expected zero matches for non-existent pattern.")
	}
}

// TestAtStartAndEnd validates that patterns located at the absolute
// boundaries of the body string are found.
func TestAtStartAndEnd(test *testing.T) {
	// Arrange.
	body := "start middle end"
	patterns := []string{"start", "e end"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	foundStart := false
	foundEnd := false

	for resultIndex := 0; resultIndex < len(actualResults); resultIndex++ {
		match := actualResults[resultIndex]

		if match.Index == 0 {
			foundStart = true
		}

		if match.Index == 11 {
			foundEnd = true
		}
	}

	if !foundStart || !foundEnd {
		test.Error("Failed to find patterns at the boundaries of the body.")
	}
}

// TestSingleCharacterPatterns ensures the rolling hash logic holds
// up for minimum length patterns of one byte.
func TestSingleCharacterPatterns(test *testing.T) {
	// Arrange.
	body := "aaaaa"
	patterns := []string{"a"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	expectedCount := 5

	if len(actualResults) != expectedCount {
		test.Errorf("Expected %d matches for single char, got %d", expectedCount, len(actualResults))
	}
}

// TestConsecutivePatterns ensures matches that touch each other
// without overlapping are correctly identified.
func TestConsecutivePatterns(test *testing.T) {
	// Arrange.
	body := "catcatcat"
	patterns := []string{"cat"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	for matchIndex := 0; matchIndex < len(actualResults); matchIndex++ {
		expectedOffset := matchIndex * 3

		if actualResults[matchIndex].Index != expectedOffset {
			test.Errorf("Consecutive match failed at index %d", matchIndex)
		}
	}
}

// TestSpecialCharacters validates searching for symbols and whitespace
// within the body string.
func TestSpecialCharacters(test *testing.T) {
	// Arrange.
	body := "100% $ (price)"
	patterns := []string{"100%", "(pri"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	expectedCount := 2

	if len(actualResults) != expectedCount {
		test.Errorf("Special character search failed. Got %d matches", len(actualResults))
	}
}

// TestLargeBodyScan simulates a larger text volume to ensure no
// overflow issues occur during long rolling sequences.
func TestLargeBodyScan(test *testing.T) {
	// Arrange.
	repeatCount := 500
	body := ""

	for iteration := 0; iteration < repeatCount; iteration++ {
		body += "data "
	}

	patterns := []string{"data "}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	if len(actualResults) != repeatCount {
		test.Errorf("Large body scan failed. Expected %d, got %d", repeatCount, len(actualResults))
	}
}

// TestCollisionResolution ensures that if two strings hypothetically
// produced the same hash, the character verification correctly ignores the wrong one.
func TestCollisionResolution(test *testing.T) {
	// Arrange.
	body := "actual"
	patterns := []string{"actual", "fakeit"}

	// Act.
	actualResults := MultiPatternSearch(body, patterns)

	// Assert.
	for resultIndex := 0; resultIndex < len(actualResults); resultIndex++ {
		if actualResults[resultIndex].Pattern == "fakeit" {
			test.Error("Collision resolution failed: matched a pattern not present in the body.")
		}
	}
}
