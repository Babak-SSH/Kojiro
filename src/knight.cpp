/***********************************************\
=================================================
                KNIGHT ATTACKS
=================================================
\***********************************************/


#include "knight.h"


namespace Kojiro {

uint64_t knight_attacks[64];

/// masking squares that can be attacked by the 
/// specified knight.
uint64_t mask_knight_attacks(int square){
    uint64_t attacks = 0ull;
    uint64_t mask_bitboard = 0ull;

    set_bit(attacks, square);

    // up
    mask_bitboard |= (attacks >> 15) & not_A_file;
    mask_bitboard |= (attacks >> 17) & not_H_file;
    // down
    mask_bitboard |= (attacks << 15) & not_H_file;
    mask_bitboard |= (attacks << 17) & not_A_file;
    // right
    mask_bitboard |= (attacks >> 6) & not_AB_file;
    mask_bitboard |= (attacks << 10) & not_AB_file;
    // left
    mask_bitboard |= (attacks << 6) & not_GH_file;
    mask_bitboard |= (attacks >> 10) & not_GH_file;

    return mask_bitboard;
}

/// initialize knight attacks for all 64 squares.
void init_knight_attacks(){
    for (int sq = 0; sq < 64; sq++){
        knight_attacks[sq] = mask_knight_attacks(sq);
    }
}

} // namespace Kojiro