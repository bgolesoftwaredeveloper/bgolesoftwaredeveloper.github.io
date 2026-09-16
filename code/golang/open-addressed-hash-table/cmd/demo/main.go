// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file contains a runnable demonstration of the open-addressed hash
//	table implementation. The program exercises core hash table operations,
//	including insertion, key overwrite, lookup, deletion, and size tracking,
//	while printing observable results to standard output.
//
//	The demo is intended to provide a clear, step-by-step illustration of
//	open-addressing behavior with linear probing, including tombstone-based
//	deletion and dynamic resizing under load.
//
//	This file serves as a manual verification tool and example usage of the
//	openaddressedhashtable package. It is not part of the library API.
//
// Usage:
//
//	go run ./cmd/demo
//
// Related Package:
//
//	github.com/bgolesoftwaredeveloper/openaddressedhashtable/openaddressedhashtable
//
// Notes:
//   - This file is not thread-safe and performs no synchronization.
//   - Output is intentionally verbose to make state transitions explicit.
//   - The demo favors clarity and correctness over conciseness.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/openaddressedhashtable/openaddressedhashtable"
)

func main() {
	// Create a hasher.
	hasher := openaddressedhashtable.StringHasher{}

	// Create a new hash table.
	table := openaddressedhashtable.New[string, int](hasher)

	// Put entries.
	table.Put("alpha", 1)
	table.Put("beta", 2)
	table.Put("gamma", 3)

	// Overwrite an existing key.
	table.Put("beta", 20)

	// Get entries.
	if value, ok := table.Get("alpha"); ok {
		fmt.Println("alpha =", value)
	}

	if value, ok := table.Get("beta"); ok {
		fmt.Println("beta =", value)
	}

	if _, ok := table.Get("delta"); !ok {
		fmt.Println("delta not found")
	}

	// Check length.
	fmt.Println()
	fmt.Println("length =", table.Length())

	// Delete an entry.
	deleted := table.Delete("gamma")

	fmt.Println()
	fmt.Println("gamma deleted =", deleted)

	// Confirm deletion.
	if _, ok := table.Get("gamma"); !ok {
		fmt.Println("gamma confirmed deleted")
	}

	// Final length.
	fmt.Println()
	fmt.Println("final length =", table.Length())
}
