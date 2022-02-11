#ifndef KING_H
#define KING_H

#include "cons.h" 
#include "bitboard.h"

// enum color {white, black};
// extern const uint64_t not_A_file;
// extern const uint64_t not_H_file;

uint64_t mask_king_attacks(int pos);

#endif