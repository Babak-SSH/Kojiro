#include "bitboard.h"
#include "eval.h"
#include "movegen.h"
#include <atomic>
#include <vector>
#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "search.h"
#include "position.h"
#include "thread.h"
#include "logger.h"
#include "timeman.h"
#include "misc.h"
#include "tt.h"

// #define MaxPly 256 

namespace Kojiro {

namespace Search{
	GameInfo Info;

	/**
	 * @brief check pv(principle variation) scoring.
	 * 
	 * @param move_list 
	 */
	static void enable_pv_scoring(moves *move_list, const Position& pos);

	/**
	 * @brief score moves depending on their type (capture, quite, killer,...)
	 * 
	 * @param move 
	 * @return int 
	 */
	static int score_move(int move, const Position& pos);

	/**
	 * @brief compare scores(first) of pair
	 * 
	 * @param t1 
	 * @param t2 
	 * @return int 
	 */
	static int compare(std::pair<int, int> t1, std::pair<int, int> t2);	

	/**
	 * @brief sorting moves according to their score.
	 * 
	 * @param move_list 
	 */
	static void sort_moves(moves *move_list, const Position& pos);

	/**
	 * @brief quiescence search for horizon effect.
	 * 
	 * @param alpha 
	 * @param beta 
	 * @return int 
	 */
	static int quiescence(int alpha, int beta, Position& pos);

	/**
	 * @brief negamax alpha beta search (recursive)
	 * 
	 * @param alpha 
	 * @param beta 
	 * @param depth 
	 * @return int 
	 */
	static int negamax(int alpha, int beta, int depth, Position& pos);
}

// enable PV move scoring
static void Search::enable_pv_scoring(moves *move_list, const Position& pos){
    // disable following PV
    pos.thread()->followPV = false;
    
    // loop over the moves within a move list
    for (int count = 0; count < move_list->count; count++){
        // make sure we hit PV move
        if (pos.thread()->pvTable[0][pos.get_ply()] == move_list->moves[count]){
            // enable move scoring and following pv
            pos.thread()->scorePV = true;
            pos.thread()->followPV = true;

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
    if (pos.thread()->scorePV){
        if (pos.thread()->pvTable[0][pos.get_ply()] == move){
    		pos.thread()->scorePV = 0;
            
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
        if (pos.thread()->killer_moves[0][pos.get_ply()] == move)
            return 9000;
        
        // score 2nd killer move
        else if (pos.thread()->killer_moves[1][pos.get_ply()] == move)
            return 8000;
        
        // score history move
        else
			return pos.thread()->history_moves[info.piece][info.target]; 
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
	// std::vector<int> move_scores(move_count);
	std::vector<std::pair<int, int>> ms(move_count);

	for(int count = 0; count < move_count; count++){
		ms[count].first = Search::score_move(move_list->moves[count], pos);
		ms[count].second = move_list->moves[count];
	}

	std::sort(ms.begin(), ms.end(), Search::compare);

	for(int count = 0; count < move_count; count++){
		move_list->moves[count] = ms[count].second;
	}
}

/// quiescence search is used to avoid the horizon effect. because evaluating in a certain depth 
/// is dangerous, we need to have not certain but acceptable view on the rest of that position
/// Thus we use this search algorithm to check for captures and avoid any bad moves in deeper depths
/// without searching all those nodes and to make sure we are only evaluating quiescence (quite) positions.
static int Search::quiescence(int alpha, int beta, Position& pos) {
	if (pos.thread() == Threads.main())
		static_cast<MainThread*>(pos.thread())->check_time();

	if(Threads.stop)
		return -InfinityValue;

	// int pv_node = (beta - alpha) > 1;
	// probe hash entry
	int score = TT::probe_hash(alpha, beta, 0, pos);
	if(score != no_hash_entry) {
	 	return score;
	}

	int standpat = Eval::evaluation(pos);
	pos.thread()->quiescence_nodes.fetch_add(1, std::memory_order_relaxed);

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
		StateInfo nst;

		if(!pos.make_move(move_list->moves[move_count], 2, nst)){
			continue;
		}

		int score = -Search::quiescence(-beta, -alpha, pos);

		if (abs(score) == InfinityValue)
			return InfinityValue;

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
	// bool found_pv = false;
	int temp_enp;
	bool in_check;
	int eval;

	if (pos.thread() == Threads.main())
		static_cast<MainThread*>(pos.thread())->check_time();

	if(Threads.stop)
		return -InfinityValue;

	int hash_flag = hashfALPHA;

	if (pos.get_ply() && Search::is_repetition(pos)) {
		return 0;
	}

	int pv_node = (beta - alpha) > 1;
	// probe hash entry
	if(pos.get_ply() && (score = TT::probe_hash(alpha, beta, depth, pos)) != no_hash_entry && pv_node==0) {
	 	return score;
	}

	pos.thread()->pvLength[pos.get_ply()] = pos.get_ply();


	// for a better evaluation and removing horizon effect we use quiescence 
	// search to evalute further and in more depth.
	if(depth == 0)
		return Search::quiescence(alpha, beta, pos);
	
	// if depth is more than usual we can return the static evaluation
	// to avoid extra calculations of quiescence search.
	if(pos.get_ply() > MaxPly-1)
		return Eval::evaluation(pos);

	in_check = pos.is_check();

	/// @todo check if increasing depth when king is in check can cause huge node increament in certain cases or not.
	// increase search depth if king is in danger.
	if(in_check){
		depth++;
	}

	pos.thread()->alphabeta_nodes.fetch_add(1, std::memory_order_relaxed);

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
	if(pos.get_ply() && depth >= 3 && !in_check){
		// handling enpassant in null move
		temp_enp = pos.enpassant();
		if(temp_enp != no_sq)
			pos.state()->key ^= Zobrist::enpassant[temp_enp];

		pos.state()->enpassant = no_sq;

		pos.state()->side ^= 1;

		pos.state()->key ^= Zobrist::side;

		score = -negamax(-beta, -beta + 1, depth-3, pos);

		pos.state()->side ^= 1;
		pos.state()->key ^= Zobrist::side;

		pos.state()->enpassant = temp_enp;

		// handling enpassant in null move
		if(temp_enp != no_sq)
			pos.state()->key ^= Zobrist::enpassant[temp_enp];

		if (Threads.stop)
			return InfinityValue;

		if (score >= beta)
			return beta;
	}


	/// razoring
	if (!pv_node && !in_check && depth <= 3) {
		int svalue, qvalue;
		svalue = eval + 125;

		// fail-low node
		if (svalue < beta) {
			if (depth == 1) {
				qvalue = quiescence(alpha, beta, pos);
				if (abs(qvalue) == InfinityValue)
					return InfinityValue;

				return std::max(qvalue, svalue);
			}

			svalue += 175;
			if (score < beta && depth <= 2) {
				qvalue = quiescence(alpha, beta, pos);
				if (abs(qvalue) == InfinityValue)
					return InfinityValue;

				if (qvalue < beta)
					return std::max(qvalue, svalue);	
			}
		}
	}

    MoveGen::generate_all(move_list, pos);

    // enable PV move scoring if we are in a principle variation route 
	if (pos.thread()->followPV)
        Search::enable_pv_scoring(move_list, pos);

	Search::sort_moves(move_list, pos);

	int moves_searched = 0;

    for (int move_count = 0; move_count < move_list->count; move_count++){
		StateInfo nst;

		// if illegal move, continue
		if(!pos.make_move(move_list->moves[move_count], 1, nst)){
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
			if (moves_searched >= FullDepthMoves &&
               depth >= ReductionLimit &&
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

		if (Threads.stop)
			return InfinityValue;

		pos.take_back();
	
		moves_searched++;

        // found a better move
        if (score > alpha) {
			hash_flag = hashfEXACT;

			mInfo = decode_move(move_list->moves[move_count]);

            // principle variation node(move)
            alpha = score;

			// found_pv = true;
            pos.thread()->pvTable[pos.get_ply()][pos.get_ply()] = move_list->moves[move_count];
            
            for (int next_ply = pos.get_ply() + 1; next_ply < pos.thread()->pvLength[pos.get_ply() + 1]; next_ply++)
                // copy move from deeper ply into a current ply's line
                pos.thread()->pvTable[pos.get_ply()][next_ply] = pos.thread()->pvTable[pos.get_ply() + 1][next_ply];
            
            pos.thread()->pvLength[pos.get_ply()] = pos.thread()->pvLength[pos.get_ply() + 1];

		    // fail-hard beta cutoff
		    /// @todo check fail-soft beta cutoff
  	        if (score >= beta) {
				TT::write_hash(beta, depth, hashfBETA, move_list->moves[move_count], pos);
				if(!mInfo.capture) {
					// store history moves
	 	           pos.thread()->history_moves[mInfo.piece][mInfo.target] = std::max(pos.thread()->history_moves[mInfo.piece][mInfo.target], depth*depth);
					// store killer moves
  	          	pos.thread()->killer_moves[1][pos.get_ply()] = pos.thread()->killer_moves[0][pos.get_ply()];
  	          	pos.thread()->killer_moves[0][pos.get_ply()] = move_list->moves[move_count];
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
            return -MateValue + pos.get_ply();
		}
		// return stalemate score
		else{
            return 0;
		}
    }

	TT::write_hash(alpha, depth, hash_flag, pos.thread()->pvTable[pos.get_ply()][pos.get_ply()], pos);

    // node(move) fails low
    return alpha;
}

void MainThread::search() {	
	Time.init(Search::Info, rootPos);
	Threads.start_searching();

	Thread::search();

	Threads.wait_until_search_finished();

	Thread* bestThread = this;

	bestThread = Threads.get_best_thread();

	std::string output = fmt::format("bestmove {}", get_move_string(bestThread->pvTable[0][0]));

	logger.logIt(output, LOG);
	sync_cout << output << sync_endl;
}

void Thread::search() {
	// clear(reset) helper data(globals)
	Search::clear();

	int totalNodes = 0;
	std::stringstream pvr;
	std::string output;

    int alpha = -50000;
    int beta = 50000;
 
	for (int current_depth = 1; current_depth <= depth; current_depth++){
		int searchScore = 0;
		alphabeta_nodes = 0;
		quiescence_nodes = 0;

		pvr.clear();
		pvr.str("");

		// enable pv following
		followPV = true;

		searchScore = Search::negamax(alpha, beta, current_depth, rootPos);

		if (abs(searchScore) != InfinityValue) {
			bestMove = pvTable[0][0];
			score = searchScore;
		}

		if(Threads.stop)
			break;

		if (this == Threads.main()) {
			for(int count = 0; count < pvLength[0]; count++) {
	    	    // print PV moves
				pvr << get_move_string(pvTable[0][count]); 
				pvr << ' ';
	    	}

			totalNodes = Threads.nodes_searched();

			if (score > -MateValue && score < -MateScore)
				output = fmt::format("info score mate {:<6} depth {:<4} nodes {:<12} time {:<12} pv {:<50}", 
								-(score + MateValue) / 2 - 1, current_depth, totalNodes,
								Time.getElapsed(), pvr.str());

			else if (score > MateScore && score < MateValue)
				output = fmt::format("info score mate {:<6} depth {:<4} nodes {:<12} time {:<12} pv {:<50}", 
								(MateValue - score) / 2 + 1, current_depth, totalNodes,
								Time.getElapsed(), pvr.str());
			else
				output = fmt::format("info score cp {:<6} depth {:<4} nodes {:<12} time {:<12} pv {:<50}", 
								score, current_depth, totalNodes,
								Time.getElapsed(), pvr.str());

			logger.logIt(fmt::format("alphabeta: {}, quiescence: {}", Threads.alphabeta_searched(), Threads.quiescence_searched()), LOG);
			logger.logIt(output, LOG);
			sync_cout << output << sync_endl;
		}
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
			if (Time.getElapsed() > Time.getOptimum())
				Threads.stop = true;

			else if (Time.getElapsed() < Time.getOptimum() * 0.8)
				depth++;
		}
		else if (Search::Info.movetime) {
			if (Time.getElapsed() > Search::Info.movetime * 0.9) {
				Search::Info.movetime = TimePoint(0);
				Threads.stop = true;
			}
			else {
			 	depth++;
			}
		}
	}
}

void MainThread::check_time() {
	/// @todo log information each 1000 ms 
	TimePoint elapsed = Time.getElapsed();
	if (Search::Info.use_time() && (elapsed > Time.getMaximum() || Time.getElapsed() > Time.getOptimum())){
		Threads.stop = true;
	}
	else if (Search::Info.movetime) {
		if (elapsed > Search::Info.movetime * 0.95) {
			Search::Info.movetime = TimePoint(0);
			Threads.stop = true;
		}
	}
}

void Search::clear(){
 	// clear helper datas for search
}

int Search::is_repetition(const Position& pos) {
    // loop over repetition indicies range
    for (int index = 0; index < pos.repetition_index(); index++){
        // if we found the hash key same with a current
        if (pos.repetition_table(index) == pos.key()){
            // we found a repetition
            return 1;
		}
	}
    // if no repetition found
    return 0;
}

} // namespace Kojiro