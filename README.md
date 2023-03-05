# kojiro

An UCI-compatible Chess Engine written in C++

## Description

Kojiro is a free, UCI chess engine written from scratch. Kojiro is not a complete chess program and requires a UCI-compatible graphical user interface (GUI) (e.g.  Arena, Sigma Chess) in order to be used comfortably

## Badges

[![Action_Status]()](c++ ci/cd)

## Features
- Search:
    - AlphaBeta Pruning
    - Aspiration Window
    - Iterative Deepening
    - Null Move Pruning
    - Late Move Reductions
    - Quiescence Search
- Move Ordering:
    - Hash Move
    - MVV/LVA
    - Killer heuristic
    - History heuristic
- Evaluation:
    - Evaluation Tapering
    - Piece Square Tables
    - Pawn Structure (Connected/Isolated/Double/Passed)
    - Piece Mobility
    - Pawn Shield
- Other:
    - Zobrist Hashing
    - Transposition Table

## Building 

to compile the engine use:
```
make main
```
include in arena engines

to compile perft tests
```
make perft
```

## Usage

you can use either by UCI commands by running the main file or use an UCI-compatible graphical user interface (GUI)

## Roadmap
nnue
razoring

## License
do wtf you want with it