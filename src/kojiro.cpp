/***********************************************\
=================================================
                    MAIN
=================================================
\***********************************************/

#include <stdlib.h>
#include <sys/time.h>

#include "bitboard.h"
#include "magic.h"
#include "types.h"
#include "position.h"
#include "pawn.h"
#include "knight.h"
#include "king.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "movegen.h"
#include "uci.h"
#include "eval.h"
#include "search.h"


void init_all(){
    // sliders
    // init_magic();
    init_rook_attacks();
    init_bishop_attacks();
    
    // leapers
    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();

    // init_start();

}

int main(int argc, char* argv[]){
    init_all();

	int debug = 0;

	if(debug){
	    parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    	init_state();
    	print_board();
		Search::search(2);
	}
	else{
	    parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    	init_state();
    	print_board();

		UCI::loop(argc, argv);
	}
	
    /// @todo optimize speed?
    // delete st;


    return 0;    
}