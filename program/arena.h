#ifndef ARENA_H
#define ARENA_H

#include "robot.h"

/* Arena is a grid of tiles. Each tile can be:
   0 = empty, 1 = wall, 2 = obstacle, 3 = marker */
#define MAX_ARENA_SIZE 20
#define TILE_SIZE 40

/* NOVEL: Shape types for Stage 5 obstacle patterns */
typedef enum {
    SHAPE_CIRCLE,
    SHAPE_DIAMOND
} ShapeType;

/* NOVEL: Movement trail visualization - tracks robot efficiency */
typedef struct {
    int horizontal_order[MAX_ARENA_SIZE][MAX_ARENA_SIZE];  /* 0=unused, 1/2/3=order */
    int vertical_order[MAX_ARENA_SIZE][MAX_ARENA_SIZE];    /* 0=unused, 1/2/3=order */
    int pass_count[MAX_ARENA_SIZE][MAX_ARENA_SIZE];        /* total passes */
} MovementTrail;

void initArena(int arena[][MAX_ARENA_SIZE], int width, int height);
int countMarkers(int arena[][MAX_ARENA_SIZE], int width, int height);
void initRobot(Robot *robot, int arena[][MAX_ARENA_SIZE], int width, int height);
void drawBackground(int arena[][MAX_ARENA_SIZE], int width, int height);
void drawRobot(Robot *robot);
int randomArenaSize(void);

/* NOVEL: Stage 5 shaped obstacle generation */
void placeShapedObstacles(int arena[][MAX_ARENA_SIZE], int w, int h, ShapeType shape);
void placeMarkersInShape(int arena[][MAX_ARENA_SIZE], int w, int h, int count, ShapeType shape);

/* NOVEL: Movement trail functions for efficiency visualization */
void initMovementTrail(MovementTrail *trail);
void recordMovement(MovementTrail *trail, int x, int y, char direction);
void drawMovementTrails(MovementTrail *trail, int width, int height);

#endif
