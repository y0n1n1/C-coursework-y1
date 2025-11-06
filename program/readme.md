# COMP0002 Robot Arena Coursework - Stage 4

## Description

This program implements Stage 4 of the robot arena coursework. The robot navigates a rectangular arena of **random size** (12-18 tiles per dimension) containing **multiple obstacles and markers**. Using BFS (Breadth-First Search) pathfinding, the robot finds and collects **all markers**, then navigates to a corner to drop them all.

**Visual improvements:**
- Markers displayed as grey **circles** (not squares)
- Obstacles displayed as black squares (ready for Stage 4)
- Marker visually disappears when picked up (background redrawn)
- Marker reappears at corner when dropped

**Algorithm:**
- **Exploration (Stage 4):** BFS-based systematic exploration - robot visits all accessible tiles, discovers markers through atMarker() API calls, stores discovered locations in MarkerList.
- **Pathfinding (Stage 4):** BFS (Breadth-First Search) - finds shortest path around obstacles. Guarantees finding path if one exists, prevents infinite loops through visited tracking.
- **Collection:** Robot pathfinds to each discovered marker location, picks up marker, repeats until all collected.
- **Resilience:** Visited tracking prevents infinite loops. Handles any obstacle configuration gracefully. No "cheating" by scanning arena array.

The program uses a modular design with separate files:
- `main.c`: Main workflow, exploration, marker discovery, collection, path following
- `robot.c/h`: Robot API implementation (8 required functions)
- `arena.c/h`: Arena initialization, obstacles, markers, drawing
- `pathfinding.c/h`: BFS algorithm, path finding, queue management ← **NEW**
- `graphics.c/h`: Drawing library (provided, not modified)

## Compile

```bash
gcc -Wall -Werror -o robot main.c robot.c arena.c pathfinding.c graphics.c
```

## Run

```bash
./robot | java -jar drawapp-4.5.jar
```

Or with custom starting position (for testing):

```bash
./robot 5 5 north | java -jar drawapp-4.5.jar
```

Arguments: `x-position y-position direction`

## Features Implemented

### Stage 1 (Complete):
- Rectangular arena with walls (red)
- Grid display (light gray)
- Random marker placement next to wall (gray)
- Random robot starting position away from walls
- Robot displayed as blue triangle pointing in movement direction
- Wall-following algorithm to find marker
- All 8 required robot API functions

### Stage 2 (Complete):
- **Random arena size** (12-18 tiles width and height, varies each run)
- **Visual enhancements** - Markers as circles, obstacles as black squares
- **Marker pickup** using pickUpMarker() API with visual feedback
- **Corner detection** - robot identifies when at any of the 4 corners
- **Marker delivery** - robot navigates to corner and drops marker
- **Complete workflow**: Find marker → Pick up (disappears) → Find corner → Drop (reappears) → Stop
- **Faster animation** (150ms delay for smoother movement)
- Command line argument support maintained

### Stage 3 (Complete):
- **Marker anywhere** - Marker placed at any interior tile (not just walls)
- **Lawnmower search** - Systematic row-by-row coverage
- **All Stage 2 features maintained**

### Stage 4 (Complete):
- **Multiple obstacles** - 3-8 randomly placed obstacles (scales with arena size)
- **Multiple markers** - 2-4 randomly placed markers
- **Authentic exploration** - Robot discovers markers by visiting tiles and calling atMarker() API
- **BFS pathfinding** - Finds shortest path around obstacles
- **MarkerList struct** - Stores discovered marker locations during exploration
- **3-phase workflow** - Explore → Collect discovered markers → Deliver to corner
- **Resilient algorithm** - Impossible to infinite loop (visited tracking prevents cycles)
- **Path struct** - Bundles path data, reduces parameter overhead
- **Graceful failure** - Handles unreachable markers and blocked paths
- **All functions <15 lines** - Maintained through aggressive decomposition
- **Zero compiler warnings** - Clean compilation with -Wall -Werror
- **Modular pathfinding** - Separate pathfinding.c/h module for clean architecture
