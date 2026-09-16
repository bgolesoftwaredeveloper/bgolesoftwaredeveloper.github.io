// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file demonstrates usage of the ConcurrentHashTable implementation.
//	It exercises all public operations, including basic CRUD operations,
//	atomic-style primitives, iteration, and introspection.
//
//	The demo is intentionally verbose and sequential to make behavior and
//	semantics explicit and easy to follow. It is designed as a learning and
//	reference example rather than a performance benchmark.
//
// Intended Demonstrations:
//   - Table construction and shard configuration
//   - Insertion and retrieval of values
//   - Deletion semantics
//   - LoadOrStore behavior
//   - CompareAndSwap semantics
//   - Safe iteration with Range
//   - Length and shard introspection
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/concurrent-hash-table/concurrenthashtable"
)

func main() {
	// -------------------------------------------------------------------------
	// Create a new concurrent hash table.
	//
	// We specify the number of shards explicitly to control lock striping.
	// A higher shard count reduces contention under concurrent access.
	// -------------------------------------------------------------------------
	table := concurrenthashtable.NewSyncTable[string, int](8)

	// -------------------------------------------------------------------------
	// Insert key-value pairs using Set.
	//
	// Set will insert a new entry or overwrite an existing one.
	// -------------------------------------------------------------------------
	table.Set("alpha", 1)
	table.Set("beta", 2)
	table.Set("gamma", 3)

	// -------------------------------------------------------------------------
	// Retrieve values using Get.
	//
	// Get returns the stored value and a boolean indicating presence.
	// -------------------------------------------------------------------------
	value, exists := table.Get("alpha")

	fmt.Println("Get alpha:", value, exists)

	value, exists = table.Get("delta")

	fmt.Println("Get delta:", value, exists)
	fmt.Println()

	// -------------------------------------------------------------------------
	// Inspect the number of stored elements using Length.
	//
	// This aggregates the sizes of all shards.
	// -------------------------------------------------------------------------
	fmt.Println("Length after inserts:", table.Length())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Inspect the shard configuration.
	//
	// This reports how many shards were allocated at construction time.
	// -------------------------------------------------------------------------
	fmt.Println("Shard count:", table.ShardCount())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Demonstrate LoadOrStore.
	//
	// If the key exists, the existing value is returned.
	// If it does not exist, the new value is stored.
	// -------------------------------------------------------------------------
	actualValue, loaded := table.LoadOrStore("alpha", 100)

	fmt.Println("LoadOrStore alpha:", actualValue, loaded)

	actualValue, loaded = table.LoadOrStore("delta", 4)

	fmt.Println("LoadOrStore delta:", actualValue, loaded)
	fmt.Println()

	// -------------------------------------------------------------------------
	// Verify that delta was stored by LoadOrStore.
	// -------------------------------------------------------------------------
	value, exists = table.Get("delta")

	fmt.Println("Get delta after LoadOrStore:", value, exists)
	fmt.Println()

	// -------------------------------------------------------------------------
	// Demonstrate CompareAndSwap.
	//
	// We attempt to update a value only if it matches the expected value.
	// -------------------------------------------------------------------------
	swapSucceeded := table.CompareAndSwap("alpha", 1, 10,
		func(compare, against int) bool {
			return compare == against
		},
	)

	fmt.Println("Compare and swap alpha (1 -> 10):", swapSucceeded)

	// -------------------------------------------------------------------------
	// Attempt a failing CompareAndSwap due to mismatched expected value.
	// -------------------------------------------------------------------------
	swapSucceeded = table.CompareAndSwap("alpha", 1, 20,
		func(compare, against int) bool {
			return compare == against
		},
	)

	fmt.Println("Compare and swap alpha (1 -> 20):", swapSucceeded)
	fmt.Println()

	// -------------------------------------------------------------------------
	// Confirm the current value after CompareAndSwap attempts.
	// -------------------------------------------------------------------------
	value, exists = table.Get("alpha")

	fmt.Println("Get alpha after CAS:", value, exists)
	fmt.Println()

	// -------------------------------------------------------------------------
	// Demonstrate Delete.
	//
	// Delete removes the key if present and reports whether removal occurred.
	// -------------------------------------------------------------------------
	wasDeleted := table.Delete("beta")

	fmt.Println("Delete beta:", wasDeleted)

	wasDeleted = table.Delete("beta")

	fmt.Println("Delete beta again:", wasDeleted)
	fmt.Println()

	// -------------------------------------------------------------------------
	// Check the length after deletion.
	// -------------------------------------------------------------------------
	fmt.Println("Length after delete:", table.Length())
	fmt.Println()

	// -------------------------------------------------------------------------
	// Demonstrate Range.
	//
	// Range iterates over all key-value pairs.
	// Returning false from the visitor stops iteration early.
	// -------------------------------------------------------------------------
	fmt.Println("Range over table contents:")

	table.Range(func(key string, value int) bool {
		fmt.Printf("  key=%s value=%d\n", key, value)

		// Continue iteration.
		return true
	})

	fmt.Println()

	// -------------------------------------------------------------------------
	// Demonstrate early termination during Range.
	// -------------------------------------------------------------------------
	fmt.Println("Range with early stop:")

	table.Range(func(key string, value int) bool {
		fmt.Printf("\tvisiting key=%s\n", key)

		// Stop iteration after the first element.
		return false
	})

	fmt.Println()

	// -------------------------------------------------------------------------
	// Final length check to confirm table state.
	// -------------------------------------------------------------------------
	fmt.Println("Final length:", table.Length())
}
