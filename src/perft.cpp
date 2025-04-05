#include "perft.h"
#include "misc.h"
#include "thread.h"


using namespace Kojiro;

long nodes = 0;

/// loop over each generated moves for current colors turn
/// by calling itself recursivly.
void perft_driver(int depth, Position& pos) {
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
void perft(int depth, Position& pos) {
    nodes = 0;
    moves move_list[1];
    MoveGen::generate_all(move_list, pos);
    pos.state()->side ^= 1;
    MoveGen::generate_all(move_list, pos);
    
    // init start time
    long PerftStartTime = now();
    
    // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++) {   
        StateInfo nST;
        int move = move_list->moves[move_count];

        if (!pos.make_move(move, 1, nST, depth))
            continue;
        
        // call perft driver recursively
        perft_driver(depth - 1, pos);
        
        pos.take_back();
    }
    
    // print results
    fmt::print("\nDepth: {:<6}\n", depth);
    fmt::print("Nodes: {:<10}\n", nodes);
    fmt::print("captures: {:<10}\nenpassant: {:<10}\ncastles: {:<10}\n", Threads.captures_searched(), Threads.enpassants_searched(), Threads.captures_searched());
    fmt::print("Time(ms): {:<10}\n", now() - PerftStartTime);
}