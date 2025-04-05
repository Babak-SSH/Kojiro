#include "uci.h"
#include "bitboard.h"
#include "cons.h"
#include "logger.h"
#include "misc.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "tt.h"
#include "perft.h"

#include <cstddef>
#include <ios>
#include <iostream>
#include <sstream>


using namespace Kojiro;

void UCI::go(Position& pos, std::istringstream& iss) {
	Search::GameInfo info;
	int depth = 6;
	std::string token;

	info.startTime = now();

	while(iss >> token){
		if (token == "depth")
			iss >> depth;
		else if (token == "wtime")
			iss >> info.time[WHITE];
		else if (token == "btime")
			iss >> info.time[BLACK];
		else if (token == "winc")
			iss >> info.inc[WHITE];
		else if (token == "binc")
			iss >> info.inc[BLACK];
		else if (token == "movetime")
			iss >> info.movetime;
	}
	
	//search best move
	Threads.start_thinking(pos, info, false, depth);
}

void UCI::bench(Position& pos, StateInfo& state ,int depth) {
    long BenchStartTime = now();

    uint64_t nodes = 0;

    for (size_t i = 0; i < benchmarkPositions.size(); i++) {
        Search::GameInfo info;
        info.startTime = now();
        
        std::istringstream iss("fen " + std::string(benchmarkPositions[i]), std::ios_base::in);
        position(iss, pos, state);

        TT::clear_tt();

	    Threads.start_thinking(pos, info, false, depth);
        Threads.main()->wait_until_search_finished();

        nodes += Threads.nodes_searched();
    }

    sync_cout << "============================\n" 
              << "Nodes Searched: " << nodes << "\n"
              << "Nodes/Second: " << nodes/1000 << "\n"
              << "Total Time: " << now() - BenchStartTime << "ms\n" 
              << "============================\n"
              << sync_endl;
}

void UCI::position(std::istringstream& iss, Position& pos, StateInfo& state) {
	std::string token, fen;
	int move;

	iss >> token;

	if(token == "startpos") {
		pos.parse_fen(START_FEN, &state, Threads.main());
		iss >> token;
	}
	else if(token == "fen") {
		while (iss >> token && token != "moves")
		{
			fen += token + " ";
		}
		state = StateInfo();
		pos.parse_fen(fen, &state, Threads.main());
	}
	if(token == "moves") {
		while (iss >> token) {
			move = UCI::parse_move(token, pos);

			if(!move) {break;} // couldn't parse the moves in fen (wrong format)

			pos.make_move(move, 1, state);
		}
	}

	pos.log_board();
}

int UCI::parse_move(std::string mov, const Position& pos) {

	int source_sq, target_sq;
	// char promoted_piece;
	moves move_list[1];
	moveInfo info;

	MoveGen::generate_all(move_list, pos);

	source_sq = (mov[0] - 'a') + (8 - (mov[1] - '0')) * 8;
	target_sq = (mov[2] - 'a') + (8 - (mov[3] - '0')) * 8;

	for(int move_count = 0; move_count < move_list->count; move_count++) {
		int move = move_list->moves[move_count];
		info = decode_move(move);

		if(info.source == source_sq && info.target == target_sq){
			if(info.promoted){
				// promoted to queen
                if ((info.promoted == Q || info.promoted == q) && mov[4] == 'q') {
                    return move;
				}
                
                // promoted to rook
                else if ((info.promoted == R || info.promoted == r) && mov[4] == 'r')
                    return move;
                
                // promoted to bishop
                else if ((info.promoted == B || info.promoted == b) && mov[4] == 'b')
                    return move;
                
                // promoted to knight
                else if ((info.promoted == N || info.promoted == n) && mov[4] == 'n')
                    return move;
                
                // continue the loop on possible wrong promotions (e.g. "e7e8f")
                continue;
			}

			return move;
		}
	}
	return 0;
}

void UCI::set_option(std::istringstream& iss) {
	std::string token, name;
	int value;

	while(iss >> token) {
		if(token == "name")
			iss >> name;
		if (token == "value")
			iss >> value;
	}

	if (name == "Hash") {
		TT::init_hash_table(value);
	}
	else if (name == "Threads") {
		Threads.set(value);
	}
}

void UCI::loop(int argc, char* argv[]) {
	Position pos;
	StateInfo state;

	pos.parse_fen(START_FEN, &state, Threads.main());
	std::string token, cmd;

	std::istringstream iss (cmd);

	for (int i = 1; i < argc; ++i)
    	cmd += std::string(argv[i]) + " ";

	do {
		if(argc == 1 && !getline(std::cin, cmd)) {
			cmd = "quit";
		}

		logger.logIt(cmd, LOG);

		std::istringstream iss(cmd);

      	token.clear(); // Avoid a stale if getline() returns empty or blank line
      	iss >> std::skipws >> token;

        if(token == "quit" || token == "stop") {
			Threads.stop = true;
		}
        else if(token == "uci") {
			sync_cout << "id name kojiro " << version << "\n"
			  << "id author babak-ssh \n"
			  << "option name Threads type spin default 1 min 1 max 512\n"
			  << "option name Hash type spin default 1 min 1 max 2048\n"
			  << "uciok"
			  << sync_endl;
		}
        else if(token == "isready") {
			sync_cout << "readyok" << sync_endl;
		}
        else if(token == "go") {
			go(pos, iss);
		}
        else if(token == "position") {
			UCI::position(iss, pos, state);
			TT::clear_tt();
		}
        else if(token == "ucinewgame") {
			TT::clear_tt();
			Search::clear();
        }	
        else if (token == "setoption") {
            set_option(iss);	
        }
        else if (token == "perft") {
            int depth = 6;
            iss >> depth;
            perft(depth, pos);
        }
        else if (token == "bench") {
            int depth = 10;
            iss >> depth;
            bench(pos, state, depth);
        }
	}while (token != "quit" && argc == 1);
}