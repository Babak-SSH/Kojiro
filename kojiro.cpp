/***********************************************\
=================================================
                    MAIN
=================================================
\***********************************************/


// shoudl include or not? who cares?
//#include "stdio.h"
//#include "cstdint"

#include "bitboard.h"
#include "pawn.h"
#include "knight.h"
#include "king.h"


int main(){
    uint64_t bitboard = 0ull;

    printf("im the greates loser...\n");
    print_bitboard(mask_king_attacks(e4));
    return 0;    
}