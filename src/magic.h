#ifndef MAGIC_H
#define MAGIC_H

#include "cons.h" 
#include "bitboard.h"
#include "types.h"
#include "bishop.h"
#include "rook.h"


extern unsigned int seed_state;

/**
 * @brief Get random unsigned 32 bit number
 * 
 * @return int 
 */
int get_random_U32_number();

/**
 * @brief Get random unsigned 64 bit number
 * 
 * @return uint64_t 
 */
uint64_t get_random_U64_number();

/**
 * @brief get random 64 bit number
 * 
 * @return uint64_t 
 */
uint64_t random_fewbits();

/**
 * @brief find the magic number
 * 
 * @param square position on board
 * @param relevant_bits count of relevant bits
 * @param type rook or bishop (sliding pieces)
 * @return uint64_t 
 */
uint64_t find_magic(int square, int relevant_bits, PieceType type);

#endif