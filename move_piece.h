#include "types.h"

#pragma once

bool castle(board_t *board, castle_t type, bool white_to_move);
bool move_piece(board_t *board, piece_t *piece, square_t *square,
                piece_type_t promotion_type);
bool move_from_san(board_t *board, char *move, bool white_to_move);
