/***********************************************\
=================================================
                BISHOP ATTACKS
=================================================
\***********************************************/


#include "bishop.h"


// bishop magic numbers
uint64_t bishop_magics[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};
// bishop attacks(relevant) masks
uint64_t bishop_masks[64];
// bishop attacks table [square][occupancies]
uint64_t bishop_attacks[64][4096];

/// masking square that can be attacked by the specified
/// bishop (this also covers the blocked squares) considering
/// the blocked squares.
uint64_t mask_bishop_attacks_on_fly(int square, uint64_t block){
    uint64_t bb = 0ull;
    uint64_t mask_attacks = 0ull;

    // rank, file
    int r, f;

    r = square/8;
    f = square%8;

    set_bit(bb, square);

    // up right
    for(int i = r-1, j = f+1, k=1; i >= 0 && j < 8; i--, j++, k++){
        mask_attacks |= (bb >> (7*k));
        if((bb >> (7*k)) & block)  
            break;
    }
    // up left
    for(int i = r-1, j = f-1, k=1; i >= 0 && j >= 0; i--, j--, k++){
        mask_attacks |= (bb >> (9*k));
        if((bb >> (9*k)) & block)  
            break;
    }
    // down right
    for(int i = r+1, j = f+1, k=1; i < 8 && j < 8; i++, j++, k++){
        mask_attacks |= (bb << (9*k));
        if((bb << (9*k)) & block)  
            break;
    }
    // down left
    for(int i = r+1, j = f-1, k=1; i < 8 && j >= 0; i++, j--, k++){
        mask_attacks |= (bb << (7*k));
        if((bb << (7*k)) & block)  
            break;
    }
    
    return mask_attacks;
}

/// masking relevant square that can be attacked by the 
/// specified bishop but does'nt count the last squares in files
/// and rows, because we will use this fucntion to find all
/// possible orientaions of occupancy and we will cover 
/// them (the last squares) in mask_bishop_attacks_on_fly.
uint64_t mask_bishop_attacks_relevant(int square){
    uint64_t bb = 0ull;
    uint64_t mask_attacks = 0ull;

    // rank, file
    int r, f;

    r = square/8;
    f = square%8;

    set_bit(bb, square);

    // up right
    for(int i = r-1, j = f+1, k=1; i > 0 && j < 7; i--, j++, k++){
        mask_attacks |= (bb >> (7*k));
    }
    // up left
    for(int i = r-1, j = f-1, k=1; i > 0 && j > 0; i--, j--, k++){
        mask_attacks |= (bb >> (9*k));
    }
    // down right
    for(int i = r+1, j = f+1, k=1; i < 7 && j < 7; i++, j++, k++){
        mask_attacks |= (bb << (9*k));
    }
    // down left
    for(int i = r+1, j = f-1, k=1; i < 7 && j > 0; i++, j--, k++){
        mask_attacks |= (bb << (7*k));
    }
    
    return mask_attacks;
}

// initialize bishop attacks table by using magic index like a hash
// for each square on the bitboard so we don't need to calculate 
// them in each iteration and access them faster.
void init_bishop_attacks(){
    // bishop_magics is predefined and can be const but this part can be uncommented to
    //  generate these numbers with another random seed or another PRNG algorithm. 
    /*
    for (int square = 0; square < 64; square++)
        // init bishop magic numbers
        bishop_magics[square] = find_magic(square, bishop_relevant_bits[square], bishop);
    */

    for (int square = 0; square < 64; square++)
    {
        // init current mask
        uint64_t mask = mask_bishop_attacks_relevant(square);
        int bit_count = count_bits(mask);
        // occupancy variations count = 2 ^ bit_count (could be possible or not)
        int occupancy_variations = 1 << bit_count;

        // init bishop masks
        bishop_masks[square] = mask;
        
        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++)
        {
                // init occupancies, magic index & attacks
                // magic index is used as hash for bishop_attacks table
                uint64_t occupancy = set_occupancy(count, mask);
                int magic_index = (occupancy * bishop_magics[square]) >> (64 - bishop_relevant_bits[square]);
                bishop_attacks[square][magic_index] = mask_bishop_attacks_on_fly(square, occupancy);                
        }
    }
}

// encoding occupancy pattern into magic indexes(hashes) 
// created for bishop_attacks to access to the specified bishop
// attacks with the given occupancy.
uint64_t get_bishop_attacks(int square, uint64_t occupancy) {
	
	// calculate magic index(hash)
	occupancy &= bishop_masks[square];
	occupancy *=  bishop_magics[square];
	occupancy >>= 64 - bishop_relevant_bits[square];
	
	return bishop_attacks[square][occupancy];
}
