#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <stdio.h>

#include "types.h"
#include "cons.h"


namespace Kojiro {
    
void print_bitboard(uint64_t bitboard);
void set_bit(uint64_t &bitboard, int pos);
int get_bit(uint64_t bitboard, int pos);
void pop_bit(uint64_t &bitboard, int pos);
int count_bits(uint64_t bitboard);
int get_ls1b_index(uint64_t bitboard);
uint64_t set_occupancy(int index, uint64_t attack_mask);

uint64_t set_rank(int rank);

uint64_t set_file(int file);

} // namespace Kojiro

#endif