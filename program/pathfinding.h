#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "arena.h"

#define MAX_PATH_LENGTH 200

/* Path struct bundles coordinates and length into single parameter */
typedef struct {
    int x[MAX_PATH_LENGTH];
    int y[MAX_PATH_LENGTH];
    int length;
} Path;

/* Find shortest path from start to end using BFS.
   Returns 1 if path found, 0 if no path exists.
   Path is stored in the provided Path struct. */
int findPath(int arena[][MAX_ARENA_SIZE], int startX, int startY,
             int endX, int endY, Path *path);

#endif
