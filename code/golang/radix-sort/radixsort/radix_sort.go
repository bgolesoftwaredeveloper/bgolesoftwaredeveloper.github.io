// -----------------------------------------------------------------------------
// Package: radixsort
// File: radixsort.go
//
// Description:
//
//	This file implements a high-performance Radix Sort algorithm for unsigned
//	integers using the Least Significant Digit (LSD) strategy.
//
//	The implementation functions by processing digits from right to left,
//	utilizing a stable Counting Sort as an auxiliary subroutine. This ensures
//	linear time complexity relative to the number of digits while maintaining
//	the relative order of duplicate elements.
//
//	By utilizing fixed-size arrays for digit frequency and minimizing
//	comparisons, this algorithm provides an efficient alternative to
//	comparison-based sorts for large datasets of integers.
//
// -----------------------------------------------------------------------------
package radixsort

import "fmt"

// findMaximum identifies the largest integer within the slice.
// This value is essential for determining the total number of iterations
// required to fully sort the collection.
func findMaximum(items []int) int {
	maximum := items[0]

	for _, number := range items {
		if number > maximum {
			maximum = number
		}
	}

	return maximum
}

// countingSort performs a stable distribution sort on the input slice
// based on the digit represented by the current placeValue.
func countingSort(items []int, placeValue int) {
	size := len(items)
	result := make([]int, size)
	counts := [10]int{}

	// 1. Map digits to their frequency in this digit place.
	for _, number := range items {
		digit := (number / placeValue) % 10
		counts[digit]++
	}

	// 2. Compute prefix sums (cumulative counts).
	// This tells us the ending position for each digit group.
	for index := 1; index < 10; index++ {
		counts[index] += counts[index-1]
	}

	// 3. Build the array.
	// We iterate backwards to maintain "stability" (keep original order).
	for index := size - 1; index >= 0; index-- {
		number := items[index]
		digit := (number / placeValue) % 10

		mappedIndex := counts[digit] - 1
		result[mappedIndex] = number
		counts[digit]--
	}

	// 4. Update the original slice.
	copy(items, result)
}

// RadixSort orchestrates the sorting process by iterating through each
// decimal place of the numbers in the input slice.
func RadixSort(dataset []int) error {
	if len(dataset) == 0 {
		return nil
	}

	// Radix sort digit logic requires non-negative integers.
	// We validate the dataset before processing to prevent incorrect sorting.
	for _, number := range dataset {
		if number < 0 {
			return fmt.Errorf("Negative value %d detected; only non-negative integers supported", number)
		}
	}

	// A slice with only one item is already sorted.
	if len(dataset) < 2 {
		return nil
	}

	maximum := findMaximum(dataset)

	// Place value increments by factors of 10 (1, 10, 100...) to
	// evaluate each digit column until the maximum value is exceeded.
	for placeValue := 1; maximum/placeValue > 0; placeValue *= 10 {
		countingSort(dataset, placeValue)
	}

	return nil
}
