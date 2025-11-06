#include <stdlib.h>
#include "pathfinding.h"

#define MAX_QUEUE_SIZE 400

/* Queue for BFS - stores coordinates to visit */
typedef struct {
    int x[MAX_QUEUE_SIZE];
    int y[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;

/* Direction offsets: N, S, E, W */
static int dx[] = {0, 0, 1, -1};
static int dy[] = {-1, 1, 0, 0};

/* Initialize empty queue */
static void initQueue(Queue *q) {
    q->front = 0;
    q->rear = 0;
}

/* Check if queue is empty */
static int isQueueEmpty(Queue *q) {
    return q->front == q->rear;
}

/* Add position to queue */
static void enqueue(Queue *q, int x, int y) {
    q->x[q->rear] = x;
    q->y[q->rear] = y;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
}

/* Remove and return position from queue */
static void dequeue(Queue *q, int *x, int *y) {
    *x = q->x[q->front];
    *y = q->y[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
}

/* Check if position is valid for movement */
static int isValidMove(int arena[][MAX_ARENA_SIZE], int visited[][MAX_ARENA_SIZE],
                       int x, int y, int w, int h) {
    if (x <= 0 || x >= w-1 || y <= 0 || y >= h-1) return 0;
    if (visited[y][x]) return 0;
    if (arena[y][x] == 1 || arena[y][x] == 2) return 0;
    return 1;
}

/* Trace path backwards from end to start */
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

/* Reverse path from end→start to start→end */
static void reversePath(int tempX[], int tempY[], int len, Path *path) {
    for (int i = 0; i < len; i++) {
        path->x[i] = tempX[len - 1 - i];
        path->y[i] = tempY[len - 1 - i];
    }
    path->length = len;
}

/* Reconstruct path from parent pointers */
static void reconstructPath(int parentX[][MAX_ARENA_SIZE],
                           int parentY[][MAX_ARENA_SIZE],
                           int startX, int startY, int endX, int endY,
                           Path *path) {
    int tempX[MAX_PATH_LENGTH], tempY[MAX_PATH_LENGTH];
    int len = tracePath(parentX, parentY, startX, startY, endX, endY,
                        tempX, tempY);
    reversePath(tempX, tempY, len, path);
}

/* Process neighbors of current position */
static void processNeighbors(int arena[][MAX_ARENA_SIZE], int w, int h,
                             int x, int y, int visited[][MAX_ARENA_SIZE],
                             int parentX[][MAX_ARENA_SIZE],
                             int parentY[][MAX_ARENA_SIZE], Queue *q) {
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (isValidMove(arena, visited, nx, ny, w, h)) {
            visited[ny][nx] = 1;
            parentX[ny][nx] = x;
            parentY[ny][nx] = y;
            enqueue(q, nx, ny);
        }
    }
}

/* Main BFS search algorithm */
static int bfsSearch(int arena[][MAX_ARENA_SIZE], int w, int h,
                     int startX, int startY, int endX, int endY,
                     int visited[][MAX_ARENA_SIZE],
                     int parentX[][MAX_ARENA_SIZE],
                     int parentY[][MAX_ARENA_SIZE], Queue *q) {
    enqueue(q, startX, startY);
    visited[startY][startX] = 1;
    while (!isQueueEmpty(q)) {
        int x, y;
        dequeue(q, &x, &y);
        if (x == endX && y == endY) return 1;
        processNeighbors(arena, w, h, x, y, visited, parentX, parentY, q);
    }
    return 0;
}

/* Initialize pathfinding data structures */
static void initPathfinding(int visited[][MAX_ARENA_SIZE], Queue *queue) {
    for (int i = 0; i < MAX_ARENA_SIZE; i++) {
        for (int j = 0; j < MAX_ARENA_SIZE; j++) {
            visited[i][j] = 0;
        }
    }
    initQueue(queue);
}

/* Find shortest path using BFS - main interface function */
int findPath(int arena[][MAX_ARENA_SIZE], int startX, int startY,
             int endX, int endY, Path *path) {
    int visited[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    int parentX[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    int parentY[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
    Queue queue;
    initPathfinding(visited, &queue);

    if (bfsSearch(arena, MAX_ARENA_SIZE, MAX_ARENA_SIZE, startX, startY,
                  endX, endY, visited, parentX, parentY, &queue)) {
        reconstructPath(parentX, parentY, startX, startY, endX, endY, path);
        return 1;
    }
    path->length = 0;
    return 0;
}
