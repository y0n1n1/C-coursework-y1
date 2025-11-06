#ifndef ARENA_H
#define ARENA_H

#include "robot.h"

/* Arena is a grid of tiles. Each tile can be:
   0 = empty, 1 = wall, 2 = obstacle, 3 = marker */
#define MAX_ARENA_SIZE 20
#define TILE_SIZE 40

void initArena(int arena[][MAX_ARENA_SIZE], int width, int height);
void placeRandomMarker(int arena[][MAX_ARENA_SIZE], int width, int height);
void placeObstacles(int arena[][MAX_ARENA_SIZE], int width, int height, int count);
void placeMultipleMarkers(int arena[][MAX_ARENA_SIZE], int width, int height, int count);
int countMarkers(int arena[][MAX_ARENA_SIZE], int width, int height);
void initRobot(Robot *robot, int arena[][MAX_ARENA_SIZE], int width, int height);
void drawBackground(int arena[][MAX_ARENA_SIZE], int width, int height);
void drawRobot(Robot *robot);
int randomArenaSize(void);

#endif
