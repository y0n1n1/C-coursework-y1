#include <stdlib.h>
#include <time.h>
#include "arena.h"
#include "graphics.h"

/* Generate random arena dimension (12-18 tiles) */
int randomArenaSize(void) {
    return rand() % 7 + 12;
}

void initArena(int arena[][MAX_ARENA_SIZE], int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == 0 || x == width-1 || y == 0 || y == height-1) {
                arena[y][x] = 1;
            } else {
                arena[y][x] = 0;
            }
        }
    }
}

/* Place marker anywhere inside arena (Stage 3) */
void placeRandomMarker(int arena[][MAX_ARENA_SIZE], int width, int height) {
    int markerX = rand() % (width - 2) + 1;
    int markerY = rand() % (height - 2) + 1;
    arena[markerY][markerX] = 3;
}

/* Place obstacles randomly inside arena (Stage 4) */
void placeObstacles(int arena[][MAX_ARENA_SIZE], int width, int height,
                    int count) {
    for (int i = 0; i < count; i++) {
        int x, y;
        do {
            x = rand() % (width - 2) + 1;
            y = rand() % (height - 2) + 1;
        } while (arena[y][x] != 0);
        arena[y][x] = 2;
    }
}

/* Place multiple markers randomly (Stage 4) */
void placeMultipleMarkers(int arena[][MAX_ARENA_SIZE], int width, int height,
                          int count) {
    for (int i = 0; i < count; i++) {
        int x, y;
        do {
            x = rand() % (width - 2) + 1;
            y = rand() % (height - 2) + 1;
        } while (arena[y][x] != 0);
        arena[y][x] = 3;
    }
}

/* Count markers remaining in arena */
int countMarkers(int arena[][MAX_ARENA_SIZE], int width, int height) {
    int count = 0;
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (arena[y][x] == 3) count++;
        }
    }
    return count;
}

void initRobot(Robot *robot, int arena[][MAX_ARENA_SIZE],
               int width, int height) {
    do {
        robot->x = rand() % (width - 4) + 2;
        robot->y = rand() % (height - 4) + 2;
    } while (arena[robot->y][robot->x] != 0);

    char directions[] = {'N', 'S', 'E', 'W'};
    robot->direction = directions[rand() % 4];
    robot->markers_held = 0;
}

static void drawGrid(int width, int height) {
    setColour(lightgray);
    for (int y = 1; y < height-1; y++) {
        for (int x = 1; x < width-1; x++) {
            drawRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
        }
    }
}

static void drawWalls(int arena[][MAX_ARENA_SIZE], int width, int height) {
    setColour(red);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (arena[y][x] == 1) {
                fillRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

static void drawObstacles(int arena[][MAX_ARENA_SIZE], int width, int height) {
    setColour(black);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (arena[y][x] == 2) {
                fillRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

static void drawMarkers(int arena[][MAX_ARENA_SIZE], int width, int height) {
    setColour(gray);
    int markerSize = TILE_SIZE - 10;
    int offset = 5;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (arena[y][x] == 3) {
                fillOval(x*TILE_SIZE + offset, y*TILE_SIZE + offset,
                         markerSize, markerSize);
            }
        }
    }
}

void drawBackground(int arena[][MAX_ARENA_SIZE], int width, int height) {
    background();
    clear();
    drawGrid(width, height);
    drawWalls(arena, width, height);
    drawObstacles(arena, width, height);
    drawMarkers(arena, width, height);
}

static void setNorthTriangle(int cx, int cy, int t, int b,
                              int x[3], int y[3]) {
    x[0] = cx; y[0] = cy - t;
    x[1] = cx - b; y[1] = cy + b;
    x[2] = cx + b; y[2] = cy + b;
}

static void setSouthTriangle(int cx, int cy, int t, int b,
                              int x[3], int y[3]) {
    x[0] = cx; y[0] = cy + t;
    x[1] = cx - b; y[1] = cy - b;
    x[2] = cx + b; y[2] = cy - b;
}

static void setEastTriangle(int cx, int cy, int t, int b,
                             int x[3], int y[3]) {
    x[0] = cx + t; y[0] = cy;
    x[1] = cx - b; y[1] = cy - b;
    x[2] = cx - b; y[2] = cy + b;
}

static void setWestTriangle(int cx, int cy, int t, int b,
                             int x[3], int y[3]) {
    x[0] = cx - t; y[0] = cy;
    x[1] = cx + b; y[1] = cy - b;
    x[2] = cx + b; y[2] = cy + b;
}

static void setTrianglePoints(char direction, int cx, int cy, int sz,
                               int xPts[3], int yPts[3]) {
    int tip = sz, base = sz/2;
    if (direction == 'N') setNorthTriangle(cx, cy, tip, base, xPts, yPts);
    else if (direction == 'S') setSouthTriangle(cx, cy, tip, base, xPts, yPts);
    else if (direction == 'E') setEastTriangle(cx, cy, tip, base, xPts, yPts);
    else setWestTriangle(cx, cy, tip, base, xPts, yPts);
}

void drawRobot(Robot *robot) {
    clear();
    int centerX = robot->x * TILE_SIZE + TILE_SIZE / 2;
    int centerY = robot->y * TILE_SIZE + TILE_SIZE / 2;
    int xPoints[3], yPoints[3];

    setTrianglePoints(robot->direction, centerX, centerY,
                      TILE_SIZE / 2, xPoints, yPoints);
    setColour(blue);
    fillPolygon(3, xPoints, yPoints);
}
