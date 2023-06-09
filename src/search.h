#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"
#include "eval.h"
#include "misc.h"


/// most valuable victim & less valuable attacker
/*
                          
    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600
*/

// MVV LVA [attacker][victim]
static const int mvv_lva[6][6] ={
	105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600
};

namespace Kojiro {

namespace Search
{
	/**
	 * @brief GameInfo stores data sent by GUI about time and analysis.
	 * 
	 */
	struct GameInfo
	{
		GameInfo() {
			movetime = time[WHITE] = time[BLACK] = inc[WHITE] = inc[BLACK] = startTime = TimePoint(0);
		}

		bool use_time() const {
			return time[WHITE] || time[BLACK];
		}

		TimePoint movetime, time[2], inc[2], startTime;
	};
	
	/**
	 * @brief check pv(principle variation) scoring.
	 * 
	 * @param move_list 
	 */
	static inline void enable_pv_scoring(moves *move_list, const Position& pos);

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

	/**
	 * @brief main search function to get best move
	 * 
	 * @param depth 
	 */
	void search(int depth);

	// void get_pv();
	
	/**
	 * @brief clear&reset helper datas(e.g node counts)
	 * 
	 */
	void clear();

	/**
	 * @brief 
	 * 
	 * @param pos 
	 * @return int 
	 */
	int is_repetition(const Position& pos);

	extern GameInfo Info;

} // namespace Search

} // namespace Kojiro
#endif