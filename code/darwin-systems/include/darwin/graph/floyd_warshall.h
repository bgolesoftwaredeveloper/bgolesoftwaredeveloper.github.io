/**
 * @file        :   floyd_warshall.h
 * @author      :   Braiden Gole
 * @brief       :   Definitions and function prototypes for the Floyd-Warshall algorithm.
 * @details     :   This header file declares the core constants, structural macros, 
 * and function interfaces required to compute all-pairs shortest paths 
 * and print multi-dimensional grid layout matrix results.
 * @version     :   1.0
 * @date        :   2026-05-27
 */
#ifndef FLOYD_WARSHALL_H
#define FLOYD_WARSHALL_H

#include <stdbool.h>

#define VERTICES 5
#define NUMBER_OF_EDGES 8
#define INFINITE 99999

// Function prototypes.
void print(int distance[][VERTICES]);
bool floyd_warshall(const int graph[][VERTICES]);

#endif