#include "search.h"

long nodes = 0;
int ply = 0; // half moves
// best move
int best_move;

static int Search::negamax(int alpha, int beta, int depth){
	if(depth == 0)
		return Eval::evaluation();
	
	moves move_list[1];

	nodes++;

	// best move so far
    int best_sofar;
    
    // old value of alpha
    int old_alpha = alpha;

    // generate moves
    generate_all(move_list, Color(st->side));

    // loop over moves within a movelist
    for (int move_count = 0; move_count < move_list->count; move_count++){
		StateInfo nst;
		if(make_move(move_list->moves[move_count], 1, nst)){
			ply++;
		}
		else{
			continue;
		}
		int score = -negamax(-beta, -alpha, depth-1);

		ply--;

		take_back();

		 // fail-hard beta cutoff
        if (score >= beta)
        {
            // node (move) fails high
            return beta;
        }
        
        // found a better move
        if (score > alpha)
        {
            // PV node (move)
            alpha = score;
            
            // if root move
            if (ply == 0)
                // associate best move with the best score
                best_sofar = move_list->moves[move_count];
        }
	}	
	// found better move
    if (old_alpha != alpha)
        // init best move
        best_move = best_sofar;
    
    // node (move) fails low
    return alpha;
}

void Search::search(int depth){
	moveInfo info;
	Search::negamax(-50000, 50000, depth);
	info = decode_move(best_move);
	sync_cout << "bestmove " 
			  << convert_to_square[info.source] << convert_to_square[info.target] << sync_endl;
}