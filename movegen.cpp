#include "movegen.h"


moves* generate_pawn_moves(moves* move_list, Color side){
    uint64_t bb, bb_attacks, promotion_rank, no_double_push, attacks;
    int source_sq, target_sq;

    Piece piece = (side ? W_PAWN : B_PAWN);
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
                add_move(move_list, encode_move(source_sq, target_sq, piece, Q, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, R, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, B, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, N, 0, 0, 0, 0));
            }
            
            else
            {
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));
                
                // two squares ahead pawn move
                if ((source_sq >= a2 && source_sq <= h2) && !get_bit(occupancies[NO_COLOR], target_sq - 8))
                    add_move(move_list, encode_move(source_sq, target_sq - 8, piece, 0, 0, 1, 0, 0));
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
                add_move(move_list, encode_move(source_sq, target_sq, piece, Q, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, R, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, B, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, N, 1, 0, 0, 0));
            }
            
            else
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
            
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
                add_move(move_list, encode_move(source_sq, target_enpassant, piece, 0, 1, 0, 1, 0));
            }
        }
        
        // pop ls1b from pawn bitboard copy
        pop_bit(bb, source_sq);
    }
}

moves* generate_king_moves(moves* move_list, Color side){
    int source_sq, target_sq;
    uint64_t bb, attacks;

    Piece piece = (side ? W_KING : B_KING);
    int ksq = (side ? e1 : e8);
    bb = bitboards[K+(side*6)];

    while (bb)
    {
        // init source square
        source_sq = get_ls1b_index(bb);
        
        // init piece attacks in order to get set of target squares
        attacks = king_attacks[source_sq] & ~occupancies[side];
        
        // loop over target squares available from generated attacks
        while (attacks)
        {
            // init target square
            target_sq = get_ls1b_index(attacks);    
            
            // quite move
            if (!get_bit(occupancies[side], target_sq))
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));
            
            else
                // capture move
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
            
            // pop ls1b in current attacks set
            pop_bit(attacks, target_sq);
        }

        // pop ls1b of the current piece bitboard copy
        pop_bit(bb, source_sq);
    }

     // king side castling is available
    if (castle & (side ? WK : BK))
    {
        int sq1 = (side ? f1 : f8);
        int sq2 = (side ? g1 : g8);

        if (!get_bit(occupancies[NO_COLOR], sq1) && !get_bit(occupancies[NO_COLOR], sq2))
        {
            if (!is_square_attacked(ksq, !side) && !is_square_attacked(sq1, !side))
                add_move(move_list, encode_move(ksq, sq2, piece, 0, 0, 0, 0, 1));
                /// @todo check consistancy for all conditions (complete rework on king moves)
                // add_move(move_list, encode_move(h1, sq1, ROOK, 0, 0, 0, 0, 1));
        }
    }
    
    // queen side castling is available
    if (castle & (side ? WQ : BQ))
    {
        int sq1 = (side ? d1 : d8);
        int sq2 = (side ? c1 : c8);
        int sq3 = (side ? b1 : b8);

        // make sure square between king and queen's rook are empty
        if (!get_bit(occupancies[NO_COLOR], sq1) && !get_bit(occupancies[NO_COLOR], sq2) && !get_bit(occupancies[NO_COLOR], sq3))
        {
            // make sure king and the d1 squares are not under attacks
            if (!is_square_attacked(e1, !side) && !is_square_attacked(d1, !side))
                add_move(move_list, encode_move(e1, c1, piece, 0, 0, 0, 0, 1));
        }
    }
}

moves* generate_moves(PieceType piece, moves* move_list){

}

void generate_all(moves* move_list, Color side){
    move_list = generate_pawn_moves(move_list, side);
    move_list = generate_king_moves(move_list, side);
    move_list = generate_moves(KNIGHT, move_list);
    move_list = generate_moves(BISHOP, move_list);
    move_list = generate_moves(ROOK, move_list);
    move_list = generate_moves(QUEEN, move_list);
}

