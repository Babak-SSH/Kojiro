#ifndef KING_H
#define KING_H

#include "cons.h" 
#include "bitboard.h"
#include "types.h"


namespace Kojiro {

extern uint64_t king_attacks[64];

/**
 * @brief masking sqition that can be attacked by the specified king.
 * 
 * @param sq 
 * @return uint64_t 
 */
uint64_t mask_king_attacks(int sq);

/**
 * @brief initialize king attacks for all 64 squares.
 * 
 */
void init_king_attacks();

} // namespace Kojiro
#endif