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
    printf("BITBOARD: %llud\n\n", bitboard);
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
    // &= ~
    bitboard ^= (1ull << pos);
}

/*there is a hardware bit count for gcc which requieres cpu type with march arg in command 
    more analyse required to compare custome bit count and built in bit count performance :)
    cool link: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable*/
//#define count_bits(bitboard) __builtin_popcountll(bitboard)
/////////////////////////////////////////////////
// counts bits in a bitboard by removing each 1 bit 
// in each loop until the bitboard becomes 0.
/////////////////////////////////////////////////
int count_bits(uint64_t bitboard){
    int count = 0;
    while(bitboard){
        bitboard &= bitboard - 1;
        count++;
    }
    
    return count;
}

/////////////////////////////////////////////////
// gets the index of the least significant 1st bit
// in the bitboard. (bb & -bb) will remove all 1s 
// except lsb 1 and by summing with -1 we will convert
// all zeros behind the lsb to 1 and count them to find
// the index.
/////////////////////////////////////////////////
int get_ls1b_index(uint64_t bitboard){
    int idx;
    if(bitboard){
        /* should try shifting too*/
        idx = count_bits((bitboard & -bitboard)-1);
        // printing square name
        // printf("%s", convert_to_square[idx]);
        return idx;
    }
    // if bitboard is invalid
    else
        return -1;
}

/////////////////////////////////////////////////
// returns some orintaion regarding of each attack
// mask and index of sliding pieces.
/////////////////////////////////////////////////
uint64_t set_occupancy(int index, uint64_t attack_mask){
    uint64_t occupancy = 0ull;
    int bit_count = count_bits(attack_mask);

    for (int i = 0; i < bit_count; i++){
        int square = get_ls1b_index(attack_mask);

        pop_bit(attack_mask, square);

        if(index & (1ull << i))
            occupancy |= (1ull << square);
    }

    return occupancy;
}
