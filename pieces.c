#include "pieces.h"
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
    int direction = rank_diff == file_diff ? 1 : -1;
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
  return abs(end->rank - start->rank) == 1 || abs(end->file - start->file) == 1;
}

bool can_move(board_t *board, piece_t *piece, square_t *square) {
  if (piece == NULL || square == NULL) {
    return false;
  }

  switch (piece->type) {
  case PAWN:
    return pawn_can_move(board, piece->square, square,
                         piece->color == WHITE ? 1 : -1, piece->has_moved);
  case KNIGHT:
    return knight_can_move(piece->square, square);
  case BISHOP:
    return bishop_can_move(board, piece->square, square);
  case ROOK:
    return rook_can_move(board, piece->square, square);
  case QUEEN:
    return queen_can_move(board, piece->square, square);
  case KING:
    return king_can_move(piece->square, square);
  }
}

void move_to(piece_t *piece, square_t *square) {
  piece->square->piece = NULL;
  piece->has_moved = true;
  square->piece = piece;
  piece->square = square;
}

bool castle(board_t *board, castle_type_t type, bool white_to_move) {
  int rank = white_to_move ? 0 : 7;

  piece_t *king = board->squares[rank][4].piece;

  if (king == NULL || king->has_moved) {
    return false;
  }

  if (type == SHORT) {
    piece_t *rook = board->squares[rank][7].piece;

    if (board->squares[rank][6].piece != NULL ||
        board->squares[rank][5].piece != NULL || rook == NULL ||
        rook->has_moved) {
      return false;
    }

    move_to(king, &board->squares[rank][6]);
    move_to(rook, &board->squares[rank][5]);
    return true;
  }

  piece_t *rook = board->squares[rank][0].piece;

  if (board->squares[rank][1].piece != NULL ||
      board->squares[rank][2].piece != NULL ||
      board->squares[rank][3].piece != NULL || rook == NULL ||
      rook->has_moved) {
    return false;
  }

  move_to(king, &board->squares[rank][2]);
  move_to(rook, &board->squares[rank][3]);
  return true;
}

bool move_piece(board_t *board, piece_t *piece, square_t *square,
                piece_type_t promotion_type) {
  if (piece == NULL || square == NULL) {
    return false;
  }

  if (!can_move(board, piece, square)) {
    return false;
  }

  if (piece->type == PAWN && board->enpassantable_pawn != NULL &&
      board->enpassantable_pawn->square->rank ==
          square->rank - (piece->color == WHITE ? 1 : -1) &&
      board->enpassantable_pawn->square->file == square->file) {
    move_to(piece, square);
    board->enpassantable_pawn->square->piece = NULL;
    free(board->enpassantable_pawn);
    board->enpassantable_pawn = NULL;
    return true;
  }

  if (piece->type == PAWN && !piece->has_moved && piece->color == WHITE
          ? square->rank == 3
          : square->rank == 4) {
    board->enpassantable_pawn = piece;
  } else {
    board->enpassantable_pawn = NULL;
  }

  if (square->piece != NULL) {
    if (square->piece->color == piece->color) {
      return false;
    }
    free(square->piece);
  }

  if (piece->type == PAWN && ((piece->color == WHITE && square->rank == 7) ||
                              (piece->color == BLACK && square->rank == 0))) {
    piece->type = promotion_type;
  }

  move_to(piece, square);
  return true;
}
