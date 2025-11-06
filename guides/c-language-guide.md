# C Programming Language - Overview & Application to Robot Arena

## C Language Rundown

**What is C?**
C is a low-level, compiled, procedural programming language created in 1972. It's the foundation for most modern operating systems (Unix, Linux, Windows) and influenced languages like C++, Java, Python, and JavaScript.

**Key Characteristics:**

**1. Compiled Language**
- You write source code (.c files)
- Compiler (gcc/clang) converts it to machine code (executable)
- Fast execution, but errors caught at compile time

**2. Low-Level Control**
- Direct memory access via pointers
- Manual memory management (malloc/free)
- Close to hardware - you control everything

**3. Procedural/Imperative**
- Programs are sequences of instructions
- Organized into functions
- No objects/classes (unlike Java/Python)

**4. Static Typing**
- Variables must have declared types (int, char, float)
- Types checked at compile time, not runtime
- No dynamic typing like Python

**5. No Garbage Collection**
- You allocate memory, you free it
- Memory leaks if you forget to free
- Segmentation faults if you access wrong memory

---

## Core C Concepts

### Variables & Types
```c
int x = 5;              // Integer (whole numbers)
float price = 19.99;    // Floating point (decimals)
char letter = 'A';      // Single character
char name[20];          // String (array of chars)
int grid[10][10];       // 2D array (our arena!)
```

### Functions
```c
// Function declaration (in .h file)
int add(int a, int b);

// Function definition (in .c file)
int add(int a, int b) {
    return a + b;
}

// Function call
int sum = add(3, 5);  // sum = 8
```

### Structs (Custom Data Types)
```c
// Define a custom type
typedef struct {
    int x;
    int y;
    char direction;
} Robot;

// Create and use it
Robot myRobot;
myRobot.x = 5;
myRobot.y = 10;
myRobot.direction = 'N';
```

### Pointers (Memory Addresses)
```c
int value = 42;
int *ptr = &value;      // ptr holds memory address of value
printf("%d", *ptr);     // *ptr accesses the value (42)

// Used to pass data to functions for modification
void moveRobot(Robot *r) {
    r->x = r->x + 1;    // -> accesses struct members via pointer
}
```

### Arrays
```c
int numbers[5] = {1, 2, 3, 4, 5};  // 1D array
int grid[10][10];                   // 2D array (matrix)

// Access elements
numbers[0] = 10;        // First element
grid[2][3] = 1;         // Row 2, Column 3
```

### Control Flow
```c
// If statements
if (x > 5) {
    printf("Big");
} else {
    printf("Small");
}

// While loops
while (x < 10) {
    x++;
}

// For loops
for (int i = 0; i < 10; i++) {
    printf("%d\n", i);
}
```

---

## How C is Used in This Coursework

### 1. Program Structure

**Multiple Files (.h and .c):**

```
project/
├── main.c           # Entry point, orchestrates everything
├── robot.c          # Robot movement functions
├── robot.h          # Robot function declarations
├── arena.c          # Arena creation and management
├── arena.h          # Arena function declarations
├── graphics.c       # Drawing functions (provided)
└── graphics.h       # Drawing declarations (provided)
```

**Why split files?**
- Organization (related code together)
- Reusability (use robot.c in other projects)
- Compilation speed (only recompile changed files)
- Readability (easier to navigate)

### 2. Data Representation

**Arena as 2D Array:**
```c
#define ARENA_WIDTH 20
#define ARENA_HEIGHT 15

int arena[ARENA_HEIGHT][ARENA_WIDTH];

// Initialize arena
void initArena() {
    for (int y = 0; y < ARENA_HEIGHT; y++) {
        for (int x = 0; x < ARENA_WIDTH; x++) {
            // Fill borders with walls
            if (x == 0 || x == ARENA_WIDTH-1 || 
                y == 0 || y == ARENA_HEIGHT-1) {
                arena[y][x] = WALL;
            } else {
                arena[y][x] = EMPTY;
            }
        }
    }
}
```

**Robot as Struct:**
```c
typedef struct {
    int x, y;
    char direction;     // 'N', 'S', 'E', 'W'
    int markers_held;
} Robot;

// Create robot
Robot robot = {5, 5, 'N', 0};
```

### 3. Function Implementation

**Example: Moving the Robot Forward**
```c
void forward(Robot *robot, int arena[][ARENA_WIDTH]) {
    // Calculate next position
    int nextX = robot->x;
    int nextY = robot->y;
    
    switch(robot->direction) {
        case 'N': nextY--; break;
        case 'S': nextY++; break;
        case 'E': nextX++; break;
        case 'W': nextX--; break;
    }
    
    // Check if move is valid
    if (arena[nextY][nextX] != WALL && 
        arena[nextY][nextX] != OBSTACLE) {
        robot->x = nextX;
        robot->y = nextY;
    }
}
```

**Why pass pointers?**
- Functions get COPIES of variables by default
- Passing pointer lets function modify the original
- `Robot *robot` means "pointer to Robot struct"
- `robot->x` accesses struct member through pointer

### 4. Graphics Integration

**How Drawing Works:**
```c
#include "graphics.h"

int main() {
    // 1. Draw background ONCE
    background();
    drawArena(arena);          // Walls, obstacles, markers
    
    // 2. Animation loop
    while (!finished) {
        foreground();          // Switch to foreground layer
        clear();               // Clear previous robot
        
        // Robot logic
        if (canMoveForward(&robot, arena)) {
            forward(&robot, arena);
        }
        
        drawRobot(&robot);     // Draw robot at new position
        sleep(200);            // Pause so human can see
    }
    
    return 0;
}
```

**Key Graphics Functions:**
```c
setColour(red);                    // Set drawing color
fillRect(x, y, width, height);     // Draw filled rectangle
drawLine(x1, y1, x2, y2);         // Draw line
fillPolygon(3, xs, ys);           // Draw triangle (robot)
background();                      // Switch to background layer
foreground();                      // Switch to foreground layer
clear();                           // Clear current layer
```

### 5. Command Line Arguments

**How C Programs Receive Input:**
```c
int main(int argc, char **argv) {
    // argc = argument count
    // argv = array of argument strings
    
    // ./a.out 5 10 east
    // argc = 4
    // argv[0] = "./a.out"
    // argv[1] = "5"
    // argv[2] = "10"
    // argv[3] = "east"
    
    if (argc == 4) {
        int startX = atoi(argv[1]);     // Convert "5" to 5
        int startY = atoi(argv[2]);     // Convert "10" to 10
        char *direction = argv[3];       // "east" stays string
    }
}
```

---

## Complete Flow: How C Creates the Final Product

### Step 1: Compilation
```bash
gcc -o robot main.c robot.c arena.c graphics.c
# Compiles all .c files into executable named "robot"
```

**What happens:**
1. Preprocessor: Handles #include, #define
2. Compiler: Converts C to assembly
3. Assembler: Converts assembly to machine code
4. Linker: Combines all .o files into executable

### Step 2: Execution
```bash
./robot 5 5 north | java -jar drawapp-4.5.jar
```

**What happens:**
1. OS loads executable into memory
2. `main()` function starts
3. Command line args parsed (5, 5, north)
4. Arena created in memory (2D array)
5. Robot struct initialized
6. Graphics commands sent to stdout
7. Drawapp reads stdout and renders graphics

### Step 3: Program Flow

```c
int main(int argc, char **argv) {
    // 1. INITIALIZATION
    int arena[HEIGHT][WIDTH];
    Robot robot = {5, 5, 'N', 0};
    
    initArena(arena);                    // Fill with walls/empty
    placeMarkers(arena, 3);              // Random marker positions
    placeObstacles(arena, 5);            // Random obstacles
    
    // 2. DRAW BACKGROUND (once)
    background();
    drawArenaGrid(arena);
    drawWalls(arena);
    drawObstacles(arena);
    drawMarkers(arena);
    
    // 3. MAIN LOOP (animation)
    while (robot.markers_held < 3) {     // Until all markers collected
        foreground();
        clear();
        
        // Robot AI logic
        if (atMarker(&robot, arena)) {
            pickUpMarker(&robot, arena);
        }
        
        if (canMoveForward(&robot, arena)) {
            forward(&robot, arena);
        } else {
            right(&robot);               // Turn if blocked
        }
        
        drawRobot(&robot);
        sleep(200);                      // Pause for visibility
    }
    
    // 4. GO TO CORNER AND DROP
    while (!atCorner(&robot, arena)) {
        // Pathfinding logic...
        forward(&robot, arena);
        sleep(200);
    }
    
    while (robot.markers_held > 0) {
        dropMarker(&robot, arena);
        foreground();
        clear();
        drawRobot(&robot);
        drawMarkers(arena);
        sleep(200);
    }
    
    return 0;
}
```

---

## Memory Management Example

**How Data Lives in Memory:**

```c
void exampleMemory() {
    // Stack memory (automatic, local)
    int x = 5;                    // Dies when function ends
    Robot robot = {0, 0, 'N', 0}; // Dies when function ends
    
    // Heap memory (manual, persistent)
    int *ptr = malloc(sizeof(int) * 100);  // Allocate 100 ints
    // ... use ptr ...
    free(ptr);                    // MUST free or memory leak!
    
    // Static/Global memory (entire program lifetime)
    static int arena[20][20];     // Exists until program ends
}
```

**For this coursework:**
- Use local variables (stack) for most things
- Global or static for arena (too big for stack)
- Structs for robot (small, can be stack or global)
- Probably don't need malloc/free

---

## Why C for This Project?

**Advantages:**
1. **Fast execution** - compiled to machine code
2. **Precise control** - you decide memory layout, function calls
3. **Simple graphics integration** - direct function calls, no frameworks
4. **Learning fundamentals** - no magic, you see how everything works
5. **Pointers teach memory** - understand how data is stored/passed

**Challenges:**
1. **Manual memory** - you must track what you allocate
2. **No safety nets** - seg faults if you access wrong memory
3. **More verbose** - no built-in data structures (lists, dictionaries)
4. **Harder debugging** - compiler errors can be cryptic

---

## Key C Features Used in This Project

### 1. Structs
```c
typedef struct {
    int x, y;
    char direction;
    int markers_held;
} Robot;
```
**Why:** Bundle related data (robot's state) together

### 2. 2D Arrays
```c
int arena[HEIGHT][WIDTH];
```
**Why:** Natural representation of grid-based arena

### 3. Pointers
```c
void forward(Robot *robot);
```
**Why:** Pass data to functions for modification

### 4. Functions
```c
int canMoveForward(Robot *r, int arena[][WIDTH]);
```
**Why:** Break problem into manageable, reusable pieces

### 5. Header Files
```c
// robot.h
#ifndef ROBOT_H
#define ROBOT_H

void forward(Robot *robot);
void left(Robot *robot);

#endif
```
**Why:** Declare interfaces, separate from implementation

### 6. Preprocessor
```c
#define WALL 1
#define OBSTACLE 2
#define MARKER 3
```
**Why:** Named constants instead of magic numbers

### 7. Standard Library
```c
#include <stdlib.h>  // atoi, rand, malloc
#include <stdio.h>   // printf
#include <unistd.h>  // sleep
```
**Why:** Common utilities for parsing, random numbers, timing

---

## Summary: C in This Coursework

**You write:**
- Data structures (Robot struct, arena array)
- Functions (forward, left, right, canMoveForward, etc.)
- Algorithms (wall following, pathfinding)
- Graphics calls (drawRect, fillPolygon)
- Main loop (initialization, animation, logic)

**Compiler produces:**
- Single executable file
- Machine code that runs directly on CPU
- Fast, efficient program

**User sees:**
- Animated robot moving through arena
- Collecting markers
- Avoiding obstacles
- Delivering markers to corner

**The magic:**
C gives you low-level control to precisely define how data is stored (arrays, structs), how it's manipulated (functions with pointers), and how it's displayed (graphics library calls), all compiled into efficient machine code that creates a real-time animated simulation.
