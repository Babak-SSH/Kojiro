#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"
#include "position.h"


namespace Kojiro {

struct moveInfo
{
	/// source square
    int source;
	/// target square
    int target;
	/// piece type
    int piece;
	/// is_promoted
    int promoted;
	/// is_move_capture
    int capture;
	/// is_double_push
    int double_push;
	/// is_enpassant
    int enpassant;
	/// is_castling
    int castling;
};

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

/**
 * @brief generate all legal pawn moves in current state.
 * 
 * @param move_list 
 * @param side 
 */
void generate_pawn_moves(moves *move_list, Color side);

/**
 * @brief generate all legal king moves in current state.
 * 
 * @param move_list 
 * @param side 
 */
void generate_king_moves(moves *move_list, Color side);

/**
 * @brief generate all legal sliding pieces and knight moves in current state.
 * 
 * @param move_list 
 * @param side 
 * @param pType 
 */
void generate_moves(moves *move_list, Color side, PieceType pType);

/**
 * @brief generate all legal moves. 
 * 
 * @param move_list 
 * @param side 
 */
void generate_all(moves *move_list, Color side);

/**
 * @brief encode move data
 * 
 * @param source 
 * @param target 
 * @param piece 
 * @param promoted 
 * @param capture 
 * @param double_push 
 * @param enpassant 
 * @param castling 
 * @return int 
 */
int encode_move(int source, int target, int piece, int promoted, int capture, int double_push, int enpassant, int castling);

/**
 * @brief decode move.
 * 
 * @param move 
 * @return moveInfo 
 */
moveInfo decode_move(int move);

/**
 * @brief Get move as human readable string.
 * 
 * @param move 
 * @return std::string 
 */
std::string get_move_string(int move);

/**
 * @brief print move list.
 * 
 * @param move_list 
 */
void print_move_list(moves *move_list);

/// @todo check if making these are good practice or not.
/// check for capture in move
inline int get_move_capture(int move){
	return (move & 0x100000);
}

/// check for promotion in move
inline int get_move_promoted(int move){
	return ((move & 0xf0000) >> 16);
}

static inline void add_move(moves *move_list, int move){
    move_list->moves[move_list->count] = move;
    
    move_list->count++;
}

} // namespace Kojiro

#endif