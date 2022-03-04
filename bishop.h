#ifndef BISHOP_H
#define BISHOP_H

#include "bitboard.h"

uint64_t generate_bishop_attacks_on_fly(int pos, uint64_t block);
uint64_t mask_bishop_attacks_relative(int pos);

#endif