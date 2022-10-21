/***********************************************\
=================================================
                ROOK ATTACKS
=================================================
\***********************************************/

#include "rook.h"


namespace Kojiro {

// rook magic numbers
uint64_t rook_magics[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};
// rook attacks(relevant) masks
uint64_t rook_masks[64];
// rook attacks table [square][occupancies]
uint64_t rook_attacks[64][4096];


/// masking square that can be attacked by the specified
/// rook (this also covers the blocked squares) considering
/// the blocked squares.
uint64_t mask_rook_attacks_on_fly(int square, uint64_t block){
    uint64_t bb = 0ull;
    uint64_t mask_attacks = 0ull;

    // rank, file
    int r, f;
    int i, j, k;

    r = square/8;
    f = square%8;

    set_bit(bb, square);

    // up 
    for(i = r-1, k=1; i >= 0; i--, k++){
        mask_attacks |= (bb >> (8*k));
        if((bb >> (8*k)) & block)  
            break;
    }
    // down 
    for(i = r+1, k=1; i < 8; i++, k++){
        mask_attacks |= (bb << (8*k));
        if((bb << (8*k)) & block)  
            break;
    }
    //  left
    for(j = f-1, k=1; j >= 0; j--, k++){
        mask_attacks |= (bb >> (k));
        if((bb >> (k)) & block)  
            break;
    }
    //  right
    for(j = f+1, k=1; j < 8; j++, k++){
        mask_attacks |= (bb << (k));
        if((bb << (k)) & block)  
            break;
    }
    
    return mask_attacks;
}

/// masking relevant square that can be attacked by the 
/// specified rook but does'nt count the last squares in files
/// and rows, because we will use this fucntion to find all
/// possible orientaions of occupancy and we will cover 
/// them (the last squares) in mask_rook_attacks_on_fly.
uint64_t mask_rook_attacks_relevant(int square){
    uint64_t bb = 0ull;
    uint64_t mask_attacks = 0ull;

    // rank, file
    int r, f;
    int i, j, k;

    r = square/8;
    f = square%8;

    set_bit(bb, square);

    // up 
    for(i = r-1, k=1; i > 0; i--, k++){
        mask_attacks |= (bb >> (8*k));
    }
    // down 
    for(i = r+1, k=1; i < 7; i++, k++){
        mask_attacks |= (bb << (8*k));
    }
    //  left
    for(j = f-1, k=1; j > 0; j--, k++){
        mask_attacks |= (bb >> (k));
    }
    //  right
    for(j = f+1, k=1; j < 7; j++, k++){
        mask_attacks |= (bb << (k));
    }
    
    return mask_attacks;
}

// initialize rook attacks table by using magic index like a hash
// for each square on the bitboard so we don't need to calculate 
// them in each iteration and access them faster.
void init_rook_attacks(){
    // rook_magics is predefined and can be const but this part can be uncommented to
    //  generate these numbers with another random seed or another PRNG algorithm. 
    /*
    for (int square = 0; square < 64; square++)
        // init rook magic numbers
        rook_magics[square] = find_magic(square, rook_relevant_bits[square], rook);
    */

    for (int square = 0; square < 64; square++)
    {
        // init current mask
        uint64_t mask = mask_rook_attacks_relevant(square);
        int bit_count = count_bits(mask);
        // occupancy variations count = 2 ^ bit_count (could be possible or not)
        int occupancy_variations = 1 << bit_count;

        // init rook masks
        rook_masks[square] = mask;
        
        // loop over occupancy variations
        for (int count = 0; count < occupancy_variations; count++)
        {
                // init occupancies, magic index & attacks
                // magic index is used as hash for rook_attacks table
                uint64_t occupancy = set_occupancy(count, mask);
                int magic_index = (occupancy * rook_magics[square]) >> (64 - rook_relevant_bits[square]);
                rook_attacks[square][magic_index] = mask_rook_attacks_on_fly(square, occupancy);                
        }
    }
}

// encoding occupancy pattern into magic indexes(hashes) 
// created for rook_attacks to access to the specified rook
// attacks with the given occupancy.
uint64_t get_rook_attacks(int square, uint64_t occupancy) {
	
	// calculate magic index(hash)
	occupancy &= rook_masks[square];
	occupancy *=  rook_magics[square];
	occupancy >>= 64 - rook_relevant_bits[square];
	
	return rook_attacks[square][occupancy];
}

} // namespace Kojiro