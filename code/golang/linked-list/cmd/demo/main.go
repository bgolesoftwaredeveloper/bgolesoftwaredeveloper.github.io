// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file contains a runnable demonstration of the linkedlist package.
//	The program exercises core linked list operations, including insertion,
//	removal by index, removal by value, and traversal, while printing the
//	list state and size after each operation.
//
//	The demo is intended to provide a clear, step-by-step illustration of
//	linked list behavior and to serve as a manual verification tool for
//	correctness and edge-case handling.
//
// Usage:
//
//	go run ./cmd/demo
//
// Related Package:
//
//	github.com/bgolesoftwaredeveloper/linked-list/linkedlist
//
// Notes:
//   - This file is not part of the linkedlist library API.
//   - Output is intentionally verbose to make state transitions explicit.
//   - The demo favors clarity over conciseness.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/linked-list/linkedlist"
)

func main() {
	// Print the demo title to clearly identify program output.
	fmt.Println("Linked List Demo")
	fmt.Println("----------------")
	fmt.Println()

	// Create a new empty linked list instance.
	list := linkedlist.New()

	// Display the initial empty list state.
	fmt.Print("Initial list:")

	fmt.Println(list)

	// Display the current size of the list.
	fmt.Println()
	fmt.Println("Size:", list.Size())
	fmt.Println()

	// Append multiple values to the end of the list.
	fmt.Println("Appending values 10, 20, 30")

	list.Append(10)
	list.Append(20)
	list.Append(30)

	// Print the list after append operations.
	fmt.Println(list)

	// Display the updated size after appending.
	fmt.Println()
	fmt.Println("Size:", list.Size())
	fmt.Println()

	// Prepend a value to the beginning of the list.
	fmt.Println("Prepending value 5")

	list.Prepend(5)

	// Print the list after the prepend operation.
	fmt.Println(list)

	// Display the updated size after prepending.
	fmt.Println()
	fmt.Println("Size:", list.Size())
	fmt.Println()

	// Remove the element at index 2 and report the result.
	fmt.Println("RemoveAt(2)")

	if value, ok := list.RemoveAt(2); ok {
		fmt.Println("Removed:", value)
	} else {
		fmt.Println("RemoveAt failed")
	}

	// Print the list after indexed removal.
	fmt.Println(list)

	// Display the size after indexed removal.
	fmt.Println()
	fmt.Println("Size:", list.Size())
	fmt.Println()

	// Attempt to remove an existing value from the list.
	fmt.Println("Remove(value = 20)")

	if list.Remove(20) {
		fmt.Println("Removed 20")
	} else {
		fmt.Println("Value not found")
	}

	// Print the list after value-based removal.
	fmt.Println(list)

	// Display the size after attempting to remove value 20.
	fmt.Println()
	fmt.Println("Size:", list.Size())
	fmt.Println()

	// Attempt to remove a value that does not exist in the list.
	fmt.Println("Remove(value = 999)")

	if list.Remove(999) {
		fmt.Println("Removed 999")
	} else {
		fmt.Println("Value not found")
	}

	// Print the final list state.
	fmt.Println(list)
	fmt.Println()

	// Display the final size of the list.
	fmt.Println("Size:", list.Size())
}
