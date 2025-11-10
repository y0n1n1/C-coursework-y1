# Stage 2 Implementation Plan

## What We Need to Achieve

### Core Requirements (from assignment):
1. **Random arena size** - Create rectangular arena of random dimensions (not fixed 12x12)
2. **Marker placement** - Place marker at random position next to a wall
3. **Robot initialization** - Random starting position and direction inside arena
4. **Find marker** - Robot navigates and finds the marker
5. **Pick up marker** - Robot picks up the marker using pickUpMarker()
6. **Navigate to corner** - Robot goes to ANY corner (doesn't have to be nearest)
7. **Drop marker** - Robot drops marker at corner using dropMarker()
8. **Stop** - Robot stops after completing task

### What Changes from Stage 1:
- Arena size: Fixed 12x12 → Random size (e.g., 8-15 width/height)
- Robot behavior: Find marker and stop → Find marker, pick up, deliver to corner, drop, stop
- Need corner detection logic
- Need navigation strategy to reach a corner after picking up marker

---

## Code Quality Goals to Maintain

From metrics.txt (current: 9.6/10 overall):

### Critical Constraints:
✓ **Function Length**: ALL functions must be <15 lines (100% compliance)
✓ **Compilation**: Zero warnings with -Wall -Werror
✓ **Naming**: No abbreviations, descriptive names (10/10)
✓ **Documentation**: Strategic comments for algorithms (8/10)
✓ **Modularity**: Keep clean separation (main/robot/arena)
✓ **No Duplication**: Use helper functions

### Target Grade: 70+ (Very Good)
- Stage 2 completion is required for 60-69 range
- Clean implementation pushes toward 70+

---

## Technical Considerations

### 1. Random Arena Size
**Challenge**: Currently hardcoded as `int arena[12][12]` and `ARENA_WIDTH/HEIGHT = 12`

**Solution Options**:
- Keep reasonable range: 8-15 tiles (too small = trivial, too large = slow)
- Update ARENA_WIDTH and ARENA_HEIGHT to variables or keep max size
- Arena must remain rectangular with solid wall border

**Implementation**:
- Define MAX_ARENA_SIZE (e.g., 20) for array allocation
- Use actual width/height variables for logic
- Randomize width/height at initialization

### 2. Marker Pickup Logic
**Already Implemented**: pickUpMarker() in robot.c
- Checks if robot is on marker tile
- Removes marker from arena
- Increments robot->markers_held

**Integration**: Call pickUpMarker() when atMarker() returns true

### 3. Corner Detection
**What is a corner?**
- Position (1, 1) - top-left interior corner
- Position (width-2, 1) - top-right interior corner
- Position (1, height-2) - bottom-left interior corner
- Position (width-2, height-2) - bottom-right interior corner

**Function needed**: `int atCorner(Robot *robot, int width, int height)`

### 4. Navigation to Corner
**Strategy Options**:

**Option A - Simple Wall Following** (easiest, Stage 2 compliant):
- After picking up marker, continue wall-following
- Wall-following naturally reaches corners
- Stop when atCorner() and carrying marker

**Option B - Direct Navigation** (more complex, overkill for Stage 2):
- Calculate path to nearest corner
- Requires more sophisticated algorithm
- Not necessary for "go to any corner" requirement

**Recommendation**: Option A - Keep wall-following, just add corner detection

### 5. Updated Algorithm Flow

```
Stage 1: Find marker → Stop
Stage 2: Find marker → Pick up → Find corner → Drop → Stop

Pseudocode:
1. Initialize random-sized arena
2. Place marker randomly next to wall
3. Initialize robot at random position (not on wall)
4. findWall() - move forward until hitting wall
5. LOOP: wall-following until atMarker()
6.   pickUpMarker()
7. LOOP: wall-following until atCorner()
8.   dropMarker()
9.   STOP
```

---

## Implementation Checklist

### Phase 1: Make Arena Size Dynamic
- [ ] Change arena initialization to accept width/height parameters
- [ ] Update all functions to use width/height instead of constants
- [ ] Test with fixed size (e.g., 10x10) before randomization
- [ ] Add random size generation (8-15 range)

### Phase 2: Add Corner Detection
- [ ] Implement atCorner() function
- [ ] Add to robot.h or create new helper in main.c
- [ ] Test corner detection logic

### Phase 3: Update Main Algorithm
- [ ] Modify runSimulation() to handle full workflow
- [ ] Add pickup logic after finding marker
- [ ] Add delivery logic to reach corner
- [ ] Add drop logic at corner

### Phase 4: Code Quality
- [ ] Ensure all functions <15 lines
- [ ] Add comments for new algorithms
- [ ] Compile with -Wall -Werror
- [ ] Test multiple random configurations
- [ ] Update metrics.txt

---

## Potential Pitfalls to Avoid

1. **Array bounds**: Must handle dynamic sizes correctly in all functions
2. **Function length**: Corner detection might need helper functions
3. **Infinite loops**: Ensure corner is reachable via wall-following
4. **Marker persistence**: Don't redraw marker on background after pickup
5. **Arena too small**: Minimum size should allow robot to move (min 4x4 interior)

---

## Files That Need Changes

### arena.h
- Make ARENA_WIDTH/HEIGHT configurable or use MAX_SIZE
- Update function signatures if needed

### arena.c
- initArena() - accept width/height parameters
- placeRandomMarker() - use dynamic size
- initRobot() - use dynamic size
- drawBackground() - use dynamic size
- drawRobot() - unchanged

### main.c
- Add atCorner() helper function
- Update setupGame() for random size
- Update runSimulation() for full workflow
- Keep functions under 15 lines

### robot.c/h
- No changes needed (API already complete)

---

## Success Criteria

✓ Arena size is random (different each run)
✓ Marker placed randomly next to wall
✓ Robot finds and picks up marker
✓ Robot navigates to any corner
✓ Robot drops marker at corner
✓ Robot stops after dropping
✓ Compiles with zero warnings
✓ All functions <15 lines
✓ Code quality maintained (9.5+ overall)

---

## Ready to Implement!

Following this plan ensures we:
- Meet all Stage 2 requirements
- Maintain code quality standards
- Keep functions short and focused
- Use existing robot API effectively
- Progress toward 70+ grade target
