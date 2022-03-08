#ifndef PAWN_H
#define PAWN_H

#include "cons.h"
#include "bitboard.h"
#include "types.h"

extern uint64_t pawn_attacks[2][64];

/**
 * @brief masking squares that can be attacked by the specified pawn.
 * 
 * @param square 
 * @param side (black or white)
 * @return uint64_t bitboard
 */
uint64_t mask_pawn_attacks(int square, int side);

/**
 * @brief initialize pawn attacks for all 64 squares.
 * 
 */
void init_pawn_attacks();

#endif