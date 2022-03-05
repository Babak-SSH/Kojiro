#ifndef PAWN_H
#define PAWN_H

#include "cons.h"
#include "bitboard.h"

// enum color {white, black};


uint64_t mask_pawn_attacks(int pos, int side);

#endif