/***********************************************\
=================================================
                PAWN ATTACKS
=================================================
\***********************************************/


#include "pawn.h"


/////////////////////////////////////////////////
// masking position that can be attacked by the 
// specified pawn. this is done by shifting 
// 7 and 9 bits according to the color and taking
// exceptaions of A and H file to considration.
/////////////////////////////////////////////////
uint64_t mask_pawn_attacks(int pos, int side){
    uint64_t bitboard = 0ull;
    uint64_t mask_bitboard = 0ull;

    set_bit(bitboard, pos);

    // white
    if(!side)
        mask_bitboard = ((bitboard >> 7) & not_A_file) | ((bitboard >> 9) & not_H_file);
    // black
    else
        mask_bitboard = ((bitboard << 7) & not_H_file) | ((bitboard << 9) & not_A_file);

    return mask_bitboard;
}

/////////////////////////////////////////////////
// save pre calculate moves????
/////////////////////////////////////////////////
// for (int i = 0 ; i < 64; i++){
    //     printf("\nwhite\n");
    //     print_bitboard(mask_pawn_attacks(i, white));

    //     printf("\nblack\n");
    //     print_bitboard(mask_pawn_attacks(i, black));
    // }