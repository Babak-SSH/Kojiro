#include <bits/stdc++.h>

#include "search.h"

#define max_ply 64

long alphabeta_nodes = 0;
long quiescence_nodes = 0;
int ply = 0; // half moves

int killer_moves[2][max_ply]; // killer moves [id][ply]

int history_moves[12][64]; // history moves [piece][square]

int pv_length[max_ply];

int pv_table[max_ply][max_ply];
bool score_pv, follow_pv;


const int full_depth_moves = 4;
const int reduction_limit = 3;

// enable PV move scoring
static void Search::enable_pv_scoring(moves *move_list){
    // disable following PV
    follow_pv = false;
    
    // loop over the moves within a move list
    for (int count = 0; count < move_list->count; count++){
        // make sure we hit PV move
        if (pv_table[0][ply] == move_list->moves[count]){
            // enable move scoring
            score_pv = true;
            
            // enable following PV
            follow_pv = true;
        }
    }
}

/// scoring moves that will have a special or big effect like captures, so the 
/// alphabet(negamax) or quisence search look at them earlier. also killer moves
/// and history moves with higher score can make a beta cutoff earlier in search 
/// and avoid searching many unnecessery nodes,(about 90% node optimization in my case)
static int Search::score_move(int move){
	moveInfo info;
	int target_piece = 0;

	info = decode_move(move);

	// score pv moves
    if (score_pv){
        if (pv_table[0][ply] == move){
            score_pv = 0;
            
            // give PV move the highest score to search it first
            return 20000;
        }
    }
	// score capture moves
	if(info.capture){
		for(int bb_piece = B_PAWN-(st->side*6); bb_piece <= B_KING-(st->side*6); bb_piece++){
			if(get_bit(st->bitboards[bb_piece], info.target)){
				target_piece = bb_piece;
				break;
			}
		}
		return mvv_lva[info.piece%6][target_piece%6] + 10000;
	}
	else{
		// score 1st killer move
        if (killer_moves[0][ply] == move)
            return 9000;
        
        // score 2nd killer move
        else if (killer_moves[1][ply] == move)
            return 8000;
        
        // score history move
        else
            return history_moves[info.piece][info.target];
	}
	return 0;
}

/// compare function to compare scores in move,score pair.
static int Search::compare(std::pair<int, int> t1, std::pair<int, int> t2){
	return t1.first > t2.first;
}

/// sorting moves by their respective score evaluated by Search::score_move function.
static void Search::sort_moves(moves *move_list){
	int move_count = move_list->count;
	int move_scores[move_count];
	std::pair<int, int> ms[move_count];

	for(int count = 0; count < move_count; count++){
		ms[count].first = Search::score_move(move_list->moves[count]);
		ms[count].second = move_list->moves[count];
	}

	std::sort(ms, ms+move_count, Search::compare);

	for(int count = 0; count < move_count; count++){
		move_list->moves[count] = ms[count].second;
	}
}

/// quiescence search is used to avoid the horizon effect. because evaluating in a certain depth 
/// is dangerous, we need to have not certain but acceptable view on the rest of that position
/// Thus we use this search algorithm to check for captures and avoid any bad moves in deeper depths
/// without searching all those nodes and to make sure we are only evaluating quiescence (quite) positions.
static int Search::quiescence(int alpha, int beta){
	int standpat = Eval::evaluation();

	quiescence_nodes++;

	 // fail-hard beta cutoff
	 /// @todo check fail-soft beta cutoff
    if (standpat >= beta){
        // node(move) fails high
        return beta;
    }
        
    // found a better move
    if (standpat > alpha){
        // principle variation node(move)
        alpha = standpat;
    }

	moves move_list[1];

    generate_all(move_list, Color(st->side));

	Search::sort_moves(move_list);

    for (int move_count = 0; move_count < move_list->count; move_count++){
		ply++;
		StateInfo nst;
		if(!make_move(move_list->moves[move_count], 2, nst)){
			ply--;
			continue;
		}

		int score = -Search::quiescence(-beta, -alpha);

		ply--;
		take_back();

		 // fail-hard beta cutoff
		 /// @todo check fail-soft beta cutoff
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
	pv_length[ply] = ply;

	// for a better evaluation and removing horizon effect we use quiescence 
	// search to evalute further and in more depth.
	if(depth == 0)
		return Search::quiescence(alpha, beta);
	
	// if depth is more than usual we can return the static evaluation
	// to avoid extra calculations of quiescence search.
	if(ply > max_ply-1)
		return Eval::evaluation();

	moves move_list[1];
	int legal_moves = 0;
	moveInfo mInfo;
	int score = 0;
	bool found_pv = false;
	int temp_enp;

    int in_check = is_square_attacked((st->side == WHITE) ? get_ls1b_index(st->bitboards[K]) : 
                                                        get_ls1b_index(st->bitboards[k]),
                                                        st->side ^ 1);

	/// @todo check if increasing depth when king is in check can cause huge node increament in certain cases or not.
	// increase search depth if king is in danger.
	if(in_check)
		depth++;

	alphabeta_nodes++;

	/// Null-Move Forward Pruning: we give the opponent a free move and if that returns a score higher than beta,
	/// then we don't need to look at our moves or even generate them because our position is so good even with 
	/// giving a free move we can exceed beta so we can cutoff this position.
	if(depth >= 3 && !in_check && ply){
		st->side ^= 1;
		temp_enp = st->enpassant;
		st->enpassant = no_sq;

		int score = -negamax(-beta, -beta + 1, depth-3);

		st->side ^= 1;

		st->enpassant = temp_enp;

		if (score >= beta)
			return beta;
	}

    generate_all(move_list, Color(st->side));

	if (follow_pv)
        // enable PV move scoring if we are in a principle variation route 
        Search::enable_pv_scoring(move_list);

	Search::sort_moves(move_list);

	int moves_searched = 0;

    for (int move_count = 0; move_count < move_list->count; move_count++){
		StateInfo nst;

		ply++;

		if(!make_move(move_list->moves[move_count], 1, nst)){
			// illegal move
			ply--;
			continue;
		}

		legal_moves++;

		score = 0;

		if(moves_searched == 0){
			score = -negamax(-beta, -alpha, depth-1);
		}
		else{
			/// LMR(late move reduction); if the move ordring is good then we will encounter a cutoff in first node
			/// thus we will search the few first moves in full depth and other moves in reduced depth and if one of
			/// them look interesting or returns a score greater than alpha that move will be re-searched in full depth.
			if(moves_searched >= full_depth_moves &&
               depth >= reduction_limit &&
               in_check == 0 && 
               get_move_capture(move_list->moves[move_count]) == 0 &&
               get_move_promoted(move_list->moves[move_count]) == 0){
        		score = -negamax(-alpha - 1, -alpha, depth - 2);
			}
            else{
				score = alpha + 1;
			}
			/* if we have found a pv node, it means that with a high chance we won't find
		 		any other pv node, therefor we use a narrow window for alphabeta: (alpha, alpha+1)
		 		in this case we will have more cutoffs because of the beta value, however if there
		 		was a node which has a value higher than our alpha and it is smaller than the main 
		 		beta then we should search in a normal window and find this new pv node and thus
		 		the new pv path, in this case we have done extra search and wasted time :)(but it is less likely to happen)
		 	*/
			if(score > alpha){
                score = -negamax(-alpha - 1, -alpha, depth-1);
            
                if((score > alpha) && (score < beta))
                    score = -negamax(-beta, -alpha, depth-1);
            }
		}

		ply--;

		take_back();

		moves_searched++;

		 // fail-hard beta cutoff
		 /// @todo check fail-soft beta cutoff
        if (score >= beta)
        {
			if(!mInfo.capture){
				// store killer moves
            	killer_moves[1][ply] = killer_moves[0][ply];
            	killer_moves[0][ply] = move_list->moves[move_count];
			}

            // node(move) fails high
            return beta;
        }
        
        // found a better move
        if (score > alpha)
        {
			mInfo = decode_move(move_list->moves[move_count]);

			if(!mInfo.capture){
				/// store history moves @todo check if not needed remove history moves
	            history_moves[mInfo.piece][mInfo.target] += depth;
			}

            // principle variation node(move)
            alpha = score;

			found_pv = true;
            pv_table[ply][ply] = move_list->moves[move_count];

            
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                // copy move from deeper ply into a current ply's line
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            
            pv_length[ply] = pv_length[ply + 1];                        
        }
	}	

	// we don't have any legal moves to make in the current postion
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

    // node(move) fails low
    return alpha;
}

void Search::search(int depth){
	// clear(reset) helper datas(globals)
	Search::clear();

	int score = 0;
	moveInfo info;

    int alpha = -50000;
    int beta = 50000;
 
	for (int current_depth = 1; current_depth <= depth; current_depth++){
		alphabeta_nodes = 0;
		quiescence_nodes = 0;

		// enable pv following
		follow_pv = true;

		score = Search::negamax(alpha, beta, current_depth);

		info = decode_move(pv_table[0][0]);

		/// @todo this part except bestmove should be logged.
		sync_cout << "info score cp " << score << " depth " << current_depth << " nodes " << alphabeta_nodes + quiescence_nodes << " pv ";

		for (int count = 0; count < pv_length[0]; count++){
    	    // print PV moves
    	    print_move(pv_table[0][count]);
    	}
        printf("\n");
		// sync_cout << "cp: " << Eval::evaluation() << "  score nega: " << score << sync_endl;
		// sync_cout << "alhpabeta(negamax) nodes: " << alphabeta_nodes << "\nquiescence nodes:" << quiescence_nodes << "\ntotal nodes:" << alphabeta_nodes + quiescence_nodes << sync_endl;
		// sync_cout << "length" << pv_length[0] << sync_endl;

		if((score <= alpha) || (score >= beta)){
            alpha = -50000;    
            beta = 50000;      
        }
		else{
        	alpha = score - 50;
        	beta = score + 50;
		}
	}

	sync_cout << "bestmove " 
			  << convert_to_square[info.source] << convert_to_square[info.target] << sync_endl;
}

void Search::clear(){
	alphabeta_nodes = 0;
	quiescence_nodes = 0;

	score_pv = false;
	follow_pv = false;

 	// clear helper datas for search
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));	
}