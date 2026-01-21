# Lichess-ChessEngine
Simple chess engine written in C++.

## File overview

### Hash.cpp
Implements hashing so you can store positions in an `unordered_map`.

### BoardUtils.cpp / Utils.cpp
Loading and printing the chessboard, plus helper functions to keep the code readable.

### Starting position
``` text
RNBQKBNR
PPPPPPPP
........
........
........
........
pppppppp
rnbqkbnr
```

### Evaluation.cpp
Simple functions for evaluating positions (+ / - score).

### Moves.cpp
Generates moves, divided into:
- LOUD – captures
- QUIET – non-capturing moves

### main.cpp
Alpha-beta search with iterative deepening.

## Missing features
Currently not implemented:
- quiescence search
- castling (handled only in `makeMove()`, not in the move generator)
- en passant captures
  
