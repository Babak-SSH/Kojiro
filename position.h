#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <cstring>

#include "cons.h"
#include "bitboard.h"
#include "types.h"


// ASCII pieces
extern const std::string ascii_pieces;

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

void print_board();

void init_start();

std::string get_fen();

void parse_fen(const std::string& fen);

void set_piece(Piece p, int sq);

inline void set_piece(Piece p, int sq){
    set_bit(bitboards[p], sq);
    // getting the color of the piece whit shifting (0-7 w, 7-11 b)
    occupancies[sq >> 3] |= bitboards[p];
    occupancies[2]  |= bitboards[p];
}

#endif