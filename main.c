#include "legal_moves.h"
#include "move_piece.h"
#include "types.h"
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

board_t *create_initial_board() {
  board_t *board = calloc(1, sizeof(board_t));

  if (!board) {
    return NULL;
  }

  board->draw_offer = NO_OFFER;

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

void game_over(gameover_t type, piece_color_t color) {
  switch (type) {
  case CHECKMATE:
    printf("CHECKMATE! %s WINS!", color == WHITE ? "WHITE" : "BLACK");
    break;
  case RESIGNATION:
    printf("%s RESIGNED! %s WINS!", color == WHITE ? "BLACK" : "WHITE",
           color == WHITE ? "WHITE" : "BLACK");
    break;
  case STALEMATE:
    printf("STALEMATE! THE GAME IS DRAWN!");
    break;
  case DRAW_OFFER:
    printf("DRAW AGREED! THE GAME IS DRAWN!");
    break;
  }

  printf("\n");

  while (true) {
    char choice;
    printf("Do you want to play again? (y/n): ");
    scanf(" %c", &choice);
    if (choice == 'y') {
      break;
    } else if (choice == 'n') {
      exit(0);
    }
  }
}

int main(void) {
  board_t *board = create_initial_board();
  piece_color_t color_to_move = WHITE;

game_loop:
  while (true) {
    print_board(board);

    bool in_check = is_in_check(board, color_to_move);
    piece_color_t opposite_color = color_to_move == WHITE ? BLACK : WHITE;

    if (!has_legal_move(board, color_to_move)) {
      game_over(in_check ? CHECKMATE : STALEMATE, opposite_color);
      break;
    } else if (in_check) {
      printf("Check!\n");
    }

    bool have_active_draw_offer =
        (color_to_move == WHITE && board->draw_offer == WHITE_OFFERED) ||
        (color_to_move == BLACK && board->draw_offer == BLACK_OFFERED);

    printf("Enter a move for %s (r to resign, d to %s): ",
           color_to_move == WHITE ? "white" : "black",
           board->draw_offer == NO_OFFER ? "offer a draw"
           : have_active_draw_offer      ? "cancel draw offer"
                                         : "accept draw offer");
    char move[10];
    scanf("%9s", move);

    if (strcmp(move, "r") == 0) {
      game_over(RESIGNATION, opposite_color);
      break;
    }

    if (strcmp(move, "d") == 0) {
      if (board->draw_offer == NO_OFFER) {
        board->draw_offer =
            color_to_move == WHITE ? WHITE_OFFERED : BLACK_OFFERED;
      } else if (have_active_draw_offer) {
        board->draw_offer = NO_OFFER;
      } else {
        game_over(DRAW_OFFER, opposite_color);
        break;
      }
      continue;
    }

    if (!is_valid_san(move)) {
      continue;
    }

    if (move_from_san(board, move, color_to_move)) {
      color_to_move = opposite_color;
      if (board->draw_offer != NO_OFFER && !have_active_draw_offer) {
        board->draw_offer = NO_OFFER;
      }
    }
  }

  free_board(board);
  board = create_initial_board();
  color_to_move = WHITE;
  goto game_loop;
}
