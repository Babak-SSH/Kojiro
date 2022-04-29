#include "uci.h"


const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; 

void UCI::go(std::istringstream& iss){
	int depth = 6;
	std::string token;

	while(iss >> token){
		if(token == "depth")
			iss >> depth;
	}
	//search best move
	Search::search(depth);
}

void UCI::position(std::istringstream& iss){
	std::string token, fen;
	int move;

	iss >> token;

	if(token == "startpos"){
		parse_fen(START_FEN);
		init_state();
		iss >> token;
	}
	else if(token == "fen"){
		while (iss >> token && token != "moves")
		{
			fen += token + " ";
		}
		parse_fen(fen);
		init_state();
	}
	if(token == "moves"){
		while (iss >> token){
			move = UCI::parse_move(token);

			if(!move)
				break;

			make_move(move, 1, *st);
		}
	}

	print_board();
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
				promoted_piece = promoted_pieces[info.promoted];
				// promoted to queen
                if ((promoted_piece == Q || promoted_piece == q) && mov[4] == 'q')
                    // return legal move
                    return move;
                
                // promoted to rook
                else if ((promoted_piece == R || promoted_piece == r) && mov[4] == 'r')
                    // return legal move
                    return move;
                
                // promoted to bishop
                else if ((promoted_piece == B || promoted_piece == b) && mov[4] == 'b')
                    // return legal move
                    return move;
                
                // promoted to knight
                else if ((promoted_piece == N || promoted_piece == n) && mov[4] == 'n')
                    // return legal move
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

		if(token == "uci"){
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
		}
		else if(token == "ucinewgame"){
			parse_fen(START_FEN);
			init_state();
			print_board();	
		}	
	}while (token != "quit" && argc == 1);
}