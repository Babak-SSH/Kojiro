#ifndef SEARCH_H
#define SEARCH_H

#include "position.h"
#include "uci.h"
#include "eval.h"


namespace Search
{
	/**
	 * @brief quiescence search for horizon effect.
	 * 
	 * @param alpha 
	 * @param beta 
	 * @return int 
	 */
	static int quiescence(int alpha, int beta);

	/**
	 * @brief negamax alpha beta search (recursive)
	 * 
	 * @param alpha 
	 * @param beta 
	 * @param depth 
	 * @return int 
	 */
	static int negamax(int alpha, int beta, int depth);

	/**
	 * @brief main search function to get best move
	 * 
	 * @param depth 
	 */
	void search(int depth);
} // namespace Search
#endif