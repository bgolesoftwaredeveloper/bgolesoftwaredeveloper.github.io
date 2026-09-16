// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file provides a demonstration program for the fifoqueue package.
//	It exercises all public queue operations, including normal FIFO usage
//	and defensive behavior under invalid conditions.
//
//	The demo walks through queue creation, inspection, enqueueing,
//	peeking, dequeuing, clearing, and safe handling of nil receivers.
//	Output is printed to standard output to illustrate state transitions
//	and error conditions in a readable, step-by-step manner.
//
//	This program is intended for manual testing, validation, and example
//	usage, rather than automated verification.
//
// Notes:
//   - All public Queue methods are exercised at least once.
//   - Both valid and invalid operations are demonstrated.
//   - Defensive error handling is explicitly shown.
//   - The program is safe to run repeatedly.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/fifo-queue/fifoqueue"
)

func main() {
	// -------------------------------------------------------------------------
	// Create a new queue instance.
	// -------------------------------------------------------------------------
	queue := fifoqueue.New[int]()

	// -------------------------------------------------------------------------
	// Inspect initial state.
	// -------------------------------------------------------------------------
	fmt.Println("Initial length:", queue.Length())
	fmt.Println("Is empty:", queue.IsEmpty())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Enqueue elements.
	// -------------------------------------------------------------------------
	if err := queue.Enqueue(10); err != nil {
		fmt.Println("Enqueue error:", err)
	}

	if err := queue.Enqueue(20); err != nil {
		fmt.Println("Enqueue error:", err)
	}

	if err := queue.Enqueue(30); err != nil {
		fmt.Println("Enqueue error:", err)
	}

	fmt.Println("Length after enqueue:", queue.Length())
	fmt.Println("Is empty after enqueue:", queue.IsEmpty())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Peek at the front element without removing it.
	// -------------------------------------------------------------------------
	front, err := queue.Peek()

	if err != nil {
		fmt.Println("Peek error:", err)
	} else {
		fmt.Println("Peek value:", front)
	}

	fmt.Println()

	// -------------------------------------------------------------------------
	// Dequeue elements until the queue is empty.
	// -------------------------------------------------------------------------
	for !queue.IsEmpty() {
		value, err := queue.Dequeue()

		if err != nil {
			fmt.Println("Dequeue error:", err)

			break
		}

		fmt.Println("Dequeued value:", value)
		fmt.Println("Current length:", queue.Length())
		fmt.Println()
	}

	// -------------------------------------------------------------------------
	// Attempt invalid operations on an empty queue.
	// -------------------------------------------------------------------------
	_, err = queue.Peek()

	if err != nil {
		fmt.Println("Peek on empty queue:", err)
	}

	_, err = queue.Dequeue()

	if err != nil {
		fmt.Println("Dequeue on empty queue:", err)
	}

	fmt.Println()

	// -------------------------------------------------------------------------
	// Clear the queue (safe even when already empty).
	// -------------------------------------------------------------------------
	queue.Clear()

	fmt.Println("Length after clear:", queue.Length())
	fmt.Println("Is empty after clear:", queue.IsEmpty())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Demonstrate defensive behavior with a nil queue.
	// -------------------------------------------------------------------------
	var nilQueue *fifoqueue.Queue[int]

	fmt.Println("Nil queue length:", nilQueue.Length())
	fmt.Println("Nil queue is empty:", nilQueue.IsEmpty())
	fmt.Println()

	if err := nilQueue.Enqueue(42); err != nil {
		fmt.Println("Enqueue on nil queue:", err)
	}

	fmt.Println()

	_, err = nilQueue.Peek()

	if err != nil {
		fmt.Println("Peek on nil queue:", err)
	}

	fmt.Println()

	_, err = nilQueue.Dequeue()

	if err != nil {
		fmt.Println("Dequeue on nil queue:", err)
	}

	// Clear on a nil queue is a no-op.
	nilQueue.Clear()
}
