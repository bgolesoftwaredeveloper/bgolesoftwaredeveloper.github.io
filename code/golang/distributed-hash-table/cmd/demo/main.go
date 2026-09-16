// -----------------------------------------------------------------------------
// Package: main
// File: main.go
//
// Description:
//
//	This file serves as the entry point and demonstration suite for the
//	distributedhashtable package. It simulates a live distributed system
//	environment where storage nodes (servers) are added, updated, and
//	removed dynamically.
//
//	The demonstration highlights the core value of Consistent Hashing:
//	deterministic data placement and graceful redistribution of keys
//	during node membership changes or hardware failures.
//
// Execution Flow:
//  1. Ring Initialization: Sets up a hash ring with a defined replica factor.
//  2. Node Discovery: Populates the ring with an initial set of server nodes.
//  3. Key Mapping: Demonstrates how specific data keys resolve to target nodes.
//  4. Atomic Updates: Shows the migration of keys when a node is renamed/upgraded.
//  5. Failure Simulation: Illustrates the "clockwise fallback" mechanism when
//     a node is deleted from the cluster.
//  6. Edge Case Handling: Verifies defensive behavior when querying an empty ring.
//
// Intended Use:
//
//	This executable is intended for validating the functional correctness
//	of the DHT implementation and observing how virtual nodes impact
//	the balance of data distribution across the hash space.
//
// -----------------------------------------------------------------------------
package main

import (
	"fmt"

	"github.com/bgolesoftwaredeveloper/distributed-hash-table/distributedhashtable"
)

func main() {
	// 1. Initialize the Ring with 3 virtual nodes (replicas) per server.
	// This ensures a more even distribution across the 32-bit hash space.
	dht := distributedhashtable.NewRing(3)

	// 2. Add some initial storage nodes.
	servers := []string{"Server-Alpha", "Server-Beta", "Server-Gamma"}

	for _, s := range servers {
		dht.Add(s)
	}

	// 3. Define some test data keys (simulating files or user IDs).
	dataKeys := []string{"profile_pic.png", "user_101_data", "video_stream_01"}

	fmt.Println("--- Initial Data Assignment ---")

	for _, key := range dataKeys {
		node, ok := dht.Get(key)

		if ok {
			fmt.Printf("Key [%s] is mapped to -> %s\n", key, node)
		}
	}

	// 4. Test the 'Update' Logic.
	// Let's say Server-Beta is being upgraded to a more powerful machine.
	fmt.Println("\n--- Updating Server-Beta to Server-Beta-v2 ---")

	dht.Update("Server-Beta", "Server-Beta-v2")

	for _, key := range dataKeys {
		node, _ := dht.Get(key)

		fmt.Printf("Key [%s] is now mapped to -> %s\n", key, node)
	}

	// 5. Test the 'Delete' and 'Defensive' Logic.
	// We delete Server-Alpha and check where its keys go.
	fmt.Println("\n--- Deleting Server-Alpha (Simulating Failure) ---")

	dht.Delete("Server-Alpha")

	for _, key := range dataKeys {
		node, ok := dht.Get(key)

		if ok {
			fmt.Printf("Key [%s] now falls back to -> %s\n", key, node)
		} else {
			fmt.Printf("Key [%s] has no available nodes!\n", key)
		}
	}

	// 6. Test the empty ring defensive check.
	fmt.Println("\n--- Cleaning the Ring ---")

	dht.Delete("Server-Beta-v2")
	dht.Delete("Server-Gamma")

	_, ok := dht.Get("any_file.txt")

	if !ok {
		fmt.Println("Success: Get() correctly returned false for an empty ring.")
	}
}
