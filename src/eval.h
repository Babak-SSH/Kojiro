#ifndef EVAL_H
#define EVAL_H

#include "types.h"
#include "position.h"


namespace Kojiro {

/// @todo may remove const in future enhancements.
/// @todo testing Simplified Evaluation Function -> https://www.chessprogramming.org/Simplified_Evaluation_Function next try https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function

const int mate_value = 49000;
const int mate_score = 48000; // it's not infinity :)

const int material_score[12] = {
    100,      // white pawn score
    305,      // white knight scrore
    333,      // white bishop score
    563,      // white rook score
    950,      // white queen score
  10000,      // white king score
   -100,      // black pawn score
   -305,      // black knight scrore
   -333,      // black bishop score
   -563,      // black rook score
   -950,      // black queen score
 -10000,      // black king score
};

// pawn positional score
const int pawn_score[64] = 
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  20,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_score[64] = 
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,  -5,   0,  10,  10,   0,  -5,  -5,
    -5,   5,  10,  15,  15,  10,   5,  -5,
    -5,  10,  15,  20,  20,  15,  10,  -5,
    -5,  10,  15,  20,  20,  15,  10,  -5,
    -5,   5,  10,  15,  15,  10,   5,  -5,
    -5,  -5,   0,   5,   5,   0,  -5,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_score[64] = 
{
   -20,   0,   0,   0,   0,   0,   0, -20,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
   -20,   0, -10,   0,   0, -10,   0, -20

};

// rook positional score
const int rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    25,  50,  50,  50,  50,  50,  50,  25,
    -5,   0,  10,  20,  20,  10,   0,  -5,
    -5,   0,  10,  20,  20,  10,   0,  -5,
    -5,   0,  10,  20,  20,  10,   0,  -5,
    -5,   0,  10,  20,  20,  10,   0,  -5,
    -5,   0,  10,  20,  20,  10,   0,  -5,
    -5,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

const int double_pawn_penalty = -3;

const int isolated_pawn_penalty = -2;

const int passed_pawn_bonus[8] = { 0, 1, 5, 50, 75, 100, 150, 200 }; 

const int semi_open_file_score = 4;

const int open_file_score = 10;

const int king_shield_bonus = 2;


//mirror positional score tables for opposite side
const int mirror_score[128] =
{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};

namespace Eval {

    void init_eval_masks();

    int evaluation();

	static inline int mirror_square(int sq){
		return mirror_score[sq];
	}
} // namespace Eval

} // namespace Kojiro

#endif