// -----------------------------------------------------------------------------
// Package: openaddressedhashtable
// File: constants.go
//
// Description:
//
//	This file defines default configuration constants for the open-addressed
//	hash table implementation. The constants specify the initial table
//	capacity and the load factor threshold used to trigger table resizing.
//
// -----------------------------------------------------------------------------
package openaddressedhashtable

const (
	defaultLoadFactor = 0.75
	defaultCapacity   = 16
)
