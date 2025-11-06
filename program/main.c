#include <stdlib.h>
#include <time.h>
#include "robot.h"
#include "arena.h"
#include "graphics.h"
#include "pathfinding.h"

#define ANIMATION_DELAY 150
#define MAX_MOVES 1000

/* NOVEL: Global movement trail for efficiency visualization */
static MovementTrail g_trail;

/* Context bundles common exploration parameters (reduces overhead) */
typedef struct {
    Robot *robot;
    int (*arena)[MAX_ARENA_SIZE];
    int (*visited)[MAX_ARENA_SIZE];
    int width, height;
} ExplorationContext;

void parseArgs(Robot *robot, int argc, char **argv,
               int arena[][MAX_ARENA_SIZE], int w, int h);
void setupGame(int arena[][MAX_ARENA_SIZE], int *w, int *h);
void runSimulation(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h);

/* Stage 5: Exploration and pathfinding with shaped arena */
void followPath(Robot *robot, int arena[][MAX_ARENA_SIZE], Path *path, int w, int h);
void exploreAndCollect(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h);

int main(int argc, char **argv) {
    int arena[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    Robot robot;
    int width, height;

    srand(time(NULL));
    setupGame(arena, &width, &height);
    parseArgs(&robot, argc, argv, arena, width, height);
    runSimulation(&robot, arena, width, height);

    return 0;
}

void setupGame(int arena[][MAX_ARENA_SIZE], int *w, int *h) {
    *w = randomArenaSize();
    *h = randomArenaSize();
    setWindowSize(*w * TILE_SIZE, *h * TILE_SIZE);
    initArena(arena, *w, *h);
    initMovementTrail(&g_trail);
    int markerCount = 3 + rand() % 5;
    placeShapedObstacles(arena, *w, *h, SHAPE_CIRCLE);
    placeMarkersInShape(arena, *w, *h, markerCount, SHAPE_CIRCLE);
    drawBackground(arena, *w, *h);
    foreground();
}

void parseArgs(Robot *robot, int argc, char **argv,
               int arena[][MAX_ARENA_SIZE], int w, int h) {
    if (argc == 4) {
        robot->x = atoi(argv[1]);
        robot->y = atoi(argv[2]);
        char *direction = argv[3];
        robot->direction = (direction[0] == 'n') ? 'N' :
                           (direction[0] == 's') ? 'S' :
                           (direction[0] == 'e') ? 'E' : 'W';
        robot->markers_held = 0;
    } else {
        initRobot(robot, arena, w, h);
    }
}

/* Stage 5: Explore and collect markers, stop when done (no delivery) */
void runSimulation(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h) {
    exploreAndCollect(robot, arena, w, h);
}

/* Check if tile is visitable and unvisited */
static int isUnvisited(int visited[][MAX_ARENA_SIZE], int arena[][MAX_ARENA_SIZE],
                       int x, int y, int w, int h) {
    if (x < 1 || x >= w-1 || y < 1 || y >= h-1) return 0;
    if (visited[y][x]) return 0;
    return (arena[y][x] == 0 || arena[y][x] == 3);
}

/* Find adjacent unvisited tile (NOVEL: enables O(1) movement) */
static int findAdjacentUnvisited(ExplorationContext *ctx, int *nextX, int *nextY) {
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    for (int i = 0; i < 4; i++) {
        int x = ctx->robot->x + dx[i];
        int y = ctx->robot->y + dy[i];
        if (isUnvisited(ctx->visited, ctx->arena, x, y, ctx->width, ctx->height)) {
            *nextX = x;
            *nextY = y;
            return 1;
        }
    }
    return 0;
}

/* Calculate direction from current to next position */
static char getDirection(int fromX, int fromY, int toX, int toY) {
    if (toX > fromX) return 'E';
    if (toX < fromX) return 'W';
    if (toY > fromY) return 'S';
    return 'N';
}

/* Turn robot to face target direction (N/S/E/W) */
static void turnToDirection(Robot *robot, char target) {
    while (robot->direction != target) {
        right(robot);
    }
}

/* Collect marker at current position if present */
static void collectAtPosition(Robot *robot, int arena[][MAX_ARENA_SIZE],
                               int w, int h) {
    if (atMarker(robot, arena)) {
        pickUpMarker(robot, arena);
        drawBackground(arena, w, h);
        foreground();
    }
}

/* Find nearest unvisited tile using search */
static int findNearestUnvisited(ExplorationContext *ctx, int *targetX, int *targetY) {
    for (int y = 1; y < ctx->height - 1; y++) {
        for (int x = 1; x < ctx->width - 1; x++) {
            if (isUnvisited(ctx->visited, ctx->arena, x, y,
                           ctx->width, ctx->height)) {
                *targetX = x;
                *targetY = y;
                return 1;
            }
        }
    }
    return 0;
}

/* NOVEL: Direct movement for adjacent tiles - O(1) instead of O(wh) BFS */
static void moveToAdjacent(ExplorationContext *ctx, int targetX, int targetY) {
    char dir = getDirection(ctx->robot->x, ctx->robot->y, targetX, targetY);
    turnToDirection(ctx->robot, dir);
    forward(ctx->robot, ctx->arena);
    recordMovement(&g_trail, ctx->robot->x, ctx->robot->y, ctx->robot->direction);
    ctx->visited[ctx->robot->y][ctx->robot->x] = 1;
    collectAtPosition(ctx->robot, ctx->arena, ctx->width, ctx->height);
    drawRobot(ctx->robot);
    drawMovementTrails(&g_trail, ctx->width, ctx->height);
    sleep(ANIMATION_DELAY);
}

/* Mark path tiles as visited and collect markers */
static void followAndCollect(Robot *robot, int arena[][MAX_ARENA_SIZE],
                              Path *path, int visited[][MAX_ARENA_SIZE],
                              int w, int h) {
    for (int i = 0; i < path->length; i++) {
        char dir = getDirection(robot->x, robot->y, path->x[i], path->y[i]);
        turnToDirection(robot, dir);
        forward(robot, arena);
        recordMovement(&g_trail, robot->x, robot->y, robot->direction);
        visited[robot->y][robot->x] = 1;
        collectAtPosition(robot, arena, w, h);
        drawRobot(robot);
        drawMovementTrails(&g_trail, w, h);
        sleep(ANIMATION_DELAY);
    }
}

/* NOVEL: Try adjacent move with O(1) direct movement (no BFS!) */
static int tryAdjacentMove(ExplorationContext *ctx) {
    int nextX, nextY;
    if (!findAdjacentUnvisited(ctx, &nextX, &nextY)) return 0;
    moveToAdjacent(ctx, nextX, nextY);
    return 1;
}

/* Try jumping to nearest unvisited tile (uses BFS when needed) */
static int tryJumpToUnvisited(ExplorationContext *ctx) {
    int nextX, nextY;
    if (!findNearestUnvisited(ctx, &nextX, &nextY)) return 0;

    Path path;
    if (findPath(ctx->arena, ctx->robot->x, ctx->robot->y, nextX, nextY, &path)) {
        followAndCollect(ctx->robot, ctx->arena, &path, ctx->visited,
                        ctx->width, ctx->height);
    }
    return 1;
}

/* NOVEL: Hybrid greedy+BFS exploration with context pattern */
void exploreAndCollect(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h) {
    int visited[MAX_ARENA_SIZE][MAX_ARENA_SIZE] = {0};
    ExplorationContext ctx = {robot, arena, visited, w, h};

    visited[robot->y][robot->x] = 1;
    collectAtPosition(robot, arena, w, h);

    while (tryAdjacentMove(&ctx) || tryJumpToUnvisited(&ctx)) {
        if (countMarkers(arena, w, h) == 0) {
            break;
        }
    }
}


/* Follow path without marking visited */
void followPath(Robot *robot, int arena[][MAX_ARENA_SIZE], Path *path, int w, int h) {
    for (int i = 0; i < path->length; i++) {
        char dir = getDirection(robot->x, robot->y, path->x[i], path->y[i]);
        turnToDirection(robot, dir);
        forward(robot, arena);
        recordMovement(&g_trail, robot->x, robot->y, robot->direction);
        drawRobot(robot);
        drawMovementTrails(&g_trail, w, h);
        sleep(ANIMATION_DELAY);
    }
}
