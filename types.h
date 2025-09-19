#pragma once

#include <stdbool.h>

typedef enum PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING } piece_type_t;

typedef enum Castle { SHORT, LONG } castle_t;

typedef enum PieceColor { WHITE, BLACK } piece_color_t;

typedef enum DrawOffer { WHITE_OFFERED, BLACK_OFFERED, NO_OFFER } draw_offer_t;

typedef enum GameOver {
  CHECKMATE,
  RESIGNATION,
  STALEMATE,
  DRAW_OFFER
} gameover_t;

typedef struct Square square_t;

typedef struct Piece {
  piece_type_t type;
  piece_color_t color;
  square_t *square;
  bool has_moved;
} piece_t;

typedef struct Square {
  piece_t *piece;
  unsigned int rank : 3;
  unsigned int file : 3;
} square_t;

typedef struct Board {
  square_t squares[8][8];
  piece_t *white_king;
  piece_t *black_king;
  piece_t *enpassantable_pawn;
  draw_offer_t draw_offer;
} board_t;
