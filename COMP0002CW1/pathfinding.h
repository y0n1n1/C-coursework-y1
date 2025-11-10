#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "arena.h"

#define MAX_PATH_LENGTH 200

typedef struct {
    int x[MAX_PATH_LENGTH];
    int y[MAX_PATH_LENGTH];
    int length;
} Path;

/* BFS pathfinding. Returns 1 if path found, 0 otherwise */
int findPath(Arena *arena, int startX, int startY,
             int endX, int endY, Path *path);

#endif
