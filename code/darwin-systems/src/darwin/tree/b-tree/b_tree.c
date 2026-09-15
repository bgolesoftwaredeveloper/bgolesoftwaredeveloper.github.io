/**
 * @file        :   b_tree.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of a balanced B-Tree structure.
 * @details     :   Provides logarithmic time complexity for insertion, deletion, 
 * and search operations by maintaining balanced, multi-way nodes.
 * @version     :   1.0
 * @date        :   2026-05-21
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/tree/b_tree.h"

// Function prototypes.
static inline void erase_at(Node* node, int key);

/**
 * @brief       :   Locates the exact key or the index where it should reside.
 * @param       :   node: Pointer to the active search node.
 * @param       :   key: The value to look for.
 * @return      :   size_t: The lower-bound index matching or flanking the key.
 */
static inline size_t find_key_index(Node* node, int key)
{
    // Return base index if not is nonexistent.
    if (node == NULL)
    {
        return 0;
    }

    size_t index = 0;

    // Advance index while current key is strictly smaller than the target key.
    while (index < node->count && node->keys[index] < key)
    {
        index++;
    }

        // Index now points to the matching key or the correct child pointer branch.
    return index;
}

/**
 * @brief       :   Splits a saturated child node during an upward cascade.
 * @param       :   parent: The parent node accommodating the split median.
 * @param       :   index: The structural insertion index inside the parent.
 * @param       :   node: The saturated child node targeted for a split.
 * @return      :   void
 */
static inline void split_child(Node* parent, size_t index, Node* node)
{
    // Guard against malformed parameters or out-of-bounds pointer arrays.
    if (parent == NULL || node == NULL || index >= ORDER)
    {
        return;
    }

    // Calculate mid-point position based on configured order degree factor.
    size_t median_split_index = ORDER / 2;

    // Instantiate a sibling node to hold the upper half of the split node's elements.
    Node* new_node = construct(node->is_leaf);

    if (new_node == NULL)
    {
        return;
    }

    // Assign the count of keys passing to the new right-hand sibling node.
    new_node->count = median_split_index - 1;

    // Migrate the upper half of keys from the full node to the sibling.
    for (size_t key_index = 0; key_index < median_split_index - 1; key_index++)
    {
        new_node->keys[key_index] = node->keys[key_index + median_split_index];
    }

    // If it's an internal branch node, migrate corresponding child pointer connections.
    if (!node->is_leaf)
    {
        for (size_t child_index = 0; child_index < median_split_index; child_index++)
        {
            new_node->children[child_index] = node->children[child_index + median_split_index];
            node->children[child_index + median_split_index] = NULL;
        }
    }

    // Reduce the capacity footprint count of the original split node.
    node->count = median_split_index - 1;

    // Shift parent child pointers to the right to clear space for the new sibling pointer.
    for (size_t shifting_index = parent->count; shifting_index > index; shifting_index--)
    {
        if (shifting_index + 1 < ORDER)
        {
            parent->children[shifting_index + 1] = parent->children[shifting_index];
        }
    }
    
    // Wire up the new sibling node pointer into the parent's children array.
    if (index + 1 < ORDER)
    {
        parent->children[index + 1] = new_node;
    }

        // Shift parent keys to the right to make space for the rising median key element.
    for (size_t shifting_index = parent->count; shifting_index > index; shifting_index--)
    {
        if (shifting_index < ORDER - 1)
        {
            parent->keys[shifting_index] = parent->keys[shifting_index - 1];
        }
    }
    
        // Copy the median key up into the parent array slot and increment total size.
    parent->keys[index] = node->keys[median_split_index - 1];
    parent->count++;
}

/**
 * @brief       :   Inserts a key down a path ensuring destination capacity.
 * @param       :   node: The working root of the non-full subtree.
 * @param       :   key: The value to insert.
 * @return      :   void
 */
static inline void insert_non_full(Node* node, int key)
{
    if (node == NULL)
    {
        return;
    }

    size_t index = node->count;

    if (node->is_leaf)
    {
        // For leaves, shift keys right until the exact sorted slot is clear.
        while (index > 0 && node->keys[index - 1] > key)
        {
            node->keys[index] = node->keys[index - 1];
            index--;
        }

        // Drop the key into the open slot and update local size.
        node->keys[index] = key;
        node->count++;
    }
    else
    {
        // For internal nodes, find which child branch should receive the key.
        while (index > 0 && node->keys[index - 1] > key)
        {
            index--;
        }

        // Preemptively split the child if it is completely saturated.
        if (node->children[index]->count == ORDER - 1)
        {
            split_child(node, index, node->children[index]);

            // Determine if the target key falls left or right of the split median.
            if (node->keys[index] < key)
            {
                index++;
            }
        }

        // Continue recursion safely down the non-full child branch.
        insert_non_full(node->children[index], key);
    }
}

/**
 * @brief       :   Retrieves the logical in-order predecessor from a subtree.
 * @param       :   node: The parent internal node.
 * @param       :   index: The target pivot index.
 * @return      :   int: The predecessor key value.
 */
static inline int get_predecessor(Node* node, size_t index)
{
    if (node == NULL || node->children[index] == NULL)
    {
        return 0;
    }

    // Move to the left child node branch.
    Node* current = node->children[index];

    // Traverse down the rightmost child paths until arriving at a terminal leaf node.
    while (!current->is_leaf)
    {
        current = current->children[current->count];
    }

    // Return the absolute largest key found at the end of that leaf node.
    return current->keys[current->count - 1];
}

/**
 * @brief       :   Retrieves the logical in-order successor from a subtree.
 * @param       :   node: The parent internal node.
 * @param       :   index: The target pivot index.
 * @return      :   int: The successor key value.
 */
static inline int get_successor(Node* node, size_t index)
{
    if (node == NULL || node->children[index + 1] == NULL)
    {
        return 0;
    }

    // Move to the right child node branch.
    Node* current = node->children[index + 1];

    // Traverse down the leftmost child paths until arriving at a terminal leaf node.
    while (!current->is_leaf)
    {
        current = current->children[0];
    }

    // Return the absolute smallest key value residing at index zero of that leaf.
    return current->keys[0];
}

/**
 * @brief       :   Borrows a balance key from a child's left-side sibling.
 * @param       :   parent: The overarching structural parent node.
 * @param       :   index: The structural position index of the recipient child.
 * @return      :   void
 */
static inline void borrow_from_previous(Node* parent, size_t index)
{
    if (parent == NULL || index == 0)
    {
        return;
    }

    Node* child = parent->children[index];
    Node* sibling = parent->children[index - 1];

    if (child == NULL || sibling == NULL)
    {
        return;
    }

    // Shift all keys inside the deficient child right by one position.
    if (child->count > 0)
    {
        for (size_t shifting_index = child->count; shifting_index > 0; shifting_index--)
        {
            if (shifting_index < ORDER - 1)
            {
                child->keys[shifting_index] = child->keys[shifting_index - 1];
            }
        }
    }

    // Shift child pointer attachments right by one position if this isn't a leaf.
    if (!child->is_leaf)
    {
        for (size_t shifting_index = child->count + 1; shifting_index > 0; shifting_index--)
        {
            if (shifting_index < ORDER)
            {
                child->children[shifting_index] = child->children[shifting_index - 1];
            }
        }
    }

    // Pull the separating parent key down into index zero of the recipient child.
    child->keys[0] = parent->keys[index - 1];

    // Transfer the sibling's last child pointer to become the child's first child.
    if (!child->is_leaf)
    {
        child->children[0] = sibling->children[sibling->count];
        sibling->children[sibling->count] = NULL;
    }

    // Move the sibling's largest key up into the parent to maintain separation balance.
    parent->keys[index - 1] = sibling->keys[sibling->count - 1];

    // Readjust size balances for both nodes.
    child->count++;
    sibling->count--;
}

/**
 * @brief       :   Borrows a balance key from a child's right-side sibling.
 * @param       :   parent: The overarching structural parent node.
 * @param       :   index: The structural position index of the recipient child.
 * @return      :   void
 */
static inline void borrow_from_next(Node* parent, size_t index)
{
    if (parent == NULL || index >= parent->count)
    {
        return;
    }

    Node* child = parent->children[index];
    Node* sibling = parent->children[index + 1];

    if (child == NULL || sibling == NULL)
    {
        return;
    }

    // Pull the separating parent key down into the end of the recipient child node.
    child->keys[child->count] = parent->keys[index];

    // Attach the sibling's first child pointer to the end of the child's pointer list.
    if (!child->is_leaf)
    {
        child->children[child->count + 1] = sibling->children[0];
    }

    // Elevate the sibling's lowest key up into the parent separation slot.
    parent->keys[index] = sibling->keys[0];

    // Shift all remaining keys inside the sibling node left by one position
    for (size_t shifting_index = 1; shifting_index < sibling->count; shifting_index++)
    {
        sibling->keys[shifting_index - 1] = sibling->keys[shifting_index];
    }

    // Shift sibling child pointers left by one position if it is an internal node.
    if (!sibling->is_leaf)
    {
        for (size_t shifting_index = 1; shifting_index <= sibling->count; shifting_index++)
        {
            sibling->children[shifting_index - 1] = sibling->children[shifting_index];
        }

        sibling->children[sibling->count] = NULL;
    }

    // Readjust counter assignments.
    child->count++;
    sibling->count--;
}

/**
 * @brief       :   Merges a left child node with its adjacent right sibling.
 * @param       :   parent: The parent node managing both child branches.
 * @param       :   index: The specific key index indicating the bridge point.
 * @return      :   void
 */
static inline void merge_child_nodes(Node* parent, size_t index)
{
    if (parent == NULL || index >= parent->count)
    {
        return;
    }

    Node* left_child = parent->children[index];
    Node* right_sibliing = parent->children[index + 1];

    if (left_child == NULL || right_sibliing == NULL)
    {
        return;
    }

    size_t median_split_index = ORDER / 2;

    // Pull down the separation key from the parent into the middle of the left child.
    left_child->keys[median_split_index - 1] = parent->keys[index];

    // Copy all elements from the right sibling into the end of the left child.
    for (size_t key_index = 0; key_index < right_sibliing->count; key_index++)
    {
        left_child->keys[key_index + median_split_index] = right_sibliing->keys[key_index];
    }

    // Append child pointers from the sibling to the left child if internal node.
    if (!left_child->is_leaf)
    {
        for (size_t child_index = 0; child_index <= right_sibliing->count; child_index++)
        {
            left_child->children[child_index + median_split_index] = right_sibliing->children[child_index];
        }
    }

    // Shift parent keys left to close the gap left by the dropped separation key.
    for (size_t shifting_index = index + 1; shifting_index < parent->count; shifting_index++)
    {
        parent->keys[shifting_index - 1] = parent->keys[shifting_index];
    }

    // Shift parent child pointers left to drop out the now obsolete right sibling pointer.
    for (size_t shifting_index = index + 2; shifting_index <= parent->count; shifting_index++)
    {
        parent->children[shifting_index - 1] = parent->children[shifting_index];
    }

    parent->children[parent->count] = NULL;

    // Update cumulative counts for modified nodes.
    left_child->count += right_sibliing->count + 1;
    parent->count--;

    // Free the empty, merged right sibling node memory allocation.
    free(right_sibliing);
}

/**
 * @brief       :   Ensures a tracking child node has critical required keys.
 * @param       :   parent: The administrative parent node.
 * @param       :   index: The offset location of the deficient branch.
 * @return      :   void
 */
static inline void fill_child_node(Node* parent, size_t index)
{
    if (parent == NULL)
    {
        return;
    }

    // Case A: If left sibling has extra elements, borrow a key from it.
    if (index != 0 && parent->children[index - 1] != NULL && parent->children[index - 1]->count >= MINIMUM_KEYS)
    {
        borrow_from_previous(parent, index);
    }
    // Case B: If right sibling has extra elements, borrow a key from it.
    else if (index != parent->count && parent->children[index + 1] != NULL && parent->children[index + 1]->count >= MINIMUM_KEYS)
    {
        borrow_from_next(parent, index);
    }
    // Case C: Ssiblings are at minimum capacity. Perform a full merge operation.
    else
    {
        if (index != parent->count)
        {
            // Merge child with its right sibling if it isn't the final element.
            merge_child_nodes(parent, index);
        }
        else
        {
            // If it is the final element, merge the left sibling with this child.
            if (index > 0)
            {
                merge_child_nodes(parent, index - 1);
            }
        }
    }
}

/**
 * @brief       :   Removes an array item directly inside a leaf node structure.
 * @param       :   node: The termination leaf containing the target index.
 * @param       :   index: The internal array offset to collapse out.
 * @return      :   void
 */
static inline void erase_from_leaf(Node* node, size_t index)
{
    if (node == NULL || index >= node->count)
    {
        return;
    }

    // Shift elements left starting from the extraction point to close the array vacancy.
    for (size_t shifting_index = index + 1; shifting_index < node->count; shifting_index++)
    {
        node->keys[shifting_index - 1] = node->keys[shifting_index];
    }

    // Reduce valid tracking count.
    node->count--;
}

/**
 * @brief       :   Handles administrative deletion operations from internal branches.
 * @param       :   node: The working internal structural node.
 * @param       :   index: The offset index representing the targeted key.
 * @return      :   void
 */
static inline void erase_from_non_leaf(Node* node, size_t index)
{
    if (node == NULL || index >= node->count)
    {
        return;
    }

    int key = node->keys[index];

    // Case A: Left child branch has extra keys. Replace target with in-order predecessor.
    if (node->children[index] != NULL && node->children[index]->count >= MINIMUM_KEYS)
    {
        int predecessor_key = get_predecessor(node, index);

        node->keys[index] = predecessor_key;

        erase_at(node->children[index], predecessor_key);
    }
    // Case B: Right child branch has extra keys. Replace target with in-order successor.
    else if (node->children[index + 1] != NULL && node->children[index + 1]->count >= MINIMUM_KEYS)
    {
        int successor_key = get_successor(node, index);

        node->keys[index] = successor_key;

        erase_at(node->children[index + 1], successor_key);
    }
    // Case C: Both child subtrees are at minimum capacity. Merge them and delete recursively.
    else
    {
        merge_child_nodes(node, index);

        if (node->children[index] != NULL)
        {
            erase_at(node->children[index], key);
        }
    }
}

/**
 * @brief       :   Executes a localized structural search and deletion cycle.
 * @param       :   node: The target subtree node containing operational elements.
 * @param       :   key: The target key query to pull from the current block.
 * @return      :   void
 */
static inline void erase_at(Node* node, int key)
{
    // Halt if processing an empty tree node structure.
    if (node == NULL || node->count == 0)
    {
        return;
    }

    // Lookup index boundary position matching key criteria.
    size_t index = find_key_index(node, key);

    // If target key is present in this node, delete it directly based on node type.
    if (index < node->count && node->keys[index] == key)
    {
        if (node->is_leaf)
        {
            erase_from_leaf(node, index);
        }
        else
        {
            erase_from_non_leaf(node, index);
        }
    }
    else
    {
        // If key is absent and we are already at a terminal leaf, the key does not exist.
        if (node->is_leaf)
        {
            return;
        }

        // Cache if the target key lies down the absolute last child subtree edge path.
        bool was_last_child = (index == node->count);

        // Preemptively bolster the target child branch if its capacity falls below minimum limits.
        if (index < ORDER && node->children[index] != NULL)
        {
            if (node->children[index]->count < MINIMUM_KEYS)
            {
                fill_child_node(node, index);
            }
        }

        // If child list merged and shifted target out of bounds, traverse down the consolidated tail.
        if (was_last_child && index > node->count)
        {
            if (node->count > 0 && node->children[index - 1] != NULL)
            {
                erase_at(node->children[node->count], key);
            }
        }
        else
        {
            // Otherwise follow standard downward index path recursion.
            if (index < ORDER && node->children[index] != NULL)
            {
                erase_at(node->children[index], key);
            }
        }
    }
}

/**
 * @brief       :   Allocates and initializes an individual B-Tree structure node.
 * @param       :   is_leaf: Flags if the initialization node is a bottom terminal leaf.
 * @return      :   Node*: Pointer to the newly instantiated heap object node.
 */
Node* construct(bool is_leaf)
{
    // Request raw memory sizing block assignment from OS allocator.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Map starting state flags.
    new_node->is_leaf = is_leaf;
    new_node->count = 0;

    // Explicitly zero out all tracking child pointers.
    for (size_t index = 0; index < ORDER; index++)
    {
        new_node->children[index] = NULL;
    }

    return new_node;
}

/**
 * @brief       :   Traverses the balanced configuration searching for a particular element.
 * @param       :   root: Const structured access pointer for top tree search traversal.
 * @param       :   key: The search key integer criteria value.
 * @return      :   Node*: The exact storage node component address containing the value match.
 */
Node* search(Node* root, int key)
{
    // Return NULL if key match lookup fails at empty tree endpoint.
    if (root == NULL)
    {
        return NULL;
    }

    size_t index = 0;

    // Advance index while input criteria value is greater than array data.
    while (index < root->count && key > root->keys[index])
    {
        index++;
    }

    // Match validation success: return actual structured object reference component.
    if (index < root->count && root->keys[index] == key)
    {
        return root;
    }

    // Key is missing and cannot traverse further if we hit a leaf.
    if (root->is_leaf)
    {
        return NULL;
    }

    // Halt recursion if expected path points to an invalid connection branch.
    if (root->children[index] == NULL)
    {
        return NULL;
    }

    // Tail-recurse down into chosen branch matching boundary location indices.
    return search(root->children[index], key);
}

/**
 * @brief       :   Inserts a new value entry into the multi-way balance structure.
 * @param       :   root: Double pointer back reference to administrative root pointer stack.
 * @param       :   key: The distinct structural assignment key element.
 * @return      :   void
 */
void insert(Node** root, int key)
{
    if (root == NULL)
    {
        return;
    }

    // Case A: Instantiating structural entry node on completely empty root reference.
    if (*root == NULL)
    {
        *root = construct(true);

        if (*root != NULL)
        {
            (*root)->keys[0] = key;
            (*root)->count = 1;
        }

        return;
    }

    // Prevent duplicate entries from polluting internal structural components.
    if (search(*root, key) != NULL)
    {
        return;
    }

    Node* node = *root;

    // Safety fallback block handling overflowing array constraints.
    if (node->count > ORDER - 1)
    {
        return;
    }

        // Case B: If root is completely full, tree height must grow. Split the root node.
    if (node->count == ORDER - 1)
    {
        Node* new_root = construct(false);

        if (new_root == NULL)
        {
            return;
        }

        // Set the old full root node to be the first child of the new root.
        new_root->children[0] = node;

        // Execute balancing division layout across old structural objects.
        split_child(new_root, 0, node);

        // Terminate execution safely if splitting components failed.
        if (new_root->count == 0)
        {
            free(new_root);

            return;
        }

        size_t index = 0;

        // Determine which side of the split root node should receive the value.
        if (new_root->keys[0] < key)
        {
            index++;
        }

        // Recursively add the element down into the selected branch channel.
        if (new_root->children[index] != NULL)
        {
            insert_non_full(new_root->children[index], key);
        }
        
                // Re-assign external tracking address context back to new overarching root pointer.
        *root = new_root;
    }
    else
    {
        // Case C: Root has available slots. Simply descend path normally.
        insert_non_full(node, key);
    }
}

/**
 * @brief       :   Erases an object entry element tracking down balance factors.
 * @param       :   root: Double pointer assignment reference tracking tree entry roots.
 * @param       :   key: The identification key component query target.
 * @return      :   void
 */
void erase(Node** root, int key)
{
    if (root == NULL || *root == NULL)
    {
        return;
    }

    Node* node = *root;

    // Begin localized recursive removal cycle across tree components.
    erase_at(node, key);
    
    // If root key count collapses to zero, shrink tree height down by one level.
    if (node->count == 0)
    {
        Node* temp = node;

        if (node->is_leaf)
        {
            // Tree is now completely empty.
            *root = NULL;
        }
        else
        {
            // Elevate the only remaining child node to be the brand new root.
            *root = node->children[0];
        }

        // Deallocate old root structural container memory.
        free(temp);
    }
}

/**
 * @brief       :   Performs an in-order traversal of the B-Tree to print keys.
 * @details     :   Recursively visits children and keys sequentially to display 
 *                  the tree contents in sorted, ascending order.
 * @param       :   root: Pointer to the sub-root node to traverse.
 * @return      :   void
 */
void traverse(const Node* root)
{
    if (root == NULL)
    {
        return;
    }

    size_t index = 0;

    // Iterate through all localized keys, exploring left child components first.
    for (index = 0; index < root->count; index++)
    {
        if (!root->is_leaf)
        {
            traverse(root->children[index]);
        }

        // Output current index value onto standard terminal buffer tracking arrays.
        printf("%d ", root->keys[index]);
    }

    // Traverse final trailing right child component path edge after loop completion.
    if (!root->is_leaf)
    {
        traverse(root->children[index]);
    }
}

/**
 * @brief       :   Recursively frees heap structures assigned across runtime execution.
 * @param       :   root: Operational sub-root structural tracker item.
 * @return      :   void
 */
void clean(Node* root)
{
    if (root == NULL)
    {
        return;
    }

    // If node has child branches, recursively traverse and free them first.
    if (!root->is_leaf)
    {
        for (size_t index = 0; index <= root->count; index++)
        {
            if (root->children[index] != NULL)
            {
                clean(root->children[index]);

                root->children[index] = NULL;
            }
        }
    }

    // Free memory block allocation tracking reference back to OS.
    free(root);
}