#ifndef ROOK_H
#define ROOK_H

#include "bitboard.h"

uint64_t mask_rook_attacks(int pos);
uint64_t mask_rook_attacks_relative(int pos);

#endif