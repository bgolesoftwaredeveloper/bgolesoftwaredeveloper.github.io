// -----------------------------------------------------------------------------
// Package: boyermoore
// File: boyer_moore.go
//
// Description:
//
//	This file provides an implementation of the Boyer–Moore string searching
//	algorithm using both the bad-character rule and the good-suffix rule.
//	The implementation operates on byte slices derived from Go strings and
//	returns all starting indices at which a pattern occurs within a text.
//
//	The Boyer–Moore algorithm improves practical search performance by
//	skipping ahead in the text when mismatches occur, rather than checking
//	every alignment sequentially.
//
// Notes:
//   - This implementation treats strings as raw byte sequences.
//   - Returned indices are byte offsets, not rune offsets.
//   - Both preprocessing tables are built once per pattern.
//   - Defensive guards exist for empty patterns and oversized patterns.
//
// -----------------------------------------------------------------------------
package boyermoore

// byteAlphabetCardinality defines the size of the byte alphabet.
// Since Go bytes range from 0–255, this value is fixed.
const byteAlphabetCardinality = 256

// BuildLastOccurenceTable constructs the bad-character table for the
// Boyer–Moore algorithm.
// For each possible byte value, the table stores the index of the
// last occurrence of that byte in the pattern.
// Bytes not present in the pattern are assigned -1.
func BuildLastOccurenceTable(pattern []byte) [byteAlphabetCardinality]int {
	var lastOccurenceTable [byteAlphabetCardinality]int

	// Initialize all entries to -1, indicating "not present".
	for tableIndex := 0; tableIndex < byteAlphabetCardinality; tableIndex++ {
		lastOccurenceTable[tableIndex] = -1
	}

	// Record the last index at which each byte appears in the pattern.
	for patternIndex := 0; patternIndex < len(pattern); patternIndex++ {
		lastOccurenceTable[pattern[patternIndex]] = patternIndex
	}

	return lastOccurenceTable
}

// BuildGoodSuffixTable constructs the good-suffix shift table for the
// Boyer–Moore algorithm.
// The resulting table indicates how far the pattern can be shifted when
// a mismatch occurs at a given position, based on suffix matches and
// border relationships within the pattern.
func BuildGoodSuffixTable(pattern []byte) []int {
	patternLength := len(pattern)

	// Shift table stores the computed shift distances.
	shiftTable := make([]int, patternLength+1)
	borderPositionTable := make([]int, patternLength+1)

	// Initialize indices to begin suffix analysis from the end of the pattern.
	currentPatternIndex := patternLength
	currentBorderIndex := patternLength + 1
	borderPositionTable[currentPatternIndex] = currentBorderIndex

	// First phase: compute border positions and partial shifts.
	for currentPatternIndex > 0 {
		// Walk back through borders while characters mismatch.
		for currentBorderIndex <= patternLength && pattern[currentPatternIndex-1] != pattern[currentBorderIndex-1] {
			// Assign shift only if it has not already been set.
			if shiftTable[currentBorderIndex] == 0 {
				shiftTable[currentBorderIndex] = currentBorderIndex - currentPatternIndex
			}

			// Fall back to the next widest border.
			currentBorderIndex = borderPositionTable[currentBorderIndex]
		}

		// Advance both indices inward.
		currentPatternIndex--
		currentBorderIndex--

		borderPositionTable[currentPatternIndex] = currentBorderIndex
	}

	// Second phase: fill remaining shifts using the widest border.
	widestBorderIndex := borderPositionTable[0]

	for tableIndex := 0; tableIndex <= patternLength; tableIndex++ {
		// Assign widest border shift where no specific shift was computed.
		if shiftTable[tableIndex] == 0 {
			shiftTable[tableIndex] = widestBorderIndex
		}

		// Move to the next border when required.
		if tableIndex == widestBorderIndex {
			widestBorderIndex = borderPositionTable[widestBorderIndex]
		}
	}

	return shiftTable
}

// BoyerMoore searches for all occurrences of pattern within text using the
// Boyer–Moore algorithm.
// The function returns a slice of byte offsets at which the pattern begins
// in the text. If the pattern is empty, all possible positions are returned.
// If the pattern is longer than the text, nil is returned.
func BoyerMoore(text string, pattern string) []int {
	textBytes := []byte(text)
	patternBytes := []byte(pattern)

	textLength := len(textBytes)
	patternLength := len(patternBytes)

	// Handle empty pattern by returning all valid positions.
	if patternLength == 0 {
		allPositions := make([]int, textLength+1)

		for positionIndex := 0; positionIndex <= textLength; positionIndex++ {
			allPositions = append(allPositions, positionIndex)
		}
	}

	// If the pattern cannot fit in the text, no matches are possible.
	if patternLength > textLength {
		return nil
	}

	// Precompute heuristic tables.
	lastOccurrenceTable := BuildLastOccurenceTable(patternBytes)
	goodSuffixTable := BuildGoodSuffixTable(patternBytes)

	matchStartIndices := make([]int, 0)

	currentAlignmentIndex := 0

	// Slide the pattern over the text until no valid alignments remain.
	for currentAlignmentIndex <= textLength-patternLength {
		patternIndex := patternLength - 1

		// Compare pattern and text from right to left.
		for patternIndex >= 0 && patternBytes[patternIndex] == textBytes[currentAlignmentIndex+patternIndex] {
			patternIndex--
		}

		// Full match found.
		if patternIndex < 0 {
			matchStartIndices = append(matchStartIndices, currentAlignmentIndex)

			shiftDistance := goodSuffixTable[0]

			// Shift using the good-suffix rule for a full match.
			if shiftDistance < 1 {
				shiftDistance = 1
			}

			currentAlignmentIndex += shiftDistance

			continue
		}

		// Compute bad-character shift.
		mismatchedTextByte := textBytes[currentAlignmentIndex+patternIndex]
		lastOccurrenceIndex := lastOccurrenceTable[mismatchedTextByte]

		badCharacterShift := patternIndex - lastOccurrenceIndex

		if badCharacterShift < 1 {
			badCharacterShift = 1
		}

		// Compute good-suffix shift.
		goodSuffixShift := goodSuffixTable[patternIndex+1]

		// Apply the maximum shift suggested by either heuristic.
		if badCharacterShift > goodSuffixShift {
			currentAlignmentIndex += badCharacterShift
		} else {
			currentAlignmentIndex += goodSuffixShift
		}
	}

	return matchStartIndices
}
