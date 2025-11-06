#ifndef ROBOT_H
#define ROBOT_H

/* Robot state: position (x,y in tile coordinates), direction (N/S/E/W),
   and number of markers currently held */
typedef struct {
    int x;
    int y;
    char direction;
    int markers_held;
} Robot;

/* Robot API - 8 required functions for controlling robot behavior */
#define MAX_ARENA_SIZE 20

void forward(Robot *robot, int arena[][MAX_ARENA_SIZE]);
void left(Robot *robot);
void right(Robot *robot);
int atMarker(Robot *robot, int arena[][MAX_ARENA_SIZE]);
int canMoveForward(Robot *robot, int arena[][MAX_ARENA_SIZE]);
void pickUpMarker(Robot *robot, int arena[][MAX_ARENA_SIZE]);
void dropMarker(Robot *robot, int arena[][MAX_ARENA_SIZE]);
int markerCount(Robot *robot);

#endif
