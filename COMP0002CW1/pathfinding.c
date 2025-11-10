#include <stdlib.h>
#include "pathfinding.h"

#define MAX_QUEUE_SIZE 400

typedef struct {
    int x[MAX_QUEUE_SIZE];
    int y[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;

static void initQueue(Queue *q) {
    q->front = 0;
    q->rear = 0;
}

static int isQueueEmpty(Queue *q) {
    return q->front == q->rear;
}

static void enqueue(Queue *q, int x, int y) {
    q->x[q->rear] = x;
    q->y[q->rear] = y;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
}

static void dequeue(Queue *q, int *x, int *y) {
    *x = q->x[q->front];
    *y = q->y[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
}

static int isValidMove(Arena *arena, int visited[][MAX_ARENA_SIZE], int x, int y) {
    if (x <= 0 || x >= arena->width-1 || y <= 0 || y >= arena->height-1) return 0;
    if (visited[y][x]) return 0;
    if (arena->grid[y][x] == WALL || arena->grid[y][x] == OBSTACLE) return 0;
    return 1;
}

static int tracePath(int parentX[][MAX_ARENA_SIZE], int parentY[][MAX_ARENA_SIZE],
                     int startX, int startY, int endX, int endY,
                     int tempX[], int tempY[]) {
    int x = endX, y = endY, len = 0;
    while (x != startX || y != startY) {
        tempX[len] = x;
        tempY[len] = y;
        int px = parentX[y][x], py = parentY[y][x];
        x = px; y = py;
        len++;
    }
    return len;
}

static void reversePath(int tempX[], int tempY[], int len, Path *path) {
    for (int i = 0; i < len; i++) {
        path->x[i] = tempX[len - 1 - i];
        path->y[i] = tempY[len - 1 - i];
    }
    path->length = len;
}

static void reconstructPath(int parentX[][MAX_ARENA_SIZE],
                           int parentY[][MAX_ARENA_SIZE],
                           int startX, int startY, int endX, int endY,
                           Path *path) {
    int tempX[MAX_PATH_LENGTH], tempY[MAX_PATH_LENGTH];
    int len = tracePath(parentX, parentY, startX, startY, endX, endY,
                        tempX, tempY);
    reversePath(tempX, tempY, len, path);
}

static void processNeighbors(Arena *arena, int x, int y,
                             int visited[][MAX_ARENA_SIZE],
                             int parentX[][MAX_ARENA_SIZE],
                             int parentY[][MAX_ARENA_SIZE], Queue *q) {
    for (int i = 0; i < 4; i++) {
        int nx = x + DIRECTION_DX[i], ny = y + DIRECTION_DY[i];
        if (isValidMove(arena, visited, nx, ny)) {
            visited[ny][nx] = 1;
            parentX[ny][nx] = x;
            parentY[ny][nx] = y;
            enqueue(q, nx, ny);
        }
    }
}

static int bfsSearch(Arena *arena, int startX, int startY, int endX, int endY,
                     int visited[][MAX_ARENA_SIZE],
                     int parentX[][MAX_ARENA_SIZE],
                     int parentY[][MAX_ARENA_SIZE], Queue *q) {
    enqueue(q, startX, startY);
    visited[startY][startX] = 1;
    while (!isQueueEmpty(q)) {
        int x, y;
        dequeue(q, &x, &y);
        if (x == endX && y == endY) return 1;
        processNeighbors(arena, x, y, visited, parentX, parentY, q);
    }
    return 0;
}

static void initPathfinding(int visited[][MAX_ARENA_SIZE], Queue *queue) {
    for (int i = 0; i < MAX_ARENA_SIZE; i++) {
        for (int j = 0; j < MAX_ARENA_SIZE; j++) {
            visited[i][j] = 0;
        }
    }
    initQueue(queue);
}

/* BFS pathfinding adapted from: https://github.com/rodriguesrenato/coverage-path-planning */
int findPath(Arena *arena, int startX, int startY,
             int endX, int endY, Path *path) {
    int visited[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    int parentX[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    int parentY[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    Queue queue;
    initPathfinding(visited, &queue);

    if (bfsSearch(arena, startX, startY, endX, endY,
                  visited, parentX, parentY, &queue)) {
        reconstructPath(parentX, parentY, startX, startY, endX, endY, path);
        return 1;
    }
    path->length = 0;
    return 0;
}
