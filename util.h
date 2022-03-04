#ifndef UTIL_H
#define TUIL_H

#include "cons.h" 
#include "bitboard.h"

int count_bits(uint64_t bitboard);

int get_ls1b_index(uint64_t bitboard);

int get_random_U32_number();

uint64_t get_random_U64_number();

uint64_t find_magic(int square, int relevant_bits, int bishop);

#endif