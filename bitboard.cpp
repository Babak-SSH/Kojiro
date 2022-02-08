#include "bitboard.h"


/////////////////////////////////////////////////
// iterates through every file and rank and 
// prints a simple represntaion of the board.
/////////////////////////////////////////////////
void print_bitboard(uint64_t  bitboard){
    int s;
    for (int rank = 0; rank < 8; rank++){
        printf("%c ", 'a'+rank);
        for (int file = 0; file < 8; file++){
            s = rank * 8 + file;
            printf(" %d ", get_bit(bitboard, square(s)));
        }
        printf("\n");
    }
    printf("   1  2  3  4  5  6  7  8\n\n");
}

/////////////////////////////////////////////////
// set a bit to 1 with or operater.
/////////////////////////////////////////////////
void set_bit(uint64_t &bitboard, square pos){
    bitboard |= (1ull << pos);
}

/////////////////////////////////////////////////
// get the value of the bit (1 or 0).
/////////////////////////////////////////////////
int get_bit(uint64_t bitboard, square pos){
    return bitboard & (1ull << pos) ? 1:0; 
}

/////////////////////////////////////////////////
// remove value 1 and turn it to 0 with xor operator.
/////////////////////////////////////////////////
void pop_bit(uint64_t &bitboard, square pos){
    bitboard ^= (1ull << pos);
}