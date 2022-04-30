#include "search.h"

long nodes = 0;
int ply = 0; // half moves
int best_move;

/// negamax search algorithm is a variant form of minimax search for zero-sum(e.g chess) two-player games.
/// it relies on the fact that max(a, b) = -min(-a, -b) to simplify the implementation of minimax algorithm.
/// whites score evaluation for black is negation of that value in that state therefor the player on move looks 
/// for a move that maximizes the negation of the value resulting from the move: this successor position must 
/// by definition have been valued by the opponent.
static int Search::negamax(int alpha, int beta, int depth){
	if(depth == 0)
		return Eval::evaluation();
	
	moves move_list[1];

	nodes++;

    int best_sofar;
    
    int old_alpha = alpha;

    generate_all(move_list, Color(st->side));

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
		 // TODO: check fail-soft beta cutoff
        if (score >= beta)
        {
            // node(move) fails high
            return beta;
        }
        
        // found a better move
        if (score > alpha)
        {
            // principle variation node(move)
            alpha = score;
            
            // if root move
            if (ply == 0)
                // associate best move with the best score
                best_sofar = move_list->moves[move_count];
        }
	}	
	// found better move
    if (old_alpha != alpha)
        best_move = best_sofar;
    
    // node(move) fails low
    return alpha;
}

void Search::search(int depth){
	moveInfo info;
	Search::negamax(-50000, 50000, depth);
	info = decode_move(best_move);
	sync_cout << "bestmove " 
			  << convert_to_square[info.source] << convert_to_square[info.target] << sync_endl;
}