#include "board.h"
#include "legal_moves.h"
#include "move_piece.h"
#include "types.h"
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  board_t *board;
  draw_offer_t draw_offer;
  piece_color_t color_to_move;
  bool illegal_move_made;

game_loop:
  board = create_board();

  if (board == NULL) {
    return 1;
  }

  draw_offer = NO_OFFER;
  color_to_move = WHITE;
  illegal_move_made = false;

  while (true) {
    print_board(board);

    bool in_check = is_in_check(board, color_to_move);
    piece_color_t opposite_color = color_to_move == WHITE ? BLACK : WHITE;

    if (illegal_move_made) {
      printf("\033[0;31mEnter a legal move!\033[0m\n");
      illegal_move_made = false;
    }

    if (!has_legal_move(board, color_to_move)) {
      game_over(in_check ? CHECKMATE : STALEMATE, opposite_color);
      break;
    }

    bool have_active_draw_offer =
        (color_to_move == WHITE && draw_offer == WHITE_OFFERED) ||
        (color_to_move == BLACK && draw_offer == BLACK_OFFERED);

    printf("Enter a move for %s (r to resign, d to %s): ",
           color_to_move == WHITE ? "white" : "black",
           draw_offer == NO_OFFER   ? "offer a draw"
           : have_active_draw_offer ? "cancel draw offer"
                                    : "accept draw offer");
    char move[10];
    scanf("%9s", move);

    if (strcmp(move, "r") == 0) {
      game_over(RESIGNATION, opposite_color);
      break;
    }

    if (strcmp(move, "d") == 0) {
      if (draw_offer == NO_OFFER) {
        draw_offer = color_to_move == WHITE ? WHITE_OFFERED : BLACK_OFFERED;
      } else if (have_active_draw_offer) {
        draw_offer = NO_OFFER;
      } else {
        game_over(DRAW_OFFER, opposite_color);
        break;
      }
      continue;
    }

    if (!is_valid_san(move)) {
      illegal_move_made = true;
      continue;
    }

    if (move_from_san(board, move, color_to_move)) {
      color_to_move = opposite_color;
      if (draw_offer != NO_OFFER && !have_active_draw_offer) {
        draw_offer = NO_OFFER;
      }
    } else {
      illegal_move_made = true;
    }
  }

  free_board(board);
  goto game_loop;
}
