#include "robot.h"

/* Helper: calculate next position based on current direction */
static void getNextPos(Robot *robot, int *newX, int *newY) {
    *newX = robot->x;
    *newY = robot->y;
    if (robot->direction == 'N') (*newY)--;
    else if (robot->direction == 'S') (*newY)++;
    else if (robot->direction == 'E') (*newX)++;
    else if (robot->direction == 'W') (*newX)--;
}

void forward(Robot *robot, int arena[][MAX_ARENA_SIZE]) {
    int newX, newY;
    getNextPos(robot, &newX, &newY);

    if (arena[newY][newX] != 1 && arena[newY][newX] != 2) {
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

int atMarker(Robot *robot, int arena[][MAX_ARENA_SIZE]) {
    return arena[robot->y][robot->x] == 3;
}

int canMoveForward(Robot *robot, int arena[][MAX_ARENA_SIZE]) {
    int newX, newY;
    getNextPos(robot, &newX, &newY);
    return arena[newY][newX] != 1 && arena[newY][newX] != 2;
}

void pickUpMarker(Robot *robot, int arena[][MAX_ARENA_SIZE]) {
    if (arena[robot->y][robot->x] == 3) {
        arena[robot->y][robot->x] = 0;
        robot->markers_held++;
    }
}

void dropMarker(Robot *robot, int arena[][MAX_ARENA_SIZE]) {
    if (robot->markers_held > 0) {
        arena[robot->y][robot->x] = 3;
        robot->markers_held--;
    }
}

int markerCount(Robot *robot) {
    return robot->markers_held;
}
