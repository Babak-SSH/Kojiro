/***********************************************\
=================================================
                BIT MANIPULATION
=================================================
\***********************************************/


#include "bitboard.h"


/////////////////////////////////////////////////
// iterates through every file and rank and 
// prints a simple represntaion of the board.
/////////////////////////////////////////////////
void print_bitboard(uint64_t  bitboard){
    int s;
    for (int rank = 0; rank < 8; rank++){
        printf("%d ", 8-rank);
        for (int file = 0; file < 8; file++){
            s = rank * 8 + file;
            printf(" %d ", get_bit(bitboard, square(s)));
        }
        printf("\n");
    }
    printf("   a  b  c  d  e  f  g  h\n\n");
}

/////////////////////////////////////////////////
// set a bit to 1 with or operater.
/////////////////////////////////////////////////
void set_bit(uint64_t &bitboard, int pos){
    bitboard |= (1ull << pos);
}

/////////////////////////////////////////////////
// get the value of the bit (1 or 0).
/////////////////////////////////////////////////
int get_bit(uint64_t bitboard, int pos){
    return bitboard & (1ull << pos) ? 1:0; 
}

/////////////////////////////////////////////////
// remove value 1 and turn it to 0 with xor operator.
/////////////////////////////////////////////////
void pop_bit(uint64_t &bitboard, int pos){
    bitboard ^= (1ull << pos);
}

/////////////////////////////////////////////////
// returns some orintaion regarding of each attack
// mask and index of sliding pieces.
/////////////////////////////////////////////////
uint64_t set_occupancy(int index, int mask_bit_count, uint64_t attack_mask){
    uint64_t occupancy = 0ull;

    for (int i = 0; i < mask_bit_count; i++){
        int square = get_ls1b_index(attack_mask);

        pop_bit(attack_mask, square);

        if(index & (1ull << square))
            occupancy |= (1ull << square);
    }

    return occupancy;
}
