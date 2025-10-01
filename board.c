#include "legal_moves.h"
#include "types.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_board(board_t *board) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      piece_t *piece = board->squares[i][j].piece;
      if (piece != NULL) {
        free(piece);
      }
    }
  }

  free(board);
}

board_t *create_board() {
  board_t *board = calloc(1, sizeof(board_t));

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

      if (!piece) {
        free_board(board);
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
          if (piece->color == WHITE) {
            board->white_king = piece;
          } else {
            board->black_king = piece;
          }
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
        if (is_in_check(board, piece->color)) {
          printf("\033[0;31m");
        }
        printf("K");
        break;
      }

      printf("\033[0m");
    }

    printf("\n");
  }

  printf("\033[90m  a b c d e f g h\033[0m\n");
}

char *board_to_fen(board_t *board, piece_color_t color_to_move) {
  char *fen = malloc(128);
  fen[0] = '\0';
  int spacer = 0;

  for (int i = 7; i >= 0; i--) {
    for (int j = 0; j < 8; j++) {
      square_t square = board->squares[i][j];
      if (square.piece == NULL) {
        spacer++;
        continue;
      }

      if (spacer > 0) {
        char buf[2] = {(char)('0' + spacer), '\0'};
        strcat(fen, buf);
        spacer = 0;
      }

      char piece_char;
      switch (square.piece->type) {
      case PAWN:
        piece_char = 'p';
        break;
      case KNIGHT:
        piece_char = 'n';
        break;
      case BISHOP:
        piece_char = 'b';
        break;
      case ROOK:
        piece_char = 'r';
        break;
      case QUEEN:
        piece_char = 'q';
        break;
      case KING:
        piece_char = 'k';
        break;
      }

      if (square.piece->color == WHITE) {
        piece_char = toupper(piece_char);
      }

      char piece_str[2] = {piece_char, '\0'};
      strcat(fen, piece_str);
    }

    if (spacer > 0) {
      char buf[2] = {(char)('0' + spacer), '\0'};
      strcat(fen, buf);
      spacer = 0;
    }

    if (i > 0) {
      strcat(fen, "/");
    }
  }

  fen = strcat(fen, color_to_move == WHITE ? " w " : " b ");

  bool castlePossible = false;

  if (!board->white_king->has_moved) {
    piece_t *king_rook = board->squares[0][7].piece;
    if (king_rook != NULL && !king_rook->has_moved) {
      fen = strcat(fen, "K");
      castlePossible = true;
    }
    piece_t *queen_rook = board->squares[0][0].piece;
    if (queen_rook != NULL && !queen_rook->has_moved) {
      fen = strcat(fen, "Q");
      castlePossible = true;
    }
  }
  if (!board->black_king->has_moved) {
    piece_t *king_rook = board->squares[7][7].piece;
    if (king_rook != NULL && !king_rook->has_moved) {
      fen = strcat(fen, "k");
      castlePossible = true;
    }
    piece_t *queen_rook = board->squares[7][0].piece;
    if (queen_rook != NULL && !queen_rook->has_moved) {
      fen = strcat(fen, "q");
      castlePossible = true;
    }
  }

  fen = strcat(fen, castlePossible ? " " : "- ");

  if (board->enpassantable_pawn != NULL) {
    char numberChar = board->enpassantable_pawn->color == WHITE ? '3' : '6';
    char buf[3] = {'a' + board->enpassantable_pawn->square->file, numberChar,
                   '\0'};
    fen = strcat(fen, buf);
  } else {
    fen = strcat(fen, "-");
  }

  fen = realloc(fen, strlen(fen) + 1);

  return fen;
}
