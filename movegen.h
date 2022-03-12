#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"
#include "position.h"


struct moveInfo
{
    int source;
    int target;
    int piece;
    int promoted;
    int capture;
    int double_push;
    int enpassant;
    int castling;
};

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

int encode_move(int source, int target, int piece, int promoted, int capture, int double_push, int enpassant, int castling);

moveInfo decode_move(int move);

void print_move(int move);

void print_move_list(moves *move_list);

static inline void add_move(moves *move_list, int move){
    move_list->moves[move_list->count] = move;
    
    move_list->count++;
}

#endif