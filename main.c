#include "pieces.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

board_t *create_initial_board() {
  board_t *board = malloc(sizeof(board_t));

  if (!board) {
    return NULL;
  }

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      board->squares[i][j].rank = i;
      board->squares[i][j].file = j;
      board->squares[i][j].piece = NULL;

      if (i > 1 && i < 6) {
        continue;
      }

      board->squares[i][j].piece = malloc(sizeof(piece_t));

      piece_t *piece = board->squares[i][j].piece;

      if (piece == NULL) {
        return NULL;
      }

      piece->has_moved = false;
      piece->square = &board->squares[i][j];

      if (i == 0 || i == 1) {
        piece->color = WHITE;
      } else {
        piece->color = BLACK;
      }

      if (i == 0 || i == 7) {
        switch (j) {
        case 0:
        case 7:
          piece->type = ROOK;
          break;
        case 1:
        case 6:
          piece->type = KNIGHT;
          break;
        case 2:
        case 5:
          piece->type = BISHOP;
          break;
        case 3:
          piece->type = QUEEN;
          break;
        case 4:
          piece->type = KING;
          break;
        }
      } else {
        piece->type = PAWN;
      }
    }
  }

  return board;
}

void print_board(board_t *board) {
  printf("\033[2J\033[H");

  for (int i = 7; i >= 0; --i) {
    printf("\033[90m%d\033[0m", i + 1);

    for (int j = 0; j < 8; ++j) {
      piece_t *piece = board->squares[i][j].piece;

      printf(" ");

      if (piece == NULL) {
        printf("-");
        continue;
      }

      if (piece->color == BLACK) {
        printf("\033[0;32m");
      }

      switch (piece->type) {
      case PAWN:
        printf("p");
        break;
      case KNIGHT:
        printf("N");
        break;
      case BISHOP:
        printf("B");
        break;
      case ROOK:
        printf("R");
        break;
      case QUEEN:
        printf("Q");
        break;
      case KING:
        printf("K");
        break;
      }

      if (piece->color == BLACK) {
        printf("\033[0m");
      }
    }

    printf("\n");
  }

  printf("\033[90m  a b c d e f g h\033[0m\n");
}

piece_t *get_piece(board_t *board, char move[5]) {
  int rank = move[0] - 'a';
  int file = move[1] - '1';
  piece_t *piece = board->squares[file][rank].piece;
  if (piece == NULL) {
    return NULL;
  }
  return piece;
}

square_t *get_square(board_t *board, char move[5]) {
  uint rank = move[2] - 'a';
  uint file = move[3] - '1';
  if (rank > 7 || file > 7) {
    return NULL;
  }
  square_t *square = &board->squares[file][rank];
  return square;
}

int main(void) {
  board_t *board = create_initial_board();
  bool white_to_move = true;
  while (true) {
    print_board(board);
    if (white_to_move) {
      printf("Enter a move for white (eg. e2e4):");
    } else {
      printf("Enter a move for black (eg. e7e5):");
    }
    char move[5];
    scanf("%4s", move);

    piece_t *piece = get_piece(board, move);
    if ((piece->color == WHITE) != white_to_move) {
      continue;
    }

    if (move_piece(board, piece, get_square(board, move))) {
      white_to_move = !white_to_move;
    }
  }
  return 0;
}
