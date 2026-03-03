# Rubik's Cube Solver (C++)

This project implements a 3x3 Rubik's Cube solver in C++ with multiple cube representations and multiple search strategies, including an IDA* solver accelerated by a corner pattern database.

## What this project does

- Models a Rubik's Cube with interchangeable internal representations.
- Supports all 18 face moves (`L, L', L2, R, R', R2, U, U', U2, D, D', D2, F, F', F2, B, B', B2`).
- Can scramble a cube, print it, and solve it.
- Includes search-based solvers:
  - DFS
  - BFS
  - IDDFS
  - IDA* (with corner pattern database heuristic)

The current `main.cpp` runs the bitboard model + IDA* solver using a precomputed corner database file.

## Repository layout

```text
rubiks-cube-solver/
  CMakeLists.txt
  main.cpp
  README.md

  Model/
    RubiksCube.h
    RubiksCube.cpp
    RubiksCube1dArray.cpp
    RubiksCube3dArray.cpp
    RubiksCubeBitboard.cpp
    PatternDatabase/
      PatternDatabase.h

  Solver/
    DFSSolver.h
    BFSSolver.h
    IDDFSSolver.h
    IDAstarSolver.h

  PatternDatabases/
    NibbleArray.h
    NibbleArray.cpp
    PatternDatabase.h
    PatternDatabase.cpp
    PermutationIndexer.h
    CornerPatternDatabase.h
    CornerPatternDatabase.cpp
    CornerDBMaker.h
    CornerDBMaker.cpp
    math.h
    math.cpp

  Databases/
    cornerDepth5V1.txt
```

## Core architecture

### 1) Cube abstraction

`Model/RubiksCube.h` is the base interface used by all solvers.

It defines:

- Enums: `FACE`, `COLOR`, `MOVE`
- Virtual operations:
  - `getColor(...)`
  - `isSolved()`
  - all move methods (`u()`, `uPrime()`, etc.)
- Shared helpers (implemented in `RubiksCube.cpp`):
  - `move(MOVE)` dispatcher
  - `invert(MOVE)`
  - `print()`
  - `randomShuffleCube(...)`
  - corner helpers used by pattern DB indexing

### 2) Cube representations

Three interchangeable implementations are provided:

- `RubiksCube3dArray` (`char cube[6][3][3]`)
- `RubiksCube1dArray` (`char cube[54]`)
- `RubiksCubeBitboard` (`uint64_t bitboard[6]`)

Each implementation provides the same move semantics and solved-state checks, plus equality/hash support for search containers.

### 3) Solvers

All solver classes are templated on cube type `T` and hash type `H`.

- `DFSSolver`: depth-limited recursive DFS.
- `BFSSolver`: shortest-path BFS with visited map and move back-pointers.
- `IDDFSSolver`: iterative deepening over DFS limits.
- `IDAstarSolver`: iterative-deepening A* style bounded search using a corner-pattern heuristic.

### 4) Pattern database system

The IDA* heuristic comes from a precomputed corner pattern database.

- `PatternDatabase`: abstract base storage for heuristic values.
- `NibbleArray`: compact 4-bit storage (two entries per byte).
- `CornerPatternDatabase`: converts cube corner permutation + orientation to a single index.
- `CornerDBMaker`: BFS generator that fills DB and saves to file.
- `PermutationIndexer`: ranks permutations (Lehmer-code style indexing).

`Databases/cornerDepth5V1.txt` is the binary file loaded by `IDAstarSolver` in the current `main.cpp`.

## Current program flow (`main.cpp`)

1. Creates a solved `RubiksCubeBitboard`.
2. Applies random shuffle moves.
3. Prints scrambled cube and shuffle sequence.
4. Loads corner DB from `Databases/cornerDepth5V1.txt`.
5. Runs `IDAstarSolver::solve()`.
6. Prints solved cube and solution move sequence.

## Build

### Requirements

- CMake >= 3.20
- A C++14 compiler

### Commands

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Run

Run the built executable from your build folder.

Note: `main.cpp` currently uses an absolute Windows path for the database file:

```cpp
string fileName = "C:\\Users\\utsav\\rubix\\rubiks-cube-solver\\Databases\\cornerDepth5V1.txt";
```

If you move the project, update this path (or switch to a relative path).

## Notes and limitations

- `README` was missing originally; this file documents the current implementation.
- Search complexity still grows quickly for deep scrambles; IDA* + pattern DB helps significantly.
- Database generation depth is bounded in `CornerDBMaker` (`curr_depth == 9` break in current code).
- Project is currently oriented around corners heuristic only.

