/**
 * @file        :   prim.h
 * @author      :   Braiden Gole
 * @brief       :   Header file containing definitions and prototypes for Prim's MST algorithm.
 * @details     :   Defines the graph dimensions and provides the interface function to compute 
 * the Minimum Spanning Tree (MST) of a graph using Prim's algorithm.
 * @version     :   1.0
 * @date        :   2026-05-28
 */
#ifndef PRIM_H
#define PRIM_H

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#define NUMBER_OF_VERTICES 5

// Function prototypes.
void prim(const int graph[NUMBER_OF_VERTICES][NUMBER_OF_VERTICES]);

#endif