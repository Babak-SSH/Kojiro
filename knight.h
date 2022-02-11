#ifndef KNIGHT_H
#define KNIGHT_H

#include <cstdint>
#include "bitboard.h"

// enum color {white, black};
extern const uint64_t not_A_file;
extern const uint64_t not_H_file;
extern const uint64_t not_AB_file;
extern const uint64_t not_GH_file;

uint64_t mask_knight_attacks(int pos);

#endif