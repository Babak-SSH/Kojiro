#ifndef UCI_H
#define UCI_H

#include <iostream>
#include <cstdio>
#include <mutex>
#include <sstream>

#include "position.h"
#include "movegen.h"
#include "search.h"
#include "misc.h"

/// Used to serialize access to std::cout to avoid multiple threads writing at
/// the same time.
inline std::ostream& operator<<(std::ostream& os, SyncCout sc) {

  static std::mutex m;

  if (sc == IO_LOCK)
      m.lock();

  if (sc == IO_UNLOCK)
      m.unlock();

  return os;
}

namespace UCI{
	/**
	 * @brief main loop for recieving and sending UCI commands.
	 * 
	 * @param argc 
	 * @param argv 
	 */
	void loop(int argc, char* argv[]);

    /**
     * @brief Benchmark command to run a standard search benchmark on pre-selected positions.
     * 
     * @param pos
     * @param state
     * @param depth 
     */
    void bench(Kojiro::Position& pos, Kojiro::StateInfo& state, int depth);

	/**
	 * @brief UCI go command (depth, searchmoves,...)
	 * 
	 * @param iss 
	 */
	void go(Kojiro::Position& pos, std::istringstream& iss);

	/**
	 * @brief UCI position command (fen, startpos, moves,...)
	 * 
	 * @param iss 
	 */
	void position(std::istringstream& iss, Kojiro::Position& pos, Kojiro::StateInfo& state);

	/**
	 * @brief get the move notaion from GUI and convert to encrypted int move.
	 * 
	 * @param mov input move 
	 * @return int 
	 */
	int parse_move(std::string mov, const Kojiro::Position& state);

	/**
	 * @brief Set options
	 * 
	 * @param iss 
	 */
	void set_option(std::istringstream& iss);
}
#endif