#ifndef TT_H
#define TT_H

#include <string>

#define	hash_size 800000
#define	no_hash_entry 100000

#define    hashfEXACT   0
#define    hashfALPHA   1
#define    hashfBETA    2


namespace Kojiro {

struct TTEntry
{
	uint64_t key;
	int depth;
	int flag;
	int score;
	int move;
	void reset();
};

/**
 * @brief Read hash entries in transposition table.
 * 
 * @param alpha 
 * @param beta 
 * @param depth 
 * @return int 
 */
int probe_hash(int alpha, int beta, int depth);

/**
 * @brief Write hash entries in transposition table.
 * 
 * @param score 
 * @param depth 
 * @param hash_flag 
 */
void write_hash(int score, int depth, int hash_flag, int move);

/**
 * @brief Reset all entries in transposition table.
 * 
 */
void clear_tt();

extern TTEntry tt[hash_size];

} // namespace Kojiro

#endif