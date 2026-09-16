// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a runnable demonstration of the priorityqueue
//	package. It exercises all major public APIs of the PriorityQueue
//	type, including insertion, inspection, priority mutation, arbitrary
//	removal, and ordered extraction.
//
//	The demo uses a minimum comparator, causing the priority queue to
//	behave as a min-heap where lower numeric Priority values represent
//	higher priority. Output is printed to standard output to illustrate
//	heap behavior and ordering guarantees at each step.
//
//	This program is intended for demonstration, validation, and
//	exploratory use. It is not a test harness and does not attempt to
//	recover from programmer errors or invariant violations.
//
// Notes:
//   - All priority queue operations are exercised at least once.
//   - Error handling is explicit and fail-fast for clarity.
//   - Output order reflects correct heap semantics for a min-heap.
//   - This file depends on the priorityqueue package but does not
//     modify or extend its behavior.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/priorityqueue/priorityqueue"
)

func main() {
	// -------------------------------------------------------------------------
	// Create a new priority queue using a minimum comparator (min-heap).
	// Lower Priority values indicate higher priority.
	// -------------------------------------------------------------------------
	queue := priorityqueue.New(priorityqueue.MinimumComparator[int]())

	// -------------------------------------------------------------------------
	// Push values into the queue with associated priorities.
	// -------------------------------------------------------------------------
	queue.Push(100, 5)
	queue.Push(200, 1)
	queue.Push(300, 3)
	queue.Push(400, 4)
	queue.Push(500, 2)

	fmt.Println("Queue length after pushes:", queue.Length())
	fmt.Println("Queue is empty:", queue.IsEmpty())

	// -------------------------------------------------------------------------
	// Peek at the highest-priority item without removing it.
	// -------------------------------------------------------------------------
	peeked, err := queue.Peek()

	if err != nil {
		panic(err)
	}

	fmt.Printf("Peek: value=%d priority=%d\n", peeked.Value, peeked.Priority)

	// -------------------------------------------------------------------------
	// Increase and decrease priorities.
	// -------------------------------------------------------------------------
	fmt.Println("\nUpdating priorities...")

	// Decrease the priority value (higher priority in min-heap).
	err = queue.DecreasePriority(peeked, 0)

	if err != nil {
		panic(err)
	}

	updated, _ := queue.Peek()

	fmt.Printf("After DecreasePriority: value=%d priority=%d\n", updated.Value, updated.Priority)

	// Increase the priority value (lower priority in min-heap).
	err = queue.IncreasePriority(updated, 10)

	if err != nil {
		panic(err)
	}

	updated, _ = queue.Peek()

	fmt.Printf("After IncreasePriority: value=%d priority=%d\n", updated.Value, updated.Priority)

	// -------------------------------------------------------------------------
	// UpdatePriority directly (generic mutation).
	// -------------------------------------------------------------------------
	err = queue.UpdatePriority(updated, 0)

	if err != nil {
		panic(err)
	}

	updated, _ = queue.Peek()

	fmt.Printf("After UpdatePriority: value=%d priority=%d\n", updated.Value, updated.Priority)

	// -------------------------------------------------------------------------
	// Remove an arbitrary item from the queue.
	// -------------------------------------------------------------------------
	fmt.Println("\nRemoving an arbitrary item...")

	removedItem := updated

	err = queue.Remove(removedItem)

	if err != nil {
		panic(err)
	}

	fmt.Println("Queue length after Remove:", queue.Length())

	// -------------------------------------------------------------------------
	// Pop all remaining items in priority order.
	// -------------------------------------------------------------------------
	fmt.Println("\nPopping remaining items:")

	for !queue.IsEmpty() {
		item, err := queue.Pop()

		if err != nil {
			panic(err)
		}

		fmt.Printf("Pop: value=%d priority=%d\n", item.Value, item.Priority)
	}

	fmt.Println("\nQueue is empty:", queue.IsEmpty())
	fmt.Println("Final queue length:", queue.Length())
}
