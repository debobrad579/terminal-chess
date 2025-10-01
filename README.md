# Terminal Chess in C

A simple terminal-based chess program written in C. Play a full game of chess entirely in the terminal.

## Features
- Move input uses standard algebraic notation (eg. `e4`, `Nbd2`, `O-O`, `fxg8=N+`)
- Validates legal moves
- Detects check, checkmate, stalemate, threefold repetition, and the 50-move rule
- Supports resignation and draw offers

## Getting Started

### Prerequisites
You’ll need:
- A C compiler (eg. `gcc`, `clang`)
- `make` (optional, but recommended)

### Clone the Repository
```bash
git clone https://github.com/debobrad579/terminal-chess.git
cd terminal-chess
```

### Build
If you're using `make`, run:
```bash
make
```
Or compile manually with:
```bash
gcc -Wall -Wextra -std=c11 -g main.c board.c move_piece.c can_move.c legal_moves.c -o main
```

### Run
```bash
./main
```

## TODO
- Clocks
- Playing against engines
