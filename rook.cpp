/***********************************************\
=================================================
                ROOK ATTACKS
=================================================
\***********************************************/


#include "rook.h"


/////////////////////////////////////////////////
// masking position that can be attacked by the 
// specified rook and not blocked by any other piece.
/////////////////////////////////////////////////
uint64_t generate_rook_attacks_on_fly(int pos, uint64_t block){
    uint64_t attack = 0ull;
    uint64_t mask_bitboard = 0ull;

    // rank, file
    int r, f;
    int i, j, k;

    r = pos/8;
    f = pos%8;

    set_bit(attack, pos);

    // up 
    for(i = r-1, k=1; i >= 0; i--, k++){
        mask_bitboard |= (attack >> (8*k));
        if((attack >> (8*k)) & block)  
            break;
    }
    // down 
    for(i = r+1, k=1; i < 8; i++, k++){
        mask_bitboard |= (attack << (8*k));
        if((attack >> (8*k)) & block)  
            break;
    }
    //  left
    for(j = f-1, k=1; j >= 0; j--, k++){
        mask_bitboard |= (attack >> (k));
        if((attack >> (8*k)) & block)  
            break;
    }
    //  right
    for(j = f+1, k=1; j < 8; j++, k++){
        mask_bitboard |= (attack << (k));
        if((attack >> (8*k)) & block)  
            break;
    }
    
    return mask_bitboard;
}

/////////////////////////////////////////////////
// masking relative position that can be attacked
// by the specified rook.
// ?change this shity function name
/////////////////////////////////////////////////
uint64_t mask_rook_attacks_relative(int pos){
    uint64_t attack = 0ull;
    uint64_t mask_bitboard = 0ull;

    // rank, file
    int r, f;
    int i, j, k;

    r = pos/8;
    f = pos%8;

    set_bit(attack, pos);

    // up 
    for(i = r-1, k=1; i > 0; i--, k++){
        mask_bitboard |= (attack >> (8*k));
    }
    // down 
    for(i = r+1, k=1; i < 7; i++, k++){
        mask_bitboard |= (attack << (8*k));
    }
    //  left
    for(j = f-1, k=1; j > 0; j--, k++){
        mask_bitboard |= (attack >> (k));
    }
    //  right
    for(j = f+1, k=1; j < 7; j++, k++){
        mask_bitboard |= (attack << (k));
    }
    
    return mask_bitboard;
}