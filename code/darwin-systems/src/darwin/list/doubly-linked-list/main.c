/**
 * @file        :   main.c
 * @author      :   Braiden Gole
 * @brief       :   Demonstration of an advanced generic Doubly Linked List.
 * @details     :   This program showcases positional insertion (front/back), 
 * searching with predicates, and the importance of automated 
 * memory management using function callbacks.
 * Compile:
 * clang ./src/darwin/list/doubly-linked-list/main.c ./src/darwin/list/doubly-linked-list/doubly_linked_list.c -I./include -o ./bin/doubly_linked_list_test
 * @version     :   1.0
 * @date        :   2026-05-13
 */
#include <stdio.h>
#include <stdlib.h>

// Include header files.
#include "darwin/list/doubly_linked_list.h"

int main(void)
{
    /**
     * We pass the 'destroy' function pointer here. This is crucial for 
     * generic containers. Since the list stores 'void*', it doesn't know 
     * if the data is a simple int or a complex heap-allocated string. 
     * By saving 'destroy' in the List struct, the list can automatically 
     * clean up its own contents during 'clear()' or 'pop()' operations.
     */
    List* list = initialize(destroy);

    if (list == NULL)
    {
        fprintf(stderr, "Out of memory!\n");

        return 1;
    }

    // Adding data to both ends to demonstrate doubly linked list flexibility.
    push_back(list, make_text("Middle"));
    push_back(list, make_text("Tail"));
    push_front(list, make_text("Head"));

    // Traverse the list using generic 'forEach' and print callback.
    printf("%s", "List: ");
    forEach(list, print);
    printf("%s", "\n\n");

    char* key = "Middle";

    printf("%s '%s'%s", "Searching for", key, "...\n");

    // Since we use void*, we pass 'is_match' to define how to compare the data.
    void* found = find(list, is_match, key);

    if (found != NULL)
    {
        printf("%s %s%c", "Found:", (char*)found, '\n');
    }
    else
    {
        printf("%s", "Not found!.\n");
    }

    printf("%c", '\n');

    // When we pop an item, the List removes the Node and returns the data.
    // At this point, the List 'gives up' ownership. We must manually call destroy
    // on the returned data to prevent a memory leak.
    char* front_data = (char*)pop_front(list);

    if (front_data != NULL)
    {
        printf("%s %s%c", "Popped:", front_data, '\n');

        destroy(front_data);
    }

    char* back_data = (char*)pop_back(list);

    if (back_data != NULL)
    {
        printf("%s %s%c", "Popped:", back_data, '\n');

        destroy(back_data);
    }

    printf("%c", '\n');

    // Display the list after modification.
    printf("%s", "List: ");
    forEach(list, print);
    printf("%c", '\n');

    /**
     * 'clear' iterates through any nodes left in the list (in this case, "Middle").
     * For every node it finds, it calls the 'destroy' function we assigned 
     * during initialize(). This ensures a "Deep Clean" of all heap memory.
     */
    clear(list);

    return 0;
}