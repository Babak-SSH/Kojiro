# Kojiro


![tests](https://github.com/Babak-SSH/Kojiro/actions/workflows/c-cpp.yml/badge.svg)

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

### inspiring open-source chess-engines:
- BBC by Maksim Korzh
- Stockfish by Tord Romstad, Joona Kiiski, Gary Linscott, Marco Costalba
- Rustic by Marcel Vanthoor 
- Vice by BlueFeverSoft

### Kojiro is still under develop
there are many techniques and features that will be added:
- use more modern cpp features to enhance the performance
- better evalution
    - pawn (push, shield)
    - center control, castling
    - piece pair, blocked pieces
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