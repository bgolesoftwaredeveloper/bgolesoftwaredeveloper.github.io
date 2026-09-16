// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file contains a runnable demonstration of the doublylinkedlist package.
//	The program exercises core doubly linked list operations, including
//	insertion at the front and back, removal from the front and back,
//	traversal, and clearing the list, while printing the list state and
//	length after each operation.
//
//	The demo is intended to provide a clear, step-by-step illustration of
//	doubly linked list behavior and to serve as a manual verification tool
//	for correctness and edge-case handling.
//
// Usage:
//
//	go run ./cmd/demo
//
// Related Package:
//
//	github.com/bgolesoftwaredeveloper/doubly-linked-list/doublylinkedlist
//
// Notes:
//   - This file is not part of the doublylinkedlist library API.
//   - Output is intentionally verbose to make state transitions explicit.
//   - The demo favors clarity over conciseness.
//   - Traversal output is printed explicitly since the list does not
//     expose internal nodes.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/doubly-linked-list/doublylinkedlist"
)

func main() {
	// Print the demo title to clearly identify program output.
	fmt.Println("Doubly Linked List")
	fmt.Println("------------------")
	fmt.Println()

	// Create a new empty doubly linked list instance.
	list := doublylinkedlist.New()

	// Display the initial empty list state.
	fmt.Println("Initial list (forward traversal):")

	list.ForEach(func(value interface{}) {
		fmt.Println(value)
	})

	// Display the current length of the list.
	fmt.Println()
	fmt.Println("Length:", list.Length())
	fmt.Println()

	// Append multiple values to the back of the list.
	list.PushBack("A")
	list.PushBack("B")
	list.PushBack("C")

	// Print the list after PushBack operations.
	fmt.Print("List after PushBack (forward traversal):")

	list.ForEach(func(value interface{}) {
		fmt.Printf(" %s", value)
	})

	fmt.Println()

	// Display the updated length.
	fmt.Println()
	fmt.Println("Length:", list.Length())
	fmt.Println()

	// Print the list after PushFront.
	fmt.Print("List after PushFront (forward traversal):")

	list.ForEach(func(value interface{}) {
		fmt.Printf(" %s", value)
	})

	fmt.Println()

	// Display the updated length.
	fmt.Println()
	fmt.Println("Length:", list.Length())
	fmt.Println()

	// Inspect the front and back elements without removal.
	fmt.Println("Front / Back inspection:")

	if value, ok := list.Front(); ok {
		fmt.Println("\tFront:", value)
	} else {
		fmt.Println("Front: <empty>")
	}

	if value, ok := list.Back(); ok {
		fmt.Println("\tBack:", value)
	} else {
		fmt.Println("Back: <empty>")
	}

	fmt.Println()

	// Remove the element at the front of the list.
	fmt.Println("PopFront():")

	if value, ok := list.PopFront(); ok {
		fmt.Println("\tRemoved from front:", value)
	} else {
		fmt.Println("\tPopFront failed")
	}

	// Print the list after PopFront.
	fmt.Print("\tList after PopFront:")

	list.ForEach(func(value interface{}) {
		fmt.Printf(" %s", value)
	})

	// Display the updated length.
	fmt.Println()
	fmt.Println("\tLength:", list.Length())
	fmt.Println()

	// Remove the element at the back of the list.
	fmt.Println("PopBack():")

	if value, ok := list.PopBack(); ok {
		fmt.Println("\tRemoved from back:", value)
	} else {
		fmt.Println("PopBack failed")
	}

	// Print the list after PopBack.
	fmt.Print("\tList after PopBack:")

	list.ForEach(func(value interface{}) {
		fmt.Printf(" %s", value)
	})

	// Display the updated length.
	fmt.Println()
	fmt.Println("\tLength:", list.Length())
	fmt.Println()

	// Clear the entire list.
	fmt.Println("Clear():")

	list.Clear()

	// Print the list after clearing.
	fmt.Print("\tList after Clear:")

	list.ForEach(func(value interface{}) {
		fmt.Println(value)
	})

	// Display the final length.
	fmt.Println()
	fmt.Println("\tLength:", list.Length())
	fmt.Println()

	// Demonstrate defensive behavior on an empty list.
	fmt.Println("Defensive checks on empty list:")

	if _, ok := list.PopFront(); !ok {
		fmt.Println("\tPopFront on empty list: safe no-op")
	}

	if _, ok := list.PopBack(); !ok {
		fmt.Println("\tPopBack on empty list: safe no-op")
	}

	if _, ok := list.Front(); !ok {
		fmt.Println("\tFront on empty list: safe no-op")
	}

	if _, ok := list.Back(); !ok {
		fmt.Println("\tBack on empty list: safe no-op")
	}
}
