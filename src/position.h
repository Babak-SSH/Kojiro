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
#include "magic.h"
#include "search.h"


namespace Kojiro {

namespace Zobrist {
	extern uint64_t psq[12][64];
	extern uint64_t enpassant[64];
	extern uint64_t castling[16];
	extern uint64_t side;
}

struct StateInfo {
    uint64_t bitboards[12];
    uint64_t occupancies[3];

    int castle;
    int side;
    int rule50;
    int enpassant;
    int play_count;
	u_int64_t key; // position hash key 

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

/**
 * @brief return an ASCII representation of the position.
 * 
 */
void print_board();
uint64_t generate_hash_key();
/**
 * @brief Initialize hash keys.
 * 
 */
void init_hash();

/**
 * @brief Basic set bit instructions to build board.
 * 
 */
void init_start();

/**
 * @brief Initialize state variables.
 * 
 */
void init_state();

/**
 * @brief take state back.
 * 
 */
void take_back();

/**
 * @brief Get the fen string
 * 
 * @return std::string 
 */
std::string get_fen();

/**
 * @brief parse fen and assign to the variables.
 * 
 * @param fen 
 */
void parse_fen(const std::string& fen);

//void set_piece(Piece p, int sq);

/**
 * @brief  check if square is under attack.
 * 
 * @param square 
 * @param side 
 * @return true 
 * @return false 
 */
bool is_square_attacked(int square, int side);

/// @todo enhance color(side) finding
inline void set_piece(Piece p, int sq){
    set_bit(st->bitboards[p], sq);
    
    st->occupancies[(p > 5)] |= st->bitboards[p];
    st->occupancies[2] |= st->bitboards[p];
}

/**
 * @brief do the move on that state.
 * 
 * @param move 
 * @param move_flag 
 * @param newST 
 * @return int 
 */
int make_move(int move, int move_flag, StateInfo& newST);

extern long captures_count;
extern long captures_flag;
extern long enpassant_count;
extern long enpassant_flag;
extern long castles_count;
extern long castles_flag;

} // namespace Kojiro

#endif