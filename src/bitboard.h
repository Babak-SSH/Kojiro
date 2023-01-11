#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <stdio.h>

#include "types.h"
#include "misc.h"
#include "cons.h"


namespace Kojiro {

/**
 * @brief prints a simple represntaion of the bitboard.
 * 
 * @param bitboard 
 */
void print_bitboard(uint64_t bitboard);

/**
 * @brief Set the bit to 1.
 * 
 * @param bitboard 
 * @param pos 
 */
void set_bit(uint64_t &bitboard, int pos);

/**
 * @brief Get value of the bit.
 * 
 * @param bitboard 
 * @param pos 
 * @return int 
 */
int get_bit(uint64_t bitboard, int pos);

/**
 * @brief change 1 to 0. (pop bit)
 * 
 * @param bitboard 
 * @param pos 
 */
void pop_bit(uint64_t &bitboard, int pos);

/**
 * @brief count bits in bitboard.
 * 
 * @param bitboard 
 * @return int 
 */
int count_bits(uint64_t bitboard);

/**
 * @brief gets the index of the least significant 1st bit in the bitboard.
 * 
 * @param bitboard 
 * @return int 
 */
int get_ls1b_index(uint64_t bitboard);

/**
 * @brief creates orientaion of occupancy according to index bit form.
 * 
 * @param index 
 * @param attack_mask 
 * @return uint64_t 
 */
uint64_t set_occupancy(int index, uint64_t attack_mask);

/**
 * @brief Set the rank to 1.
 * 
 * @param rank 
 * @return uint64_t 
 */
uint64_t set_rank(int rank);

/**
 * @brief Set the file to 1.
 * 
 * @param file 
 * @return uint64_t 
 */
uint64_t set_file(int file);

} // namespace Kojiro

#endif