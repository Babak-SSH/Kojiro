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

    // rank, file
    int r, f;

    r = pos/8;
    f = pos%8;

    set_bit(attacks, pos);

    // up right
    for(int i = r-1, j = f+1, k=1; i >= 0 && j < 8; i--, j++, k++){
        mask_bitboard |= (attacks >> (7*k));
    }
    // up left
    for(int i = r-1, j = f-1, k=1; i >= 0 && j >= 0; i--, j--, k++){
        mask_bitboard |= (attacks >> (9*k));
    }
    // down right
    for(int i = r+1, j = f+1, k=1; i < 8 && j < 8; i++, j++, k++){
        mask_bitboard |= (attacks << (9*k));
    }
    // down left
    for(int i = r+1, j = f-1, k=1; i < 8 && j >= 0; i++, j--, k++){
        mask_bitboard |= (attacks << (7*k));
    }
    
    return mask_bitboard;
}

/////////////////////////////////////////////////
// masking relative position that can be attacked
// by the specified bishop.
// ?change this shity function name
/////////////////////////////////////////////////
uint64_t mask_bishop_attacks_relative(int pos){
    uint64_t attacks = 0ull;
    uint64_t mask_bitboard = 0ull;

    // rank, file
    int r, f;

    r = pos/8;
    f = pos%8;

    set_bit(attacks, pos);

    // up right
    for(int i = r-1, j = f+1, k=1; i > 0 && j < 7; i--, j++, k++){
        mask_bitboard |= (attacks >> (7*k));
    }
    // up left
    for(int i = r-1, j = f-1, k=1; i > 0 && j > 0; i--, j--, k++){
        mask_bitboard |= (attacks >> (9*k));
    }
    // down right
    for(int i = r+1, j = f+1, k=1; i < 7 && j < 7; i++, j++, k++){
        mask_bitboard |= (attacks << (9*k));
    }
    // down left
    for(int i = r+1, j = f-1, k=1; i < 7 && j > 0; i++, j--, k++){
        mask_bitboard |= (attacks << (7*k));
    }
    
    return mask_bitboard;
}