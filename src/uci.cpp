#include "uci.h"


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

		}
		else if(token == "position"){

		}
		else if(token == "ucinewgame"){

		}	
	}while (token != "quit" && argc == 1);
}