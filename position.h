#ifndef POSITION_H
#define POSITION_H

#include <string.h>

#include "cons.h"
#include "bitboard.h"
#include "types.h"


// ASCII pieces
extern const char ascii_pieces[13];

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

#endif