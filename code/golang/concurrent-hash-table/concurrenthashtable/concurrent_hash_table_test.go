// -----------------------------------------------------------------------------
// Package: concurrenthashtable
// File: concurrent_hash_table_test.go
//
// Description:
//
//	This file contains unit tests for the ConcurrentHashTable implementation.
//	The tests validate correctness, concurrency safety, and API semantics
//	across all public operations.
//
//	Each test follows a strict Arrange / Act / Assert structure and uses
//	descriptive variable naming to make intent explicit. The test suite
//	covers both single-threaded behavior and concurrent access patterns
//	using goroutines and synchronization primitives.
//
// Design Notes:
//   - Tests are written in the same package to allow direct access.
//   - All public APIs are exercised at least once.
//   - Concurrency tests validate safety, not performance.
//   - The Go race detector should be used when running these tests.
//
// Recommended Invocation:
//
//	go test -race ./...
//
// -----------------------------------------------------------------------------
package concurrenthashtable

import (
	"sync"
	"testing"
)

// -----------------------------------------------------------------------------
// Construction and Basic State.
// -----------------------------------------------------------------------------

// TestNewConcurrentHashTableInitializesEmptyTable verifies correct construction.
func TestNewConcurrentHashTableInitializesEmptyTable(test *testing.T) {
	// Arrange.
	requestedShardCount := 8

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	// Assert.
	if table == nil {
		test.Fatalf("expected non-nil table.")
	}

	if table.ShardCount() != requestedShardCount {
		test.Fatalf("expected shard count %d, got %d.",
			requestedShardCount,
			table.ShardCount(),
		)
	}

	if table.Length() != 0 {
		test.Fatalf("expected newly constructed table to be empty.")
	}
}

// TestNewConcurrentHashTableNormalizesInvalidShardCount verifies normalization.
func TestNewConcurrentHashTableNormalizesInvalidShardCount(test *testing.T) {
	// Arrange.
	requestedShardCount := 0

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	// Assert.
	if table.ShardCount() != defaultSharedCount {
		test.Fatalf("expected default shard count %d, got %d.",
			defaultSharedCount,
			table.ShardCount(),
		)
	}
}

// -----------------------------------------------------------------------------
// Set, Get, and Delete.
// -----------------------------------------------------------------------------

// TestSetStoresValueAndGetRetrievesIt verifies basic CRUD behavior.
func TestSetStoresValueAndGetRetrievesIt(test *testing.T) {
	// Arrange.
	requestedShardCount := 4
	testKey := "alpha"
	testValue := 42

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	table.Set(testKey, testValue)

	retrievedValue, wasPresent := table.Get(testKey)

	// Assert.
	if !wasPresent {
		test.Fatalf("expected key to be present.")
	}

	if retrievedValue != testValue {
		test.Fatalf("expected value %d, got %d.",
			testValue,
			retrievedValue,
		)
	}
}

// TestDeleteRemovesExistingKey verifies deletion behavior.
func TestDeleteRemovesExistingKey(test *testing.T) {
	// Arrange.
	requestedShardCount := 4
	testKey := "beta"

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	table.Set(testKey, 100)

	wasDeleted := table.Delete(testKey)

	// Assert.
	if !wasDeleted {
		test.Fatalf("expected delete to report success.")
	}

	if table.Length() != 0 {
		test.Fatalf("expected table to be empty after deletion.")
	}
}

// TestDeleteReturnsFalseForMissingKey verifies delete miss semantics.
func TestDeleteReturnsFalseForMissingKey(test *testing.T) {
	// Arrange.
	requestedShardCount := 4

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	wasDeleted := table.Delete("missing")

	// Assert.
	if wasDeleted {
		test.Fatalf("expected delete to return false for missing key.")
	}
}

// -----------------------------------------------------------------------------
// LoadOrStore.
// -----------------------------------------------------------------------------

// TestLoadOrStoreStoresValueWhenKeyIsMissing verifies insert path.
func TestLoadOrStoreStoresValueWhenKeyIsMissing(test *testing.T) {
	// Arrange.
	requestedShardCount := 4
	testKey := "gamma"
	testValue := 7

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	returnedValue, wasLoaded := table.LoadOrStore(testKey, testValue)

	// Assert.
	if wasLoaded {
		test.Fatalf("expected wasLoaded to be false.")
	}

	if returnedValue != testValue {
		test.Fatalf("expected value %d, got %d.",
			testValue,
			returnedValue,
		)
	}
}

// TestLoadOrStoreLoadsExistingValue verifies load path.
func TestLoadOrStoreLoadsExistingValue(test *testing.T) {
	// Arrange.
	requestedShardCount := 4
	testKey := "delta"

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	table.Set(testKey, 10)

	returnedValue, wasLoaded := table.LoadOrStore(testKey, 99)

	// Assert.
	if !wasLoaded {
		test.Fatalf("expected wasLoaded to be true.")
	}

	if returnedValue != 10 {
		test.Fatalf("expected existing value 10, got %d.", returnedValue)
	}
}

// -----------------------------------------------------------------------------
// CompareAndSwap.
// -----------------------------------------------------------------------------

// TestCompareAndSwapSucceedsWhenExpectedMatches verifies CAS success.
func TestCompareAndSwapSucceedsWhenExpectedMatches(test *testing.T) {
	// Arrange.
	requestedShardCount := 4
	testKey := "epsilon"

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	table.Set(testKey, 1)

	swapSucceeded := table.CompareAndSwap(testKey, 1, 10,
		func(compare, against int) bool {
			return compare == against
		},
	)

	// Assert.
	if !swapSucceeded {
		test.Fatalf("expected compare-and-swap to succeed.")
	}

	updatedValue, _ := table.Get(testKey)

	if updatedValue != 10 {
		test.Fatalf("expected updated value 10, got %d.", updatedValue)
	}
}

// TestCompareAndSwapFailsWhenExpectedDoesNotMatch verifies CAS failure.
func TestCompareAndSwapFailsWhenExpectedDoesNotMatch(test *testing.T) {
	// Arrange.
	requestedShardCount := 4
	testKey := "zeta"

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	table.Set(testKey, 5)

	swapSucceeded := table.CompareAndSwap(testKey, 1, 99,
		func(compare, against int) bool {
			return compare == against
		},
	)

	// Assert.
	if swapSucceeded {
		test.Fatalf("expected compare-and-swap to fail.")
	}
}

// -----------------------------------------------------------------------------
// Range.
// -----------------------------------------------------------------------------

// TestRangeVisitsAllEntries verifies full traversal.
func TestRangeVisitsAllEntries(test *testing.T) {
	// Arrange.
	requestedShardCount := 8

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	table.Set("a", 1)
	table.Set("b", 2)
	table.Set("c", 3)

	visitedEntryCount := 0

	table.Range(func(key string, value int) bool {
		visitedEntryCount++
		return true
	})

	// Assert.
	if visitedEntryCount != 3 {
		test.Fatalf("expected to visit 3 entries, got %d.",
			visitedEntryCount,
		)
	}
}

// TestRangeStopsEarlyWhenVisitorReturnsFalse verifies early termination.
func TestRangeStopsEarlyWhenVisitorReturnsFalse(test *testing.T) {
	// Arrange.
	requestedShardCount := 8

	// Act.
	table := NewSyncTable[string, int](requestedShardCount)

	table.Set("x", 1)
	table.Set("y", 2)

	visitedEntryCount := 0

	table.Range(func(key string, value int) bool {
		visitedEntryCount++

		return false
	})

	// Assert.
	if visitedEntryCount != 1 {
		test.Fatalf("expected early termination after one visit.")
	}
}

// -----------------------------------------------------------------------------
// Concurrency.
// -----------------------------------------------------------------------------

// TestConcurrentSetOperationsDoNotCorruptState verifies concurrent safety.
func TestConcurrentSetOperationsDoNotCorruptState(test *testing.T) {
	// Arrange.
	requestedShardCount := 16
	workerCount := 8
	operationsPerWorker := 1000

	// Act.
	table := NewSyncTable[int, int](requestedShardCount)

	var waitGroup sync.WaitGroup

	waitGroup.Add(workerCount)

	for workerIndex := 0; workerIndex < workerCount; workerIndex++ {
		currentWorkerIndex := workerIndex

		go func() {
			defer waitGroup.Done()

			for operationIndex := 0; operationIndex < operationsPerWorker; operationIndex++ {
				key := currentWorkerIndex*operationsPerWorker + operationIndex
				table.Set(key, operationIndex)
			}
		}()
	}

	waitGroup.Wait()

	// Assert.
	expectedElementCount := workerCount * operationsPerWorker

	if table.Length() != expectedElementCount {
		test.Fatalf("expected length %d, got %d.",
			expectedElementCount,
			table.Length(),
		)
	}
}
