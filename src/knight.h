#ifndef KNIGHT_H
#define KNIGHT_H

#include "cons.h"
#include "bitboard.h"
#include "types.h"


namespace Kojiro {

extern uint64_t knight_attacks[64];

/**
 * @brief masking squares that can be attacked by the specified knight.
 * 
 * @param square 
 * @return uint64_t 
 */
uint64_t mask_knight_attacks(int square);

/**
 * @brief initialize king attacks for all 64 squares.
 * 
 */
void init_knight_attacks();

} // namespace Kojiro
#endif