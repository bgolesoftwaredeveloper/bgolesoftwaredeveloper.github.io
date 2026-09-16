// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file contains a runnable demonstration of the
//	circulardoublylinkedlist package. The program exercises core circular
//	doubly linked list operations, including insertion at both ends,
//	removal from both ends, predicate-based search, state inspection,
//	and defensive nil-receiver handling, while printing list state and
//	size after each operation.
//
//	The demo is intended to provide a clear, step-by-step illustration of
//	circular doubly linked list behavior and to serve as a manual
//	verification tool for correctness, invariant preservation, and
//	edge-case handling.
//
// Usage:
//
//	go run ./cmd/demo
//
// Related Package:
//
//	github.com/bgolesoftwaredeveloper/circular-doubly-linked-list/circulardoublylinkedlist
//
// Notes:
//   - This file is not part of the circulardoublylinkedlist library API.
//   - Output is intentionally verbose to make state transitions explicit.
//   - Traversal and inspection are bounded to prevent infinite loops
//     inherent to circular data structures.
//   - The demo favors clarity and correctness over conciseness.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/circulardoublylinkedlist/circulardoublylinkedlist"
)

// dump prints a bounded, linearized view of the circular doubly linked list
// to standard output.
//
// The function starts traversal from the list head and iterates exactly
// list.Size() elements to avoid infinite loops inherent to circular
// data structures. Each node's value is printed in logical order.
//
// If the provided list is nil or empty, an empty list representation
// ("List: []") is printed and the function returns immediately.
//
// This function is intended strictly for diagnostic and demonstration
// purposes and is not part of the library API.
//
// Parameters:
//   - list: The circular doubly linked list to inspect and print.
func dump(list *circulardoublylinkedlist.List) {
	if list == nil || list.Head() == nil {
		fmt.Println("List: []")

		return
	}

	fmt.Print("List: ")

	current := list.Head()

	for index := 0; index < list.Size(); index++ {
		fmt.Printf("%v ", current.Value)

		current = current.Next
	}

	fmt.Println()
}

// checkInvariants validates core structural invariants of a circular
// doubly linked list.
//
// Specifically, this function asserts that:
//   - tail.Next points to head.
//   - head.Previous points to tail.
//
// These invariants are fundamental to correctness for circular traversal
// in both directions. Any violation results in a panic, as such a state
// indicates structural corruption.
//
// If the provided list is nil or empty, the function performs no checks
// and returns silently.
//
// This function is intended for internal verification during demos
// and manual testing, not for production use.
//
// Parameters:
//   - list: The circular doubly linked list whose invariants are checked.
func checkInvariants(list *circulardoublylinkedlist.List) {
	if list == nil || list.Head() == nil {
		return
	}

	head := list.Head()
	tail := list.Tail()

	if tail.Next != head {
		panic("INVARIANT BROKEN: tail.Next != head")
	}
	if head.Previous != tail {
		panic("INVARIANT BROKEN: head.Previous != tail")
	}
}

func main() {
	// -------------------------------------------------------------------------
	// Create a new empty list.
	// -------------------------------------------------------------------------
	list := circulardoublylinkedlist.New()

	fmt.Println("Initial state:")
	fmt.Println("\tIsEmpty:", list.IsEmpty())
	fmt.Println("\tSize:", list.Size())
	fmt.Println("\tHead:", list.Head())
	fmt.Println("\tTail:", list.Tail())
	fmt.Println()

	// -------------------------------------------------------------------------
	// PushFront operations.
	// -------------------------------------------------------------------------
	fmt.Println("PushFront:")

	list.PushFront(10)
	list.PushFront(20)
	list.PushFront(30)

	fmt.Println("\tHead:", list.Head().Value)
	fmt.Println("\tTail:", list.Tail().Value)
	fmt.Println("\tSize:", list.Size())
	fmt.Println()

	// -------------------------------------------------------------------------
	// PushBack operations.
	// -------------------------------------------------------------------------
	fmt.Println("PushBack:")

	list.PushBack(40)
	list.PushBack(50)

	fmt.Println("\tHead:", list.Head().Value)
	fmt.Println("\tTail:", list.Tail().Value)
	fmt.Println("\tSize:", list.Size())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Find operation.
	// -------------------------------------------------------------------------
	fmt.Print("Find: ")

	found := list.Find(func(value interface{}) bool {
		return value == 20
	})

	if found != nil {
		fmt.Println(found.Value)
	} else {
		fmt.Println("Value not found")
	}

	fmt.Println()

	// -------------------------------------------------------------------------
	// PopFront operation.
	// -------------------------------------------------------------------------
	fmt.Println("PopFront:")

	value, ok := list.PopFront()

	fmt.Println("\tPopped:", value, "Success:", ok)

	fmt.Println("\tNew Head:", list.Head().Value)
	fmt.Println("\tSize:", list.Size())
	fmt.Println()

	// -------------------------------------------------------------------------
	// PopBack operation.
	// -------------------------------------------------------------------------
	fmt.Println("PopBack:")

	value, ok = list.PopBack()

	fmt.Println("\tPopped:", value, "Success:", ok)

	fmt.Println("\tNew Tail:", list.Tail().Value)
	fmt.Println("\tSize:", list.Size())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Clear operation.
	// -------------------------------------------------------------------------
	fmt.Println("Clear:")

	list.Clear()

	fmt.Println("\tIsEmpty:", list.IsEmpty())
	fmt.Println("\tSize:", list.Size())
	fmt.Println("\tHead:", list.Head())
	fmt.Println("\tTail:", list.Tail())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Defensive behavior with nil receiver.
	// -------------------------------------------------------------------------
	fmt.Println("Nil receiver behavior:")

	var nilList *circulardoublylinkedlist.List

	fmt.Println("\tNil Size:", nilList.Size())
	fmt.Println("\tNil IsEmpty:", nilList.IsEmpty())
	fmt.Println("\tNil Head:", nilList.Head())
	fmt.Println("\tNil Tail:", nilList.Tail())

	nilList.PushFront(1)
	nilList.PushBack(2)

	fmt.Println()

	value, ok = nilList.PopFront()

	fmt.Println("Nil PopFront:", value, ok)

	value, ok = nilList.PopBack()

	fmt.Println("Nil PopBack:", value, ok)
}
