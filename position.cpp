#include "position.h"


// ASCII pieces
char ascii_pieces[13] = "PNBRQKpnbrqk";

// unicode pieces
char *unicode_pieces[12] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};

// piece bitboards
uint64_t bitboards[12];

// occupancy bitboards
uint64_t occupancies[3];

// side to move
int side = -1;

// enpassant square
int enpassant = no_sq; 

// castling rights
int castle;