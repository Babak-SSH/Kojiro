#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"
#include "move.h"
#include "position.h"


namespace Kojiro {

namespace MoveGen {
/**
 * @brief generate all legal pawn moves in current state.
 * 
 * @param move_list 
 * @param side 
 */
void generate_pawn_moves(moves *move_list, const Position& pos);

/**
 * @brief generate all legal king moves in current state.
 * 
 * @param move_list 
 * @param side 
 */
void generate_king_moves(moves *move_list, const Position& pos);

/**
 * @brief generate all legal sliding pieces and knight moves in current state.
 * 
 * @param move_list 
 * @param side 
 * @param pType 
 */
void generate_moves(moves *move_list, const Position& pos, PieceType pType);

/**
 * @brief generate all legal moves. 
 * 
 * @param move_list 
 * @param side 
 */
void generate_all(moves *move_list, const Position& pos);

static inline void add_move(moves *move_list, int move){
    move_list->moves[move_list->count] = move;
    
    move_list->count++;
}

} // namespace MoveGen

} // namespace Kojiro

#endif