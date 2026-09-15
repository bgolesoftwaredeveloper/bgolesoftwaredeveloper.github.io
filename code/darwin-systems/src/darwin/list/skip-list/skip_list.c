/**
 * @file        :   skip_list.c
 * @author      :   Braiden Gole
 * @brief       :   Implementation of the Skip List data structure.
 * @details     :   Contains core algorithmic mechanisms for node allocation, 
 * probabilistic level calculation, structural rendering, element 
 * insertion with pointer splicing, lookup optimization, and system cleanup.
 * @version     :   1.0
 * @date        :   2026-05-25
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/list/skip_list.h"

/**
 * @brief       :   Generates a randomized level height for a new node.
 * @details     :   Simulates sequential independent coin flips using rand(). The height 
 *                  increments on each successive heads outcome up to the allowed cap.
 * @return      :   size_t: The generated level index (bounded between 0 and MAXIMUM_SKIP_LIST_LEVEL).
 */
static inline size_t random_level(void)
{
    size_t level = 0;

    // Simulate geometric distribution.
    while (rand() % 2 && level < MAXIMUM_SKIP_LIST_LEVEL)
    {
        level++;
    }

    return level;
}

/**
 * @brief       :   Constructs a single detached skip list node wrapper on the heap.
 * @param       :   key: Unique lookup identifier used for sorting.
 * @param       :   value: Payload value bound to the tracking key.
 * @param       :   level: The dynamic forward pointer capacity array height allocated for this node.
 * @return      :   Node*: Pointer to the freshly allocated node structure, or NULL if system memory runs out.
 */
Node* construct(int key, int value, size_t level)
{
    // Safety check to ensure requested height does not violate limits.
    if (level > MAXIMUM_SKIP_LIST_LEVEL)
    {
        return NULL;
    }

    // Allocate heap memory for the primary Node structural container.
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return NULL;
    }

    // Assign payload properties.
    new_node->key = key;
    new_node->value = value;

    // Allocate an array of Node pointers to hold the forward links for each level.
    new_node->forward = (Node**)malloc(sizeof(Node*) * (level + 1));

    // Cleanup and abort if the pointer array allocation failed.
    if (new_node->forward == NULL)
    {
        free(new_node);

        return NULL;
    }

    // Initialize all forward pointers to NULL to prevent dangerous garbage address access.
    for (size_t index = 0; index <= level; index++)
    {
        new_node->forward[index] = NULL;
    }

    return new_node;
}

/**
 * @brief       :   Allocates and initializes a new empty skip list container.
 * @details     :   Sets the initial peak level to 0 and builds a sentinel head node
 *                  holding minimum and maximum limits to anchor all possible lanes.
 * @return      :   SkipList*: A pointer to the newly allocated SkipList structure, or NULL if allocation fails.
 */
SkipList* initialize(void)
{
    // Allocate the master control header tracking structure.
    SkipList* list = (SkipList*)malloc(sizeof(SkipList));

    if (list == NULL)
    {
        return NULL;
    }

    // Default initialization.
    list->level = 0;

    // INT_MIN and INT_MAX guarantee it always acts as the absolute starting boundary.
    list->header = construct(INT_MIN, INT_MAX, MAXIMUM_SKIP_LIST_LEVEL);

    // If the sentinel node initialization failed, free the allocation container wrapper.
    if (list->header == NULL)
    {
        free(list);

        return NULL;
    }

    return list;
}

/**
 * @brief       :   Inserts or updates a key-value pair inside the skip list layers.
 * @details     :   Navigates structural lanes downward to locate the insertion coordinates,
 *                  caches previous intersection updates, dynamically scales the list bounds 
 *                  if a tall height is rolled, and splices pointers cleanly.
 * @param       :   list: The skip list instance to modify.
 * @param       :   key: Sorted comparison identifier key to position.
 * @param       :   value: Value to store alongside the specific key tracking slot.
 * @return      :   bool: true upon valid creation or modification, false if memory errors trigger.
 */
bool insert(SkipList* list, int key, int value)
{
    // Validate that our tracking pointers exist before evaluating.
    if (list == NULL || list->header == NULL)
    {
        return false;
    }

    // Current will traverse through the nodes.
    Node* current = list->header;

    // Array to record the rightmost node visited at each level before dropping down.
    Node* update_trackers[MAXIMUM_SKIP_LIST_LEVEL + 1];

    // Initialize all tracking slot variables to NULL.
    for (size_t level = 0; level <= MAXIMUM_SKIP_LIST_LEVEL; level++)
    {
        update_trackers[level] = NULL;
    }

    // Start navigating from the highest active level of the skip list.
    size_t level = list->level;

    bool cycle = true;

    while (cycle)
    {
        // Move horizontally along the current level as long as the next key is smaller than our target.
        while (current->forward[level] != NULL && current->forward[level]->key < key)
        {
            current = current->forward[level];
        }

        // Record this node as the point where we drop down to the next level.
        update_trackers[level] = current;

        if (level == 0)
        {
            break;
        }

        // Drop down vertically by one level.
        level--;
    }

    // Step forward one position on the base layer to inspect the target location.
    current = current->forward[0];

    // Case A: The key doesn't exist yet (we hit NULL or found a larger key), so insert it.
    if (current == NULL || current->key != key)
    {
        // Probability check to determine how high this new node will climb.
        size_t new_level = random_level();

        // If the rolled height exceeds the list's current peak, adjust the upper trackers.
        if (new_level > list->level)
        {
            // Connected unpopulated high lanes directly back to the master sentinel header.
            for (size_t adjustment_index = list->level + 1; adjustment_index <= new_level; adjustment_index++)
            {
                update_trackers[adjustment_index] = list->header;
            }

            // Raise the global list hight metric to match the new peak height.
            list->level = new_level;
        }

        // Allocate system memory space to build our new standalone node.
        Node* node = construct(key, value, new_level);

        if (node == NULL)
        {
            return false;
        }

        // Link the new node into the list, layer by layer, up to its generated level.
        for (size_t adjustment_level = 0; adjustment_level <= new_level; adjustment_level++)
        {
            if (update_trackers[adjustment_level] == NULL)
            {
                return false;
            }

            // The new node points forward to what the previous tracking node pointer to.  
            node->forward[adjustment_level] = update_trackers[adjustment_level]->forward[adjustment_level];
            
            // The previous tracking node points forward to our newly created node.
            update_trackers[adjustment_level]->forward[adjustment_level] = node;
        }

        return true;
    }
    // Case B: The key already exists in the list. Overwrite its value.
    else
    {
        current->value = value;

        return true;
    }
}

/**
 * @brief       :   Searches for a specific key and extracts its related value safely.
 * @details     :   Drops down lane heights using express lanes to bypass long segments,
 *                  approaching an O(log n) efficiency baseline.
 * @param       :   list: The immutable skip list instance to read.
 * @param       :   key: Target lookup identifier to find.
 * @param       :   value: Output pointer reference space where the found value is copied.
 * @return      :   bool: true if a match is found and value updated, false if the key is missing.
 */
bool search(const SkipList* list, int key, int* value)
{
    // Abort if the list framework doesn't exist of if no output destination is provided.
    if (list == NULL || list->header == NULL)
    {
        return false;
    }

    if (value == NULL)
    {
        return false;
    }

    // Set tracker focal index to start at the absolute sentinel origin.
    Node* current = list->header;

    size_t level_index = list->level;

    bool cycle = true;

    // Traverse downward through the levels.
    while (cycle)
    {
        // Move horizontally along the current level while the next node's key is less than the target.
        while (current->forward[level_index] != NULL && current->forward[level_index]->key < key)
        {
            current = current->forward[level_index];
        }

        // If we hit the base layer, we can't drop down any further.
        if (level_index == 0)
        {
            break;
        }

        // Drop down vertically to the next lower express or standard lane.
        level_index--;
    }

    // Advance one step forward on level 0 to land on the potential match node.
    current = current->forward[0];

    // Confirm the node exists and its key matches our search target.
    if (current != NULL && current->key == key)
    {
        *value = current->value;

        return true;
    }

    return false;
}

/**
 * @brief       :   Renders a visual layout representation of the skip list lanes to standard stdout.
 * @details     :   Iterates vertically down active level indexes, then traverses horizontally across 
 *                  each row level sequence printing keys and tracking elements.
 * @param       :   list: The immutable skip list instance to visually inspect.
 * @return      :   void
 */
void display(const SkipList* list)
{
    // Return early if there is no data structure available to print.
    if (list == NULL || list->header == NULL)
    {
        return;
    }

    printf("%s", "-- Skip List Structure ---\n");

    // Loop through each active vertical layer level from the bottom up.
    for (size_t level = 0; level <= list->level; level++)
    {
        // Grab the first node connected to the sentinel header on this specific level.
        Node* current = list->header->forward[level];

        // Format and print the row level identifier.
        printf("%s %zu %s", "Level", level, ": ");

        // Walk horizontally along this level until we hit the end of the chain.
        while (current != NULL)
        {
            printf("[%i: %i]", current->key, current->value);

            current = current->forward[level];
        }

        printf("%c", '\n');
    }
}

/**
 * @brief       :   Frees all nodes, internal pointer arrays, and the top-level list container.
 * @details     :   Traverses sequentially across the base row level (Level 0) to ensure every isolated 
 *                  element is caught, pops their internal pointer array layers, and dissolves the list wrapper.
 * @param       :   list: The skip list instance to destroy.
 * @return      :   void
 */
void clean(SkipList* list)
{
    // Return immediately if the pointer is already NULL.
    if (list == NULL)
    {
        return;
    }

    // Start tracking from the sentinel header node.
    Node* current = list->header;

    // Traverse sequentially across level 0 to visit and free every node.
    while (current != NULL)
    {
        // Temporarily store the next base node pointer before destroying the current node.
        Node* next = current->forward[0];

        // Check if the internal forward array pointer maps are populated.
        if (current->forward != NULL)
        {
            // Deallocate the internal forward array of pointers.
            free(current->forward);
            
            // Nullify the variable pointer reference to avoid dangling pointer states.
            current->forward = NULL;
        }

        // Deallocate the actual Node structure from heap space.
        free(current);

        current = next;
    }

    // Finally, free the master structural control wrapper layout itself.
    free(list);
}