#include "types.h"

#pragma once

bool square_attacked(board_t *board, square_t *square, piece_color_t color);
bool is_in_check(board_t *board, piece_color_t color);
bool is_legal_move(board_t *board, piece_t *piece, square_t *square);
bool has_legal_move(board_t *board, piece_color_t color);
