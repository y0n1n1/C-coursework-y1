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

/* NOVEL: Stage 5 - Calculate shape parameters (center and radius) */
static void calculateShapeParams(int w, int h, int *cx, int *cy, int *radius) {
    *cx = w / 2;
    *cy = h / 2;
    *radius = (w < h ? w : h) / 3;
}

/* NOVEL: Stage 5 - Check if position is inside shape */
static int isInsideShape(int x, int y, int cx, int cy, int radius, ShapeType shape) {
    if (shape == SHAPE_CIRCLE) {
        int dx = x - cx, dy = y - cy;
        return (dx*dx + dy*dy) <= (radius * radius);
    }
    int mdist = (x > cx ? x - cx : cx - x) + (y > cy ? y - cy : cy - y);
    return mdist <= radius;
}

/* NOVEL: Stage 5 - Place obstacles forming a shape */
void placeShapedObstacles(int arena[][MAX_ARENA_SIZE], int w, int h, ShapeType shape) {
    int cx, cy, radius;
    calculateShapeParams(w, h, &cx, &cy, &radius);

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            if (!isInsideShape(x, y, cx, cy, radius, shape)) {
                arena[y][x] = 2;
            }
        }
    }
}

/* NOVEL: Stage 5 - Place markers inside shaped area only */
void placeMarkersInShape(int arena[][MAX_ARENA_SIZE], int w, int h,
                          int count, ShapeType shape) {
    int cx, cy, radius;
    calculateShapeParams(w, h, &cx, &cy, &radius);

    for (int i = 0; i < count; i++) {
        int x, y, attempts = 0;
        do {
            x = rand() % (w - 2) + 1;
            y = rand() % (h - 2) + 1;
        } while ((arena[y][x] != 0 || !isInsideShape(x, y, cx, cy, radius, shape))
                 && ++attempts < 100);
        if (attempts < 100) arena[y][x] = 3;
    }
}

/* NOVEL: Initialize movement trail tracking */
void initMovementTrail(MovementTrail *trail) {
    for (int y = 0; y < MAX_ARENA_SIZE; y++) {
        for (int x = 0; x < MAX_ARENA_SIZE; x++) {
            trail->horizontal_order[y][x] = 0;
            trail->vertical_order[y][x] = 0;
            trail->pass_count[y][x] = 0;
        }
    }
}

/* NOVEL: Record robot movement for trail visualization */
void recordMovement(MovementTrail *trail, int x, int y, char direction) {
    if (trail->pass_count[y][x] < 3) {
        trail->pass_count[y][x]++;
    }

    if (direction == 'E' || direction == 'W') {
        if (trail->horizontal_order[y][x] == 0) {
            trail->horizontal_order[y][x] = trail->pass_count[y][x];
        }
    } else {
        if (trail->vertical_order[y][x] == 0) {
            trail->vertical_order[y][x] = trail->pass_count[y][x];
        }
    }
}

/* Set color based on pass order (1=green, 2=yellow, 3=red) */
static void setOrderColor(int order) {
    if (order == 1) setColour(green);
    else if (order == 2) setColour(yellow);
    else setColour(red);
}

/* Draw single trail line with offset (thicker lines) */
static void drawTrailLine(int x, int y, int isHorizontal, int offset) {
    int centerX = x * TILE_SIZE + TILE_SIZE / 2;
    int centerY = y * TILE_SIZE + TILE_SIZE / 2;
    int lineLen = TILE_SIZE / 3;

    if (isHorizontal) {
        for (int i = -2; i <= 3; i++) {
            drawLine(centerX - lineLen, centerY + offset + i,
                    centerX + lineLen, centerY + offset + i);
        }
    } else {
        for (int i = -2; i <= 3; i++) {
            drawLine(centerX + offset + i, centerY - lineLen,
                    centerX + offset + i, centerY + lineLen);
        }
    }
}

/* Draw horizontal trail for a tile based on order */
static void drawHorizontalTrail(int x, int y, int order) {
    if (order > 0) {
        setOrderColor(order);
        drawTrailLine(x, y, 1, 0);
    }
}

/* Draw vertical trail for a tile based on order */
static void drawVerticalTrail(int x, int y, int order) {
    if (order > 0) {
        setOrderColor(order);
        drawTrailLine(x, y, 0, 0);
    }
}

/* NOVEL: Draw movement trails showing robot efficiency */
void drawMovementTrails(MovementTrail *trail, int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            drawHorizontalTrail(x, y, trail->horizontal_order[y][x]);
            drawVerticalTrail(x, y, trail->vertical_order[y][x]);
        }
    }
}
