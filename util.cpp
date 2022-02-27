#include "util.h"

unsigned int seed_state = 1804289383;

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
// generate 32-bit pseudo random number. 
/////////////////////////////////////////////////
int get_random_number(){
    unsigned int number = seed_state;
    // XOR shift algorithm
    number ^= number << 13;
    number ^= number << 17;
    number ^= number << 5;

    seed_state = number;

    return number;
}
