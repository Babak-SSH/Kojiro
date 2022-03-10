#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"


moves* generate_pawn_moves(moves* move_list, Color side);

moves* generate_king_moves(moves* move_list);

moves* generate_moves(PieceType piece, moves* move_list);

void generate_all(moves* move_list);
  
#endif