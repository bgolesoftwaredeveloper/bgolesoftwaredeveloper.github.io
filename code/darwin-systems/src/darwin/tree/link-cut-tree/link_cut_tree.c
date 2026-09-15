/**
 * @file        :   link_cut_tree.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of an augmented Link-Cut Tree forest API.
 * @details     :   This file contains the foundational implementations for managing a dynamic 
 * forest representation using auxiliary Splay Trees. It exposes interfaces for structural alterations, 
 * path optimizations, aggregate evaluations, lazy property propagation, and meticulous memory reclamation.
 * @version     :   1.0
 * @date        :   2026-05-22
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/link_cut_tree.h"

/**
 * @brief       :   Checks if a node is the root of its local auxiliary splay tree.
 * @details     :   Determines splay root status by verifying if the parent pointer 
 *                  is null, or if the parent's left and right child pointers do 
 *                  not point back to this node (representing a dashed path pointer).
 * @param       :   node Pointer to the constant node evaluating structural boundary orientation.
 * @return      :   true if the node acts as a local splay root, false otherwise.
 */
static inline bool is_splay(const Node* node)
{
    // A null node cannot belong to a splay tree.
    if (node == NULL)
    {
        return false;
    }

    // Returns true if parent is null or if parent does not point to this node as a child.
    return (node->parent == NULL) || (node->parent->children[0] != node && node->parent->children[1] != node);
}

/**
 * @brief       :   Propagates lazy reversal tags down to child subtrees.
 * @details     :   Swaps the left and right children of the current node if the 
 *                  reversal flag is active, and defers the flag down to existing children.
 * @param       :   node Pointer to the target node whose structural fields require evaluation.
 */
static inline void push_down(Node* node)
{
    // Only process nodes that explicitly hold a pending reversal instruction tag.
    if (node != NULL && node->is_reversed)
    {
        // Swap left and right child pointers to reverse path orientation locally.
        Node* temp = node->children[0];
        node->children[0] = node->children[1];
        node->children[1] = temp;

        // Pass down the reversal flag to the left child using bitwise XOR.
        if (node->children[0] != NULL)
        {
            node->children[0]->is_reversed ^= true;
        }

        // Pass down the reversal flag to the right child using bitwise XOR.
        if (node->children[1] != NULL)
        {
            node->children[1]->is_reversed ^= true;
        }

        // Clear local flag since modification has been fully propagated downwards.
        node->is_reversed = false;
    }
}

/**
 * @brief       :   Recalculates subtree maximum aggregates up from child paths.
 * @param       :   node Pointer to the node whose aggregate value field requires updating.
 * @details     :   Inspects the local value and both child nodes to find and cache 
 * the highest maximum path attribute under this subtree frame.
 */
static inline void push_up(Node* node)
{
    // Base case: Exit immediately if the target structure context is invalid.
    if (node == NULL)
    {
        return;
    }

    // Default the maximum path aggregate back to the node's individual raw value.
    node->maximum = node->value;

    // Check left subtree maximum bounds.
    if (node->children[0] != NULL)
    {
        node->maximum = MAXIMUM(node->maximum, node->children[0]->maximum);
    }

    // Check right subtree maximum bounds.
    if (node->children[1] != NULL)
    {
        node->maximum = MAXIMUM(node->maximum, node->children[1]->maximum);
    }
}

/**
 * @brief       :   Performs a localized tree rotation operation to adjust balanced height.
 * @details     :   Executes a standard balanced splay tree rotation step, updating 
 *                  parent-child relations and re-evaluating aggregate properties.
 * @param       :   node Pointer to the target node traversing upwards via structural re-linking.
 */
static inline void rotate(Node* node)
{
    // Verify parameters and stop rotation if the element is already a splay root.
    if (node == NULL || is_splay(node))
    {
        return;
    }

    Node* parent = node->parent;
    Node* grandparent = parent->parent;

    // Determine if the current node is a right-side child.
    bool is_right_child = (parent->children[1] == node);

    // Link grandparent to the current node if the parent wasn't a splay tree root.
    if (!is_splay(parent))
    {
        grandparent->children[grandparent->children[1] == parent] = node;
    }

    // Set the current node's parent pointer to the grandparent.
    node->parent = grandparent;

    // Shift subtrees: assign the node's opposite inner child to the parent.
    parent->children[is_right_child] = node->children[is_right_child ^ 1];

    // Re-link parent back pointer inside the shifted subtree if it exists.
    if (node->children[is_right_child ^ 1] != NULL)
    {
        node->children[is_right_child ^ 1]->parent = parent;
    }

    // Move parent node down to become the opposite child of this current node.
    node->children[is_right_child ^ 1] = parent;

    // Update parent's back pointer to point to the current node.
    parent->parent = node;

    // Fix path maximum properties starting from the lowest modified ancestor up.
    push_up(parent);
    push_up(node);
}

/**
 * @brief       :   Recursively descends from the splay root to evaluate pending lazy tags.
 * @details     :   Guarantees that all deferred lazy modification flags are pushed down 
 *                  completely before any splay structural changes take place on the target lineage.
 * @param       :   node Pointer to the target node whose baseline lineage requires evaluation.
 */
static inline void push_all_ancestors(Node* node)
{
    // Base case: End traversal when hitting the boundary of the tree view.
    if (node == NULL)
    {
        return;
    }

    // Ascend recursively to the root of the auxiliary splay tree first.
    if (!is_splay(node))
    {
        push_all_ancestors(node->parent);
    }

    // Distribute deferred modification indicators down as recursion unwinds.
    push_down(node);
}

/**
 * @brief       :   Allocates and initializes a new link-cut tree forest element wrapper.
 * @details     :   Sets all child and parent relations to empty fields and sets the 
 *                  initial maximum tracking aggregate equal to the single raw weight value.
 * @param       :   id Unique structural index value for identifier mappings.
 * @param       :   value Initial baseline numerical data weight assigned to the element.
 * @return      :   Pointer to the newly instantiated Node, or NULL on memory allocation failure.
 */
Node* construct(size_t id, int value)
{
    // Request raw stack heap slice from memory manager.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Bind metadata variables.
    new_node->id = id;

    // Establish metric baselines.
    new_node->value = value;
    new_node->maximum = value;

    // Initialize state flags to deactivated defaults.
    new_node->is_reversed = false;

    // Initialize tree connection fields.
    new_node->parent = NULL;

    new_node->children[0] = NULL;
    new_node->children[1] = NULL;

    return new_node;
}

/**
 * @brief       :   Brings a node to the absolute root of its local auxiliary splay tree representation.
 * @details     :   Repeatedly applies zig-zig and zig-zag rotation steps to lift the node 
 *                  to the localized root position after updating lazy markers.
 * @param       :   node Pointer to the node tracking upwards into primary execution focus.
 */
void splay(Node* node)
{
    if (node == NULL)
    {
        return;
    }

    // Force evaluation and cleaning of all pending lazy operations across ancestors.
    push_all_ancestors(node);

    // Loop rotations until this node becomes the local root of its auxiliary tree.
    while (!is_splay(node))
    {
        Node* parent = node->parent;
        Node* grandparent = parent->parent;

        // Perform double-rotation configurations if grandparent can be safely traversed.
        if (!is_splay(parent))
        {
            bool parent_is_right = (grandparent->children[1] == parent);
            bool node_is_right = (parent->children[1] == node);

            // Zig-Zag case: rotate the target node twice.
            if (parent_is_right ^ node_is_right)
            {
                rotate(node);
            }
            // Zig-Zig case: rotate parent first, then the target node.
            else
            {
                rotate(parent);
            }
        }

        // Complete final adjustment step.
        rotate(node);
    }
}

/**
 * @brief       :   Exposes a dedicated preferred path running from the absolute forest root down to this node.
 * @details     :   Splices the target node sequence into a single preferred path splay tree 
 *                  by altering right-side child relationships and cascading towards the root parent.
 * @param       :   node Pointer to the structural endpoint completing the target path sequence.
 */
void access(Node* node)
{
    if (node == NULL)
    {
        return;
    }

    // Ascend towards the forest root, shifting child configurations step-by-step.
    for (Node* current = NULL; node != NULL; current = node, node = node->parent)
    {
        // Bring the ancestor directly into the root slot of its auxiliary tree.
        splay(node);

        // Convert right child to a dashed pointer, replacing it with the preferred path branch.
        node->children[1] = current;

        // Refresh path maximum values now that subtrees have been altered.
        push_up(node);
    }
}
/**
 * @brief       :   Alters global forest topology to assign this node as the absolute root of its component tree.
 * @details     :   Accesses and splays the node before applying an inversion flag to flip 
 *                  the sequence direction completely.
 * @param       :   node Pointer to the node assuming complete structural lineage dominance.
 * @return      :   true if topological reconfiguration succeeds, false if the node parameter is invalid.
 */
bool assign_root(Node* node)
{
    if (node == NULL)
    {
        return false;
    }

    // Expose a dedicated path stretching from the node up to the current absolute root.
    access(node);

    // Lift the node up to become the local splay root framework anchor.
    splay(node);

    // Flip the inversion state to mirror child tree paths completely.
    node->is_reversed ^= true;

    return true;
}

/**
 * @brief       :   Identifies the absolute root component element currently governing this node sequence.
 * @details     :   Traces down the leftmost paths of the auxiliary tree view after exposing 
 *                  the structure to find the original source component root node.
 * @param       :   node Pointer to the node whose base structure tracking lineage is evaluated.
 * @return      :   Pointer to the primary root node element validating component convergence.
 */
Node* find(Node* node)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Construct an uninterrupted preferred path linking up to the component root.
    access(node);
    splay(node);

    // Track leftmost branches down to find the oldest ancestor in the path.
    while (node != NULL)
    {
        push_down(node);

        if (node->children[0] == NULL)
        {
            break;
        }

        node = node->children[0];
    }

    // Maintain tree balance properties by splaying the identified root element.
    splay(node);

    return node;
}

/**
 * @brief       :   Establishes a valid structural edge connecting a detached source tree into a target tree.
 * @details     :   Designates the source element as the root of its localized framework, verifies 
 *                  connectivity bounds to block loop creation, and assigns the target node as parent.
 * @param       :   source Pointer to the node acting as the origin element of the new path relation.
 * @param       :   target Pointer to the node acting as the parent destination receiver.
 * @return      :   true if the link successfully materializes, false if an illegal cycle or invalidity is caught.
 */
bool link(Node* source, Node* target)
{
    if (source == NULL || target == NULL)
    {
        return false;
    }

    // Convert the source element into the root of its distinct structural system component.
    assign_root(source);

    // Prevent loop formation by ensuring the source and target are completely disconnected.
    if (find(target) == source)
    {
        return false;
    }

    // Connect the components together using a dashed path-pointer relationship.
    source->parent = target;

    return true;
}

/**
 * @brief       :   Breaks an active dynamic tracking edge connecting two adjacent forest node elements.
 * @details     :   Isolates the connection sequence into an exclusive path frame and verifies 
 *                  direct structural contact before erasing adjacent pointer properties.
 * @param       :   source Pointer to the first endpoint validating edge structural removal.
 * @param       :   target Pointer to the adjacent endpoint confirming separation.
 * @return      :   true if the edge existed and was broken cleanly, false if elements are disconnected.
 */
bool cut(Node* source, Node* target)
{
    if (source == NULL || target == NULL)
    {
        return false;
    }

    // Isolate the tree structure orientation around the source point.
    assign_root(source);

    // Construct an explicit preferred path targeting the second edge component.
    access(target);
    splay(target);

    // Verify if source is a direct child of target and check for any intermediate nodes.
    if (target->children[0] == source && source->children[1] == NULL)
    {
        // Sever pointer bonds to separate the components.
        target->children[0] = NULL;
        source->parent = NULL;

        // Update target aggregate fields now that the left branch is gone.
        push_up(target);

        return true;
    }

    return false;
}

/**
 * @brief       :   Evaluates the maximum path aggregate value tracked between two distinct forest nodes.
 * @details     :   Transforms the target nodes into a shared localized preferred path framework, 
 *                  asserts connectivity state matching, and harvests the calculated maximum metadata value.
 * @param       :   source Pointer to the initial path bounds tracking anchor element.
 * @param       :   target Pointer to the terminal destination path boundary element.
 * @param       :   flag Pointer to a status tracking boolean variable indicating success or isolation states.
 * @return      :   The highest numerical weight aggregate value found along the structural path.
 */
int query(Node* source, Node* target, bool* flag)
{
    if (source == NULL || target == NULL)
    {
        if (flag != NULL)
        {
            *flag = false;
        }

        return 0;
    }

    // Align structural parentages around the initial source anchor.
    assign_root(source);

    // Check components to make sure a valid path actually exists between both targets.
    if (find(target) != source)
    {
        if (flag != NULL)
        {
            *flag = false;
        }

        return 0;
    }

    // Group the path into an isolated auxiliary tree component to read its metadata.
    access(target);
    splay(target);

    // Set success flag indicating clear verification.
    if (flag != NULL)
    {
        *flag = true;
    }

    // Return the precalculated path maximum aggregate cached directly at the root.
    return target->maximum;
}

/**
 * @brief       :   Severs all connected tree path relationships and isolates an element before freeing it.
 * @details     :   Brings the target node into the structural focus root frame, severs left 
 *                  and right child ties from neighbors, and reclaims allocated heap memory.
 * @param       :   node Pointer to the target tree node element destined for immediate removal.
 * @return      :   true if the node is structurally detached and freed successfully, false otherwise.
 */
bool erase(Node* node)
{
    if (node == NULL)
    {
        return false;
    }

    // Force the target element to become the absolute root of the forest tracking view.
    assign_root(node);

    // Separate the left child tree branch completely.
    if (node->children[0] != NULL)
    {
        node->children[0]->parent = NULL;
        node->children[0] = NULL;
    }

    // Separate the right child tree branch completely.
    if (node->children[1] != NULL)
    {
        node->children[1]->parent = NULL;
        node->children[1] = NULL;
    }

    // Reclaim heap memory space safely.
    free(node);

    return true;
}

/**
 * @brief       :   Recursively descends through an entire isolated tree structural component to free memory spaces.
 * @details     :   Uses a clean structural traversal tracking model to dissolve sub-branches 
 *                  and deallocate nodes cleanly without leaving detached tracking points.
 * @param       :   node Pointer to the localized root element managing the targeted clearing component.
 */
void clean(Node* node)
{
    // Base case: Exit traversal when hitting the boundary leaf coordinates.
    if (node == NULL)
    {
        return;
    }

    // Descend along the left splay sub-branch direction.
    clean(node->children[0]);

    // Descend along the right splay sub-branch direction.
    clean(node->children[1]);

    // Deallocate the current tracking node container safely.
    free(node);
}
