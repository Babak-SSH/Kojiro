#include "uci.h"
#include "thread.h"
#include "tt.h"


using namespace Kojiro;

void UCI::go(std::istringstream& iss){

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
	}

	//search best move
	Threads.start_thinking(info, false, depth);
}

void UCI::position(std::istringstream& iss){

	std::string token, fen;
	int move;

	iss >> token;

	if(token == "startpos"){
		init_state();
		parse_fen(START_FEN);
		iss >> token;
	}
	else if(token == "fen"){
		while (iss >> token && token != "moves")
		{
			fen += token + " ";
		}
		init_state();
		parse_fen(fen);
	}
	if(token == "moves"){
		while (iss >> token){
			move = UCI::parse_move(token);

			if(!move)
				break;

			make_move(move, 1, *st);
		}
	}

	log_board();
}

int UCI::parse_move(std::string mov){

	int source_sq, target_sq;
	char promoted_piece;
	moves move_list[1];
	moveInfo info;

	generate_all(move_list, Color(st->side));

	source_sq = (mov[0] - 'a') + (8 - (mov[1] - '0')) * 8;
	target_sq = (mov[2] - 'a') + (8 - (mov[3] - '0')) * 8;

	for(int move_count = 0; move_count < move_list->count; move_count++){
		int move = move_list->moves[move_count];
		info = decode_move(move);

		if(info.source == source_sq && info.target == target_sq){
			if(info.promoted){
				// promoted to queen
                if ((info.promoted == Q || info.promoted == q) && mov[4] == 'q'){
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

void UCI::loop(int argc, char* argv[]){

	std::string token, cmd;

	std::istringstream iss (cmd);

	

	for (int i = 1; i < argc; ++i)
    	cmd += std::string(argv[i]) + " ";

	do{
		if(argc == 1 && !getline(std::cin, cmd)){
			cmd = "quit";
		}

		
		std::istringstream iss(cmd);

      	token.clear(); // Avoid a stale if getline() returns empty or blank line
      	iss >> std::skipws >> token;

		if(token == "quit" || token == "stop")
			Threads.stop = true;

		else if(token == "uci"){
			sync_cout << "id name kojiro\n"
			  << "id author babak sefidgar\n"
			  << "uciok"
			  << sync_endl;
		}
		else if(token == "isready"){
			sync_cout << "readyok" << sync_endl;
		}
		else if(token == "go"){
			go(iss);
		}
		else if(token == "position"){
			position(iss);
			clear_tt();
		}
		else if(token == "ucinewgame"){
			init_state();
			parse_fen(START_FEN);
			clear_tt();
			log_board();	
		}	
	}while (token != "quit" && argc == 1);
}