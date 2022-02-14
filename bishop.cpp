/***********************************************\
=================================================
                BISHOP ATTACKS
=================================================
\***********************************************/


#include "bishop.h"


/////////////////////////////////////////////////
// masking position that can be attacked by the 
// specified bishop.
/////////////////////////////////////////////////
uint64_t mask_bishop_attacks(int pos){
    uint64_t attacks = 0ull;
    uint64_t mask_bitboard = 0ull;

    set_bit(attacks, pos);

    return mask_bitboard;
}