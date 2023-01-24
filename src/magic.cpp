#include "rook.h"
#include <string.h>
#include <sstream>
#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "magic.h"


namespace Kojiro {

// random seed. 1804289383 is the first number generated with rand() function in "linux"
unsigned int seed_state = 1804289383;

/// generate 32-bit pseudo random number with xorshift algorithm.
/// @TODO possible candidates to replace xorshift : xoshiro256** - xorshift*
int get_random_U32_number() {
    unsigned int number = seed_state;
    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    seed_state = number;

    return number;
}

/// generate 64-bit pseudo random number by putting together first 16 bits of 
/// four 32-bit random numbers.
uint64_t get_random_U64_number() {
    // define 4 random numbers
    uint64_t number, n1, n2, n3, n4;
    
    // slicing 16 bits from MS1B sides of random numbers
    n1 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
    n2 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
    n3 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
    n4 = (uint64_t)(get_random_U32_number()) & 0xFFFF;
    
    // putting toghether the first 16 bits of each part to create
    // a new 64 bit random number.
    number = n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);

    return number;
}

/// get random number with few 1s by performing and between 3 random numbers.
uint64_t random_fewbits() {
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

// generate magics.(yoinked from stockfish source code :))
// www.chessprogramming.org/Looking_for_Magics
// @TODO needs rework!
uint64_t find_magic(int square, int relevant_bits, PieceType type) {
    uint64_t occupancies[4096];
    uint64_t attacks[4096];
    uint64_t used_attacks[4096];
    uint64_t attack_mask;
    
    if(type == BISHOP) {
        attack_mask = mask_bishop_attacks_relevant(square);
    }
    else if(type == ROOK) {
        attack_mask = mask_rook_attacks_relevant(square);
    }
    
    // 2 ^ relevant_bits all possible orientations
    int occupancy_indicies = 1 << relevant_bits;
    
    // loop over occupancy indicies
    for (int index = 0; index < occupancy_indicies; index++) {
        occupancies[index] = set_occupancy(index, attack_mask);
    
        if(type == BISHOP){
            attacks[index] = mask_bishop_attacks_on_fly(square, occupancies[index]);
        }
        else if(type == ROOK){
            attacks[index] = mask_rook_attacks_on_fly(square, occupancies[index]);
        }
    }
    
    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++) {
        uint64_t magic_number = random_fewbits();
        
        // skip inappropriate magic numbers ( but how, why, who ?????????)
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
        
        memset(used_attacks, 0ULL, sizeof(used_attacks));
        
        int index, fail;
        
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++) {
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));
            
            // if magic index works
            if (used_attacks[magic_index] == 0ULL)
                used_attacks[magic_index] = attacks[index];
            else if (used_attacks[magic_index] != attacks[index])
                // magic index doesn't work
                fail = 1;
        }
        
        // if magic number works
        if (!fail)
            return magic_number;
    }
    
    // if magic number doesn't work
    fmt::print("Magic number failed!");
    return 0ULL;
}

/// bishop_magics is predefined and constant but can be generated 
/// with another random seed or another PRNG algorithm. 
void init_bishop_magics() {
    for (int square = 0; square < 64; square++)
        bishop_magics[square] = find_magic(square, bishop_relevant_bits[square], BISHOP);
}

/// rook_magics is predefined and constant but can be generated 
/// with another random seed or another PRNG algorithm. 
void init_rook_magics() {
    for (int square = 0; square < 64; square++)
        rook_magics[square] = find_magic(square, rook_relevant_bits[square], ROOK);
}

} // namespace Kojiro