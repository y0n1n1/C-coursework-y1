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

/* Try to add neighbor to BFS queue */
static void tryAddNeighbor(int arena[][MAX_ARENA_SIZE], int visited[][MAX_ARENA_SIZE],
                           int qx[], int qy[], int *rear, int nx, int ny,
                           int w, int h) {
    if (nx > 0 && nx < w-1 && ny > 0 && ny < h-1 &&
        !visited[ny][nx] && arena[ny][nx] != 2) {
        visited[ny][nx] = 1;
        qx[*rear] = nx;
        qy[(*rear)++] = ny;
    }
}

/* Initialize BFS queue from center */
static void initFloodQueue(int qx[], int qy[], int *rear, int visited[][MAX_ARENA_SIZE],
                           int width, int height) {
    qx[*rear] = width / 2;
    qy[*rear] = height / 2;
    (*rear)++;
    visited[height/2][width/2] = 1;
}

/* BFS flood fill from center */
static void floodFill(int arena[][MAX_ARENA_SIZE], int visited[][MAX_ARENA_SIZE],
                      int width, int height) {
    int qx[MAX_ARENA_SIZE * MAX_ARENA_SIZE], qy[MAX_ARENA_SIZE * MAX_ARENA_SIZE];
    int front = 0, rear = 0;
    int dx[] = {0, 1, 0, -1}, dy[] = {-1, 0, 1, 0};

    initFloodQueue(qx, qy, &rear, visited, width, height);

    while (front < rear) {
        int x = qx[front], y = qy[front++];
        for (int i = 0; i < 4; i++) {
            tryAddNeighbor(arena, visited, qx, qy, &rear,
                          x + dx[i], y + dy[i], width, height);
        }
    }
}

/* Check all empty tiles are reachable */
static int checkConnectivity(int arena[][MAX_ARENA_SIZE], int width, int height) {
    int visited[MAX_ARENA_SIZE][MAX_ARENA_SIZE] = {0};
    floodFill(arena, visited, width, height);

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (arena[y][x] != 2 && !visited[y][x]) return 0;
        }
    }
    return 1;
}

/* Clear all obstacles from arena */
static void clearObstacles(int arena[][MAX_ARENA_SIZE], int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (arena[y][x] == 2) arena[y][x] = 0;
        }
    }
}

/* Place N random obstacles */
static void placeRandomObstacles(int arena[][MAX_ARENA_SIZE], int width,
                                  int height, int count) {
    for (int i = 0; i < count; i++) {
        int x, y;
        do {
            x = rand() % (width - 2) + 1;
            y = rand() % (height - 2) + 1;
        } while (arena[y][x] != 0);
        arena[y][x] = 2;
    }
}

/* Place obstacles randomly, ensure connectivity */
void placeObstacles(int arena[][MAX_ARENA_SIZE], int width, int height,
                    int count) {
    for (int attempts = 0; attempts < 100; attempts++) {
        clearObstacles(arena, width, height);
        placeRandomObstacles(arena, width, height, count);
        if (checkConnectivity(arena, width, height)) return;
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

/* NOVEL: Initialize movement trail tracking */
void initMovementTrail(MovementTrail *trail) {
    for (int y = 0; y < MAX_ARENA_SIZE; y++) {
        for (int x = 0; x < MAX_ARENA_SIZE; x++) {
            trail->horizontal_passes[y][x] = 0;
            trail->vertical_passes[y][x] = 0;
        }
    }
}

/* NOVEL: Record robot movement for trail visualization */
void recordMovement(MovementTrail *trail, int x, int y, char direction) {
    if (direction == 'E' || direction == 'W') {
        if (trail->horizontal_passes[y][x] < 4) {
            trail->horizontal_passes[y][x]++;
        }
    } else {
        if (trail->vertical_passes[y][x] < 4) {
            trail->vertical_passes[y][x]++;
        }
    }
}

/* Draw single trail line with offset */
static void drawTrailLine(int x, int y, int isHorizontal, int offset) {
    int centerX = x * TILE_SIZE + TILE_SIZE / 2;
    int centerY = y * TILE_SIZE + TILE_SIZE / 2;
    int lineLen = TILE_SIZE / 3;

    if (isHorizontal) {
        drawLine(centerX - lineLen, centerY + offset,
                centerX + lineLen, centerY + offset);
    } else {
        drawLine(centerX + offset, centerY - lineLen,
                centerX + offset, centerY + lineLen);
    }
}

/* Draw horizontal trails for a tile */
static void drawHorizontalTrails(int x, int y, int passes) {
    if (passes >= 1) {
        setColour(green);
        drawTrailLine(x, y, 1, -3);
    }
    if (passes >= 2) {
        setColour(yellow);
        drawTrailLine(x, y, 1, 0);
    }
    if (passes >= 3) {
        setColour(red);
        drawTrailLine(x, y, 1, 3);
    }
}

/* Draw vertical trails for a tile */
static void drawVerticalTrails(int x, int y, int passes) {
    if (passes >= 1) {
        setColour(green);
        drawTrailLine(x, y, 0, -3);
    }
    if (passes >= 2) {
        setColour(yellow);
        drawTrailLine(x, y, 0, 0);
    }
    if (passes >= 3) {
        setColour(red);
        drawTrailLine(x, y, 0, 3);
    }
}

/* NOVEL: Draw movement trails showing robot efficiency */
void drawMovementTrails(MovementTrail *trail, int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            drawHorizontalTrails(x, y, trail->horizontal_passes[y][x]);
            drawVerticalTrails(x, y, trail->vertical_passes[y][x]);
        }
    }
}
