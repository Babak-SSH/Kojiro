#ifndef ROOK_H
#define ROOK_H

#include "bitboard.h"
#include "types.h"


namespace Kojiro {

extern uint64_t rook_magics[64];
extern uint64_t rook_masks[64];
extern uint64_t rook_attacks[64][4096];

/**
 * @brief masking square that can be attacked by the specified rook.
 * 
 * @param square rook position
 * @param block blocked squares in bitboard format
 * @return uint64_t mask_attacks
 */
uint64_t mask_rook_attacks_on_fly(int square, uint64_t block);

/**
 * @brief masking relevant square that can be attacked by the specified rook.
 * 
 * @param square rook position
 * @return uint64_t mask_attacks 
 */
uint64_t mask_rook_attacks_relevant(int square);

/**
 * @brief initialize rook attacks with magic indexes(hash) rook_attacks[64][4096]
 * 
 */
void init_rook_attacks();

/**
 * @brief Get the specified rookattacks with the given occupancy.
 * 
 * @param square position on board
 * @param occupancy occupancy bitboard
 * @return uint64_t
 */
uint64_t get_rook_attacks(int square, uint64_t occupancy);

} // namespace Kojiro
#endif