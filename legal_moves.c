#include "can_move.h"
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>

bool square_attacked(board_t *board, square_t *square, piece_color_t color) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      piece_t *piece = board->squares[i][j].piece;

      if (piece == NULL) {
        continue;
      }

      if (piece->color != color) {
        continue;
      }

      if (can_move(board, piece, square)) {
        return true;
      }
    }
  }

  return false;
}

bool is_in_check(board_t *board, piece_color_t color) {
  return (color == WHITE &&
          square_attacked(board, board->white_king->square, BLACK)) ||
         (color == BLACK &&
          square_attacked(board, board->black_king->square, WHITE));
}

bool is_legal_move(board_t *board, piece_t *piece, square_t *square) {
  if (!can_move(board, piece, square)) {
    return false;
  }

  piece->square->piece = NULL;
  piece_t *prev_piece = square->piece;
  square->piece = piece;
  square_t *prev_square = piece->square;
  piece->square = square;

  bool in_check = is_in_check(board, piece->color);

  piece->square = prev_square;
  square->piece = prev_piece;
  piece->square->piece = piece;

  return !in_check;
}

bool has_legal_move_from_deltas(board_t *board, piece_t *piece,
                                int (*get_rank_delta)(int),
                                int (*get_file_delta)(int), int num_moves) {
  for (int i = 0; i < num_moves; ++i) {
    int new_rank = piece->square->rank + get_rank_delta(i);
    if (new_rank < 0 || new_rank > 7) {
      continue;
    }

    int new_file = piece->square->file + get_file_delta(i);
    if (new_file < 0 || new_file > 7) {
      continue;
    }

    if (is_legal_move(board, piece, &board->squares[new_rank][new_file])) {
      return true;
    }
  }

  return false;
}

bool pawn_has_legal_move(board_t *board, piece_t *pawn) {
  int rank = pawn->square->rank;
  int file = pawn->square->file;
  int direction = pawn->color == WHITE ? 1 : -1;

  if (is_legal_move(board, pawn, &board->squares[rank + direction][file])) {
    return true;
  }

  if (!pawn->has_moved &&
      is_legal_move(board, pawn, &board->squares[rank + 2 * direction][file])) {
    return true;
  }

  if (file != 0 &&
      is_legal_move(board, pawn, &board->squares[rank + direction][file - 1])) {
    return true;
  }

  if (file != 7 &&
      is_legal_move(board, pawn, &board->squares[rank + direction][file + 1])) {
    return true;
  }

  return false;
}

int get_knight_rank_delta(int i) {
  return (i % 2 == 0 ? 1 : 2) * ((i % 4 < 2) ? 1 : -1);
}
int get_knight_file_delta(int i) {
  return (i % 2 == 0 ? 2 : 1) * (i < 4 ? 1 : -1);
}

bool knight_has_legal_move(board_t *board, piece_t *knight) {
  return has_legal_move_from_deltas(board, knight, get_knight_rank_delta,
                                    get_knight_file_delta, 8);
}

bool bishop_has_legal_move(board_t *board, piece_t *bishop) {
  int rank = bishop->square->rank;
  int file = bishop->square->file;

  for (int d = 0; d < 4; ++d) {
    for (int i = 1; i < 8; ++i) {
      square_t *square;

      if (d == 0) {
        if (file + i > 7 || rank + i > 7) {
          break;
        }

        square = &board->squares[rank + i][file + i];
      } else if (d == 1) {
        if (file - i < 0 || rank - i < 0) {
          break;
        }

        square = &board->squares[rank - i][file - i];
      } else if (d == 2) {
        if (rank + i > 7 || file - i < 0) {
          break;
        }

        square = &board->squares[rank + i][file - i];
      } else if (d == 3) {
        if (rank - i < 0 || file + i > 7) {
          break;
        }

        square = &board->squares[rank - i][file + i];
      }

      if (is_legal_move(board, bishop, square)) {
        return true;
      }

      if (square->piece != NULL) {
        break;
      }
    }
  }

  return false;
}

bool rook_has_legal_move(board_t *board, piece_t *rook) {
  int rank = rook->square->rank;
  int file = rook->square->file;

  for (int d = 0; d < 4; ++d) {
    for (int i = 1; i < 8; ++i) {
      square_t *square;

      if (d == 0) {
        if (file + i > 7) {
          break;
        }

        square = &board->squares[rank][file + i];
      } else if (d == 1) {
        if (file - i < 0) {
          break;
        }

        square = &board->squares[rank][file - i];
      } else if (d == 2) {
        if (rank + i > 7) {
          break;
        }

        square = &board->squares[rank + i][file];
      } else if (d == 3) {
        if (rank - i < 0) {
          break;
        }

        square = &board->squares[rank - i][file];
      }

      if (is_legal_move(board, rook, square)) {
        return true;
      }

      if (square->piece != NULL) {
        break;
      }
    }
  }

  return false;
}

bool queen_has_legal_move(board_t *board, piece_t *queen) {
  return bishop_has_legal_move(board, queen) ||
         rook_has_legal_move(board, queen);
}

int get_king_rank_delta(int i) { return (i < 4 ? i : i + 1) / 3 - 1; }
int get_king_file_delta(int i) { return (i < 4 ? i : i + 1) % 3 - 1; }

bool king_has_legal_move(board_t *board, piece_t *king) {
  return has_legal_move_from_deltas(board, king, get_king_rank_delta,
                                    get_king_file_delta, 8);
}

bool has_legal_move(board_t *board, piece_color_t color) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      piece_t *piece = board->squares[i][j].piece;
      if (piece == NULL || piece->color != color) {
        continue;
      }

      bool res;

      switch (piece->type) {
      case PAWN:
        res = pawn_has_legal_move(board, piece);
        break;
      case KNIGHT:
        res = knight_has_legal_move(board, piece);
        break;
      case BISHOP:
        res = bishop_has_legal_move(board, piece);
        break;
      case ROOK:
        res = rook_has_legal_move(board, piece);
        break;
      case QUEEN:
        res = queen_has_legal_move(board, piece);
        break;
      case KING:
        res = king_has_legal_move(board, piece);
        break;
      }

      if (res) {
        return true;
      }
    }
  }

  return false;
}

bool insufficient_material(board_t *board) {
  bool hasKnight[2] = {false, false};
  bool hasBishop[2] = {false, false};

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      piece_t *piece = board->squares[i][j].piece;
      if (piece == NULL) {
        continue;
      }

      switch (piece->type) {
      case PAWN:
      case ROOK:
      case QUEEN:
        return false;
      case KNIGHT:
        if (hasBishop[piece->color]) {
          return false;
        }
        hasKnight[piece->color] = true;
        break;
      case BISHOP:
        if (hasKnight[piece->color] || hasBishop[piece->color]) {
          return false;
        }
        hasBishop[piece->color] = true;
        break;
      case KING:
        break;
      }
    }
  }

  return true;
}
