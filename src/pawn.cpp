/***********************************************\
=================================================
                PAWN ATTACKS
=================================================
\***********************************************/


#include "pawn.h"


namespace Kojiro {

uint64_t pawn_attacks[2][64];

/// masking position that can be attacked by the 
/// specified pawn. this is done by shifting 
/// 7 or 9 bits according to the color and taking
/// exceptaions of A and H file to considration
/// because shiftin that ends up in this files gets
/// square to one upper rank.
uint64_t mask_pawn_attacks(int square, int side){
    uint64_t bitboard = 0ull;
    uint64_t mask_bitboard = 0ull;

    set_bit(bitboard, square);

    // white
    if(!side)
        mask_bitboard = ((bitboard >> 7) & not_A_file) | ((bitboard >> 9) & not_H_file);
    // black
    else
        mask_bitboard = ((bitboard << 7) & not_H_file) | ((bitboard << 9) & not_A_file);

    return mask_bitboard;
}

/// initialize pawn attacks for all 64 squares.
void init_pawn_attacks(){
    for (int sq = 0; sq < 64; sq++){
        // white pawn
        pawn_attacks[WHITE][sq] = mask_pawn_attacks(sq, WHITE);

        // black pawn
        pawn_attacks[BLACK][sq] = mask_pawn_attacks(sq, BLACK);
    }
}

} // namespace Kojiro