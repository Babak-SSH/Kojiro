#include "tt.h"
#include "position.h"


namespace Kojiro {

TTEntry tt[hash_size];

void TTEntry::reset() {
	key = 0;
	depth = 0;
	flag = 0;
	score = 0;
	move = 0;
}

/// read transposition table entries data and find the proper score or return no_hash_entry
int probe_hash(int alpha, int beta, int depth) {
    TTEntry *hash_entry = &tt[st->key % hash_size];
    
    // check position hash keys
    if (hash_entry->key == st->key)
    {
        // we don't want to use tt if the depth is smaller than our current depth
        if (hash_entry->depth >= depth)
        {
            // (PV node) score 
            if (hash_entry->flag == hashfEXACT)
                return hash_entry->score;
            
            // alpha (fail-low node) score
            if ((hash_entry->flag == hashfALPHA) && (hash_entry->score <= alpha))
                return alpha;
            
            // beta (fail-high node) score
            if ((hash_entry->flag == hashfBETA) && (hash_entry->score >= beta))
                return beta;
        }
    }
    
    return no_hash_entry;
}

/// write to transposition table entries
void write_hash(int score, int depth, int hash_flag, int move) {
    TTEntry *hash_entry = &tt[st->key % hash_size];
  
    hash_entry->key = st->key;
    hash_entry->score = score;
    hash_entry->flag = hash_flag;
    hash_entry->depth = depth;
	hash_entry->move = move;
}

/// reset all items in our transposition table
void clear_tt() {
	for(int i=0; i<hash_size; i++) {
		tt[i].reset();
	}
}

} // namespace Kojiro