# COMP0002 Robot Arena Coursework

UCL CS y1 assignment. **Target for 70+:** All stages + clean code

Build a robot that navigates a tile-based arena, collects markers, avoids obstacles.

---

## Key Definitions

**Arena:** Tile-based grid surrounded by solid walls. Tiles are square, robot moves from center to center.

**Robot:** 
- Moves forward only (to tile in front)
- Cannot move sideways or diagonally
- Turns 90° left/right only
- Can carry unlimited markers
- Can have memory (e.g., map of visited tiles)

**Tiles:**
- **Empty (0):** Robot can move here
- **Wall (1):** Arena boundary, robot cannot pass
- **Obstacle (2):** Blocks robot movement, placed inside arena
- **Marker (3):** Item robot collects, can be on any non-obstacle tile

**Directions:**
- **North:** Up on screen
- **South:** Down on screen  
- **East:** Right on screen
- **West:** Left on screen

**Drawing Layers:**
- **Background:** Walls, obstacles, markers, grid (draw ONCE at start)
- **Foreground:** Robot only (clear and redraw each move for animation)

**Command Line Arguments (argc/argv):**
```c
// ./a.out 2 3 east | java -jar drawapp-4.5.jar
int main(int argc, char **argv) {
    // argc = 4 (program name + 3 args)
    // argv[0] = "./a.out"
    // argv[1] = "2"      (convert with atoi())
    // argv[2] = "3"      (convert with atoi())
    // argv[3] = "east"   (use as string)
}
```

---

## Core Requirements

### Robot API (implement these 8 functions)
```c
void forward(...)       // Move to next tile
void left(...)          // Turn 90° left  
void right(...)         // Turn 90° right
int atMarker(...)       // Check if on marker
int canMoveForward(...) // Check if path clear
void pickUpMarker(...)  // Collect marker
void dropMarker(...)    // Drop one marker
int markerCount(...)    // Return markers held
```
*You decide the parameters - likely need robot state passed in.*

---

## Implementation Stages

**Stage 1** (50-59): Rectangle arena, wall-adjacent marker, wall-follow algorithm  
**Stage 2** (60-69): Random size arena, collect marker → corner → drop  
**Stage 3** (70+): Marker anywhere (not just walls), more complex pathfinding  
**Stage 4** (70-79): Multiple obstacles + markers, robust algorithm  
**Stage 5** (80+): Obstacles form shapes (circle), collect all markers

**IMPORTANT Stage Details:**
- **Stage 1:** Robot starts NOT next to wall, random position/direction
- **Stage 2:** Random arena size, marker next to wall, deliver to ANY corner
- **Stage 3:** Marker can be ANYWHERE inside arena
- **Stage 4:** Prevent infinite loops - algorithm must handle any obstacle configuration
- **Stage 5:** Use obstacles to create irregular arena shapes (circular open area)

---

## Constraints & Requirements

**Must Use:**
- gcc or clang compiler only
- Standard C libraries only
- graphics.h and graphics.c from Moodle (don't modify them)
- Drawing app version 4.5

**Cannot Use:**
- Platform-specific headers (e.g., sys/windows.h)
- Additional libraries or addons
- Modified graphics.h/c files

**Code Organization:**
- Use multiple .c and .h files (recommended, not required to be all in one file)
- Separate concerns (e.g., robot.c, arena.c, main.c)

**Anonymous Marking:**
- Don't include your name or student number in ANY submitted files
- Moodle handles identity after marking

**Plagiarism:**
- Clearly reference ANY copied code in comments
- Include sources from AI tools (CoPilot, ChatGPT)
- Individual work only - ask questions at Week 5 labs

---

## Technical Setup

**Key data structures:**
```c
typedef struct {
    int x, y;           // Position in grid
    char direction;     // 'N','S','E','W'
    int markers_held;   // Count of markers carried
} Robot;

int arena[MAX_Y][MAX_X];  // 0=empty, 1=wall, 2=obstacle, 3=marker
```

**Example robot control loop:**
```c
while(...) {
    if (canMoveForward(aRobot)) {
        forward(aRobot);
    }
    right(aRobot);
    sleep(500);  // Milliseconds - adjust to see movement clearly
}
```

**Drawing layers:**
- Background (draw once): walls, obstacles, markers, grid
- Foreground (redraw per move): robot only
- Use `sleep(100-300)` for visible animation (not too fast, not too slow)

**Command line args:**
```c
#include <stdlib.h>  // For atoi()

int main(int argc, char **argv) {
    // Default values if no args given
    int initialX = 0;
    int initialY = 0;
    char *initialDirection = "south";
    
    if (argc == 4) {  // Check 4 args provided (program name + 3 args)
        initialX = atoi(argv[1]);        // Convert string to int
        initialY = atoi(argv[2]);        // Convert string to int
        initialDirection = argv[3];      // Already a string
    }
    
    // Use these values to initialize robot...
}
```

**Run example:**
```bash
./a.out 2 3 east | java -jar drawapp-4.5.jar
# Robot starts at row 2, column 3, facing east
```

**Note:** Drawing programs redirect stdout to drawapp, so you cannot use scanf/printf for user input. Use command line args instead.

---

## Code Quality Rules (Critical for 70+)

**Functions:** <15 lines, one clear purpose, good parameters  
**Variables:** Descriptive names, minimal global scope  
**Comments:** Only when code can't be self-explanatory  
**Formatting:** Consistent (use VSCode formatter)  
**Compile:** `gcc -Wall -Werror` - fix ALL warnings

**Development Philosophy (from assignment):**
- **Keep things straightforward!** (repeated twice in spec for a reason)
- Straightforward ≠ trivial
- Brainstorm/sketch BEFORE coding
- Understand needed variables/functions before writing code
- Think through function call sequences
- Role-play the algorithm
- Functions should be short and cohesive
- Can't start? Do a simpler subset first, then expand

**Key Design Questions:**
- How is behavior implemented through function calls?
- Are functions calling each other logically?
- Can you explain the sequence of calls?

---

## Algorithm Strategies

**Wall following (Stage 1-2):**
```
while not at marker:
    if can_move_forward: move_forward
    else: turn_right
```

**Area coverage (Stage 3+):** Build memory map, systematic search, avoid revisits  
**Obstacles (Stage 4-5):** Backtracking, visit counters, prevent infinite loops

---

## Submission Package

**COMP0002CW1.zip** containing:
- Source files (.c, .h)
- readMe.txt (1 page: description, compile command, run command)
- Data files if needed

**DON'T include:** .o files, executables, graphics.c/h, your name/ID

**Compile:** `gcc -o robot robot.c graphics.c`  
**Run:** `./robot | java -jar drawapp-4.5.jar`

---

## Development Workflow

1. Display arena grid and walls
2. Add static robot, then basic movement
3. Implement wall following, test thoroughly
4. Add randomization (positions, size)
5. Implement marker pickup/drop
6. Add obstacles, test robustness
7. Clean code, remove debug prints
8. Write readMe

**Debug tips:**
- Run without `| drawapp` to see printf/errors
- Use `sleep()` to slow animation
- Test with fixed positions first
- Compile with `-Wall -Werror` regularly

---

## Mark Breakdown (from UCL CS Marking Criteria)

**0-39 (Fail):** Fundamental errors, won't compile, nothing significant achieved

**40-49 (Just Adequate):** Basic understanding, compiles but doesn't work properly, serious design deficiencies

**50-59 (Satisfactory - Pass):** 
- Code compiles and runs
- Implements reasonable subset of spec
- More or less right functionality
- Reasonable design using functions
- **This is default for straightforward answer to basic stages**

**60-69 (Good):**
- Good understanding, minor issues only
- Code compiles, runs, reasonable design
- Most expectations met, most stages completed

**70-79 (Very Good):**
- Clear proficiency in design and programming
- Comfortably above average
- **All stages completed**
- Very good program required

**80-89 (Excellent):**
- Really outstanding program
- Well above normal expectations
- Deep understanding in all aspects
- **All stages completed very proficiently**

**90+ (Exceptional):**
- Reserved for something exceptional
- Used sparingly, must be fully justified

**Quality Factors (all marking levels):**
- Proper declaration, definition, use of functions/variables/data structures
- Layout and presentation of source code
- Appropriate variable and function names
- Appropriate comments (add information, don't duplicate code)
- Code readable without needing comments
- Suitable design for effective solution

**"Clean straightforward and working code, making good use of functions, is considered better than longer and more complex but poorly organised code."**

**Credit given for:**
- Using C language properly
- Novelty
- Quality

---

## Development Workflow

1. Display arena grid and walls (background only)
2. Add static robot, then basic movement
3. Implement wall following, test thoroughly  
4. Add randomization (positions, arena size)
5. Implement marker pickup/drop
6. Add obstacles, test algorithm robustness
7. Clean code, remove debug statements
8. Write readMe

**Debug tips:**
- Run without `| drawapp` to see printf/errors
- Use `sleep(100-300)` to see movement clearly (not too fast/slow)
- Test with fixed positions before randomization
- Compile with `-Wall -Werror` frequently

---

## Common Mistakes to Avoid

**Code Quality:**
- Long functions (>15 lines)
- Poor variable names (`a`, `tmp`, `x1`)
- Too many global variables
- Ignoring compiler warnings
- Commented-out code in submission
- Unused variables/functions
- Comments that duplicate code

**Design:**
- All code in main() instead of using functions
- Not using structs for robot
- Not using 2D array for arena
- Redrawing background every frame (draw once only!)

**Algorithm:**
- Infinite loops with obstacles
- Robot moves too fast (no sleep or too short)
- Not handling wall/obstacle collisions

**Submission:**
- Including .o, .exe, .out files
- Including graphics.h/c files
- Wrong zip name (must be COMP0002CW1.zip)
- Including name/student number in files
- No readMe or incomplete readMe

---

## Quick Reference

**Drawing (graphics.h):**
```c
setColour(red/green/blue);
drawLine(x1, y1, x2, y2);
fillRect(x, y, w, h);
fillPolygon(3, xs, ys);  // Triangle for robot
background(); foreground(); clear();
```

**Useful functions:**
```c
#include <stdlib.h>
atoi(str);              // String to int
rand() % N;             // Random 0 to N-1
srand(time(NULL));      // Seed random

#include <unistd.h>  
sleep(milliseconds);    // Pause animation
```

---

## Final Checklist

- [ ] Compiles with `-Wall -Werror` (no warnings)
- [ ] Stages 3+4 complete (for 70+)
- [ ] Functions <15 lines, cohesive
- [ ] Variables properly named and scoped
- [ ] No commented-out code
- [ ] No unused variables/functions
- [ ] readMe included (1 page max)
- [ ] Correct zip name: COMP0002CW1.zip
- [ ] No .o/.exe/.out files in zip
- [ ] No name/ID in files
- [ ] Tested multiple arena configs
- [ ] Background drawn once only
- [ ] Visible animation (proper sleep)

**Keep it straightforward. Clean, working code beats complex, messy code.**
