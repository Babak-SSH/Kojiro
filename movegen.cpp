#include "movegen.h"


moves* generate_pawn_moves(moves* move_list, Color side){
    uint64_t bb, bb_attacks, promotion_rank, no_double_push, attacks;
    int source_sq, target_sq;

    bb = bitboards[P+(side*6)];
    /// @todo using these kind of bitboards instead of <> operaters in if statements
    promotion_rank = bb & (side ? RANK2 : RANK7);
    no_double_push = bb & (side ? RANK6 : RANK3); 

    while (bb)
    {
        source_sq = get_ls1b_index(bb);
        target_sq = (side ? source_sq - 8 : source_sq + 8);

        // generate quite pawn moves
        if (!(target_sq < a8) && !get_bit(occupancies[NO_COLOR], target_sq))
        {
            // pawn promotion
            if (source_sq >= a7 && source_sq <= h7)
            {                            
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, Q, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, R, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, B, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, N, 0, 0, 0, 0));
            }
            
            else
            {
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, 0, 0, 0, 0, 0));
                
                // two squares ahead pawn move
                if ((source_sq >= a2 && source_sq <= h2) && !get_bit(occupancies[NO_COLOR], target_sq - 8))
                    add_move(move_list, encode_move(source_sq, target_sq - 8, PAWN, 0, 0, 1, 0, 0));
            }
        }
        
        // init pawn attacks bitboard
        attacks = pawn_attacks[side][source_sq] & occupancies[!side];
        
        // generate pawn captures
        while (attacks)
        {
            // init target square
            target_sq = get_ls1b_index(attacks);
            
            // pawn promotion
            if (source_sq >= a7 && source_sq <= h7)
            {
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, Q, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, R, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, B, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, N, 1, 0, 0, 0));
            }
            
            else
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, PAWN, 0, 1, 0, 0, 0));
            
            // pop ls1b of the pawn attacks
            pop_bit(attacks, target_sq);
        }
        
        // generate enpassant captures
        if (enpassant != no_sq)
        {
            // lookup pawn attacks and bitwise AND with enpassant square (bit)
            uint64_t enpassant_attacks = pawn_attacks[side][source_sq] & (1ULL << enpassant);
            
            // make sure enpassant capture available
            if (enpassant_attacks)
            {
                // init enpassant capture target square
                int target_enpassant = get_ls1b_index(enpassant_attacks);
                add_move(move_list, encode_move(source_sq, target_enpassant, PAWN, 0, 1, 0, 1, 0));
            }
        }
        
        // pop ls1b from pawn bitboard copy
        pop_bit(bb, source_sq);
    }
}

moves* generate_king_moves(moves* move_list){

}

moves* generate_moves(PieceType piece, moves* move_list){

}

void generate_all(moves* move_list, Color side){
    move_list = generate_pawn_moves(move_list, side);
    move_list = generate_king_moves(move_list);
    move_list = generate_moves(KNIGHT, move_list);
    move_list = generate_moves(BISHOP, move_list);
    move_list = generate_moves(ROOK, move_list);
    move_list = generate_moves(QUEEN, move_list);
}

