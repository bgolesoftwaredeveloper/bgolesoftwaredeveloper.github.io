// -----------------------------------------------------------------------------
// Package: linkcuttree
// File: link_cut_tree_test.go
//
// Description:
//
//	This file contains unit tests for the Link-Cut Tree (LCT) implementation.
//	The tests validate core dynamic tree operations, including linking,
//	cutting, rerooting, and connectivity queries.
//
//	Each test is written in a defensive, step-by-step manner to make the
//	intended tree structure and expected behavior explicit. The tests favor
//	clarity and correctness over brevity, and are designed to mirror realistic
//	usage patterns exercised by the demonstration program.
//
//	Test functions follow a consistent Arrange / Act / Assert structure and
//	use descriptive variable names to clearly communicate intent.
//
// -----------------------------------------------------------------------------
package linkcuttree

import "testing"

// TestLinkAndConnected verifies that linking nodes results in correct
// connectivity within a represented tree.
func TestLinkAndConnected(test *testing.T) {
	// Arrange.
	rootNode := &LCTNode{}
	intermediateNode := &LCTNode{}
	terminalNode := &LCTNode{}

	// Act.
	Link(intermediateNode, rootNode)
	Link(terminalNode, intermediateNode)

	connectedRootToTerminal := Connected(rootNode, terminalNode)
	connectedTerminalToRoot := Connected(terminalNode, rootNode)

	// Assert.
	if !connectedRootToTerminal {
		test.Fatalf("expected rootNode to be connected to terminalNode")
	}

	if !connectedTerminalToRoot {
		test.Fatalf("expected terminalNode to be connected to rootNode")
	}
}

// TestDisconnectedNodes verifies that nodes which are never linked are
// correctly reported as disconnected.
func TestDisconnectedNodes(test *testing.T) {
	// Arrange.
	connectedNode := &LCTNode{}
	isolatedNode := &LCTNode{}

	// Act.
	connected := Connected(connectedNode, isolatedNode)

	// Assert.
	if connected {
		test.Fatalf("expected isolatedNode to be disconnected from connectedNode")
	}
}

// TestCutSeparatesTrees verifies that cutting an edge splits a tree into
// two independent represented trees.
func TestCutSeparatesTrees(test *testing.T) {
	// Arrange.
	rootNode := &LCTNode{}
	intermediateNode := &LCTNode{}
	terminalNode := &LCTNode{}

	Link(intermediateNode, rootNode)
	Link(terminalNode, intermediateNode)

	if !Connected(rootNode, terminalNode) {
		test.Fatalf("precondition failed: nodes should be connected before cut")
	}

	// Act.
	Cut(terminalNode, intermediateNode)

	connectedRootToTerminal := Connected(rootNode, terminalNode)
	connectedIntermediateToTerminal := Connected(intermediateNode, terminalNode)

	// Assert.
	if connectedRootToTerminal {
		test.Fatalf("expected rootNode to be disconnected from terminalNode after cut")
	}

	if connectedIntermediateToTerminal {
		test.Fatalf("expected intermediateNode to be disconnected from terminalNode after cut")
	}
}

// TestMakeRootPreservesConnectivity verifies that rerooting a tree changes
// its logical root without affecting connectivity.
func TestMakeRootPreservesConnectivity(test *testing.T) {
	// Arrange.
	originalRootNode := &LCTNode{}
	newRootCandidateNode := &LCTNode{}
	leafNode := &LCTNode{}

	Link(newRootCandidateNode, originalRootNode)
	Link(leafNode, newRootCandidateNode)

	if !Connected(originalRootNode, leafNode) {
		test.Fatalf("precondition failed: nodes should be connected before reroot")
	}

	// Act.
	newRootCandidateNode.MakeRoot()

	connectedOriginalToNewRoot := Connected(originalRootNode, newRootCandidateNode)
	connectedNewRootToLeaf := Connected(newRootCandidateNode, leafNode)

	// Assert.
	if !connectedOriginalToNewRoot {
		test.Fatalf("expected originalRootNode to remain connected after reroot")
	}

	if !connectedNewRootToLeaf {
		test.Fatalf("expected leafNode to remain connected after reroot")
	}
}

// TestNilSafety verifies that public operations tolerate nil inputs
// without panicking and behave defensively.
func TestNilSafety(test *testing.T) {
	// Arrange.
	var nilNode *LCTNode

	realNode := &LCTNode{}

	// Act.
	connected := Connected(nilNode, realNode)

	Link(nilNode, realNode)
	Link(realNode, nilNode)

	Cut(nilNode, realNode)
	Cut(realNode, nilNode)

	realNode.MakeRoot()
	nilNode.MakeRoot()

	root := nilNode.FindRoot()

	// Assert.
	if connected {
		test.Fatalf("expected Connected(nilNode, realNode) to return false")
	}
	if root != nil {
		test.Fatalf("expected FindRoot on nil receiver to return nil")
	}
}

// TestLinkDoesNotCreateCycle verifies that attempting to link two nodes
// already in the same tree does not create a cycle.
func TestLinkDoesNotCreateCycle(test *testing.T) {
	// Arrange.
	rootNode := &LCTNode{}
	childNode := &LCTNode{}

	Link(childNode, rootNode)

	if !Connected(rootNode, childNode) {
		test.Fatalf("precondition failed: nodes should be connected before cycle attempt")
	}

	// Act.
	Link(rootNode, childNode)

	connectedAfterCycleAttempt := Connected(rootNode, childNode)

	// Assert.
	if !connectedAfterCycleAttempt {
		test.Fatalf("expected nodes to remain connected after cycle prevention")
	}
}

// TestCutNonExistentEdge verifies that cutting a non-existent edge
// leaves the tree structure unchanged.
func TestCutNonExistentEdge(test *testing.T) {
	// Arrange.
	rootNode := &LCTNode{}
	leftChildNode := &LCTNode{}
	unrelatedNode := &LCTNode{}

	Link(leftChildNode, rootNode)

	if !Connected(rootNode, leftChildNode) {
		test.Fatalf("precondition failed: nodes should be connected")
	}

	// Act.
	Cut(unrelatedNode, rootNode)

	stillConnected := Connected(rootNode, leftChildNode)

	// Assert.
	if !stillConnected {
		test.Fatalf("expected tree to remain unchanged after cutting non-existent edge")
	}
}

// TestRepeatedMakeRoot verifies that calling MakeRoot multiple times
// does not corrupt tree connectivity.
func TestRepeatedMakeRoot(test *testing.T) {
	// Arrange.
	firstNode := &LCTNode{}
	secondNode := &LCTNode{}
	thirdNode := &LCTNode{}

	Link(secondNode, firstNode)
	Link(thirdNode, secondNode)

	if !Connected(firstNode, thirdNode) {
		test.Fatalf("precondition failed: nodes should be connected")
	}

	// Act.
	firstNode.MakeRoot()
	secondNode.MakeRoot()
	thirdNode.MakeRoot()

	connectedAfterReroots := Connected(firstNode, thirdNode)

	// Assert.
	if !connectedAfterReroots {
		test.Fatalf("expected connectivity to persist after repeated MakeRoot calls")
	}
}

// TestFindRootConsistency verifies that FindRoot returns the same
// representative node for all nodes in the same tree.
func TestFindRootConsistency(test *testing.T) {
	// Arrange.
	rootNode := &LCTNode{}
	middleNode := &LCTNode{}
	leafNode := &LCTNode{}

	Link(middleNode, rootNode)
	Link(leafNode, middleNode)

	// Act.
	rootFromRoot := rootNode.FindRoot()
	rootFromMiddle := middleNode.FindRoot()
	rootFromLeaf := leafNode.FindRoot()

	// Assert.
	if rootFromRoot != rootFromMiddle {
		test.Fatalf("expected consistent root for rootNode and middleNode")
	}

	if rootFromMiddle != rootFromLeaf {
		test.Fatalf("expected consistent root for middleNode and leafNode")
	}
}

// TestSingleNodeTree verifies that a single node behaves correctly
// as a standalone represented tree.
func TestSingleNodeTree(test *testing.T) {
	// Arrange.
	singleNode := &LCTNode{}

	// Act.
	root := singleNode.FindRoot()

	connectedToSelf := Connected(singleNode, singleNode)

	// Assert.
	if root != singleNode {
		test.Fatalf("expected single node to be its own root")
	}

	if !connectedToSelf {
		test.Fatalf("expected node to be connected to itself")
	}
}

// TestDeepLinearChain verifies that the LCT handles very deep paths.
// This triggers repeated splay operations and tests the amortized efficiency.
func TestDeepLinearChain(test *testing.T) {
	// Arrange.
	const nodeCount = 1000

	nodes := make([]*LCTNode, nodeCount)

	for index := 0; index < nodeCount; index++ {
		nodes[index] = &LCTNode{}
	}

	for index := 0; index < nodeCount-1; index++ {
		Link(nodes[index+1], nodes[index])
	}

	// Act.
	connectedInitially := Connected(nodes[0], nodes[nodeCount-1])

	nodes[nodeCount-1].MakeRoot()

	newRoot := nodes[nodeCount-1].FindRoot()

	// Assert.
	if !connectedInitially {
		test.Fatalf("expected head and tail of deep chain to be connected")
	}

	if newRoot != nodes[nodeCount-1] {
		test.Fatalf("expected tail to become the new root of the deep chain")
	}
}

// TestStarTopology verifies a hub-and-spoke model where one node has many children.
func TestStarTopology(test *testing.T) {
	// Arrange.
	hub := &LCTNode{}
	spokes := make([]*LCTNode, 10)

	for index := range spokes {
		spokes[index] = &LCTNode{}

		Link(spokes[index], hub)
	}

	// Act.
	for index := 0; index < 5; index++ {
		Cut(spokes[index], hub)
	}

	// Assert.
	for index := 0; index < 10; index++ {
		isConnected := Connected(hub, spokes[index])

		if index < 5 && isConnected {
			test.Errorf("spoke %d should be disconnected", index)
		}

		if index >= 5 && !isConnected {
			test.Errorf("spoke %d should still be connected", index)
		}
	}
}

// TestPathReversal verifies that MakeRoot correctly re-orients the path
// without breaking the internal splay order.
func TestPathReversal(test *testing.T) {
	// Arrange.
	nodeAlpha := &LCTNode{}
	nodeBeta := &LCTNode{}
	nodeGamma := &LCTNode{}

	Link(nodeBeta, nodeAlpha)
	Link(nodeGamma, nodeBeta)

	if nodeAlpha.FindRoot() != nodeAlpha {
		test.Fatalf("expected nodeAlpha to be root")
	}

	// Act.
	nodeGamma.MakeRoot()

	newRoot := nodeGamma.FindRoot()
	alphaNewRoot := nodeAlpha.FindRoot()

	// Assert.
	if newRoot != nodeGamma {
		test.Fatalf("expected nodeGamma to be the new root")
	}

	if alphaNewRoot != nodeGamma {
		test.Fatalf("expected nodeAlpha's root to now be nodeGamma")
	}
}

// TestConnectivityAfterMultipleCuts ensures that complex forest fragmentation
// works correctly and nodes don't stay "ghost connected."
func TestConnectivityAfterMultipleCuts(test *testing.T) {
	// Arrange.
	nodes := make([]*LCTNode, 6)

	for index := range nodes {
		nodes[index] = &LCTNode{}
	}

	Link(nodes[1], nodes[0])
	Link(nodes[2], nodes[1])

	Link(nodes[4], nodes[3])
	Link(nodes[5], nodes[4])

	// Act.
	initialConnected := Connected(nodes[0], nodes[3])

	Link(nodes[3], nodes[2])

	mergedConnected := Connected(nodes[0], nodes[5])

	Cut(nodes[3], nodes[2])

	finalConnected := Connected(nodes[0], nodes[5])

	// Assert.
	if initialConnected {
		test.Fatalf("components should be separate")
	}

	if !mergedConnected {
		test.Fatalf("components should now be merged")
	}

	if finalConnected {
		test.Fatalf("components should be separate again after bridge cut")
	}
}
