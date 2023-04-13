#include "bitboard.h"


namespace Kojiro {
    
/// iterates through every file and rank and 
/// prints a simple represntaion of the bitboard.
void print_bitboard(uint64_t  bitboard) {
    int s;
    for (int rank = 0; rank < 8; rank++) {
        printf("%d ", 8-rank);
        for (int file = 0; file < 8; file++) {
            s = rank * 8 + file;
            printf(" %d ", get_bit(bitboard, square(s)));
        }
        printf("\n");
    }
    printf("   a  b  c  d  e  f  g  h\n\n");
    printf("BITBOARD: %lud\n\n", bitboard);
}

/// set a bit to 1 using or operater.
void set_bit(uint64_t &bitboard, int sq) {
    bitboard |= (1ull << sq);
}

/// get the value of the bit (1 or 0).
int get_bit(uint64_t bitboard, int sq) {
    return bitboard & (1ull << sq) ? 1:0; 
}

/// change 1s to 0 using xor operator.
void pop_bit(uint64_t &bitboard, int sq) {
    /// @TODO &= ~
    bitboard ^= (1ull << sq);
}

/* @TODO there is a hardware bit count for gcc which requieres cpu type with march arg in command 
    more analyse required to compare custom bit count and built in bit count performance :)
    cool link: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable*/
//#define count_bits(bitboard) __builtin_popcountll(bitboard)

/// counts bits in a bitboard by removing each 1 bit 
/// in each loop until the bitboard became empty.
int count_bits(uint64_t bitboard) {
    int count = 0;
    while(bitboard) {
        bitboard &= bitboard - 1;
        count++;
    }
    
    return count;
}

/// gets the index of the least significant 1st bit in the bitboard. 
/// (bb & -bb) will remove all 1s except lsb 1 and by summing with -1
/// we will convert all zeros behind the lsb to 1 and count them to find
/// the index.
int get_ls1b_index(uint64_t bitboard) {
    int idx;
    if(bitboard) {
        /* @TODO try shifting*/
        idx = count_bits((bitboard & -bitboard)-1);
        return idx;
    }
    // if bitboard is invalid
    else
        return -1;
}

/// creates orientaion of occupancy according to index bit form.
uint64_t set_occupancy(int index, uint64_t attack_mask) {
    uint64_t occupancy = 0ull;
    int bit_count = count_bits(attack_mask);

    for (int i = 0; i < bit_count; i++) {
        int square = get_ls1b_index(attack_mask);

        pop_bit(attack_mask, square);

        if(index & (1ull << i))
            occupancy |= (1ull << square);
    }

    return occupancy;
}

uint64_t set_rank(int rank) {
    uint64_t mask = RANK1;

    if (rank < 0 | rank > 7) 
        return 0;
    return mask >> (8*(rank));
}

uint64_t set_file(int file) {
    uint64_t mask = A_file;
    
    if (file < 0 | file > 7)
        return 0;
    return mask << file;
}

} // namespace Kojiro