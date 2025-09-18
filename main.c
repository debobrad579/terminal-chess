#include "pieces.h"
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

bool is_valid_san(const char *move) {
  const char *pattern =
      "^(O-O(-O)?|([KQRBN]?[a-h]?[1-8]?x?[a-h][1-8](=[QRBN])?[+#]?))$";

  regex_t regex;

  if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
    return false;
  }

  int status = regexec(&regex, move, 0, NULL, 0);
  regfree(&regex);

  return status == 0;
}

bool move_from_san(board_t *board, char *move, bool white_to_move) {
  piece_type_t piece_type;
  int piece_file = -1, piece_rank = -1, dest_file = -1, dest_rank = -1;
  int i = 0;

  switch (move[i]) {
  case 'N':
    piece_type = KNIGHT;
    i++;
    break;
  case 'B':
    piece_type = BISHOP;
    i++;
    break;
  case 'R':
    piece_type = ROOK;
    i++;
    break;
  case 'Q':
    piece_type = QUEEN;
    i++;
    break;
  case 'K':
    piece_type = KING;
    i++;
    break;
  default:
    piece_type = PAWN;
    break;
  }

  if (move[i] >= 'a' && move[i] <= 'h' &&
      !(move[i + 1] >= '1' && move[i + 1] <= '8')) {
    piece_file = move[i] - 'a';
    i++;
  }

  if (move[i] >= '1' && move[i] <= '8' &&
      !(move[i + 1] >= 'a' && move[i + 1] <= 'h')) {
    piece_rank = move[i] - '1';
    i++;
  }

  if (move[i] == 'x') {
    i++;
  }

  if (move[i] >= 'a' && move[i] <= 'h' && move[i + 1] >= '1' &&
      move[i + 1] <= '8') {
    dest_file = move[i] - 'a';
    dest_rank = move[i + 1] - '1';
    i += 2;
  } else {
    return false;
  }

  square_t *dest_square = &board->squares[dest_rank][dest_file];

  piece_t *final_piece = NULL;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      piece_t *piece = board->squares[i][j].piece;

      if (piece == NULL || piece->type != piece_type ||
          (piece->color == WHITE) != white_to_move ||
          (piece_rank != -1 && piece->square->rank != piece_rank) ||
          (piece_file != -1 && piece->square->file != piece_file)) {
        continue;
      }

      if (can_move(board, piece, dest_square)) {
        if (final_piece != NULL) {
          return false;
        }
        final_piece = piece;
      }
    }
  }

  if (final_piece == NULL) {
    return false;
  }

  return move_piece(board, final_piece, dest_square);
}

int main(void) {
  board_t *board = create_initial_board();
  bool white_to_move = true;
  while (true) {
    print_board(board);
    printf("Enter a move for %s: ", white_to_move ? "white" : "black");
    char move[10];
    scanf("%9s", move);

    if (!is_valid_san(move)) {
      continue;
    }

    if (move_from_san(board, move, white_to_move)) {
      white_to_move = !white_to_move;
    }
  }

  return 0;
}
