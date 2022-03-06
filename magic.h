#ifndef MAGIC_H
#define MAGIC_H

#include "cons.h" 
#include "bitboard.h"


int get_random_U32_number();

uint64_t get_random_U64_number();

uint64_t random_fewbits();

uint64_t find_magic(int square, int relevant_bits, piece type);

#endif