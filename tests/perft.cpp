/***********************************************\
=================================================
                    PERFT TEST
=================================================
\***********************************************/

#include "perft.h"

long nodes = 0;

int get_time_ms()
{
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}

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

/// loop over each generated moves for current colors turn
/// by calling itself recursivly.
static inline void perft_driver(int depth)
{
    // reccursion escape condition
    if (depth == 0)
    {
        // increment nodes count (count reached positions)
        nodes++;
        return;
    }
    
    moves move_list[1];
    // generate_all(move_list, WHITE);
    // generate_all(move_list, BLACK);
    generate_all(move_list, Color(st->side));
    
        // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        StateInfo nST;
        int move = move_list->moves[move_count];

        if (!make_move(move, depth, nST))
            continue;
        perft_driver(depth - 1);

        take_back();
    }
}

/// generates all moves and nodes and uses perft_driver.
/// calculates the time spend for the depth given in ms.
void perft_test(int depth)
{
    captures_count = 0;
    enpassant_count = 0;
    castles_count = 0;
    printf("\n     Performance test\n\n");
    
    nodes = 0;
    moves move_list[1];
    generate_all(move_list, WHITE);
    generate_all(move_list, BLACK);
    
    // init start time
    long start = get_time_ms();
    
    // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {   
        StateInfo nST;
        int move = move_list->moves[move_count];

        if (!make_move(move, move_count, nST))
            continue;
        
        // cummulative nodes
        long cummulative_nodes = nodes;
        
        // call perft driver recursively
        perft_driver(depth - 1);
        
        // old nodes
        long old_nodes = nodes - cummulative_nodes;
        
        // take back
        take_back();
        moveInfo info = decode_move(move);

    // printf("     move: %s%s%c  nodes: %ld\n", convert_to_square[info.source],
    //                  convert_to_square[info.target],
    //                  promoted_pieces[info.promoted],
    //                  old_nodes);
    }
    
    // print results
    printf("\n    Depth: %d\n", depth);
    printf("    Nodes: %ld\n", nodes);
    printf("     Time: %ld\n\n", get_time_ms() - start);
    printf("captures: %d\nenpassant: %d\ncastles: %d\n", captures_count, enpassant_count, castles_count);
}

int main(){
    init_all();
    
    parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    init_state();
    print_board();

    int start = get_time_ms();

    // perft_driver(3);

    perft_test(1);
    perft_test(2);
    perft_test(3);
    perft_test(4);
    perft_test(5);
    perft_test(6);
    // perft_test(10);

    // moves move_list[1];
    // generate_all(move_list, WHITE);
    // generate_all(move_list, BLACK);
    // print_move_list(move_list);

    // for(int i = 0; i < move_list->count; i++){
    //     StateInfo nST;
    //     int move = move_list->moves[i];

    //     if (make_move(move, i, nST)){
    //         print_board();
    //         getchar();

    //         take_back();
    //         print_board();
    //         getchar();
    //     }
    //     else{
    //         printf("%d\n", i);
    //     }
    // }
    // printf("excution time: %d ms\n", get_time_ms() - start);
    // printf("nodes: %ld\n", nodes);
    // printf("captures: %d\nenpassant: %d\ncastles: %d\n", captures_count, enpassant_count, castles_count);
    /// @todo optimize speed?
    // delete st;

    return 0;    
}