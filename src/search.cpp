#include "eval.h"
#include "movegen.h"
#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "search.h"
#include "position.h"
#include "thread.h"
#include "logger.h"
#include "timeman.h"
#include "misc.h"
#include "tt.h"

#define max_ply 64

namespace Kojiro {

namespace Search{
	GameInfo Info;
	uint64_t repetition_table[1000];
	int repetition_index;
}

Thread* thisThread;

long alphabeta_nodes = 0;
long quiescence_nodes = 0;
int ply = 0; // half moves

int killer_moves[2][max_ply] {0}; // killer moves [id][ply]
int history_moves[12][64] = {0}; // history moves [piece][square]

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
            // enable move scoring and following pv
            score_pv = true;
            follow_pv = true;

			break;
        }
    }
}

/// scoring moves that will have a special or big effect like captures, so the 
/// alphabet(negamax) or quisence search look at them earlier. also killer moves
/// and history moves with higher score can make a beta cutoff earlier in search 
/// and avoid searching many unnecessery nodes,(about 90% node optimization in my case)
static int Search::score_move(int move, const Position& pos){
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
	if (info.capture){
		for(int bb_piece = B_PAWN-(pos.side()*6); bb_piece <= B_KING-(pos.side()*6); bb_piece++){
			if(get_bit(pos.bitboards(bb_piece), info.target)){
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
static void Search::sort_moves(moves *move_list, const Position& pos){
	int move_count = move_list->count;
	int move_scores[move_count];
	std::pair<int, int> ms[move_count];

	for(int count = 0; count < move_count; count++){
		ms[count].first = Search::score_move(move_list->moves[count], pos);
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
static int Search::quiescence(int alpha, int beta, Position& pos) {
	if (thisThread == Threads.main())
		static_cast<MainThread*>(thisThread)->check_time();

	if(Threads.stop)
		return 0;
	int pv_node = (beta - alpha) > 1;
	// probe hash entry
	int score = TT::probe_hash(alpha, beta, 0, pos);
	if(score != no_hash_entry) {
	 	return score;
	}

	int standpat = Eval::evaluation(pos);
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
    MoveGen::generate_all(move_list, pos);
	Search::sort_moves(move_list, pos);

    for (int move_count = 0; move_count < move_list->count; move_count++){
		ply++;
		Search::repetition_index++;
		Search::repetition_table[Search::repetition_index] = pos.key();

		StateInfo nst;

		if(!pos.make_move(move_list->moves[move_count], 2, nst)){
			ply--;
			Search::repetition_index--;
			continue;
		}

		int score = -Search::quiescence(-beta, -alpha, pos);

		ply--;

		Search::repetition_index--;

		pos.take_back();

		// found a better move
        if (score > alpha)
        {
            // PV node (position)
            alpha = score;
            
            // fail-hard beta cutoff
            if (score >= beta)
            {
                // node (position) fails high
                return beta;
            }
        }
	}	

	return alpha;
}

/// negamax search algorithm is a variant form of minimax search for zero-sum(e.g chess) two-player games.
/// it relies on the fact that max(a, b) = -min(-a, -b) to simplify the implementation of minimax algorithm.
/// whites score evaluation for black is negation of that value in that state therefor the player on move looks 
/// for a move that maximizes the negation of the value resulting from the move: this successor position must 
/// by definition have been valued by the opponent.
static int Search::negamax(int alpha, int beta, int depth, Position& pos) {
	moves move_list[1];
	int legal_moves = 0;
	moveInfo mInfo;
	int score = 0;
	bool found_pv = false;
	int temp_enp;
	bool in_check;
	int eval;

	if (thisThread == Threads.main())
		static_cast<MainThread*>(thisThread)->check_time();

	if(Threads.stop || (Search::Info.use_time() && Time.getElapsed() > Search::Info.time[pos.side()]))
		return 0;

	int hash_flag = hashfALPHA;

	if (ply && Search::is_repetition()) {
		return 0;
	}

	int pv_node = (beta - alpha) > 1;
	// probe hash entry
	if(ply && (score = TT::probe_hash(alpha, beta, depth, pos)) != no_hash_entry && pv_node==0) {
	 	return score;
	}

	pv_length[ply] = ply;


	// for a better evaluation and removing horizon effect we use quiescence 
	// search to evalute further and in more depth.
	if(depth == 0)
		return Search::quiescence(alpha, beta, pos);
	
	// if depth is more than usual we can return the static evaluation
	// to avoid extra calculations of quiescence search.
	if(ply > max_ply-1)
		return Eval::evaluation(pos);

    // int in_check = is_square_attacked((st->side == WHITE) ? get_ls1b_index(st->bitboards[K]) : 
                                                        // get_ls1b_index(st->bitboards[k]),
                                                        // (st->side) ^ 1);
	in_check = pos.is_check();
	/// @todo check if increasing depth when king is in check can cause huge node increament in certain cases or not.
	// increase search depth if king is in danger.
	if(in_check){
		depth++;
	}

	alphabeta_nodes++;

	eval = Eval::evaluation(pos);

	/// Reverse Futility Purning @todo check for different depth assessment
	if (depth < 3 && !pv_node && !in_check &&  abs(beta - 1) > -InfinityValue + 100) {
		int margin = 120 * depth;

		if (eval - margin >= beta)
			return eval - margin;
	}

	/// Null-Move Forward Pruning: we give the opponent a free move and if that returns a score higher than beta,
	/// then we don't need to look at our moves or even generate them because our position is so good even with 
	/// giving a free move we can exceed beta so we can cutoff this position.
	if(depth >= 3 && !in_check && ply){
		ply++;

		Search::repetition_index++;
        Search::repetition_table[Search::repetition_index] = pos.key();
		
		// handling enpassant in null move
		temp_enp = pos.enpassant();
		if(temp_enp != no_sq)
			pos.state()->key ^= Zobrist::enpassant[temp_enp];

		pos.state()->enpassant = no_sq;

		pos.state()->side ^= 1;

		pos.state()->key ^= Zobrist::side;

		score = -negamax(-beta, -beta + 1, depth-3, pos);

		ply--;

		Search::repetition_index--;

		pos.state()->side ^= 1;
		pos.state()->key ^= Zobrist::side;

		pos.state()->enpassant = temp_enp;

		// handling enpassant in null move
		if(temp_enp != no_sq)
			pos.state()->key ^= Zobrist::enpassant[temp_enp];

		if (score >= beta)
			return beta;
	}
	
	/// razoring
	if (!pv_node && !in_check && depth <= 3) {
		int svalue, qvalue;
		svalue = eval + 125;

		// fail-low node
		if (svalue < beta)
			if (depth == 1) {
				qvalue = quiescence(alpha, beta, pos);
				return std::max(qvalue, svalue);
			}

		svalue += 175;
		if (score < beta && depth <= 2) {
			if (qvalue < beta)
				return std::max(qvalue, svalue);	
		}
	}

    MoveGen::generate_all(move_list, pos);

    // enable PV move scoring if we are in a principle variation route 
	if (follow_pv)
        Search::enable_pv_scoring(move_list);

	Search::sort_moves(move_list, pos);

	int moves_searched = 0;

    for (int move_count = 0; move_count < move_list->count; move_count++){
		StateInfo nst;

		ply++;

		Search::repetition_index++;
        Search::repetition_table[Search::repetition_index] = pos.key();

		// if illegal move, continue
		if(!pos.make_move(move_list->moves[move_count], 1, nst)){
			ply--;

			Search::repetition_index--;

			continue;
		}

		legal_moves++;

		if (moves_searched == 0) {
			score = -negamax(-beta, -alpha, depth-1, pos);
		}
		else {
			/// LMR(late move reduction); if the move ordring is good then we will encounter a cutoff in first node
			/// thus we will search the few first moves in full depth and other moves in reduced depth and if one of
			/// them look interesting or returns a score greater than alpha that move will be re-searched in full depth.
			if (moves_searched >= full_depth_moves &&
               depth >= reduction_limit &&
               !in_check && 
               get_move_capture(move_list->moves[move_count]) == 0 &&
               get_move_promoted(move_list->moves[move_count]) == 0){
        		score = -negamax(-alpha - 1, -alpha, depth - 2, pos);
			}
			else {
				score = alpha + 1;
			}
			/* if we have found a pv node, it means that with a high chance we won't find
		 		any other pv node, therefor we use a narrow window for alphabeta: (alpha, alpha+1)
		 		in this case we will have more cutoffs because of the beta value, however if there
		 		was a node which has a value higher than our alpha and it is smaller than the main 
		 		beta then we should search in a normal window and find this new pv node and thus
		 		the new pv path, in this case we have done extra search and wasted time :)(but it is less likely to happen)
		 	*/
			if (score > alpha) {
                score = -negamax(-alpha - 1, -alpha, depth-1, pos);

                if((score > alpha) && (score < beta))
                    score = -negamax(-beta, -alpha, depth-1, pos);
            }
		}

		ply--;

		Search::repetition_index--;

		pos.take_back();
	
		moves_searched++;

        // found a better move
        if (score > alpha)
        {
			hash_flag = hashfEXACT;

			mInfo = decode_move(move_list->moves[move_count]);

            // principle variation node(move)
            alpha = score;

			found_pv = true;
            pv_table[ply][ply] = move_list->moves[move_count];
            
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                // copy move from deeper ply into a current ply's line
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            
            pv_length[ply] = pv_length[ply + 1];                        

		 // fail-hard beta cutoff
		 /// @todo check fail-soft beta cutoff
        if (score >= beta) {
			TT::write_hash(beta, depth, hashfBETA, move_list->moves[move_count], pos);
			if(!mInfo.capture) {
				// store history moves
	            history_moves[mInfo.piece][mInfo.target] = std::max(history_moves[mInfo.piece][mInfo.target], depth*depth);
				// store killer moves
            	killer_moves[1][ply] = killer_moves[0][ply];
            	killer_moves[0][ply] = move_list->moves[move_count];
			}

            // node(move) fails high
            return beta;
        }
        }
	}	

	// no legal moves to make in the current postion so stalemate or checkmate.
    if (legal_moves == 0) {
        // return mating score (assuming closest distance to mating position)
		if (in_check){
            return -MateValue + ply;
		}
		// return stalemate score
		else{
            return 0;
		}
    }

	TT::write_hash(alpha, depth, hash_flag, pv_table[ply][ply], pos);

    // node(move) fails low
    return alpha;
}

void MainThread::search() {	
	Time.init(Search::Info, rootPos);
	Threads.start_searching();

	Thread::search();

	Threads.stop = true;

	Threads.wait_until_search_finished();
}

void Thread::search() {
	// clear(reset) helper data(globals)
	Search::clear();

	thisThread = this;

	if (this == Threads.main()) //{
		printf("in main thread\n");
	else
		printf("thread id: %ld\n", this->idx);
		// thisThread = this;

	int score = 0;
	std::stringstream pvr;
	std::string output;
	int bestmove;

    int alpha = -50000;
    int beta = 50000;
 
	for (int current_depth = 1; current_depth <= depth; current_depth++){
		alphabeta_nodes = 0;
		quiescence_nodes = 0;

		pvr.clear();
		pvr.str("");

		// enable pv following
		follow_pv = true;

		score = Search::negamax(alpha, beta, current_depth, rootPos);

		if(Threads.stop)
			break;

		for(int count = 0; count < pv_length[0]; count++) {
    	    // print PV moves
			pvr << get_move_string(pv_table[0][count]); 
			pvr << ' ';
    	}

		bestmove = pv_table[0][0];

		if (score > -MateValue && score < -MateScore) 
			output = fmt::format("info score mate {:<6} depth {:<4} nodes {:<12} time {:<12} pv {:<50}", 
							-(score + MateValue) / 2 - 1, current_depth, alphabeta_nodes+quiescence_nodes,
							Time.getElapsed(), pvr.str());

		else if (score > MateScore && score < MateValue)
			output = fmt::format("info score mate {:<6} depth {:<4} nodes {:<12} time {:<12} pv {:<50}", 
							(MateValue - score) / 2 + 1, current_depth, alphabeta_nodes+quiescence_nodes,
							Time.getElapsed(), pvr.str());
		else
			output = fmt::format("info score cp {:<6} depth {:<4} nodes {:<12} time {:<12} pv {:<50}", 
							score, current_depth, alphabeta_nodes+quiescence_nodes,
							Time.getElapsed(), pvr.str());

		logger.logIt(fmt::format("alphabeta: {}, quiescence: {}", alphabeta_nodes, quiescence_nodes), LOG);
		logger.logIt(output, LOG);
		sync_cout << output << sync_endl;

		// outside the window so we need to check again from scratch. 	
		if ((score <= alpha) || (score >= beta)) {
            alpha = -50000;    
            beta = 50000;      
        }
		else {
			/// @todo why 50???????????????????????????????????
        	alpha = score - 50;
        	beta = score + 50;
		}
		
		if (Search::Info.use_time()) {
			if (Time.getElapsed() > Time.getOptimum()) {
				Threads.stop = true;
			}
			else if(Time.getElapsed() < Time.getOptimum() * 0.8) {
				depth++;
			}
		}
	}

	output = fmt::format("bestmove {}", get_move_string(bestmove));

	logger.logIt(output, LOG);
	sync_cout << output << sync_endl;
	// }
	// else {
		// printf("im still alive\n");
	// }
}

/// @todo can we get pvr by tracing the hash table back?????
// void Search::get_pv() {
// 	TTEntry *hash_entry = &tt[st->key % hash_size];
// 	// getchar();
// 	int joke = hash_entry->move;
// 	StateInfo nst;

// 	if(joke!=0) {
// 		joke_pvr << get_move_string(joke);
// 		make_move(joke, 1, nst);
// 		Search::get_pv();
// 		take_back();
// 	}
// }

void MainThread::check_time(){
	/// @todo log information each 1000 ms 
	TimePoint elapsed = Time.getElapsed();

	if (Search::Info.use_time() && (elapsed > Time.getMaximum() || Time.getElapsed() > Time.getOptimum())){
		Threads.stop = true;
	}
}

void Search::clear(){
	alphabeta_nodes = 0;
	quiescence_nodes = 0;

	score_pv = false;
	follow_pv = false;

	/// @todo check for correctness
    Search::repetition_index = 0;
    memset(Search::repetition_table, 0ULL, sizeof(Search::repetition_table));


 	// clear helper datas for search
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));	
}

int Search::is_repetition()
{
    // loop over repetition indicies range
    for (int index = 0; index < Search::repetition_index; index++){
        // if we found the hash key same with a current
        if (Search::repetition_table[index] == thisThread->rootPos.key()){
            // we found a repetition
            return 1;
		}
	}
    // if no repetition found
    return 0;
}
} // namespace Kojiro