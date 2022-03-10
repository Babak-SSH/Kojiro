#include "movegen.h"


moves* generate_pawn_moves(moves* move_list, Color side){

}

moves* generate_king_moves(moves* move_list){

}

moves* generate_moves(PieceType piece, moves* move_list){

}

void generate_all(moves* move_list){
    move_list = generate_pawn_moves(move_list);
    move_list = generate_king_moves(move_list);
    move_list = generate_moves(KNIGHT, move_list);
    move_list = generate_moves(BISHOP, move_list);
    move_list = generate_moves(ROOK, move_list);
    move_list = generate_moves(QUEEN, move_list);
}

