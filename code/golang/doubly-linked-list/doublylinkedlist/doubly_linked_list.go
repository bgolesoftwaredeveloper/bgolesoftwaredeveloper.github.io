// -----------------------------------------------------------------------------
// Package: doublylinkedlist
// File: doubly_linked_list.go
//
// Description:
//
//	This file contains an explicit implementation of a doubly linked list.
//	The implementation prioritizes clarity, correctness, and safe pointer
//	manipulation over performance micro-optimizations.
//
//	The list maintains both head and tail pointers, enabling constant-time
//	insertion and removal at both ends. All operations preserve internal
//	invariants and are defensively implemented to avoid panics due to
//	nil receivers or empty-list operations.
//
// Design Notes:
//   - The list stores values as interface{} to allow heterogeneous data.
//   - The head pointer always references the first element or nil.
//   - The tail pointer always references the last element or nil.
//   - The length field tracks the number of elements and is updated eagerly.
//   - All public methods are safe to call on a nil receiver.
//   - Removal operations properly unlink nodes and clear references.
//   - The zero value of DoublyLinkedList represents a valid empty list.
//
// Intended Use:
//
//	This package is intended for learning and demonstration of pointer-based
//	data structures in Go and as a reference implementation of a classic
//	doubly linked list.
//
// -----------------------------------------------------------------------------
package doublylinkedlist

// DoublyLinkedList represents a classic doubly linked list with
// head and tail pointers and a tracked element count.
type DoublyLinkedList struct {
	head   *node
	tail   *node
	length int
}

// New creates and returns a new empty doubly linked list.
// The returned list is initialized to a valid zero state.
func New() *DoublyLinkedList {
	return &DoublyLinkedList{}
}

// IsEmpty reports whether the list contains no elements.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (list *DoublyLinkedList) IsEmpty() bool {
	if list == nil {
		return true
	}

	return list.length == 0
}

// Length returns the number of elements currently stored in the list.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (list *DoublyLinkedList) Length() int {
	if list == nil {
		return 0
	}

	return list.length
}

// Front returns the value stored at the front of the list without removing it.
// If the list is empty or nil, nil and false are returned.
// This operation runs in constant time.
func (list *DoublyLinkedList) Front() (interface{}, bool) {
	if list == nil || list.head == nil {
		return nil, false
	}

	return list.head.value, true
}

// Back returns the value stored at the back of the list without removing it.
// If the list is empty or nil, nil and false are returned.
// This operation runs in constant time.
func (list *DoublyLinkedList) Back() (interface{}, bool) {
	if list == nil || list.tail == nil {
		return nil, false
	}

	return list.tail.value, true
}

// PushFront inserts a new value at the front of the list.
// If the list is empty, the new element becomes both the head and tail.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (list *DoublyLinkedList) PushFront(value interface{}) {
	if list == nil {
		return
	}

	newNode := &node{value: value}

	// Handle insertion into an empty list.
	if list.head == nil {
		list.head = newNode
		list.tail = newNode
		list.length = 1

		return
	}

	// Link the new node before the current head.
	newNode.next = list.head
	list.head.previous = newNode
	list.head = newNode

	list.length++
}

// PushBack inserts a new value at the back of the list.
// If the list is empty, the new element becomes both the head and tail.
// This method is safe to call on a nil receiver.
// This operation runs in constant time.
func (list *DoublyLinkedList) PushBack(value interface{}) {
	if list == nil {
		return
	}

	newNode := &node{value: value}

	// Handle insertion into an empty list.
	if list.tail == nil {
		list.head = newNode
		list.tail = newNode
		list.length = 1

		return
	}

	// Link the new node after the current tail.
	newNode.previous = list.tail
	list.tail.next = newNode
	list.tail = newNode

	list.length++
}

// PopFront removes and returns the value at the front of the list.
// If the list is empty or nil, no removal occurs.
// This operation runs in constant time.
func (list *DoublyLinkedList) PopFront() (interface{}, bool) {
	if list == nil || list.head == nil {
		return nil, false
	}

	// Save a reference to the current head.
	headReference := list.head

	// Advance the head pointer.
	list.head = headReference.next

	if list.head != nil {
		list.head.previous = nil
	} else {
		list.tail = nil
	}

	list.length--

	// Extract the value and fully unlink the node.
	value := headReference.value
	headReference.previous = nil
	headReference.next = nil

	return value, true
}

// PopBack removes and returns the value at the back of the list.
// If the list is empty or nil, no removal occurs.
// This operation runs in constant time.
func (list *DoublyLinkedList) PopBack() (interface{}, bool) {
	if list == nil || list.tail == nil {
		return nil, false
	}

	// Save a reference to the current tail.
	tailReference := list.tail

	// Move the tail pointer backward.
	list.tail = tailReference.previous

	if list.tail != nil {
		list.tail.next = nil
	} else {
		list.head = nil
	}

	list.length--

	// Extract the value and fully unlink the node.
	value := tailReference.value
	tailReference.previous = nil
	tailReference.next = nil

	return value, true
}

// ForEach iterates over the list from front to back and invokes the provided
// callback for each stored value.
// If the list or callback is nil, iteration does not occur.
// This operation runs in linear time.
func (list *DoublyLinkedList) ForEach(callback func(value interface{})) {
	if list == nil || callback == nil {
		return
	}

	// Traverse the list from head to tail.
	for current := list.head; current != nil; current = current.next {
		callback(current.value)
	}
}

// Clear removes all elements from the list and resets its state.
// All internal node references are explicitly cleared to allow prompt
// garbage collection.
// This method is safe to call on a nil receiver.
// This operation runs in linear time.
func (list *DoublyLinkedList) Clear() {
	if list == nil {
		return
	}

	current := list.head

	// Traverse and unlink all nodes.
	for current != nil {
		next := current.next

		current.previous = nil
		current.next = nil

		current = next
	}

	list.head = nil
	list.tail = nil
	list.length = 0
}
