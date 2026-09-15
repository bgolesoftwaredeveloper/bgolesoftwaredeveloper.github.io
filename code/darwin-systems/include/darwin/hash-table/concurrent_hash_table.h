/**
 * @file        :   concurrent_hash_table.h
 * @author      :   Braiden Gole
 * @brief       :   Interface for a lock-striped concurrent hash table.
 * @details     :   This header defines the data structures and API signatures for a 
 * high-throughput, multi-threaded hash table. The design combines 
 * fine-grained lock striping (via POSIX mutexes) with C11 atomic 
 * atomic pointers. Lock striping splits synchronization across a 
 * fixed array of locks to minimize thread contention, while lock-free 
 * reads (using memory_order_acquire) allow lookups to bypass mutexes.
 * @version     :   1.0
 * @date        :   2026-05-25
 */
#ifndef CONCURRENT_HASH_TABLE_H
#define CONCURRENT_HASH_TABLE_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>

#define NUMBER_OF_STRIPES 64
#define NUMBER_OF_BUCKETS 1024
#define NUMBER_OF_THREADS 4

#define OPERATIONS_PER_THREAD 50

/**
 * @struct Node
 * @brief  Individual bucket entry in a singly-linked collision chain.
 * @details Encapsulates the unique string key, user payload, and a atomic pointer 
 * referencing the next node in the chain. This enables concurrent readers 
 * to traverse lists lock-free while a separate writer is modifying structural links.
 */
typedef struct Node
{
    char* key;
    void* value;
    _Atomic(struct Node*) next;
} Node;

/**
 * @struct ConcurrentHashTable
 * @brief  The core hash table instance layout utilizing lock striping.
 * @details Implements a fixed-size array of bucket heads alongside a smaller, 
 * striped array of mutual exclusion locks. Buckets are mathematically 
 * mapped to specific locks using a modulo constraint:
 * Stripe Index = Bucket Index % NUMBER_OF_STRIPES
 */
typedef struct
{
    _Atomic(Node*) buckets[NUMBER_OF_BUCKETS];
    pthread_mutex_t strip_locks[NUMBER_OF_STRIPES];
} ConcurrentHashTable;

/**
 * @struct ThreadPayload
 * @brief  Context pass-through structure for multi-threaded work routines.
 * @details Packages the shared hash table target alongside a unique operating sequence ID, 
 * preventing thread tasks from executing overlaps or colliding on local stack frames.
 */
typedef struct
{
    uint32_t id;
    ConcurrentHashTable* table;
} ThreadPayload;

// Function prototypes.
void* worker_thread_task(void* argument);

ConcurrentHashTable* initialize(void);
bool insert(ConcurrentHashTable* table, const char* key, void* value);
void* lookup(const ConcurrentHashTable* table, const char* key);
bool erase(ConcurrentHashTable* table, const char* key);
void display(ConcurrentHashTable* table);
void destroy(ConcurrentHashTable* table);

#endif