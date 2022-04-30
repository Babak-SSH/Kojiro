#include "search.h"

long nodes = 0;
int ply = 0; // half moves
int best_move;

/// quiescence search is used to avoid the horizon effect. because evaluating in a certain depth 
/// is dangerous, we need to have not certain but acceptable view on the rest of that position
/// Thus we use this search algorithm to check for captures and avoid any bad moves in deeper depths
/// without searching all those nodes and to make sure we are only evaluating quiescence (quite) positions.
static int Search::quiescence(int alpha, int beta){
	int standpat = Eval::evaluation();

	 /// fail-hard beta cutoff
	 /// @todo check fail-soft beta cutoff
    if (standpat >= beta){
        // node(move) fails high
        return beta;
    }
        
    /// found a better move
    if (standpat > alpha){
        // principle variation node(move)
        alpha = standpat;
    }

	moves move_list[1];

    generate_all(move_list, Color(st->side));

    for (int move_count = 0; move_count < move_list->count; move_count++){

		StateInfo nst;
		if(!make_move(move_list->moves[move_count], 2, nst)){
			continue;
		}


		int score = -Search::quiescence(-beta, -alpha);

		take_back();

		 /// fail-hard beta cutoff
		 /// @todo check fail-soft beta cutoff
        if (score >= beta)
        {
            // node(move) fails high
            return beta;
        }
        
        /// found a better move
        if (score > alpha)
        {
            // principle variation node(move)
            alpha = score;
        }
	}	


	return alpha;
}

/// negamax search algorithm is a variant form of minimax search for zero-sum(e.g chess) two-player games.
/// it relies on the fact that max(a, b) = -min(-a, -b) to simplify the implementation of minimax algorithm.
/// whites score evaluation for black is negation of that value in that state therefor the player on move looks 
/// for a move that maximizes the negation of the value resulting from the move: this successor position must 
/// by definition have been valued by the opponent.
static int Search::negamax(int alpha, int beta, int depth){
	if(depth == 0)
		return Search::quiescence(alpha, beta);
	
	moves move_list[1];
	int legal_moves = 0;
    int best_sofar;
    int old_alpha = alpha;
    int in_check = is_square_attacked((st->side == WHITE) ? get_ls1b_index(bitboards[K]) : 
                                                        get_ls1b_index(bitboards[k]),
                                                        st->side ^ 1);

	nodes++;

    generate_all(move_list, Color(st->side));

    for (int move_count = 0; move_count < move_list->count; move_count++){

		StateInfo nst;
		if(!make_move(move_list->moves[move_count], 1, nst)){
			continue;
		}

		ply++;
		legal_moves++;

		int score = -negamax(-beta, -alpha, depth-1);

		ply--;

		take_back();

		 /// fail-hard beta cutoff
		 /// @todo check fail-soft beta cutoff
        if (score >= beta)
        {
            // node(move) fails high
            return beta;
        }
        
        /// found a better move
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

	/// we don't have any legal moves to make in the current postion
    if (legal_moves == 0)
    {
        // king is in check
        if (in_check)
            // return mating score (assuming closest distance to mating position)
            return -49000 + ply;
        
        // king is not in check
        else
            // return stalemate score
            return 0;
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
	if(best_move){
	info = decode_move(best_move);
	sync_cout << "bestmove " 
			  << convert_to_square[info.source] << convert_to_square[info.target] << sync_endl;
	}
}