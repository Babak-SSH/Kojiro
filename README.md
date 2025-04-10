# Kojiro

<div align="center">


[![GitHub release (latest by date)](https://img.shields.io/github/v/release/Babak-SSH/Kojiro?style=for-the-badge)](https://github.com/Babak-SSH/Kojiro/releases/latest)
[![GitHub Downloads (specific asset, all releases)](https://img.shields.io/github/downloads/Babak-SSH/Kojiro/Kojiro0.1.3-ubuntu-x86-64?style=for-the-badge)](https://github.com/Babak-SSH/Kojiro/releases/latest)

![Ubuntu](https://github.com/Babak-SSH/Kojiro/actions/workflows/ubuntu-gcc.yml/badge.svg)
![Windows (MinGW64)](https://github.com/Babak-SSH/Kojiro/actions/workflows/windows-mingw64.yml/badge.svg)

</div>

An UCI-compatible Chess Engine written in C++

## Description

**Kojiro** is a free, UCI chess engine written from scratch in C++,which i have developed for fun and to learn about chess programming, it uses well-known concepts used in chess programming for many decades and a lot of ideas are taken from *chessprogramming wiki* and *talk chess* forum and open-source engines.

## Features
- Search:
    - AlphaBeta Pruning
    - Aspiration Window
    - Iterative Deepening
    - Null Move Pruning
    - Razoring
    - Late Move Reductions
    - Quiescence Search
    - Multi Threads(Lazy SMP)
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
    - UCI

## Usage

you can use UCI protocol by running the main file or use an UCI-compatible graphical user interface(GUI) (e.g.  Arena, Sigma Chess) in order to be used comfortably.

also you can challenge it on Lichess! --> https://lichess.org/@/tha_kojiro

## Compiling Kojiro

### Linux
To Build the engine use:
```
cd src
make native
./kojiro
```
### Windows
To build on **windows** use mingw64 
```
mingw32-make.exe native
```
To Build it for specific ISA look into:
```
make help
```

## Tests

### perft test
```
kojiro perft <depth>
```
### Bench
```
kojiro bench
```
---
## inspiring open-source chess-engines:
- BBC by Maksim Korzh
- Stockfish by Tord Romstad, Joona Kiiski, Gary Linscott, Marco Costalba
- Rustic by Marcel Vanthoor 
- Vice by BlueFeverSoft

## Kojiro is still under develop
there are many techniques and features that will be added:
- use more modern cpp features to enhance the performance
- better evalution
    - pawn (push, shield)
    - center control, castling
    - piece pair, blocked pieces
- more tests
- futility prunning (in alpha-beta and quiescence)
- time distribution model for time management of moves
- opening books
- Syzygy support
- NNUE
- internal iterative deepening
- better sorting
    - Counter
    - LastCaptureBonus
    - SEE
    - Checks priority
    - En-passant priority
- Xboard protocol
- wiki
