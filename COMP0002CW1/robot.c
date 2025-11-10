#include "robot.h"
#include "arena.h"

static void getNextPos(Robot *robot, int *newX, int *newY) {
    *newX = robot->x;
    *newY = robot->y;
    if (robot->direction == 'N') (*newY)--;
    else if (robot->direction == 'S') (*newY)++;
    else if (robot->direction == 'E') (*newX)++;
    else if (robot->direction == 'W') (*newX)--;
}

void forward(Robot *robot, Arena *arena) {
    int newX, newY;
    getNextPos(robot, &newX, &newY);

    if (arena->grid[newY][newX] != WALL && arena->grid[newY][newX] != OBSTACLE) {
        robot->x = newX;
        robot->y = newY;
    }
}

void left(Robot *robot) {
    if (robot->direction == 'N') robot->direction = 'W';
    else if (robot->direction == 'W') robot->direction = 'S';
    else if (robot->direction == 'S') robot->direction = 'E';
    else if (robot->direction == 'E') robot->direction = 'N';
}

void right(Robot *robot) {
    if (robot->direction == 'N') robot->direction = 'E';
    else if (robot->direction == 'E') robot->direction = 'S';
    else if (robot->direction == 'S') robot->direction = 'W';
    else if (robot->direction == 'W') robot->direction = 'N';
}

int atMarker(Robot *robot, Arena *arena) {
    return arena->grid[robot->y][robot->x] == MARKER;
}

int canMoveForward(Robot *robot, Arena *arena) {
    int newX, newY;
    getNextPos(robot, &newX, &newY);
    return arena->grid[newY][newX] != WALL && arena->grid[newY][newX] != OBSTACLE;
}

void pickUpMarker(Robot *robot, Arena *arena) {
    if (arena->grid[robot->y][robot->x] == MARKER) {
        arena->grid[robot->y][robot->x] = EMPTY;
        robot->markers_held++;
        arena->marker_count--;
    }
}

void dropMarker(Robot *robot, Arena *arena) {
    if (robot->markers_held > 0) {
        arena->grid[robot->y][robot->x] = MARKER;
        robot->markers_held--;
        arena->marker_count++;
    }
}

int markerCount(Robot *robot) {
    return robot->markers_held;
}
