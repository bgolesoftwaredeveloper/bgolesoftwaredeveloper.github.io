// -----------------------------------------------------------------------------
// Package: lrucache
// File: cache_test.go
//
// Description:
//
//	This file contains unit tests for the LRUCache implementation.
//	The tests validate correctness, eviction behavior, recency updates,
//	explicit deletion, and defensive handling of missing keys.
//
//	All tests are written with clarity and determinism in mind, favoring
//	explicit setup and assertions over compact test patterns.
//
// Design Notes:
//   - Each test validates exactly one behavioral contract.
//   - Test names are descriptive and unique.
//   - Arrange / Act / Assert sections are clearly separated.
//   - The testing parameter is named `test` for semantic clarity.
//   - No test relies on unspecified map ordering or timing.
//
// Intended Use:
//
//	This test suite is intended to validate correctness during development
//	and serve as a reference for expected LRU cache behavior.
//
// -----------------------------------------------------------------------------
package lrucache

import (
	"fmt"
	"sync"
	"testing"
)

// TestLRUCache_PutAndGetStoresAndRetrievesValue verifies that a value
// inserted into the cache can be retrieved successfully.
func TestLRUCache_PutAndGetStoresAndRetrievesValue(test *testing.T) {
	// Arrange.
	cacheWithSingleEntryCapacity := NewLRUCache(1)
	expectedSessionPayload := "session-payload-northstar"

	// Act.
	cacheWithSingleEntryCapacity.Put("session-key-northstar", expectedSessionPayload)

	retrievedPayload, entryExists := cacheWithSingleEntryCapacity.Get("session-key-northstar")

	// Assert.
	if !entryExists {
		test.Fatalf("expected cache entry to exist after Put")
	}

	if retrievedPayload != expectedSessionPayload {
		test.Fatalf("expected payload %v, got %v",
			expectedSessionPayload,
			retrievedPayload,
		)
	}
}

// TestLRUCache_EvictsLeastRecentlyUsedEntry verifies that inserting
// beyond capacity evicts the least recently used entry.
func TestLRUCache_EvictsLeastRecentlyUsedEntry(test *testing.T) {
	// Arrange.
	cacheWithCapacityTwo := NewLRUCache(2)

	cacheWithCapacityTwo.Put("session-key-apex", "payload-apex")
	cacheWithCapacityTwo.Put("session-key-zenith", "payload-zenith")

	// Act.
	// Access apex to make zenith the least recently used.
	cacheWithCapacityTwo.Get("session-key-apex")

	// Insert a third entry to trigger eviction.
	cacheWithCapacityTwo.Put("session-key-orbit", "payload-orbit")

	_, zenithExists := cacheWithCapacityTwo.Get("session-key-zenith")
	apexPayload, apexExists := cacheWithCapacityTwo.Get("session-key-apex")

	// Assert.
	if zenithExists {
		test.Fatalf("expected least recently used entry to be evicted")
	}

	if !apexExists || apexPayload != "payload-apex" {
		test.Fatalf("expected most recently used entry to remain in cache")
	}
}

// TestLRUCache_GetPromotesEntryToMostRecentlyUsed verifies that
// accessing an entry updates its recency ordering.
func TestLRUCache_GetPromotesEntryToMostRecentlyUsed(test *testing.T) {
	// Arrange.
	cacheWithCapacityTwo := NewLRUCache(2)

	cacheWithCapacityTwo.Put("session-key-comet", "payload-comet")
	cacheWithCapacityTwo.Put("session-key-nova", "payload-nova")

	// Act.
	// Access comet to make nova least recently used.
	cacheWithCapacityTwo.Get("session-key-comet")

	cacheWithCapacityTwo.Put("session-key-pulsar", "payload-pulsar")

	_, novaExists := cacheWithCapacityTwo.Get("session-key-nova")
	_, cometExists := cacheWithCapacityTwo.Get("session-key-comet")

	// Assert.
	if novaExists {
		test.Fatalf("expected entry not accessed recently to be evicted")
	}

	if !cometExists {
		test.Fatalf("expected recently accessed entry to remain in cache")
	}
}

// TestLRUCache_DeleteRemovesExistingEntry verifies that Delete
// removes an existing entry from the cache.
func TestLRUCache_DeleteRemovesExistingEntry(test *testing.T) {
	// Arrange.
	cacheWithCapacityThree := NewLRUCache(3)

	cacheWithCapacityThree.Put("session-key-vortex", "payload-vortex")

	// Act.
	deleteResult := cacheWithCapacityThree.Delete("session-key-vortex")

	_, existsAfterDeletion := cacheWithCapacityThree.Get("session-key-vortex")

	// Assert.
	if !deleteResult {
		test.Fatalf("expected Delete to return true for existing key")
	}

	if existsAfterDeletion {
		test.Fatalf("expected entry to be removed after Delete")
	}
}

// TestLRUCache_DeleteReturnsFalseForMissingKey verifies that Delete
// returns false when the key does not exist.
func TestLRUCache_DeleteReturnsFalseForMissingKey(test *testing.T) {
	// Arrange.
	cacheWithSingleCapacity := NewLRUCache(1)

	// Act.
	deleteResult := cacheWithSingleCapacity.Delete("session-key-nonexistent")

	// Assert.
	if deleteResult {
		test.Fatalf("expected Delete to return false for missing key")
	}
}

// TestLRUCache_LengthReflectsNumberOfEntries verifies that Length
// accurately reports the current number of stored entries.
func TestLRUCache_LengthReflectsNumberOfEntries(test *testing.T) {
	// Arrange.
	cacheWithCapacityTwo := NewLRUCache(2)

	// Act.
	cacheWithCapacityTwo.Put("session-key-alpha", "payload-alpha")
	cacheWithCapacityTwo.Put("session-key-beta", "payload-beta")

	cacheWithCapacityTwo.Delete("session-key-alpha")

	reportedLength := cacheWithCapacityTwo.Length()

	// Assert.
	if reportedLength != 1 {
		test.Fatalf("expected cache length to be 1, got %d",
			reportedLength,
		)
	}
}

// TestLRUCache_DisabledCapacityDoesNotStoreEntries verifies that
// a cache created with non-positive capacity does not store entries.
func TestLRUCache_DisabledCapacityDoesNotStoreEntries(test *testing.T) {
	// Arrange.
	disabledCapacityCache := NewLRUCache(0)

	// Act.
	disabledCapacityCache.Put("session-key-disabled", "payload-disabled")

	_, entryExists := disabledCapacityCache.Get("session-key-disabled")

	// Assert.
	if entryExists {
		test.Fatalf("expected disabled-capacity cache to store no entries")
	}
}

// TestLRUCache_PutUpdatesExistingKeyAndPromotes verifies that updating
// an existing key changes its value and moves it to the front.
func TestLRUCache_PutUpdatesExistingKeyAndPromotes(test *testing.T) {
	// Arrange.
	cacheWithCapacityTwo := NewLRUCache(2)

	cacheWithCapacityTwo.Put("session-key-alpha", "payload-alpha-initial")
	cacheWithCapacityTwo.Put("session-key-beta", "payload-beta")

	// Act.
	cacheWithCapacityTwo.Put("session-key-alpha", "payload-alpha-updated")
	cacheWithCapacityTwo.Put("session-key-gamma", "payload-gamma")

	_, betaExists := cacheWithCapacityTwo.Get("session-key-beta")
	alphaPayload, alphaExists := cacheWithCapacityTwo.Get("session-key-alpha")

	// Assert.
	if betaExists {
		test.Fatalf("expected 'session-key-beta' to be evicted after 'session-key-alpha' was promoted")
	}

	if !alphaExists || alphaPayload != "payload-alpha-updated" {
		test.Fatalf("expected 'session-key-alpha' to exist with the updated value")
	}
}

// TestLRUCache_ConcurrentAccess ensures the cache is thread-safe
// and does not trigger data races under heavy load.
func TestLRUCache_ConcurrentAccess(test *testing.T) {
	// Arrange.
	cache := NewLRUCache(50)

	workerCount := 100
	iterations := 1000

	var waitGroup sync.WaitGroup

	// Act.
	for workerIndex := 0; workerIndex < workerCount; workerIndex++ {
		waitGroup.Add(1)

		go func(workerID int) {
			defer waitGroup.Done()

			for index := 0; index < iterations; index++ {
				key := fmt.Sprintf("key-%d", index%100)

				cache.Put(key, index)
				cache.Get(key)

				if index%10 == 0 {
					cache.Delete(key)
				}
			}
		}(workerIndex)
	}

	waitGroup.Wait()

	// Assert.
}

// TestLRUCache_NilReceiverSafety verifies that all public methods
// can be safely called on a nil pointer without causing a panic.
func TestLRUCache_NilReceiverSafety(test *testing.T) {
	// Arrange.
	var nilCache *LRUCache

	// Act & Assert.
	nilCache.Put("key", "value")

	_, exists := nilCache.Get("key")

	if exists {
		test.Fatalf("expected Get on nil cache to return false")
	}

	if length := nilCache.Length(); length != 0 {
		test.Fatalf("expected Length on nil cache to be 0, got %d", length)
	}

	if deleted := nilCache.Delete("key"); deleted {
		test.Fatalf("expected Delete on nil cache to return false")
	}
}

// TestLRUCache_UpdateKeyAtCapacityDoesNotEvict verifies that updating
// an existing key when the cache is at maximum capacity does not
// trigger an unnecessary eviction of other entries.
func TestLRUCache_UpdateKeyAtCapacityDoesNotEvict(test *testing.T) {
	// Arrange.
	cacheAtCapacity := NewLRUCache(2)

	cacheAtCapacity.Put("key-a", "payload-a")
	cacheAtCapacity.Put("key-b", "payload-b")

	// Act.
	cacheAtCapacity.Put("key-a", "payload-a-updated")

	_, keyAExists := cacheAtCapacity.Get("key-a")
	_, keyBExists := cacheAtCapacity.Get("key-b")

	currentLength := cacheAtCapacity.Length()

	// Assert.
	if !keyAExists || !keyBExists {
		test.Fatalf("expected both entries to remain in cache after update")
	}

	if currentLength != 2 {
		test.Fatalf("expected length 2, got %d", currentLength)
	}
}

// TestLRUCache_ClearEmptyCacheIsSafe verifies that calling Delete or Remove
// on an empty cache or a key that never existed results in a safe no-op.
func TestLRUCache_ClearEmptyCacheIsSafe(test *testing.T) {
	// Arrange.
	emptyCache := NewLRUCache(10)

	// Act.
	deleteResult := emptyCache.Delete("non-existent-key")
	removeResult := emptyCache.Remove("non-existent-key")
	finalLength := emptyCache.Length()

	// Assert.
	if deleteResult || removeResult {
		test.Fatalf("expected deletion of non-existent key to return false")
	}

	if finalLength != 0 {
		test.Fatalf("expected length to remain 0, got %d", finalLength)
	}
}

// TestLRUCache_RemoveLeastRecentlyUsedRepeatedly verifies that the cache
// remains stable and accurate when forced to evict multiple items in
// rapid succession to maintain capacity.
func TestLRUCache_RemoveLeastRecentlyUsedRepeatedly(test *testing.T) {
	// Arrange.
	capacity := 3

	cache := NewLRUCache(capacity)

	// Act.
	cache.Put("1", "v1")
	cache.Put("2", "v2")
	cache.Put("3", "v3")

	cache.Put("4", "v4")
	cache.Put("5", "v5")
	cache.Put("6", "v6")

	// Assert.
	if length := cache.Length(); length != capacity {
		test.Fatalf("expected length %d, got %d", capacity, length)
	}

	keysThatShouldBeEvicted := []string{"1", "2", "3"}

	for _, key := range keysThatShouldBeEvicted {
		if _, exists := cache.Get(key); exists {
			test.Fatalf("expected key %s to be evicted", key)
		}
	}
}

// TestLRUCache_GetOnNonExistentKeyDoesNotUpdateRecency verifies that
// attempting to retrieve a missing key does not accidentally promote
// or disturb the existing order of valid entries.
func TestLRUCache_GetOnNonExistentKeyDoesNotUpdateRecency(test *testing.T) {
	// Arrange.
	cache := NewLRUCache(2)

	cache.Put("oldest", "payload-old")
	cache.Put("newest", "payload-new")

	// Act.
	cache.Get("ghost-key")

	cache.Put("trigger", "payload-trigger")

	// Assert.
	_, oldestExists := cache.Get("oldest")
	_, newestExists := cache.Get("newest")

	if oldestExists {
		test.Fatalf("expected 'oldest' to be evicted; Get on missing key should not change recency")
	}
	if !newestExists {
		test.Fatalf("expected 'newest' to remain in cache")
	}
}

// TestLRUCache_ReinsertionAfterDelete verifies that a key can be
// successfully re-added to the cache after being explicitly deleted.
func TestLRUCache_ReinsertionAfterDelete(test *testing.T) {
	// Arrange.
	cache := NewLRUCache(1)

	key := "reincarnation-key"

	// Act.
	cache.Put(key, "first-life")

	cache.Delete(key)

	cache.Put(key, "second-life")

	value, exists := cache.Get(key)

	// Assert.
	if !exists {
		test.Fatalf("expected key to exist after re-insertion")
	}

	if value != "second-life" {
		test.Fatalf("expected value 'second-life', got %v", value)
	}
}

// TestLRUCache_MassiveKeyCollision verifies that the cache correctly
// handles a high volume of updates to the same key, ensuring the
// linked list does not develop circular references or leaks.
func TestLRUCache_MassiveKeyCollision(test *testing.T) {
	// Arrange.
	cache := NewLRUCache(10)

	constantKey := "static-node"

	// Act.
	for index := 0; index < 1000; index++ {
		cache.Put(constantKey, index)
	}

	// Assert.
	value, exists := cache.Get(constantKey)

	if !exists || value != 999 {
		test.Fatalf("expected value 999 after 1000 updates, got %v", value)
	}

	if length := cache.Length(); length != 1 {
		test.Fatalf("expected length 1, got %d", length)
	}
}

// TestLRUCache_PartialInitializationSafety verifies that if a struct
// is created without the NewLRUCache constructor, it handles the
// missing controller gracefully.
func TestLRUCache_PartialInitializationSafety(test *testing.T) {
	// Arrange.
	incompleteCache := &LRUCache{}

	// Act & Assert.
	incompleteCache.Put("key", "val")

	if length := incompleteCache.Length(); length != 0 {
		test.Fatalf("expected 0 length for uninitialized cache")
	}

	if _, exists := incompleteCache.Get("key"); exists {
		test.Fatalf("expected Get to fail on uninitialized cache")
	}
}
