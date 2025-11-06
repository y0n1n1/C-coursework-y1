#include <stdlib.h>
#include <time.h>
#include "robot.h"
#include "arena.h"
#include "graphics.h"
#include "pathfinding.h"

#define ANIMATION_DELAY 150
#define MAX_MOVES 1000
#define MAX_MARKERS 20

/* MarkerList stores discovered marker locations during exploration */
typedef struct {
    int x[MAX_MARKERS];
    int y[MAX_MARKERS];
    int count;
} MarkerList;

void parseArgs(Robot *robot, int argc, char **argv,
               int arena[][MAX_ARENA_SIZE], int w, int h);
void setupGame(int arena[][MAX_ARENA_SIZE], int *w, int *h);
void runSimulation(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h);
int atCorner(Robot *robot, int width, int height);

/* Stage 4: Exploration and pathfinding */
void followPath(Robot *robot, int arena[][MAX_ARENA_SIZE], Path *path);
void turnToDirection(Robot *robot, char target);
void exploreAndDiscover(Robot *robot, int arena[][MAX_ARENA_SIZE],
                        MarkerList *markers, int w, int h);
void storeMarkerLocation(MarkerList *markers, int x, int y);
void collectDiscoveredMarkers(Robot *robot, int arena[][MAX_ARENA_SIZE],
                              MarkerList *markers, int w, int h);
void deliverToCorner(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h);

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
    int obstacleCount = (*w + *h) / 6;
    int markerCount = 2 + rand() % 3;
    placeObstacles(arena, *w, *h, obstacleCount);
    placeMultipleMarkers(arena, *w, *h, markerCount);
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

/* Stage 4: Explore, discover, collect markers, deliver to corner */
void runSimulation(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h) {
    MarkerList discoveredMarkers = {.count = 0};
    exploreAndDiscover(robot, arena, &discoveredMarkers, w, h);
    collectDiscoveredMarkers(robot, arena, &discoveredMarkers, w, h);
    deliverToCorner(robot, arena, w, h);
}

/* Store marker location in discovered list (no duplicates) */
void storeMarkerLocation(MarkerList *markers, int x, int y) {
    for (int i = 0; i < markers->count; i++) {
        if (markers->x[i] == x && markers->y[i] == y) return;
    }
    if (markers->count < MAX_MARKERS) {
        markers->x[markers->count] = x;
        markers->y[markers->count] = y;
        markers->count++;
    }
}

/* Check if tile should be explored */
static int shouldExplore(int visited[][MAX_ARENA_SIZE], int arena[][MAX_ARENA_SIZE],
                         int x, int y) {
    return !visited[y][x] && arena[y][x] == 0;
}

/* Visit tile and check for marker */
static void visitTile(Robot *robot, int arena[][MAX_ARENA_SIZE],
                      int visited[][MAX_ARENA_SIZE], MarkerList *markers,
                      int x, int y) {
    Path path;
    if (findPath(arena, robot->x, robot->y, x, y, &path)) {
        followPath(robot, arena, &path);
        visited[y][x] = 1;
        if (atMarker(robot, arena)) {
            storeMarkerLocation(markers, robot->x, robot->y);
        }
    }
}

/* Explore arena systematically, discover and store markers */
void exploreAndDiscover(Robot *robot, int arena[][MAX_ARENA_SIZE],
                        MarkerList *markers, int w, int h) {
    int visited[MAX_ARENA_SIZE][MAX_ARENA_SIZE] = {0};
    visited[robot->y][robot->x] = 1;
    if (atMarker(robot, arena)) storeMarkerLocation(markers, robot->x, robot->y);

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            if (shouldExplore(visited, arena, x, y)) {
                visitTile(robot, arena, visited, markers, x, y);
            }
        }
    }
}

/* Collect all discovered markers using pathfinding */
void collectDiscoveredMarkers(Robot *robot, int arena[][MAX_ARENA_SIZE],
                              MarkerList *markers, int w, int h) {
    for (int i = 0; i < markers->count; i++) {
        Path path;
        if (findPath(arena, robot->x, robot->y,
                     markers->x[i], markers->y[i], &path)) {
            followPath(robot, arena, &path);
            if (atMarker(robot, arena)) {
                pickUpMarker(robot, arena);
                drawBackground(arena, w, h);
                foreground();
            }
        }
    }
}

/* Navigate to corner and drop all markers */
void deliverToCorner(Robot *robot, int arena[][MAX_ARENA_SIZE], int w, int h) {
    int cornerX = w - 2, cornerY = h - 2;
    Path path;
    if (findPath(arena, robot->x, robot->y, cornerX, cornerY, &path)) {
        followPath(robot, arena, &path);
    }
    while (markerCount(robot) > 0) {
        dropMarker(robot, arena);
    }
    drawBackground(arena, w, h);
    foreground();
    drawRobot(robot);
}

/* Check if robot is at any corner of the arena */
int atCorner(Robot *robot, int width, int height) {
    int x = robot->x, y = robot->y;
    return (x == 1 && y == 1) || (x == width-2 && y == 1) ||
           (x == 1 && y == height-2) || (x == width-2 && y == height-2);
}

/* Calculate direction from current to next position */
static char getDirection(int fromX, int fromY, int toX, int toY) {
    if (toX > fromX) return 'E';
    if (toX < fromX) return 'W';
    if (toY > fromY) return 'S';
    return 'N';
}

/* Turn robot to face target direction (N/S/E/W) */
void turnToDirection(Robot *robot, char target) {
    while (robot->direction != target) {
        right(robot);
    }
}

/* Follow path from start to end */
void followPath(Robot *robot, int arena[][MAX_ARENA_SIZE], Path *path) {
    for (int i = 0; i < path->length; i++) {
        char dir = getDirection(robot->x, robot->y, path->x[i], path->y[i]);
        turnToDirection(robot, dir);
        forward(robot, arena);
        drawRobot(robot);
        sleep(ANIMATION_DELAY);
    }
}
