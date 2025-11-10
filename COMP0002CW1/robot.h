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

#endif
