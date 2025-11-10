#include <stdlib.h>
#include <time.h>
#include "arena.h"
#include "graphics.h"
#include "pathfinding.h"

#define ANIMATION_DELAY 150
#define MAX_MOVES 1000

const int TILE_SIZE = 20;

static MovementTrail g_trail;

typedef struct {
    Robot *robot;
    Arena *arena;
    int (*visited)[MAX_ARENA_SIZE];
} ExplorationContext;

void setupGame(Arena *arena);
void runSimulation(Robot *robot, Arena *arena);
void followPath(Robot *robot, Arena *arena, Path *path);
void exploreAndCollect(Robot *robot, Arena *arena);
void deliverToCorner(Robot *robot, Arena *arena);

int main(int argc, char **argv) {
    Arena arena;
    Robot robot;

    srand(time(NULL));
    setupGame(&arena);
    initRobot(&robot, &arena);
    runSimulation(&robot, &arena);

    return 0;
}

void setupGame(Arena *arena) {
    initArena(arena);
    setWindowSize(arena->width * TILE_SIZE, arena->height * TILE_SIZE);
    initMovementTrail(&g_trail);
    int markerCount = 3 + rand() % 5;
    int obstacleCount = arena->width / 6;
    ShapeType shape = rand() % 5;
    placeShapedObstacles(arena, shape);
    placeRandomObstacles(arena, obstacleCount, shape);
    placeMarkersInShape(arena, markerCount, shape);
    drawBackground(arena);
    foreground();
}

void runSimulation(Robot *robot, Arena *arena) {
    exploreAndCollect(robot, arena);
}

static int isUnvisited(ExplorationContext *ctx, int x, int y) {
    if (x < 1 || x >= ctx->arena->width-1 || y < 1 || y >= ctx->arena->height-1) return 0;
    if (ctx->visited[y][x]) return 0;
    return (ctx->arena->grid[y][x] == EMPTY || ctx->arena->grid[y][x] == MARKER);
}

static int findAdjacentUnvisited(ExplorationContext *ctx, int *nextX, int *nextY) {
    for (int i = 0; i < 4; i++) {
        int x = ctx->robot->x + DIRECTION_DX[i];
        int y = ctx->robot->y + DIRECTION_DY[i];
        if (isUnvisited(ctx, x, y)) {
            *nextX = x;
            *nextY = y;
            return 1;
        }
    }
    return 0;
}

static char getDirection(int fromX, int fromY, int toX, int toY) {
    if (toX > fromX) return 'E';
    if (toX < fromX) return 'W';
    if (toY > fromY) return 'S';
    return 'N';
}

static void turnToDirection(Robot *robot, char target) {
    while (robot->direction != target) {
        right(robot);
    }
}

static void collectAtPosition(Robot *robot, Arena *arena) {
    if (atMarker(robot, arena)) {
        pickUpMarker(robot, arena);
        drawBackground(arena);
        foreground();
    }
}

static int findNearestUnvisited(ExplorationContext *ctx, int *targetX, int *targetY) {
    for (int y = 1; y < ctx->arena->height - 1; y++) {
        for (int x = 1; x < ctx->arena->width - 1; x++) {
            if (isUnvisited(ctx, x, y)) {
                *targetX = x;
                *targetY = y;
                return 1;
            }
        }
    }
    return 0;
}

/* Avoids pathfinding overhead for adjacent tiles */
static void moveToAdjacent(ExplorationContext *ctx, int targetX, int targetY) {
    char dir = getDirection(ctx->robot->x, ctx->robot->y, targetX, targetY);
    turnToDirection(ctx->robot, dir);
    forward(ctx->robot, ctx->arena);
    recordMovement(&g_trail, ctx->robot->x, ctx->robot->y, ctx->robot->direction);
    ctx->visited[ctx->robot->y][ctx->robot->x] = 1;
    collectAtPosition(ctx->robot, ctx->arena);
    drawRobot(ctx->robot);
    drawMovementTrails(&g_trail, ctx->arena->width, ctx->arena->height);
    sleep(ANIMATION_DELAY);
}

static void followAndCollect(ExplorationContext *ctx, Path *path) {
    for (int i = 0; i < path->length; i++) {
        char dir = getDirection(ctx->robot->x, ctx->robot->y, path->x[i], path->y[i]);
        turnToDirection(ctx->robot, dir);
        forward(ctx->robot, ctx->arena);
        recordMovement(&g_trail, ctx->robot->x, ctx->robot->y, ctx->robot->direction);
        ctx->visited[ctx->robot->y][ctx->robot->x] = 1;
        collectAtPosition(ctx->robot, ctx->arena);
        drawRobot(ctx->robot);
        drawMovementTrails(&g_trail, ctx->arena->width, ctx->arena->height);
        sleep(ANIMATION_DELAY);
    }
}

static int tryAdjacentMove(ExplorationContext *ctx) {
    int nextX, nextY;
    if (!findAdjacentUnvisited(ctx, &nextX, &nextY)) return 0;
    moveToAdjacent(ctx, nextX, nextY);
    return 1;
}

static int tryJumpToUnvisited(ExplorationContext *ctx) {
    int nextX, nextY;
    if (!findNearestUnvisited(ctx, &nextX, &nextY)) return 0;

    Path path;
    if (findPath(ctx->arena, ctx->robot->x, ctx->robot->y, nextX, nextY, &path)) {
        followAndCollect(ctx, &path);
    }
    return 1;
}

/* Prioritizes adjacent moves, falls back to BFS pathfinding when needed */
void exploreAndCollect(Robot *robot, Arena *arena) {
    int visited[MAX_ARENA_SIZE][MAX_ARENA_SIZE] = {0};
    ExplorationContext ctx = {robot, arena, visited};

    visited[robot->y][robot->x] = 1;
    collectAtPosition(robot, arena);

    while (tryAdjacentMove(&ctx) || tryJumpToUnvisited(&ctx)) {
        if (countMarkers(arena) == 0) {
            break;
        }
    }
}

/* Follows path without tracking visited tiles (for non-exploration movement) */
void followPath(Robot *robot, Arena *arena, Path *path) {
    for (int i = 0; i < path->length; i++) {
        char dir = getDirection(robot->x, robot->y, path->x[i], path->y[i]);
        turnToDirection(robot, dir);
        forward(robot, arena);
        recordMovement(&g_trail, robot->x, robot->y, robot->direction);
        drawRobot(robot);
        drawMovementTrails(&g_trail, arena->width, arena->height);
        sleep(ANIMATION_DELAY);
    }
}

static int findNearestCorner(Arena *arena, int x, int y, int *cornerX, int *cornerY) {
    int corners[4][2] = {
        {1, 1},
        {arena->width - 2, 1},
        {1, arena->height - 2},
        {arena->width - 2, arena->height - 2}
    };

    int minDist = arena->width * arena->height;
    int bestCorner = -1;

    for (int i = 0; i < 4; i++) {
        if (arena->grid[corners[i][1]][corners[i][0]] == EMPTY) {
            int dx = corners[i][0] - x;
            int dy = corners[i][1] - y;
            int dist = dx * dx + dy * dy;
            if (dist < minDist) {
                minDist = dist;
                bestCorner = i;
            }
        }
    }

    if (bestCorner == -1) return 0;

    *cornerX = corners[bestCorner][0];
    *cornerY = corners[bestCorner][1];
    return 1;
}

void deliverToCorner(Robot *robot, Arena *arena) {
    int cornerX, cornerY;
    if (!findNearestCorner(arena, robot->x, robot->y, &cornerX, &cornerY)) {
        while (markerCount(robot) > 0) {
            dropMarker(robot, arena);
        }
        drawBackground(arena);
        foreground();
        drawRobot(robot);
        drawMovementTrails(&g_trail, arena->width, arena->height);
        return;
    }

    Path path;
    if (findPath(arena, robot->x, robot->y, cornerX, cornerY, &path)) {
        followPath(robot, arena, &path);
    }

    while (markerCount(robot) > 0) {
        dropMarker(robot, arena);
    }
    drawBackground(arena);
    foreground();
    drawRobot(robot);
    drawMovementTrails(&g_trail, arena->width, arena->height);
}
