# COMP0002 Robot Arena Coursework

## Description

This program simulates a robot navigating a randomly-shaped arena to collect markers. Obstacles are arranged in geometric patterns (circle, diamond, rectangle, oval, or triangle) creating an open area in the center where 3-7 markers are placed. The robot explores the area, collects all markers, and stops at the final marker location.

**Key Features:**
- 5 different arena shapes (randomly selected)
- Exploration using hybrid greedy+BFS algorithm
- Movement trail visualization (green/yellow/red lines show efficiency)
- Random arena size (24-40 tiles per dimension)

**Algorithm:**
The robot uses a hybrid greedy+BFS exploration strategy:
1. **Greedy Phase**: First attempts to move to any adjacent unvisited tile (checking 4 neighbors). This creates continuous exploration paths through open areas.
2. **Jump Phase**: When no adjacent tiles are available (blocked by obstacles or all neighbors visited), the robot scans the arena for the nearest unvisited tile and uses BFS pathfinding to navigate there.
3. **Termination**: Stops immediately after collecting the final marker (Stage 5 requirement).

This approach efficiently handles both open areas and complex obstacle configurations.

## Compile & Run

```bash
gcc -Wall -Werror -o robot main.c robot.c arena.c pathfinding.c graphics.c

./robot | java -jar drawapp-4.5.jar
```

## Technical Details

**Program Structure:**
- `main.c`: Main workflow and exploration algorithm
- `robot.c/h`: Robot API (8 functions: forward, left, right, atMarker, canMoveForward, pickUpMarker, dropMarker, markerCount)
- `arena.c/h`: Arena generation, shape placement, drawing, trail visualization
- `pathfinding.c/h`: BFS shortest-path algorithm

**Code Quality:**
- All 71 functions under 15 lines
- Zero compiler warnings
- Clean modular architecture

## References

BFS pathfinding algorithm adapted from: https://github.com/rodriguesrenato/coverage-path-planning
