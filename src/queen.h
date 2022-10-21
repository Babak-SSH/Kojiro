#ifndef QUEEN_H
#define QUEEN_H

#include "bitboard.h"
#include "magic.h"
#include "types.h"


namespace Kojiro {

/**
 * @brief Get the specified queenattacks with the given occupancy.
 * 
 * @param square position on board
 * @param occupancy occupancy bitboard
 * @return uint64_t
 */
uint64_t get_queen_attacks(int square, uint64_t occupancy);

} // namespace Kojiro
#endif