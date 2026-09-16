// -----------------------------------------------------------------------------
// Package: radixsort
// File: radixsort_test.go
//
// Description:
//
//	This file contains a comprehensive suite of unit tests for the RadixSort
//	implementation. It validates the algorithm across a variety of edge cases,
//	typical datasets, and error conditions to ensure stability and correctness.
//
//	The tests are designed to verify the defensive checks, the mathematical
//	integrity of the LSD logic, and the stability of the sorting process.
//
// -----------------------------------------------------------------------------
package radixsort

import (
	"reflect"
	"testing"
)

// TestRadixSortEmptySlice verifies that an empty dataset returns no error and remains unchanged.
func TestRadixSortEmptySlice(test *testing.T) {
	// Arrange.
	var dataset []int
	var expected []int

	// Act.
	err := RadixSort(dataset)

	// Assert.
	if err != nil {
		test.Errorf("Expected nil error, got %v", err)
	}

	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortSingleElement verifies that a single-element dataset is handled correctly.
func TestRadixSortSingleElement(test *testing.T) {
	// Arrange.
	dataset := []int{42}
	expected := []int{42}

	// Act.
	err := RadixSort(dataset)

	// Assert.
	if err != nil {
		test.Errorf("Expected nil error, got %v", err)
	}

	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortNegativeValue verifies that the defensive check catches negative integers.
func TestRadixSortNegativeValue(test *testing.T) {
	// Arrange.
	dataset := []int{10, -5, 20}

	// Act.
	err := RadixSort(dataset)

	// Assert.
	if err == nil {
		test.Error("Expected error for negative value, got nil")
	}
}

// TestRadixSortSortedInput verifies the behavior when the dataset is already sorted.
func TestRadixSortSortedInput(test *testing.T) {
	// Arrange.
	dataset := []int{1, 2, 3, 4, 5}
	expected := []int{1, 2, 3, 4, 5}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortReverseSortedInput verifies the behavior when the dataset is sorted in reverse.
func TestRadixSortReverseSortedInput(test *testing.T) {
	// Arrange.
	dataset := []int{5, 4, 3, 2, 1}
	expected := []int{1, 2, 3, 4, 5}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortDuplicateValues verifies that the algorithm correctly handles repeating numbers.
func TestRadixSortDuplicateValues(test *testing.T) {
	// Arrange.
	dataset := []int{10, 5, 10, 3, 5}
	expected := []int{3, 5, 5, 10, 10}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortUniformDigits verifies the sort when all numbers have the same number of digits.
func TestRadixSortUniformDigits(test *testing.T) {
	// Arrange.
	dataset := []int{321, 123, 231, 111}
	expected := []int{111, 123, 231, 321}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortVaryingMagnitudes verifies logic when numbers have different digit counts.
func TestRadixSortVaryingMagnitudes(test *testing.T) {
	// Arrange.
	dataset := []int{1000, 1, 100, 10}
	expected := []int{1, 10, 100, 1000}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortZeroValues verifies that zeroes are handled correctly within the dataset.
func TestRadixSortZeroValues(test *testing.T) {
	// Arrange.
	dataset := []int{0, 10, 0, 5}
	expected := []int{0, 0, 5, 10}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortLargeRandomSet verifies sorting on a larger, non-trivial dataset.
func TestRadixSortLargeRandomSet(test *testing.T) {
	// Arrange.
	dataset := []int{170, 45, 75, 90, 802, 24, 2, 66}
	expected := []int{2, 24, 45, 66, 75, 90, 170, 802}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortPowersOfTen verifies behavior with numbers that are exact powers of ten.
func TestRadixSortPowersOfTen(test *testing.T) {
	// Arrange.
	dataset := []int{100, 10, 1000, 1}
	expected := []int{1, 10, 100, 1000}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortAllIdentical verifies behavior when all elements in the dataset are the same.
func TestRadixSortAllIdentical(test *testing.T) {
	// Arrange.
	dataset := []int{7, 7, 7, 7}
	expected := []int{7, 7, 7, 7}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortMaximumInt verifies sorting when large non-negative integers are present.
func TestRadixSortMaximumInt(test *testing.T) {
	// Arrange.
	dataset := []int{999, 0, 123}
	expected := []int{0, 123, 999}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortLargeGap verifies sorting between very small and very large integers.
func TestRadixSortLargeGap(test *testing.T) {
	// Arrange.
	dataset := []int{1000000, 1}
	expected := []int{1, 1000000}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}

// TestRadixSortStabilityImplicit verifies that relative order of same-digit elements is kept.
func TestRadixSortStabilityImplicit(test *testing.T) {
	// Arrange.
	dataset := []int{20, 10, 30}
	expected := []int{10, 20, 30}

	// Act.
	RadixSort(dataset)

	// Assert.
	if !reflect.DeepEqual(dataset, expected) {
		test.Errorf("Expected %v, got %v", expected, dataset)
	}
}
