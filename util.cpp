#include "util.h"

/*there is a hardware bit count for gcc which requieres cpu type with march arg in command 
    more analyse required to compare custome bit count and built in bit count performance :)
    cool link: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable*/
//#define count_bits(bitboard) __builtin_popcountll(bitboard)

// counts bits in a bitboard
int count_bits(uint64_t bitboard){
    int count = 0;
    while(bitboard){
        bitboard &= bitboard - 1;
        count++;
    }
    
    return count;
}