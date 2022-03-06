/***********************************************\
=================================================
                    CONSTANTS
=================================================
\***********************************************/


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

// numbers are bitboards in ull format
const uint64_t not_A_file = 18374403900871474942ull;
const uint64_t not_H_file = 9187201950435737471ull;
const uint64_t not_AB_file = 18229723555195321596ull;
const uint64_t not_GH_file = 4557430888798830399ull;

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
