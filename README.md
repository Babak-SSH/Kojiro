# Kojiro


![tests](https://github.com/Babak-SSH/Kojiro/actions/workflows/c-cpp.yml/badge.svg)

An UCI-compatible Chess Engine written in C++

## Description

Kojiro is a free, UCI chess engine written from scratch in C++(20), it uses well-known concepts used in chess programming for many decades.

## Features
- Search:
    - AlphaBeta Pruning
    - Aspiration Window
    - Iterative Deepening
    - Null Move Pruning
    - Razoring
    - Late Move Reductions
    - Quiescence Search
    - Lazy SMP
- Move Ordering:
    - Hash Move
    - TT Move
    - MVV/LVA
    - Killer heuristic
    - History heuristic
- Evaluation:
    - Evaluation Tapering
    - Piece Square Tables
    - Pawn Structure
    - Piece Mobility
    - Open Files
- Other:
    - Zobrist Hashing
    - Transposition Table
    - logging

## Usage

you can use UCI protocol by running the main file or use an UCI-compatible graphical user interface(GUI) (e.g.  Arena, Sigma Chess) in order to be used comfortably.

also you can challenge it on Lichess! --> https://lichess.org/@/tha_kojiro

## Compiling Kojiro

to compile the engine use:
```
cd src
make kojiro
./kojiro
```

## Tests

to compile perft tests
```
cd src
make perft
```
usage:
```
./perft depth fen
./perft 5 rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```