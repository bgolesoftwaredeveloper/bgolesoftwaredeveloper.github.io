// -----------------------------------------------------------------------------
// Package: rabinkarp
// File: rabinkarp.go
//
// Description:
//
//	This file implements a high-performance multi-pattern string search using
//	the Rabin-Karp rolling hash algorithm.
//
//	The implementation is designed for efficiency by using a rolling hash
//	technique that allows the algorithm to scan the text body in linear time,
//	regardless of the number of patterns being searched (provided they share
//	a uniform length).
//
//	To optimize performance, the algorithm leverages unsigned 64-bit integer
//	overflow as an implicit modulo operation and utilizes a hash registry for
//	O(1) average-time pattern lookups.
//
// -----------------------------------------------------------------------------
package rabinkarp

// Result captures the state of a successful match within the text.
// It stores the zero-based starting index of the match and the
// specific pattern string that was identified.
type Result struct {
	Index   int
	Pattern string
}

// MultiPatternSearch scans a text body for all occurrences of the provided
// search patterns. It returns a slice of Results containing match locations.
// The algorithm assumes all patterns in the input slice are of the same length.
func MultiPatternSearch(body string, patterns []string) []Result {
	// Reject empty search parameters to avoid unnecessary processing.
	if len(patterns) == 0 || len(body) == 0 {
		return nil
	}

	textLength := len(body)
	patternLength := len(patterns[0])

	// A pattern cannot be found if it exceeds the length of the text body.
	if patternLength > textLength {
		return nil
	}

	// primeBase is the multiplier used to shift digits in the rolling hash.
	// 16777619 is a prime number that provides a good distribution of bits.
	const primeBase uint64 = 16777619

	var highPowerWeight uint64 = 1
	var matches []Result

	// patternHashRegistry maps computed uint64 hashes to pattern strings.
	// We use a slice of strings to handle potential hash collisions safely.
	patternHashRegistry := make(map[uint64][]string)

	// Precompute the weight of the leading digit (primeBase^(patternLength - 1)).
	// This is used to remove the leading character's influence when the window slides.
	for index := 0; index < patternLength-1; index++ {
		highPowerWeight *= primeBase
	}

	// Calculate and register the hash for every target pattern.
	for _, pattern := range patterns {
		var hashValue uint64

		for charIndex := 0; charIndex < len(pattern); charIndex++ {
			// Update the hash value using the Horner's method equivalent.
			hashValue = hashValue*primeBase + uint64(pattern[charIndex])
		}

		// Store the pattern in the registry indexed by its hash.
		patternHashRegistry[hashValue] = append(patternHashRegistry[hashValue], pattern)
	}

	// Compute the initial rolling hash for the first window in the text body.
	var currentWindowHash uint64

	for charIndex := 0; charIndex < patternLength; charIndex++ {
		currentWindowHash = currentWindowHash*primeBase + uint64(body[charIndex])
	}

	// Slide the window across the text body byte by byte.
	for position := 0; position <= textLength-patternLength; position++ {
		// If the current window's hash is recognized, verify the match.
		if matchedPatterns, exists := patternHashRegistry[currentWindowHash]; exists {
			for _, original := range matchedPatterns {
				// Perform explicit string comparison to resolve potential hash collisions.
				if body[position:position+patternLength] == original {
					matches = append(matches, Result{
						Index:   position,
						Pattern: original,
					})
				}
			}
		}

		// Calculate the hash for the next window if we haven't reached the end.
		if position < textLength-patternLength {
			leadingByte := uint64(body[position])
			trailingByte := uint64(body[position+patternLength])

			// Update the hash by removing the leftmost byte and adding the rightmost byte.
			// This "rolling" operation maintains O(1) time complexity per shift.
			currentWindowHash = (currentWindowHash-leadingByte*highPowerWeight)*primeBase + trailingByte
		}
	}

	return matches
}
