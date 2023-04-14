#include "perft.h"
#include "../src/thread.h"


using namespace Kojiro;

long nodes = 0;

int get_time_ms() {
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}

void init_all() {
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

/// loop over each generated moves for current colors turn
/// by calling itself recursivly.
static inline void perft_driver(int depth, Position& pos) {
    // reccursion escape condition
    if (depth == 0) {
        // increment nodes count (count reached positions)
        nodes++;
        return;
    }
    
    moves move_list[1];
    MoveGen::generate_all(move_list, pos);
    
        // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++) {
        StateInfo nST;
        int move = move_list->moves[move_count];

        if (!pos.make_move(move, 1, nST, depth))
            continue;
        perft_driver(depth - 1, pos);

        pos.take_back();
    }
}

/// generates all moves and nodes and uses perft_driver.
/// calculates the time spend for the depth given in ms.
void perft_test(int depth, Position& pos) {
    printf("\n     Performance test\n\n");
    
    nodes = 0;
    moves move_list[1];
    MoveGen::generate_all(move_list, pos);
    pos.state()->side ^= 1;
    MoveGen::generate_all(move_list, pos);
    
    // init start time
    long start = get_time_ms();
    
    // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++) {   
        StateInfo nST;
        int move = move_list->moves[move_count];

        if (!pos.make_move(move, 1, nST, depth))
            continue;
        
        // call perft driver recursively
        perft_driver(depth - 1, pos);
        
        pos.take_back();
        moveInfo info = decode_move(move);
    }
    
    // print results
    fmt::print("\nDepth: {:<6}\n", depth);
    fmt::print("Nodes: {:<6}\n", nodes);
    fmt::print("Time: {:<6}\n\n", get_time_ms() - start);
    fmt::print("captures: {:<6}\nenpassant: {:<6}\ncastles: {:<6}\n", Threads.captures_searched(), Threads.enpassants_searched(), Threads.captures_searched());
}


/// use format example: perft 6 rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
int main(int argc, char* argv[]) {
	// set all moves and attacks of leaper and slider pieces
	if(argc == 2 && strcmp(argv[1], "--help")==0){
		fmt::print("perft depth fen\ndefault:perft 5 rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\n");
	}
	else if(argc == 3){
		init_all();

        Position pos;
        StateInfo state;
		
		pos.parse_fen(argv[2], &state, Threads.main());
		pos.log_board();

		int start = get_time_ms();
		perft_test(atoi(argv[1]), pos);
		
		/// @todo optimize speed?
		// delete st;
	}
	else{
		fmt::print("invalid input try: ./perft --help\n");
	}

    return 0;    
}