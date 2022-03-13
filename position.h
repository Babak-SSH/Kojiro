#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <cstring>

#include "cons.h"
#include "bitboard.h"
#include "types.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "king.h"
#include "movegen.h"


struct StateInfo
{
    uint64_t bitboards[12];
    uint64_t occupancies[3];

    int castle;
    int side;
    int rule50;
    int enpassant;
    int play_count;

    StateInfo* previous;
};

// ASCII pieces
extern const std::string ascii_pieces;

// temp state info
extern StateInfo* st;

// piece bitboards
extern uint64_t bitboards[12];

// occupancy bitboards
extern uint64_t occupancies[3];

// side to move
extern int side;

// enpassant square
extern int enpassant; 

// castling rights
extern int castle;

extern int rule50;

extern int play_count;

void print_board();

void init_start();

void init_state();

void take_back();

std::string get_fen();

void parse_fen(const std::string& fen);

void set_piece(Piece p, int sq);

bool is_square_attacked(int square, int side);

/// @todo enhance color(side) finding
inline void set_piece(Piece p, int sq){
    set_bit(bitboards[p], sq);
    
    occupancies[(p > 5)] |= bitboards[p];
    occupancies[2] |= bitboards[p];
}

void make_move(int move, int move_flag, StateInfo& newST);

#endif