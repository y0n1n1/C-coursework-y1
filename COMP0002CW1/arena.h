#ifndef ARENA_H
#define ARENA_H

#include "robot.h"

#define MAX_ARENA_SIZE 40

#define EMPTY 0
#define WALL 1
#define OBSTACLE 2
#define MARKER 3

/* N, E, S, W (clockwise from North) */
static const int DIRECTION_DX[] = {0, 1, 0, -1};
static const int DIRECTION_DY[] = {-1, 0, 1, 0};

typedef enum {
    SHAPE_CIRCLE,
    SHAPE_DIAMOND,
    SHAPE_RECTANGLE,
    SHAPE_OVAL,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int grid[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    int width;
    int height;
    int marker_count;
} Arena;

void forward(Robot *robot, Arena *arena);
void left(Robot *robot);
void right(Robot *robot);
int atMarker(Robot *robot, Arena *arena);
int canMoveForward(Robot *robot, Arena *arena);
void pickUpMarker(Robot *robot, Arena *arena);
void dropMarker(Robot *robot, Arena *arena);
int markerCount(Robot *robot);

#define FIRST_PASS 1
#define SECOND_PASS 2
#define THIRD_PASS 3

typedef struct {
    int horizontal_passes[MAX_ARENA_SIZE][MAX_ARENA_SIZE][3];
    int vertical_passes[MAX_ARENA_SIZE][MAX_ARENA_SIZE][3];
    int horizontal_count[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    int vertical_count[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    int total_passes[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
} MovementTrail;

void initArena(Arena *arena);
int countMarkers(Arena *arena);
void initRobot(Robot *robot, Arena *arena);
void drawBackground(Arena *arena);
void drawRobot(Robot *robot);
int randomArenaSize(void);

void placeShapedObstacles(Arena *arena, ShapeType shape);
void placeMarkersInShape(Arena *arena, int count, ShapeType shape);
void placeRandomObstacles(Arena *arena, int count, ShapeType shape);

void initMovementTrail(MovementTrail *trail);
void recordMovement(MovementTrail *trail, int x, int y, char direction);
void drawMovementTrails(MovementTrail *trail, int width, int height);

#endif
