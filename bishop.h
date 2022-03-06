#ifndef BISHOP_H
#define BISHOP_H

#include "bitboard.h"
#include "magic.h"
#include "types.h"


extern uint64_t bishop_magics[64];
extern uint64_t bishop_masks[64];
extern uint64_t bishop_attacks[64][4096];

/**
 * @brief masking square that can be attacked by the specified bishop.
 * 
 * @param square bishop position
 * @param block blocked squares in bitboard format
 * @return uint64_t mask_attacks
 */
uint64_t mask_bishop_attacks_on_fly(int square, uint64_t block);

/**
 * @brief masking relevant square that can be attacked by the specified bishop.
 * 
 * @param square bishop position
 * @return uint64_t mask_attacks 
 */
uint64_t mask_bishop_attacks_relevant(int square);

/**
 * @brief initialize bishop attacks with magic indexes(hash) bishop_attacks[64][4096]
 * 
 */
void init_bishop_attacks();

/**
 * @brief Get the specified bishopattacks with the given occupancy.
 * 
 * @param square position on board
 * @param occupancy occupancy bitboard
 * @return uint64_t
 */
uint64_t get_bishop_attacks(int square, uint64_t occupancy);

#endif