// -----------------------------------------------------------------------------
// Package: distributedhashtable
// File: distributed_hash_table_test.go
//
// Description:
//
//	This file contains unit tests for the distributedhashtable package. The tests
//	verify the correctness of the consistent hashing algorithm, thread safety,
//	and defensive behavior for node membership operations.
//
//	The test suite exercises normal operation, such as adding and retrieving
//	nodes, as well as edge cases like empty rings, duplicate nodes, and
//	node failures (deletion).
//
// Design Notes:
//   - Tests are written using explicit Arrange / Act / Assert sections.
//   - Validates that consistent hashing invariants (clockwise lookup) hold.
//   - Ensures that virtual node replication behaves as expected.
//
// Intended Use:
//
//	This file is intended to validate the behavior of the Distributed Hash Table
//	implementation during development and ensure reliability in distributed
//	service discovery scenarios.
//
// -----------------------------------------------------------------------------
package distributedhashtable

import (
	"fmt"
	"sync"
	"testing"
)

// -----------------------------------------------------------------------------
// Initialization and Initial State
// -----------------------------------------------------------------------------

// TestNewRingInitialization verifies that NewRing sets up a valid state.
func TestNewRingInitialization(test *testing.T) {
	// Arrange & Act.
	ring := NewRing(10)

	// Assert.
	if ring.replicas != 10 {
		test.Fatalf("expected 10 replicas, got %d", ring.replicas)
	}
	if len(ring.nodes) != 0 {
		test.Fatalf("expected empty nodes map")
	}
}

// TestNewRingDefensiveReplicaCount verifies that negative replicas are corrected.
func TestNewRingDefensiveReplicaCount(test *testing.T) {
	// Arrange & Act.
	ring := NewRing(-5)

	// Assert.
	if ring.replicas != 1 {
		test.Fatalf("expected replica count to be defaulted to 1, got %d", ring.replicas)
	}
}

// -----------------------------------------------------------------------------
// Add Operations
// -----------------------------------------------------------------------------

// TestAddSingleNode verifies that adding a node populates the ring correctly.
func TestAddSingleNode(test *testing.T) {
	// Arrange.
	replicas := 3

	ring := NewRing(replicas)

	// Act.
	ring.Add("Server-A")

	// Assert.
	if len(ring.sorted) != replicas {
		test.Fatalf("expected %d hash entries, got %d", replicas, len(ring.sorted))
	}
	if len(ring.nodes) != replicas {
		test.Fatalf("expected %d map entries, got %d", replicas, len(ring.nodes))
	}
}

// TestAddEmptyServerName verifies defensive handling of empty strings.
func TestAddEmptyServerName(test *testing.T) {
	// Arrange.
	ring := NewRing(3)

	// Act.
	ring.Add("")

	// Assert.
	if len(ring.sorted) != 0 {
		test.Fatalf("expected ring to remain empty")
	}
}

// TestAddDuplicateNode verifies that adding the same node twice is idempotent.
func TestAddDuplicateNode(test *testing.T) {
	// Arrange.
	ring := NewRing(3)

	ring.Add("Server-A")

	// Act.
	ring.Add("Server-A")

	// Assert.
	if len(ring.sorted) != 3 {
		test.Fatalf("expected length 3 (idempotency), got %d", len(ring.sorted))
	}
}

// -----------------------------------------------------------------------------
// Get / Retrieval Operations
// -----------------------------------------------------------------------------

// TestGetOnEmptyRing verifies safe failure when no nodes exist.
func TestGetOnEmptyRing(test *testing.T) {
	// Arrange.
	ring := NewRing(3)

	// Act.
	_, ok := ring.Get("some-key")

	// Assert.
	if ok {
		test.Fatalf("expected Get to fail on empty ring")
	}
}

// TestGetConsistency verifies that the same key always maps to the same node.
func TestGetConsistency(test *testing.T) {
	// Arrange.
	ring := NewRing(10)

	ring.Add("Server-A")
	ring.Add("Server-B")

	key := "persistent-key"

	// Act.
	node1, _ := ring.Get(key)
	node2, _ := ring.Get(key)

	// Assert.
	if node1 != node2 {
		test.Fatalf("hashing is not deterministic: got %s then %s", node1, node2)
	}
}

// TestGetClockwiseWrapAround verifies that keys larger than the last hash wrap to the first node.
func TestGetClockwiseWrapAround(test *testing.T) {
	// Arrange.
	ring := NewRing(1)

	ring.Add("Server-A")
	ring.Add("Server-B")

	// Act.
	node, ok := ring.Get("wrap-test-key")

	// Assert.
	if !ok || node == "" {
		test.Fatalf("failed to retrieve node via wrap-around logic")
	}
}

// -----------------------------------------------------------------------------
// Delete Operations
// -----------------------------------------------------------------------------

// TestDeleteRemovesAllReplicas verifies that a server is completely purged.
func TestDeleteRemovesAllReplicas(test *testing.T) {
	// Arrange.
	ring := NewRing(5)

	ring.Add("Server-Delete-Me")

	// Act.
	ring.Delete("Server-Delete-Me")

	// Assert.
	if len(ring.sorted) != 0 || len(ring.nodes) != 0 {
		test.Fatalf("server replicas were not fully removed")
	}
}

// TestDeleteNonExistentServer verifies defensive behavior.
func TestDeleteNonExistentServer(test *testing.T) {
	// Arrange.
	ring := NewRing(3)

	ring.Add("Server-A")

	// Act.
	ring.Delete("Server-Non-Existent")

	// Assert.
	if len(ring.sorted) != 3 {
		test.Fatalf("Delete corrupted the ring when server didn't exist")
	}
}

// -----------------------------------------------------------------------------
// Update Operations
// -----------------------------------------------------------------------------

// TestUpdateSuccessfullyRemaps verifies node replacement logic.
func TestUpdateSuccessfullyRemaps(test *testing.T) {
	// Arrange.
	ring := NewRing(3)

	ring.Add("Old-Server")

	// Act.
	ring.Update("Old-Server", "New-Server")

	// Assert.
	for hash, name := range ring.nodes {
		if name == "Old-Server" {
			test.Fatalf("Old-Server still exists in map at hash %d", hash)
		}
	}

	foundNew := false

	for _, name := range ring.nodes {
		if name == "New-Server" {
			foundNew = true
			break
		}
	}

	if !foundNew {
		test.Fatalf("New-Server was not added to the ring during Update")
	}

	if len(ring.sorted) != 3 {
		test.Fatalf("Expected 3 hashes in sorted slice, got %d", len(ring.sorted))
	}
}

// -----------------------------------------------------------------------------
// Concurrency and Thread Safety
// -----------------------------------------------------------------------------

// TestConcurrentAccess verifies that the ring does not panic under load.
func TestConcurrentAccess(test *testing.T) {
	// Arrange.
	ring := NewRing(10)

	var group sync.WaitGroup

	count := 100

	// Act.
	group.Add(count)

	for index := 0; index < count; index++ {
		go func(value int) {
			defer group.Done()

			ring.Add(fmt.Sprintf("Server-%d", value))
			ring.Get(fmt.Sprintf("Key-%d", value))
		}(index)
	}

	group.Wait()

	// Assert.
	if len(ring.nodes) == 0 {
		test.Fatalf("concurrent operations failed to populate ring")
	}
}

// -----------------------------------------------------------------------------
// Internal Logic (Hash Distribution)
// -----------------------------------------------------------------------------

// TestHashDistribution verifies that virtual nodes help distribute keys.
func TestHashDistribution(test *testing.T) {
	// Arrange.
	ring := NewRing(100)

	ring.Add("Node-1")
	ring.Add("Node-2")

	distribution := make(map[string]int)

	// Act.
	for index := 0; index < 1000; index++ {
		node, _ := ring.Get(fmt.Sprintf("test-key-%d", index))
		distribution[node]++
	}

	// Assert.
	if distribution["Node-1"] < 100 || distribution["Node-2"] < 100 {
		test.Fatalf("Distribution too skewed: Node1: %d, Node2: %d", distribution["Node-1"], distribution["Node-2"])
	}
}

// TestRemoveFromSortedIndexBoundary verifies internal slice manipulation safety.
func TestRemoveFromSortedIndexBoundary(test *testing.T) {
	// Arrange.
	ring := NewRing(1)

	ring.Add("A")

	hashValue := ring.sorted[0]

	// Act / Assert.
	ring.removeFromSorted(hashValue)

	if len(ring.sorted) != 0 {
		test.Fatalf("failed to remove item from slice")
	}
}
