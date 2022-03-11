#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"
#include "position.h"


moves* generate_pawn_moves(moves* move_list, Color side);

moves* generate_king_moves(moves* move_list);

moves* generate_moves(PieceType piece, moves* move_list);

void generate_all(moves* move_list, Color side);
  
/*
          binary move bits                               hexidecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/
/// @todo define structure or fluent interface
static inline int encode_move(int source, int target, int piece, int promoted, int capture, int double_push, int enpassant, int castling){
    (source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double_push << 21) |    \
    (enpassant << 22) | \
    (castling << 23);
}

static inline void add_move(moves* move_list, int move){
    move_list->moves[move_list->count] = move;
    
    move_list->count++;
}
#endif