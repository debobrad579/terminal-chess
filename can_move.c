#include "types.h"
#include <stdbool.h>
#include <stdlib.h>

#define sign(x) ((x >= 0) ? 1 : -1)

bool pawn_can_move(board_t *board, square_t *start, square_t *end,
                   int direction, bool has_moved) {
  int diff = end->rank - start->rank;

  if (end->piece != NULL) {
    return diff == direction && abs(end->file - start->file) == 1;
  }

  if (board->enpassantable_pawn != NULL &&
      board->enpassantable_pawn->square->rank == end->rank - direction &&
      board->enpassantable_pawn->square->file == end->file) {
    return diff == direction && abs(end->file - start->file) == 1;
  }

  if (end->file != start->file)
    return false;

  if (board->squares[start->rank + direction][start->file].piece != NULL)
    return false;

  if (diff == 2 * direction && !has_moved) {
    return board->squares[start->rank + 2 * direction][start->file].piece ==
           NULL;
  }

  return diff == direction;
}

bool knight_can_move(square_t *start, square_t *end) {
  return (abs(end->rank - start->rank) == 1 &&
          abs(end->file - start->file) == 2) ||
         (abs(end->rank - start->rank) == 2 &&
          abs(end->file - start->file) == 1);
}

bool bishop_can_move(board_t *board, square_t *start, square_t *end) {
  int rank_diff = end->rank - start->rank;
  int file_diff = end->file - start->file;

  if (abs(rank_diff) == abs(file_diff)) {
    for (int i = 1; i < abs(rank_diff); ++i) {
      if (board
              ->squares[start->rank + i * sign(rank_diff)]
                       [start->file + i * sign(file_diff)]
              .piece != NULL) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool rook_can_move(board_t *board, square_t *start, square_t *end) {
  if (start->rank == end->rank) {
    int file_diff = end->file - start->file;

    for (int i = 1; i < abs(file_diff); ++i) {
      if (board->squares[start->rank][start->file + i * sign(file_diff)]
              .piece != NULL) {
        return false;
      }
    }

    return true;
  }

  if (start->file == end->file) {
    int rank_diff = end->rank - start->rank;

    for (int i = 1; i < abs(rank_diff); ++i) {
      if (board->squares[start->rank + i * sign(rank_diff)][start->file]
              .piece != NULL) {
        return false;
      }
    }

    return true;
  }

  return false;
}

bool queen_can_move(board_t *board, square_t *start, square_t *end) {
  return rook_can_move(board, start, end) || bishop_can_move(board, start, end);
}

bool king_can_move(square_t *start, square_t *end) {
  return abs(end->rank - start->rank) <= 1 && abs(end->file - start->file) <= 1;
}

bool can_move(board_t *board, piece_t *piece, square_t *square) {
  if (piece == NULL || square == NULL) {
    return false;
  }

  if (square == piece->square) {
    return false;
  }

  if (square->piece != NULL && piece->color == square->piece->color) {
    return false;
  }

  switch (piece->type) {
  case PAWN:
    return pawn_can_move(board, piece->square, square,
                         piece->color == WHITE ? 1 : -1, piece->has_moved);
    break;
  case KNIGHT:
    return knight_can_move(piece->square, square);
    break;
  case BISHOP:
    return bishop_can_move(board, piece->square, square);
    break;
  case ROOK:
    return rook_can_move(board, piece->square, square);
    break;
  case QUEEN:
    return queen_can_move(board, piece->square, square);
    break;
  case KING:
    return king_can_move(piece->square, square);
    break;
  }

  return false;
}
