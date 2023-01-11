#include "cons.h"

// an array to convert index numbers to square positions.
const char *convert_to_square[64] = {"a8","b8","c8","d8","e8","f8","g8","h8",
                                     "a7","b7","c7","d7","e7","f7","g7","h7",
                                     "a6","b6","c6","d6","e6","f6","g6","h6",
                                     "a5","b5","c5","d5","e5","f5","g5","h5",
                                     "a4","b4","c4","d4","e4","f4","g4","h4",
                                     "a3","b3","c3","d3","e3","f3","g3","h3",
                                     "a2","b2","c2","d2","e2","f2","g2","h2",
                                     "a1","b1","c1","d1","e1","f1","g1","h1"};

// number repesention of ranks and files on bitboard.
const uint64_t A_file = 72340172838076673ull;
const uint64_t not_A_file = 18374403900871474942ull;
const uint64_t not_H_file = 9187201950435737471ull;
const uint64_t not_AB_file = 18229723555195321596ull;
const uint64_t not_GH_file = 4557430888798830399ull;

const uint64_t RANK1 = 0xff00000000000000;
const uint64_t RANK2 = (RANK1 >> (8*1));
const uint64_t RANK3 = (RANK1 >> (8*2));
const uint64_t RANK4 = (RANK1 >> (8*3));
const uint64_t RANK5 = (RANK1 >> (8*4));
const uint64_t RANK6 = (RANK1 >> (8*5));
const uint64_t RANK7 = (RANK1 >> (8*6));
const uint64_t RANK8 = (RANK1 >> (8*7));

// number of bits available(relative) for each bishop in each position of board.
const int bishop_relevant_bits[64] = {6, 5, 5, 5, 5, 5, 5, 6,
                                      5, 5, 5, 5, 5, 5, 5, 5,
                                      5, 5, 7, 7, 7, 7, 5, 5,
                                      5, 5, 7, 9, 9, 7, 5, 5,
                                      5, 5, 7, 9, 9, 7, 5, 5,
                                      5, 5, 7, 7, 7, 7, 5, 5,
                                      5, 5, 5, 5, 5, 5, 5, 5,
                                      6, 5, 5, 5, 5, 5, 5, 6};
// number of bits available(relative) for each rook in each position of board.
const int rook_relevant_bits[64] = {12, 11, 11, 11, 11, 11, 11, 12,
                                    11, 10, 10, 10, 10, 10, 10, 11,
                                    11, 10, 10, 10, 10, 10, 10, 11,
                                    11, 10, 10, 10, 10, 10, 10, 11,
                                    11, 10, 10, 10, 10, 10, 10, 11,
                                    11, 10, 10, 10, 10, 10, 10, 11,
                                    11, 10, 10, 10, 10, 10, 10, 11,
                                    12, 11, 11, 11, 11, 11, 11, 12};

/*
                           castling   move     in      in
                              right update     binary  decimal
 king & rooks didn't move:     1111 & 1111  =  1111    15
        white king  moved:     1111 & 1100  =  1100    12
  white king's rook moved:     1111 & 1110  =  1110    14
 white queen's rook moved:     1111 & 1101  =  1101    13
     
         black king moved:     1111 & 0011  =  1011    3
  black king's rook moved:     1111 & 1011  =  1011    11
 black queen's rook moved:     1111 & 0111  =  0111    7
*/

// castling rights update constants
const int castling_rights[64] = {
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};