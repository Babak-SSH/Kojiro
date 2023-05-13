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
#include "tt.h"
#include "thread.h"
#include "logger.h"


using namespace Kojiro;

void init_all() {
    /// init_magic generates the magic hashes for leaper pieces,
    /// it only needs to run once if you chnage the random seed.
    // init_magic();

    // sliders
    init_rook_attacks();
    init_bishop_attacks();
    
    // leapers
    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();

	Eval::init_eval_masks();

	Position::init_hash();

	TT::init_hash_table(128);
}

int main(int argc, char* argv[]){
	logger.setDebug(true);

    init_all();
	Threads.set(size_t(1));

	UCI::loop(argc, argv);
	
	Threads.set(0);
    /// @todo optimize speed?
    // delete st;


    return 0;    
}