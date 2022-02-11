/***********************************************\
=================================================
                KNIGHT ATTACKS
=================================================
\***********************************************/


#include "pawn.h"


const uint64_t not_A_file = 18374403900871474942ull;
const uint64_t not_H_file = 9187201950435737471ull;
const uint64_t not_AB_file = 18229723555195321596ull;
const uint64_t not_GH_file = 4557430888798830399ull;


/////////////////////////////////////////////////
// kngiiiiiiiiiits
/////////////////////////////////////////////////
uint64_t mask_knight_attacks(int pos){
    uint64_t attacks = 0ull;
    uint64_t mask_bitboard = 0ull;

    set_bit(attacks, pos);

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