#ifndef ROOK_H
#define ROOK_H

#include "bitboard.h"

uint64_t generate_rook_attacks_on_fly(int pos, uint64_t block);

uint64_t mask_rook_attacks_relative(int pos);

#endif