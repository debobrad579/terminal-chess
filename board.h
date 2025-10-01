#include "types.h"

#pragma once

board_t *create_board();
void free_board(board_t *board);
void print_board(board_t *board);
char *board_to_fen(board_t *board, piece_color_t color_to_move);
