#include "tt.h"
#include "fmt/format.h"
#include "logger.h"
#include "position.h"
#include "eval.h"
#include <cstddef>


namespace Kojiro {

TTEntry* tt = NULL;
int hash_size = 0;

void TTEntry::reset() {
	key = 0;
	depth = 0;
	flag = 0;
	score = 0;
	move = 0;
}

void init_hash_table(int mb) {
    int size = 0x100000 * mb;
    
    // number of transposition table entries
    hash_size =  size / sizeof(TTEntry);

    // free hash table if not empty
    if (tt != NULL) {
        logger.logIt("Clearing TT hash memory...\n", LOG);
        free(tt);
    }
     
    // allocate memory
    tt = (TTEntry *) malloc(hash_size * sizeof(TTEntry));
    
    // clear hash table
    clear_tt();
    logger.logIt(fmt::format("Hash table is initialied with %d entries\n", hash_size), LOG) ;
}

/// read transposition table entries data and find the proper score or return no_hash_entry
int probe_hash(int alpha, int beta, int depth) {
    TTEntry *hash_entry = &tt[st->key % hash_size];
    
    // check position hash keys
    if (hash_entry->key == st->key){
        // we don't want to use tt if the depth is smaller than our current depth
        if (hash_entry->depth >= depth)
        {
            int score = hash_entry->score;

            // retrieve score independent from the actual path
            // from root node (position) to current node (position)
            if (score < -MateScore) score += st->play_count;
            if (score > MateScore) score -= st->play_count;

            // (PV node) score 
            if (hash_entry->flag == hashfEXACT)
                return score;
            
            // alpha (fail-low node) score
            if ((hash_entry->flag == hashfALPHA) && (score <= alpha))
                return alpha;
            
            // beta (fail-high node) score
            if ((hash_entry->flag == hashfBETA) && (score >= beta))
                return beta;
        }
    }
    
    return no_hash_entry;
}

/// write to transposition table entries
void write_hash(int score, int depth, int hash_flag, int move) {
    TTEntry *hash_entry = &tt[st->key % hash_size];

    if (score < -MateScore) score -= play_count;
    if (score > MateScore) score += play_count;

    hash_entry->key = st->key;
    hash_entry->score = score;
    hash_entry->flag = hash_flag;
    hash_entry->depth = depth;
	hash_entry->move = move;
}

/// reset all items in our transposition table
void clear_tt() {
	for(int i=0; i < hash_size; i++) {
		tt[i].reset();
	}
}

} // namespace Kojiro