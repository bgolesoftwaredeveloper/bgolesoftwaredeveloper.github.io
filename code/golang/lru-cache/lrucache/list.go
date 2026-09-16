// -----------------------------------------------------------------------------
// Package: lrucache
// File: list.go
//
// Description:
//
//	This file implements a doubly linked list specifically designed to manage
//	the recency ordering of cache entries.
//
//	The recency list allows the cache to track which items are "Most Recently
//	Used" (MRU) and "Least Recently Used" (LRU) with O(1) efficiency for
//	all operations including detachment, promotion, and eviction.
//
// Design Notes:
//   - The 'head' represents the Most Recently Used (MRU) entry.
//   - The 'tail' represents the Least Recently Used (LRU) entry.
//   - All methods are designed to be defensive against null nodes or lists.
//   - Internal pointers (next/previous) are strictly managed to prevent
//     memory leaks and circular references.
//
// -----------------------------------------------------------------------------
package lrucache

// recencyList manages the head and tail of the doubly linked recency tracking.
type recencyList struct {
	head *cacheEntry
	tail *cacheEntry
}

// newRecencyList creates and returns a pointer to an empty recency list.
func newRecencyList() *recencyList {
	return &recencyList{}
}

// detechNode removes a node from its current position in the list.
// This method is defensive:
//   - A nil list or node results in a no-op.
//   - It validates if the node is actually part of the list before processing.
//   - It correctly updates head and tail pointers when the node is a boundary.
func (list *recencyList) detechNode(node *cacheEntry) {
	// Handle nil receiver or node.
	if list == nil || node == nil {
		return
	}

	// Guard against attempting to detach a node that is not currently linked
	// unless it is the only node in the list (head/tail match).
	if node.previous == nil && node.next == nil && list.head != node && list.tail != node {
		return
	}

	// Update the previous node's forward pointer.
	if node.previous != nil {
		node.previous.next = node.next
	} else {
		// If the node was the head, move head to the next entry.
		if list.head == node {
			list.head = node.next
		}
	}

	// Update the next node's backward pointer.
	if node.next != nil {
		node.next.previous = node.previous
	} else {
		// If the node was the tail, move tail to the previous entry.
		if list.tail == node {
			list.tail = node.previous
		}
	}

	// Clear the node's individual pointers to fully isolate it.
	node.previous = nil
	node.next = nil
}

// addNodeToFront inserts a detached node at the head (MRU) position.
// This method is defensive:
//   - It ensures the node is not already linked elsewhere.
//   - It initializes the tail if the list was previously empty.
func (list *recencyList) addNodeToFront(node *cacheEntry) {
	// Handle nil receiver or node.
	if list == nil || node == nil {
		return
	}

	// Guard against adding a node that is already linked.
	if node.previous != nil || node.next != nil || list.head == node {
		return
	}

	// Link the node to the current head.
	node.previous = nil
	node.next = list.head

	if list.head != nil {
		list.head.previous = node
	}

	// Establish the node as the new head.
	list.head = node

	// Initialize tail if this is the first node in the list.
	if list.tail == nil {
		list.tail = node
	}
}

// moveNodeToFront promotes an existing node to the most recently used status.
// If the node is already at the head, no operation is performed.
func (list *recencyList) moveNodeToFront(node *cacheEntry) {
	// Handle nil receiver or node.
	if list == nil || node == nil {
		return
	}

	// Optimization: avoid work if node is already MRU.
	if node == list.head {
		return
	}

	// Detach from current position and re-insert at front.
	list.detechNode(node)
	list.addNodeToFront(node)
}

// removeNode removes a specific node from the recency list entirely.
func (list *recencyList) removeNode(node *cacheEntry) {
	// Handle nil receiver or node.
	if list == nil || node == nil {
		return
	}

	list.detechNode(node)
}

// removeLeastRecentlyUsed identifies and detaches the tail of the list.
// Returns the removed entry so it can be deleted from the cache's hash map.
func (list *recencyList) removeLeastRecentlyUsed() *cacheEntry {
	// Handle nil receiver or empty list.
	if list == nil || list.tail == nil {
		return nil
	}

	// Select the tail (LRU).
	node := list.tail

	// Detach it from the list structure.
	list.detechNode(node)

	return node
}
