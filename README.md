# 2048 — C++ / Qt QML

A fully functional implementation of the **2048 puzzle game**, built with C++ for the game logic and QML (Qt Quick) for the graphical interface.

Developed as a two-person team project at **Centrale Lyon** (2023–2024).

## Gameplay

The classic 2048 rules: slide tiles in four directions to merge identical values and reach the 2048 tile.

## Features

- **Configurable grid size** — choose between different board dimensions before starting
- **Touchpad support** — two-finger swipe on laptop trackpads, in addition to arrow keys
- **Undo** — revert the last move
- **Restart** — reset the board at any time

## Tech Stack

- **C++** — game engine (`DamierDyn` class handles board state, merges, and move logic)
- **QML / Qt Quick** — declarative UI with smooth tile animations
- **CMake** — build system

## Build & Run

Requires Qt 6.8+ with Qt Quick.

```bash
cmake -B build
cmake --build build
./build/Projet_2048
```

Or open `CMakeLists.txt` directly in Qt Creator and hit Run.

## Team

Carl-Henri Gegout · Clément Guilhaumon
