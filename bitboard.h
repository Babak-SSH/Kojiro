#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <stdio.h>

// names representing each square in chess board 
// according to their file and rank
enum square {a8,b8,c8,d8,e8,f8,g8,h8,
             a7,b7,c7,d7,e7,f7,g7,h7,
             a6,b6,c6,d6,e6,f6,g6,h6,
             a5,b5,c5,d5,e5,f5,g5,h5,
             a4,b4,c4,d4,e4,f4,g4,h4,
             a3,b3,c3,d3,e3,f3,g3,h3,
             a2,b2,c2,d2,e2,f2,g2,h2,
             a1,b1,c1,d1,e1,f1,g1,h1};

enum color {white, black};

enum piece {king, queen, rook, bishop, knight, pawn};

void print_bitboard(uint64_t bitboard);
void set_bit(uint64_t &bitboard, int pos);
int get_bit(uint64_t bitboard, int pos);
void pop_bit(uint64_t &bitboard, int pos);
int count_bits(uint64_t bitboard);
int get_ls1b_index(uint64_t bitboard);
uint64_t set_occupancy(int index, uint64_t attack_mask);

#endif