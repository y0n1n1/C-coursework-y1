# Stage 3 Implementation Plan

## The Challenge

### What Changes from Stage 2:
**Stage 2:** Marker placed **next to wall** → Simple wall-following works ✓
**Stage 3:** Marker placed **anywhere inside arena** → Wall-following FAILS ✗

### Why Wall-Following Fails:
- Wall-following only covers perimeter tiles
- Marker could be in the center (never touched by wall-following)
- Need **systematic search** of entire arena interior

### Core Requirements (from assignment):
1. **Random marker placement** - Anywhere inside arena (not just walls)
2. **Robot finds marker** - Must search interior, not just perimeter
3. **Pick up marker** - Using pickUpMarker()
4. **Navigate to corner** - Can use wall-following after pickup
5. **Drop marker** - Using dropMarker()
6. **More complex algorithm** - Required by specification

---

## Algorithm Selection

### Options Considered:

**Option A - Random Walk**
- ❌ Could loop forever
- ❌ Not guaranteed to find marker
- ❌ Inefficient

**Option B - Breadth-First Search (BFS)**
- ✓ Guaranteed to find marker
- ❌ Complex to implement
- ❌ Hard to keep functions <15 lines
- ❌ Requires queue data structure

**Option C - Depth-First Search (DFS)**
- ✓ Guaranteed to find marker
- ❌ Complex recursion
- ❌ Stack management
- ❌ Hard to keep functions short

**Option D - Systematic Row-by-Row Search (Lawnmower Pattern)** ⭐ RECOMMENDED
- ✓ Guaranteed to find marker
- ✓ Simple to understand
- ✓ Easy to implement in short functions
- ✓ Fits "straightforward" philosophy
- ✓ No complex data structures needed
- ✓ Predictable, testable behavior

### Selected Algorithm: **Row-by-Row Search (Lawnmower Pattern)**

---

## Lawnmower Algorithm Explained

### Visual Pattern:
```
Start → → → → → → → Turn
        ↓           ↓
Turn ← ← ← ← ← ← ← ←
↓                   ↓
→ → → → → → → → → Turn
        ↓           ↓
Turn ← ← ← ← ← ← ← ←
        ... continue until marker found
```

### How It Works:
1. **Row 1:** Move across entire row (left to right or right to left)
2. **Turn:** Move down one row, turn 180°
3. **Row 2:** Move across entire row (opposite direction)
4. **Repeat:** Continue until marker found

### Key Properties:
- **Coverage:** Every interior tile visited exactly once
- **Guaranteed:** Will find marker no matter where it is
- **Simple:** Just forward movement + turns at edges
- **Efficient:** O(width × height) - visits every tile once

---

## Implementation Strategy

### Phase 1: Visited Tile Tracking

**Why Needed:**
- Robot needs to know which tiles already checked
- Prevents redundant searching
- Enables systematic coverage

**Data Structure:**
```c
int visited[MAX_ARENA_SIZE][MAX_ARENA_SIZE];
// 0 = not visited, 1 = visited
```

**Functions Needed:**
- `initVisited()` - Set all to 0
- `markVisited()` - Mark current tile as visited
- `isVisited()` - Check if tile already visited

### Phase 2: Lawnmower Navigation

**Core Functions:**
- `moveToStartRow()` - Position robot at row start (left or right edge)
- `sweepRow()` - Move across entire row, mark tiles visited
- `moveToNextRow()` - Go down one row, prepare for reverse sweep
- `isRowComplete()` - Check if reached row end

**Algorithm Loop:**
```c
1. Initialize visited array
2. While (!atMarker):
     a. Sweep current row (moving east or west)
     b. Check each tile for marker
     c. Mark each tile as visited
     d. If reached row end:
        - Move down one row
        - Turn around (reverse direction)
```

### Phase 3: Direction Management

**Challenge:** Robot alternates between moving east and west

**Solution:**
- Track current sweep direction (east vs west)
- `sweepEast()` - move right across row
- `sweepWest()` - move left across row
- `turnAround()` - 180° turn (two 90° turns)

---

## Detailed Pseudocode

```c
void systematicSearch(Robot *robot, int arena[][], int visited[][], int w, int h) {
    initVisited(visited, w, h);

    // Start at top-left interior corner
    moveToPosition(robot, arena, 1, 1);
    robot->direction = 'E';  // Start moving east

    int sweepingEast = 1;
    int row = 1;

    while (!atMarker(robot, arena)) {
        if (sweepingEast) {
            sweepRowEast(robot, arena, visited, w);
        } else {
            sweepRowWest(robot, arena, visited, w);
        }

        row++;
        if (row < h - 1) {  // Not at bottom yet
            moveDown(robot, arena);
            turnAround(robot);
            sweepingEast = !sweepingEast;
        } else {
            break;  // Searched entire arena
        }
    }
}

void sweepRowEast(Robot *robot, int arena[][], int visited[][], int w) {
    while (robot->x < w - 2) {  // Until right wall
        markVisited(visited, robot->x, robot->y);
        if (atMarker(robot, arena)) return;
        forward(robot, arena);
        drawRobot(robot);
        sleep(ANIMATION_DELAY);
    }
    markVisited(visited, robot->x, robot->y);
}

void sweepRowWest(Robot *robot, int arena[][], int visited[][], int w) {
    while (robot->x > 1) {  // Until left wall
        markVisited(visited, robot->x, robot->y);
        if (atMarker(robot, arena)) return;
        forward(robot, arena);
        drawRobot(robot);
        sleep(ANIMATION_DELAY);
    }
    markVisited(visited, robot->x, robot->y);
}

void moveDown(Robot *robot, int arena[][]) {
    // Turn to face south
    while (robot->direction != 'S') {
        right(robot);
    }
    forward(robot, arena);
}

void turnAround(Robot *robot) {
    right(robot);
    right(robot);
}
```

---

## Function Organization

### Keeping Functions Under 15 Lines:

**Visited Array Management (3 functions):**
- `initVisited()` - 7 lines
- `markVisited()` - 2 lines
- `isVisited()` - 2 lines

**Navigation Helpers (4 functions):**
- `moveToPosition()` - 10 lines (move to specific x,y)
- `turnToDirection()` - 8 lines (turn to face N/S/E/W)
- `moveDown()` - 6 lines (go down one row)
- `turnAround()` - 3 lines (180° turn)

**Lawnmower Search (3 functions):**
- `sweepRowEast()` - 12 lines
- `sweepRowWest()` - 12 lines
- `systematicSearch()` - 14 lines (orchestrates search)

**Main Workflow Update:**
- `findAndPickupMarkerAnywhere()` - 8 lines (replaces findAndPickupMarker)

**Total New Functions:** ~10
**All Functions:** <15 lines ✓

---

## Code Quality Considerations

### Maintaining Standards:

**Function Length:**
- All new functions <15 lines
- Break complex loops into helpers
- Use early returns for clarity

**No Duplication:**
- `sweepRowEast()` and `sweepRowWest()` are similar but needed
- Could abstract further, but might hurt readability
- Trade-off: slight duplication vs clarity

**Naming:**
- `systematicSearch()` - clear purpose
- `sweepRow()` - describes action
- `markVisited()` - verb + noun pattern
- `turnAround()` - action-oriented

**Comments:**
- Document lawnmower pattern concept
- Explain direction alternation
- Comment visited array purpose

---

## Implementation Checklist

### Phase 1: Visited Tracking
- [ ] Add visited array to main()
- [ ] Implement initVisited()
- [ ] Implement markVisited()
- [ ] Implement isVisited() (may not need for basic version)
- [ ] Test visited tracking separately

### Phase 2: Helper Functions
- [ ] Implement turnToDirection()
- [ ] Implement moveToPosition()
- [ ] Implement moveDown()
- [ ] Implement turnAround()
- [ ] Test navigation helpers

### Phase 3: Lawnmower Search
- [ ] Implement sweepRowEast()
- [ ] Implement sweepRowWest()
- [ ] Implement systematicSearch()
- [ ] Test with marker in various positions

### Phase 4: Integration
- [ ] Update runSimulation() for Stage 3
- [ ] Add mode selection (Stage 2 vs Stage 3)
- [ ] Test full workflow: search → pickup → corner → drop
- [ ] Verify all functions <15 lines

### Phase 5: Polish
- [ ] Ensure zero warnings with -Wall -Werror
- [ ] Add strategic comments
- [ ] Update readme.md
- [ ] Update CLAUDE.md
- [ ] Update metrics.txt

---

## Potential Challenges & Solutions

### Challenge 1: Robot Starting Position
**Problem:** Robot could start anywhere, need to get to starting row
**Solution:** `moveToPosition(robot, arena, 1, 1)` to reach top-left before search

### Challenge 2: Function Length for Search Loop
**Problem:** Main search loop could exceed 15 lines
**Solution:** Break into `systematicSearch()` (orchestration) and `sweepRow()` helpers

### Challenge 3: Duplication Between sweepRowEast/West
**Problem:** Very similar code
**Solution:** Accept controlled duplication for clarity (still DRY at high level)

### Challenge 4: Visited Array Initialization
**Problem:** Large array to initialize
**Solution:** Simple nested loop in `initVisited()`, keep function focused

### Challenge 5: Animation Speed
**Problem:** Searching entire arena could be slow
**Solution:** Keep ANIMATION_DELAY at 150ms (already faster than Stage 2)

### Challenge 6: Edge Case - Marker at Start Position
**Problem:** Robot might start on marker
**Solution:** Check `atMarker()` before starting search loop

---

## Alternative Approaches (If Time Permits)

### Optimization: Skip Visited Rows
- Add intelligence to skip rows already fully searched
- Probably not needed for arena sizes 12-18

### Optimization: Spiral Pattern
- Start from edges, spiral inward
- More complex, not worth the effort for this stage

### Enhancement: Visual Feedback
- Draw visited tiles differently (lighter shade?)
- Would need background layer updates
- Probably too complex for Stage 3

---

## Testing Strategy

### Test Cases:
1. **Marker at (1, 1)** - Top-left corner (found immediately)
2. **Marker at (w-2, h-2)** - Bottom-right (worst case, searches entire arena)
3. **Marker at (w/2, h/2)** - Center of arena
4. **Multiple runs** - Verify randomization works

### Success Criteria:
- ✓ Finds marker anywhere in arena
- ✓ No infinite loops
- ✓ Smooth animation
- ✓ Picks up and delivers to corner
- ✓ All functions <15 lines
- ✓ Zero compiler warnings
- ✓ Code quality maintained

---

## Expected Outcomes

### Metrics Impact:
- **Functions:** 32 → ~42 (+10 new functions)
- **LOC:** 372 → ~480 (+~110 lines for search logic)
- **Function compliance:** 100% maintained
- **Complexity:** Slight increase (acceptable)

### Grade Impact:
- Stage 3 completion required for 70+
- Clean implementation → 70-75 range
- Excellent code quality → 75-80 range

---

## Ready to Implement!

**Strategy:**
1. Start simple: get basic lawnmower working
2. Test thoroughly with various marker positions
3. Refactor to keep functions short
4. Polish and document
5. Maintain code quality standards

**Key Principle:** "Keep it straightforward" - even though algorithm is more complex, implementation should be clean and understandable.

The lawnmower pattern is perfect because it's:
- ✅ Straightforward to understand
- ✅ Straightforward to implement
- ✅ Straightforward to test
- ✅ Guaranteed to work

Let's build Stage 3!
