#ifndef PAWN_H
#define PAWN_H

#include <cstdint>
#include "bitboard.h"

enum color {white, black};


uint64_t mask_pawn_attacks(int pos, int side);

#endif