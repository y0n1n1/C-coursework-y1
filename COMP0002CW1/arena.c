#include <stdlib.h>
#include <time.h>
#include "arena.h"
#include "graphics.h"

extern const int TILE_SIZE;

int randomArenaSize(void) {
    int minSize = MAX_ARENA_SIZE * 3 / 5;
    int range = MAX_ARENA_SIZE - minSize;
    return rand() % range + minSize;
}

void initArena(Arena *arena) {
    arena->width = randomArenaSize();
    arena->height = randomArenaSize();
    arena->marker_count = 0;

    for (int y = 0; y < arena->height; y++) {
        for (int x = 0; x < arena->width; x++) {
            if (x == 0 || x == arena->width-1 || y == 0 || y == arena->height-1) {
                arena->grid[y][x] = WALL;
            } else {
                arena->grid[y][x] = EMPTY;
            }
        }
    }
}

int countMarkers(Arena *arena) {
    return arena->marker_count;
}

void initRobot(Robot *robot, Arena *arena) {
    do {
        robot->x = rand() % (arena->width - 4) + 2;
        robot->y = rand() % (arena->height - 4) + 2;
    } while (arena->grid[robot->y][robot->x] != EMPTY);

    char directions[] = {'N', 'S', 'E', 'W'};
    robot->direction = directions[rand() % 4];
    robot->markers_held = 0;
}

static void drawGrid(Arena *arena) {
    setColour(lightgray);
    for (int y = 1; y < arena->height-1; y++) {
        for (int x = 1; x < arena->width-1; x++) {
            drawRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
        }
    }
}

static void drawWalls(Arena *arena) {
    setColour(red);
    for (int y = 0; y < arena->height; y++) {
        for (int x = 0; x < arena->width; x++) {
            if (arena->grid[y][x] == WALL) {
                fillRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

static void drawObstacles(Arena *arena) {
    setColour(black);
    for (int y = 0; y < arena->height; y++) {
        for (int x = 0; x < arena->width; x++) {
            if (arena->grid[y][x] == OBSTACLE) {
                fillRect(x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

static void drawMarkers(Arena *arena) {
    setColour(gray);
    int markerSize = TILE_SIZE - 10;
    int offset = 5;
    for (int y = 0; y < arena->height; y++) {
        for (int x = 0; x < arena->width; x++) {
            if (arena->grid[y][x] == MARKER) {
                fillOval(x*TILE_SIZE + offset, y*TILE_SIZE + offset,
                         markerSize, markerSize);
            }
        }
    }
}

void drawBackground(Arena *arena) {
    background();
    clear();
    drawGrid(arena);
    drawWalls(arena);
    drawObstacles(arena);
    drawMarkers(arena);
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

static void calculateShapeParams(Arena *arena, int *cx, int *cy, int *radius) {
    *cx = arena->width / 2;
    *cy = arena->height / 2;
    *radius = (arena->width < arena->height ? arena->width : arena->height) / 3;
}

static int isInsideShape(int x, int y, int cx, int cy, int radius, ShapeType shape) {
    int dx = x - cx, dy = y - cy;

    if (shape == SHAPE_CIRCLE) {
        return (dx*dx + dy*dy) <= (radius * radius);
    }
    if (shape == SHAPE_DIAMOND) {
        int mdist = (x > cx ? x - cx : cx - x) + (y > cy ? y - cy : cy - y);
        return mdist <= radius;
    }
    if (shape == SHAPE_RECTANGLE) {
        return (dx >= -radius && dx <= radius && dy >= -radius && dy <= radius);
    }
    if (shape == SHAPE_OVAL) {
        int radiusX = radius;
        int radiusY = radius * 2 / 3;
        return (dx*dx * radiusY*radiusY + dy*dy * radiusX*radiusX) <= (radiusX*radiusX * radiusY*radiusY);
    }
    if (shape == SHAPE_TRIANGLE) {
        if (dy > radius) return 0;
        int maxWidth = radius + dy;
        return (dx >= -maxWidth && dx <= maxWidth);
    }
    return 0;
}

void placeShapedObstacles(Arena *arena, ShapeType shape) {
    int cx, cy, radius;
    calculateShapeParams(arena, &cx, &cy, &radius);

    for (int y = 1; y < arena->height - 1; y++) {
        for (int x = 1; x < arena->width - 1; x++) {
            if (!isInsideShape(x, y, cx, cy, radius, shape)) {
                arena->grid[y][x] = OBSTACLE;
            }
        }
    }
}

void placeMarkersInShape(Arena *arena, int count, ShapeType shape) {
    int cx, cy, radius;
    calculateShapeParams(arena, &cx, &cy, &radius);

    for (int i = 0; i < count; i++) {
        int x, y, attempts = 0;
        do {
            x = rand() % (arena->width - 2) + 1;
            y = rand() % (arena->height - 2) + 1;
        } while ((arena->grid[y][x] != EMPTY || !isInsideShape(x, y, cx, cy, radius, shape))
                 && ++attempts < 100);
        if (attempts < 100) {
            arena->grid[y][x] = MARKER;
            arena->marker_count++;
        }
    }
}

static int hasAdjacentObstacles(Arena *arena, int x, int y) {
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    int obstacleCount = 0;

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (arena->grid[ny][nx] == OBSTACLE) {
            obstacleCount++;
        }
    }
    return obstacleCount >= 2;
}

void placeRandomObstacles(Arena *arena, int count, ShapeType shape) {
    int cx, cy, radius;
    calculateShapeParams(arena, &cx, &cy, &radius);

    for (int i = 0; i < count; i++) {
        int x, y, attempts = 0;
        do {
            x = rand() % (arena->width - 2) + 1;
            y = rand() % (arena->height - 2) + 1;
        } while ((arena->grid[y][x] != EMPTY ||
                  !isInsideShape(x, y, cx, cy, radius, shape) ||
                  hasAdjacentObstacles(arena, x, y))
                 && ++attempts < 200);
        if (attempts < 200) {
            arena->grid[y][x] = OBSTACLE;
        }
    }
}

void initMovementTrail(MovementTrail *trail) {
    for (int y = 0; y < MAX_ARENA_SIZE; y++) {
        for (int x = 0; x < MAX_ARENA_SIZE; x++) {
            trail->horizontal_count[y][x] = 0;
            trail->vertical_count[y][x] = 0;
            trail->total_passes[y][x] = 0;
            for (int i = 0; i < 3; i++) {
                trail->horizontal_passes[y][x][i] = 0;
                trail->vertical_passes[y][x][i] = 0;
            }
        }
    }
}

void recordMovement(MovementTrail *trail, int x, int y, char direction) {
    if (trail->total_passes[y][x] < 3) {
        trail->total_passes[y][x]++;
    }

    if (direction == 'E' || direction == 'W') {
        if (trail->horizontal_count[y][x] < 3) {
            int count = trail->horizontal_count[y][x];
            trail->horizontal_passes[y][x][count] = trail->total_passes[y][x];
            trail->horizontal_count[y][x]++;
        }
    } else {
        if (trail->vertical_count[y][x] < 3) {
            int count = trail->vertical_count[y][x];
            trail->vertical_passes[y][x][count] = trail->total_passes[y][x];
            trail->vertical_count[y][x]++;
        }
    }
}

static void setOrderColor(int order) {
    if (order == FIRST_PASS) setColour(green);
    else if (order == SECOND_PASS) setColour(yellow);
    else setColour(red);
}

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

static void drawHorizontalTrail(MovementTrail *trail, int x, int y) {
    int count = trail->horizontal_count[y][x];
    int spacing = 6;
    int startOffset = -(count - 1) * spacing / 2;

    for (int i = 0; i < count; i++) {
        int order = trail->horizontal_passes[y][x][i];
        if (order > 0) {
            setOrderColor(order);
            drawTrailLine(x, y, 1, startOffset + i * spacing);
        }
    }
}

static void drawVerticalTrail(MovementTrail *trail, int x, int y) {
    int count = trail->vertical_count[y][x];
    int spacing = 6;
    int startOffset = -(count - 1) * spacing / 2;

    for (int i = 0; i < count; i++) {
        int order = trail->vertical_passes[y][x][i];
        if (order > 0) {
            setOrderColor(order);
            drawTrailLine(x, y, 0, startOffset + i * spacing);
        }
    }
}

void drawMovementTrails(MovementTrail *trail, int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            drawHorizontalTrail(trail, x, y);
            drawVerticalTrail(trail, x, y);
        }
    }
}
