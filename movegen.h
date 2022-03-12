#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"
#include "position.h"


void generate_pawn_moves(moves *move_list, Color side);

void generate_king_moves(moves *move_list, Color side);

void generate_moves(moves *move_list, Color side, PieceType pType);

void generate_all(moves *move_list, Color side);
  
// promoted pieces
static char promoted_pieces[] = {
    'Q',
    'R',
    'B',
    'N',
    'q',
    'r',
    'b',
    'n'
};

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
    return ((source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double_push << 21) |    \
    (enpassant << 22) | \
    (castling << 23));
}

static inline moveInfo decode_move(int move){
    moveInfo info;

    info.source = (move & 0x3f);
    info.target = ((move & 0xfc0) >> 6);
    info.piece = ((move & 0xf000) >> 12);
    info.promoted = ((move & 0xf0000) >> 16);
    info.capture = (move & 0x100000);
    info.double_push = (move & 0x200000);
    info.enpassant = (move & 0x400000);
    info.castling = (move & 0x800000);

    return info;
}

static inline void add_move(moves *move_list, int move){
    move_list->moves[move_list->count] = move;
    
    move_list->count++;
}

void print_move(int move);

void print_move_list(moves *move_list);

#endif