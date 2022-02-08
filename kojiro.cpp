// shoudl include or not? who cares?
//#include "stdio.h"
//#include "cstdint"

#include "bitboard.h"

int main(){
    uint64_t bitboard = 0ull;

    printf("im the greates loser...\n");

    print_bitboard(bitboard);

    set_bit(bitboard, e4);
    set_bit(bitboard, d5);

    print_bitboard(bitboard);

    pop_bit(bitboard, d5);

    print_bitboard(bitboard);
    getchar();
    
    return 0;    
}