#include <string.h>

#include "magic.h"
#include "bishop.h"
#include "rook.h"


unsigned int seed_state = 1804289383;

/////////////////////////////////////////////////
// generate 32-bit pseudo random number. 
// possible candidates to replace xorshift : xoshiro256** - xorshift*
/////////////////////////////////////////////////
int get_random_U32_number(){
    unsigned int number = seed_state;
    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    seed_state = number;

    return number;
}

/////////////////////////////////////////////////
// generate 64-bit pseudo random number. 
/////////////////////////////////////////////////
uint64_t get_random_U64_number(){
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

// get random few bits
uint64_t random_fewbits() {
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

/////////////////////////////////////////////////
// generate magics.(yoinked from stockfish source code :))
// www.chessprogramming.org/Looking_for_Magics
// needs rework!
/////////////////////////////////////////////////
uint64_t find_magic(int square, int relevant_bits, piece type)
{
    uint64_t occupancies[4096];
    uint64_t attacks[4096];
    uint64_t used_attacks[4096];
    uint64_t attack_mask;
    
    if(type == bishop){
        attack_mask = mask_bishop_attacks_relative(square);
    }
    else if(type == rook){
        attack_mask = mask_rook_attacks_relative(square);
    }
    
    int occupancy_indicies = 1 << relevant_bits;
    
    // loop over occupancy indicies
    for (int index = 0; index < occupancy_indicies; index++)
    {
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);
    
        if(type == bishop){
            attacks[index] = generate_bishop_attacks_on_fly(square, occupancies[index]);
        }
        else if(type == rook){
            attacks[index] = generate_rook_attacks_on_fly(square, occupancies[index]);
        }
    }
    
    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        uint64_t magic_number = random_fewbits();
        
        // skip inappropriate magic numbers
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;
        
        memset(used_attacks, 0ULL, sizeof(used_attacks));
        
        int index, fail;
        
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
        {
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
    printf("  Magic number fails!\n");
    return 0ULL;
}