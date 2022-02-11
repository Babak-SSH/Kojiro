/***********************************************\
=================================================
                KING ATTACKS
=================================================
\***********************************************/


#include "king.h"


/////////////////////////////////////////////////
// masking position that can be attacked by the 
// specified king.
/////////////////////////////////////////////////
uint64_t mask_king_attacks(int pos){
    uint64_t attacks = 0ull;
    uint64_t mask_bitboard = 0ull;

    set_bit(attacks, pos);

    // right
    mask_bitboard |= (attacks << 1) & not_A_file;
    // left
    mask_bitboard |= (attacks >> 1) & not_H_file;
    // down
    mask_bitboard |= (attacks << 8);
    // up
    mask_bitboard |= (attacks >> 8);
    // right up
    mask_bitboard |= (attacks >> 7) & not_A_file;
    // right down
    mask_bitboard |= (attacks << 9) & not_A_file;
    // left up
    mask_bitboard |= (attacks >> 9) & not_H_file;
    // left down
    mask_bitboard |= (attacks << 7) & not_H_file;

    return mask_bitboard;
}