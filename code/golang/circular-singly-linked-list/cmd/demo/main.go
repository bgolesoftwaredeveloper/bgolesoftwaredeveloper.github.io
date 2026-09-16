// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file contains a runnable demonstration of the
//	circularsinglylinkedlist package. The program exercises core circular
//	singly linked list operations, including append, prepend, indexed
//	removal, search, traversal, and conversion to a slice, while printing
//	the list state and length after each operation.
//
//	The demo is intended to provide a clear, step-by-step illustration of
//	circular singly linked list behavior and to serve as a manual
//	verification tool for correctness and edge-case handling.
//
// Usage:
//
//	go run ./cmd/demo
//
// Related Package:
//
//	github.com/bgolesoftwaredeveloper/circular-slingly-linked-list/circularsinglylinkedlist
//
// Notes:
//   - This file is not part of the circularsinglylinkedlist library API.
//   - Output is intentionally verbose to make state transitions explicit.
//   - Traversal is bounded to prevent infinite loops inherent to
//     circular data structures.
//   - The demo favors clarity over conciseness.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/circular-slingly-linked-list/circularsinglylinkedlist"
)

func main() {
	// Create a new empty circular singly linked list of integers.
	list := circularsinglylinkedlist.New[int]()

	// Append values to the end of the list to establish initial ordering.
	list.Append(10)
	list.Append(20)
	list.Append(30)

	// Prepend a value to the front of the list to validate head updates.
	list.Prepend(5)

	// Print the full list to verify circular structure and ordering.
	fmt.Println(list)

	// Print a blank line to separate output sections.
	fmt.Println()

	// Print the current length of the list to verify element count.
	fmt.Println("Length:", list.Length())

	// Convert the list to a slice and print it to validate traversal logic.
	fmt.Println("Slice:", list.ToSlice())

	// Remove the element at index 2 to validate indexed removal behavior.
	removed, err := list.RemoveAt(2)

	// Panic if removal fails, as this indicates a logic error.
	if err != nil {
		panic(err)
	}

	// Print a blank line to separate output sections.
	fmt.Println()

	// Print the removed value to confirm correct element removal.
	fmt.Println("Removed:", removed)
	fmt.Println()

	// Print the list after removal to verify updated structure.
	fmt.Println(list)
	fmt.Println()

	// Attempt to find a value in the list using a predicate function.
	found := list.Find(func(value int) bool { return value == 20 })

	// Print the found node and its successor if the value exists.
	if found != nil {
		fmt.Println("Found node:", found.Value, "Next:", found.Next().Value)
	}

	// Print a label to indicate iteration output.
	fmt.Print("ForEach: ")

	// Traverse the list once and print each value to validate iteration.
	list.ForEach(func(value int) { fmt.Print(value, " ") })

	// Print a newline to terminate the output cleanly.
	fmt.Println()
}
