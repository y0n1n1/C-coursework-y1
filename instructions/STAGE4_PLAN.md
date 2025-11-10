# Stage 4 Implementation Plan - Obstacles & Multiple Markers

## The Challenge

### What Changes from Stage 3:
**Stage 3:** Single marker, no obstacles → Lawnmower pattern works ✓
**Stage 4:** Multiple markers + obstacles → Lawnmower FAILS ✗

### Why Lawnmower Fails with Obstacles:
- Lawnmower assumes clear path across each row
- Obstacles block robot's path
- Robot gets stuck when obstacle in sweep path
- No backtracking or path planning

### Core Requirements (from assignment):
1. **Multiple obstacles** - Placed randomly inside arena
2. **Multiple markers** - Placed randomly (not on obstacles)
3. **Find ALL markers** - Robot must collect every marker before corner
4. **Resilient algorithm** - NO infinite loops regardless of obstacle placement
5. **Pick up all markers** - Using pickUpMarker() for each
6. **Navigate to corner** - After collecting all markers
7. **Drop all markers** - Using dropMarker() at corner

---

## Critical Design Decisions

### Decision 1: How Many Obstacles/Markers?

**Assignment states:** "one or more obstacles and markers"

**Proposed Configuration:**
- **Obstacles:** 3-8 random (scales with arena size: ~5% of interior tiles)
- **Markers:** 2-4 random (enough to test multi-marker collection)
- **Justification:**
  - Tests algorithm resilience with significant obstacles
  - Multiple markers ensure collection logic works
  - Still leaves enough open space for pathfinding

### Decision 2: Pathfinding Algorithm Selection

**Options Considered:**

| Algorithm | Shortest Path? | Handles Obstacles? | Complexity | Function Length |
|-----------|---------------|-------------------|------------|-----------------|
| **Random Walk** | ❌ No | ❌ Gets stuck | Low | ✓ Short |
| **Wall Following** | ❌ No | ❌ Fails with obstacles | Low | ✓ Short |
| **Modified Lawnmower** | ❌ No | ⚠️ Needs backtracking | Medium | ⚠️ Long |
| **DFS (Depth-First)** | ❌ No | ✓ Yes | Medium | ⚠️ Recursion issues |
| **BFS (Breadth-First)** | ✓ Yes | ✓ Yes | Medium | ✓ Can be short |
| **A\*** | ✓ Yes | ✓ Yes | High | ❌ Too complex |

**SELECTED: Breadth-First Search (BFS)**

**Why BFS?**
✓ Guaranteed to find path if one exists
✓ Finds shortest path (optimal)
✓ Handles arbitrary obstacle configurations
✓ Well-understood, straightforward algorithm
✓ Can be decomposed into <15 line functions
✓ Natural infinite-loop prevention (visited tracking)

**Why NOT A\*?**
❌ More complex than needed
❌ Harder to keep functions <15 lines
❌ Heuristic calculation adds complexity
❌ BFS is sufficient for small arenas (12-18 tiles)

### Decision 3: Code Organization

**New Module: pathfinding.c/h**

**Rationale:**
- Separates pathfinding logic from robot control
- Keeps main.c focused on high-level workflow
- Allows pathfinding to be reused/tested independently
- Follows single-responsibility principle

**Module Responsibilities:**

```
pathfinding.c/h:
├── BFS pathfinding algorithm
├── Path representation and storage
├── Queue implementation for BFS
└── Bridge: findPath() returns path, main.c executes it

main.c:
├── High-level workflow (collect markers → corner)
├── Path execution (followPath)
├── Marker collection orchestration
└── Robot control coordination

arena.c:
├── Obstacle placement
├── Multiple marker placement
└── Drawing updates

robot.c:
└── (unchanged - API stable)
```

### Decision 4: Path Representation

**Option A: Array of Directions**
```c
char path[] = {'E', 'E', 'S', 'E', 'N'};
```
✓ Simple to follow
✓ Direct mapping to robot API (forward after turning)
❌ Less flexible

**Option B: Array of Coordinates**
```c
struct {int x, y} path[] = {{2,1}, {3,1}, {3,2}};
```
✓ More flexible
✓ Can validate each step
❌ Requires conversion to directions

**Option C: Path Struct with Both**
```c
typedef struct {
    int x[MAX_PATH_LENGTH];
    int y[MAX_PATH_LENGTH];
    int length;
} Path;
```
✓ Complete path information
✓ Easy to pass as single parameter
✓ Reduces parameter overhead
✓ Can calculate directions from coordinates

**SELECTED: Option C - Path Struct**

---

## Architecture Design

### File Structure

```
program/
├── main.c          (workflow orchestration)
├── robot.c/h       (8 API functions - unchanged)
├── arena.c/h       (arena setup, obstacles, markers, drawing)
├── pathfinding.c/h (BFS algorithm, path finding) ← NEW
└── graphics.c/h    (drawing library - unchanged)
```

### pathfinding.h Interface

```c
#define MAX_PATH_LENGTH 200

typedef struct {
    int x[MAX_PATH_LENGTH];
    int y[MAX_PATH_LENGTH];
    int length;
} Path;

/* Main pathfinding function - finds shortest path using BFS */
int findPath(int arena[][MAX_ARENA_SIZE], int startX, int startY,
             int endX, int endY, Path *path);

/* Returns 1 if path found, 0 if no path exists */
```

**Key Design Choice:** Single function interface
- Keeps external API simple
- All BFS complexity hidden inside pathfinding.c
- Helper functions marked `static` (not exposed)

---

## BFS Algorithm Implementation

### BFS Pseudocode

```c
findPath(arena, startX, startY, endX, endY, path):
    1. Create queue, visited array
    2. Enqueue start position with parent=NULL
    3. Mark start as visited

    4. While queue not empty:
        a. Dequeue current position
        b. If current == end:
            - Reconstruct path from parents
            - Return success
        c. For each neighbor (N, S, E, W):
            - If valid && not visited && not obstacle:
                - Mark visited
                - Set parent
                - Enqueue neighbor

    5. Queue empty, no path found
    6. Return failure
```

### Function Decomposition (<15 lines each)

**pathfinding.c functions:**

1. **`findPath()`** - 12 lines
   - Initialize BFS structures
   - Call bfsSearch()
   - If found, call reconstructPath()
   - Return result

2. **`bfsSearch()`** - 14 lines (static)
   - Main BFS loop
   - Dequeue, check goal, enqueue neighbors
   - Returns 1 if found, 0 if not

3. **`enqueueNeighbors()`** - 13 lines (static)
   - Check all 4 directions
   - Validate and enqueue valid neighbors
   - Mark as visited

4. **`isValidMove()`** - 4 lines (static)
   - Check bounds, obstacles, walls, visited
   - Returns boolean

5. **`reconstructPath()`** - 12 lines (static)
   - Follow parent pointers from goal to start
   - Reverse to get start→goal path
   - Fill Path struct

6. **Queue helpers:** (3-5 lines each, static)
   - `initQueue()`
   - `enqueue()`
   - `dequeue()`
   - `isQueueEmpty()`

**Total:** ~7 functions, all <15 lines, most marked `static`

---

## main.c Workflow Changes

### Stage 4 Workflow (CORRECTED - Authentic Exploration)

**CRITICAL ISSUE IDENTIFIED:**
The robot should NOT scan the arena array to find markers - that's "cheating"!
The robot must EXPLORE to DISCOVER markers through actual movement.

**Corrected Workflow:**

```
1. Setup arena with obstacles and markers
2. EXPLORATION PHASE:
    a. Use modified lawnmower/BFS exploration
    b. When atMarker() returns true:
        - Store marker location in discovered list
        - Continue exploring
    c. Explore until all accessible tiles visited OR timeout
3. COLLECTION PHASE:
    a. For each discovered marker location:
        - BFS pathfind to marker
        - Follow path
        - Pick up marker (use pickUpMarker())
        - Update display
4. DELIVERY PHASE:
    a. BFS pathfind to corner
    b. Follow path to corner
    c. Drop all markers (use dropMarker())
    d. Stop
```

**Key Difference:**
- ❌ OLD: `findClosestMarker()` scans entire arena array (omniscient knowledge)
- ✓ NEW: Robot explores arena, discovers markers through `atMarker()`, stores locations

This is more authentic and respects the "finding markers" requirement.

### New/Modified Functions in main.c (CORRECTED)

**New Data Structure:**
```c
typedef struct {
    int x[MAX_MARKERS];  // Max 10 markers
    int y[MAX_MARKERS];
    int count;
} MarkerList;
```

**New Functions:**

1. **`exploreAndDiscover()`** - 14 lines
   - Explores arena systematically (BFS-based exploration)
   - When atMarker() == true, stores location in MarkerList
   - Returns discovered markers

2. **`storeMarkerLocation()`** - 6 lines
   - Adds marker location to MarkerList
   - Prevents duplicates

3. **`followPath()`** - 10 lines
   - Takes Path struct, executes each step
   - Turns robot to face direction
   - Moves forward
   - Updates display

4. **`collectDiscoveredMarkers()`** - 14 lines
   - For each marker in discovered list:
     - BFS pathfind to it
     - Follow path
     - Pick up marker
   - Uses actual pathfinding, not omniscient scanning

**Modified Functions:**

1. **`runSimulation()`** - 5 lines
   - Phase 1: exploreAndDiscover()
   - Phase 2: collectDiscoveredMarkers()
   - Phase 3: deliverToCorner()

2. **`collectAllMarkers()`** - REMOVED
   - Replaced by exploreAndDiscover() + collectDiscoveredMarkers()

3. **`findClosestMarker()`** - REMOVED
   - Was "cheating" by scanning arena array

---

## Exploration Strategy

### How Robot Discovers Markers

**Option 1: BFS Exploration (RECOMMENDED)**
- Start from robot position
- Use BFS to visit every accessible tile
- At each tile: check `atMarker(robot, arena)`
- If true: store (robot->x, robot->y) in MarkerList
- Continue until all accessible tiles visited

**Advantages:**
✓ Guaranteed to visit all accessible tiles
✓ Systematic coverage
✓ Works with obstacles (BFS naturally navigates around them)
✓ No backtracking issues
✓ Combines pathfinding with exploration

**Implementation:**
```c
void exploreAndDiscover(Robot *robot, int arena[][], MarkerList *markers, int w, int h) {
    // Use BFS to explore all accessible tiles
    // When atMarker() returns true, store location
    // Continue exploring until visited all accessible tiles
}
```

**Option 2: Modified Lawnmower (Alternative)**
- Use Stage 3 lawnmower pattern
- When blocked by obstacle: use BFS to go around
- Store marker locations when discovered
- More complex, less elegant

**SELECTED: BFS Exploration**

---

## Infinite Loop Prevention

### Challenge from Assignment:
> "The robot algorithm should be as resilient as possible, such that the robot cannot get stuck in a never-ending cycle of movement due to the way that obstacles are positioned."

### Prevention Strategies:

**1. Pathfinding-Level Prevention:**
```c
// BFS naturally prevents loops:
if (visited[y][x]) continue;  // Never revisit tiles
if (queue_full) return 0;      // Fail gracefully
```

**2. Execution-Level Prevention:**
```c
#define MAX_MOVES 1000

int moveCount = 0;
while (collecting_markers) {
    if (++moveCount > MAX_MOVES) {
        // Give up, exit gracefully
        break;
    }
    // ... execute path
}
```

**3. Path Validation:**
```c
if (!findPath(arena, robotX, robotY, markerX, markerY, &path)) {
    // No path exists - skip this marker or try alternative
    continue;
}
```

**4. Obstacle Detection:**
```c
// In arena setup:
if (obstacle_blocks_all_markers) {
    // Regenerate arena or adjust placement
    placeObstacles(...);
}
```

**Strategy:** Multi-layer defense
- BFS prevents loops in pathfinding
- Move counter prevents infinite execution
- Path validation prevents attempting impossible paths
- Graceful failure (return to corner) if stuck

---

## Parameter Overhead Management

### Goal: Maintain avg <3.5 params (target <3.0)

**Path Struct Benefit:**

Before:
```c
void navigate(Robot *robot, int arena[][], int path[], int length, int w, int h);
// 6 parameters!
```

After:
```c
void followPath(Robot *robot, int arena[][], Path *path);
// 3 parameters ✓
```

**Savings:** 3 fewer params by bundling path data into struct

### Projected Parameter Counts:

**pathfinding.c:**
- findPath: 6 params (arena, start x/y, end x/y, path) - necessary
- Helper functions: 3-4 params (mostly static, internal)

**main.c:**
- followPath: 3 params
- findClosestMarker: 4 params
- collectAllMarkers: 4 params

**Estimated avg:** ~3.2 params/function (within target ✓)

---

## arena.c Updates

### New Functions Needed:

1. **`placeObstacles()`** - 11 lines
   - Place N random obstacles (N based on arena size)
   - Avoid walls, robot start, markers
   - Check each placement is valid

2. **`placeMultipleMarkers()`** - 12 lines
   - Place M random markers (2-4)
   - Avoid walls, obstacles, robot start
   - Store marker count

3. **`countMarkers()`** - 8 lines
   - Scan arena, count markers remaining
   - Used to know when all collected

### Modified Functions:

1. **`setupGame()`** - Update to call placeObstacles()
2. **`drawObstacles()`** - Already exists from Stage 3 ✓

---

## Testing Strategy

### Test Cases for Resilience:

**Test 1: Obstacle Corridor**
```
###############
#M...........M#
#..#########..#
#..R.........C#
###############
```
Robot must navigate around obstacle wall

**Test 2: Obstacle Maze**
```
###############
#M.#.#.#.#.#M#
#.#.#.#.#.#.#.#
#R...........C#
###############
```
Robot must find winding path

**Test 3: Blocked Marker**
```
###############
#M...#####...C#
#....#M#M#....#
#....#####...R#
###############
```
One marker unreachable - should skip gracefully

**Test 4: Scattered Markers**
```
###############
#M.#...#...#M#
#..#...#...#.#
#M.......R..C#
###############
```
Multiple markers, multiple obstacles

### Success Criteria:
✓ Finds and collects all reachable markers
✓ Never gets stuck in infinite loop
✓ Handles unreachable markers gracefully
✓ Completes within reasonable move limit
✓ All functions <15 lines
✓ Zero compiler warnings

---

## Implementation Checklist

### Phase 1: Pathfinding Module
- [ ] Create pathfinding.h with Path struct
- [ ] Create pathfinding.c stub
- [ ] Implement queue data structure (simple array-based)
- [ ] Implement isValidMove() helper
- [ ] Implement BFS main loop (bfsSearch)
- [ ] Implement reconstructPath()
- [ ] Implement findPath() interface
- [ ] Test pathfinding in isolation (print paths)

### Phase 2: Arena Updates
- [ ] Implement placeObstacles() in arena.c
- [ ] Implement placeMultipleMarkers() in arena.c
- [ ] Implement countMarkers() in arena.c
- [ ] Update setupGame() to place obstacles/markers
- [ ] Test arena generation (check obstacles/markers placed correctly)

### Phase 3: Main Workflow Integration
- [ ] Implement followPath() in main.c
- [ ] Implement findClosestMarker() in main.c
- [ ] Implement collectAllMarkers() in main.c
- [ ] Update runSimulation() for Stage 4
- [ ] Remove/deprecate systematicSearch() (Stage 3 algorithm)
- [ ] Add move counter for infinite loop prevention

### Phase 4: Testing & Debugging
- [ ] Test with single marker, single obstacle
- [ ] Test with multiple markers, multiple obstacles
- [ ] Test with blocked paths
- [ ] Test infinite loop prevention (max moves)
- [ ] Verify all functions <15 lines
- [ ] Check parameter overhead (<3.5 avg)

### Phase 5: Polish & Documentation
- [ ] Compile with -Wall -Werror (zero warnings)
- [ ] Update readme.md for Stage 4
- [ ] Update CLAUDE.md marking Stage 4 complete
- [ ] Update metrics.txt with Stage 4 analysis
- [ ] Add comments to pathfinding code

---

## Potential Challenges & Solutions

### Challenge 1: Queue Implementation in C
**Problem:** BFS needs queue, C has no built-in queue
**Solution:** Simple circular array-based queue
```c
typedef struct {
    int x[MAX_QUEUE_SIZE];
    int y[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;
```
Keep functions simple: enqueue, dequeue, isEmpty

### Challenge 2: Parent Tracking for Path Reconstruction
**Problem:** Need to remember path from start to goal
**Solution:** 2D parent array
```c
int parentX[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
int parentY[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
```
Each cell stores coordinates of cell that led to it

### Challenge 3: Function Length with BFS
**Problem:** BFS main loop could exceed 15 lines
**Solution:** Break into helpers:
- Main loop in bfsSearch() (14 lines)
- Neighbor processing in enqueueNeighbors() (13 lines)
- Validation in isValidMove() (4 lines)

### Challenge 4: Path Following vs Direct Control
**Problem:** followPath() needs to navigate precisely
**Solution:** Use existing robot API + turnToDirection()
```c
for each step in path:
    calculate direction needed
    turnToDirection(robot, direction)
    forward(robot, arena)
    draw and sleep
```

### Challenge 5: Multiple Markers Tracking
**Problem:** Need to know when all markers collected
**Solution:**
- Count markers in arena at start
- Decrement when picked up
- Or: count remaining markers each loop

### Challenge 6: Parameter Overhead with Pathfinding
**Problem:** findPath needs many params (6 total)
**Solution:** Acceptable - it's THE pathfinding function
- All params necessary (start, end, arena, output)
- Hidden helpers have fewer params
- Average kept low by other functions

---

## Expected Outcomes

### Metrics Impact:

**Functions:**
- Stage 3: 37 functions
- Stage 4: ~45 functions (+8 pathfinding, +3 main, +3 arena)
- **All functions <15 lines ✓**

**Lines of Code:**
- Stage 3: 430 LOC
- Stage 4: ~580 LOC (+150 for pathfinding + workflow)
- pathfinding.c: ~120 lines
- main.c: +40 lines
- arena.c: +30 lines

**Parameter Overhead:**
- Stage 3: 3.07 avg
- Stage 4 target: <3.5 avg
- Path struct helps keep this low

**Complexity:**
- Significant increase (BFS algorithm)
- But: well-decomposed, each function simple
- Clear separation of concerns

### Grade Impact:

**Stage 4 completion required for 70-79 range**
- Clean implementation → 70-75
- Excellent resilience + code quality → 75-79
- All functions <15 lines, zero warnings → 78+

---

## Alternative Approaches (If Needed)

### If BFS Too Complex:

**Fallback Option: Enhanced Lawnmower**
- Keep Stage 3 lawnmower
- Add obstacle avoidance (turn when blocked)
- Add backtracking (return if stuck)
- Pro: Simpler, reuses code
- Con: Not guaranteed to find all markers

**Hybrid Approach:**
- Use lawnmower for exploration
- Use BFS only when stuck
- Pro: Best of both
- Con: Complex coordination

### If Function Length Issues:

**More Aggressive Decomposition:**
- Break BFS into even more helpers
- Might sacrifice some readability
- But: maintains <15 line requirement

---

## Key Principle: "Resilient by Design"

The assignment emphasizes resilience. Our approach achieves this through:

1. **BFS guarantees:** If path exists, we find it
2. **Visited tracking:** Never revisit tiles (no loops in pathfinding)
3. **Move limits:** Timeout prevents infinite execution
4. **Graceful failure:** If marker unreachable, continue with others
5. **Path validation:** Check path exists before attempting
6. **Modular design:** Pathfinding isolated, easy to debug/test

**Assignment quote:**
> "The robot algorithm should be as resilient as possible, such that the robot cannot get stuck in a never-ending cycle"

Our BFS approach + move counter + path validation = **mathematically impossible to infinite loop** ✓

---

## Ready to Implement!

**Implementation Order:**
1. Pathfinding module (self-contained, testable)
2. Arena updates (obstacles + markers)
3. Main workflow (integrate pathfinding)
4. Testing (resilience verification)
5. Polish (docs, metrics)

**Success Criteria:**
- ✅ Collects all reachable markers
- ✅ Never infinite loops
- ✅ All functions <15 lines
- ✅ Parameter overhead <3.5
- ✅ Zero warnings
- ✅ Clean, maintainable code
- ✅ 70+ grade target maintained

Let's build Stage 4!
