#include "can_move.h"
#include "legal_moves.h"
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

  piece_color_t attacker = white_to_move ? BLACK : WHITE;

  if (type == SHORT) {
    piece_t *rook = board->squares[rank][7].piece;

    if (board->squares[rank][6].piece != NULL ||
        board->squares[rank][5].piece != NULL || rook == NULL ||
        rook->has_moved) {
      return false;
    }

    if (square_attacked(board, &board->squares[rank][6], attacker) ||
        square_attacked(board, &board->squares[rank][5], attacker)) {
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

  if (square_attacked(board, &board->squares[rank][2], attacker) ||
      square_attacked(board, &board->squares[rank][3], attacker)) {
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

  if (!is_legal_move(board, piece, square)) {
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

  if (piece->type == PAWN && ((piece->color == WHITE && square->rank == 7) ||
                              (piece->color == BLACK && square->rank == 0))) {
    piece->type = promotion_type;
  }

  if (square->piece != NULL) {
    free(square->piece);
  }

  move_to(piece, square);

  return true;
}

bool move_from_san(board_t *board, char *move, bool white_to_move) {
  if (strcmp(move, "O-O") == 0) {
    return castle(board, SHORT, white_to_move);
  }

  if (strcmp(move, "O-O-O") == 0) {
    return castle(board, LONG, white_to_move);
  }

  piece_type_t piece_type;
  int piece_file = -1, piece_rank = -1, dest_file = -1, dest_rank = -1;
  piece_type_t promotion_type = QUEEN;
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

  if (move[i] == '=') {
    switch (move[i + 1]) {
    case 'Q':
      promotion_type = QUEEN;
      break;
    case 'R':
      promotion_type = ROOK;
      break;
    case 'N':
      promotion_type = KNIGHT;
      break;
    case 'B':
      promotion_type = BISHOP;
      break;
    default:
      return false;
    }

    i += 2;
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

  return move_piece(board, final_piece, dest_square, promotion_type);
}
