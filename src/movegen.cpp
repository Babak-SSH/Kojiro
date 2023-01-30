#include "movegen.h"
#include "bitboard.h"


namespace Kojiro {

/// @todo bitboards instead of <> operaters in if statements and methods for numric operations.
/// @todo shatter to small functions.
/// generate all legal pawn moves in current state.
void generate_pawn_moves(moves *move_list, Color side){
    uint64_t bb, bb_attacks, promotion_rank, double_push, attacks;
    int source_sq, target_sq;

    Piece piece = (side ? B_PAWN : W_PAWN);
    bb = st->bitboards[P+(side*6)];
    promotion_rank = (side ? RANK2 : RANK7);
    double_push = (side ? RANK7 : RANK2); 

    while (bb) {
        source_sq = get_ls1b_index(bb);
        target_sq = (side ? source_sq + 8 : source_sq - 8);

        // generate quite pawn moves
        if (!(target_sq < a8) && !get_bit(st->occupancies[NO_COLOR], target_sq)) {
            // pawn promotion
            if ((1ull << source_sq) & promotion_rank) {
                add_move(move_list, encode_move(source_sq, target_sq, piece, Q, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, R, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, B, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, N, 0, 0, 0, 0));
            }
            else {
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));

                // two squares ahead pawn move
                if (((1ull << source_sq) & double_push) && !get_bit(st->occupancies[NO_COLOR], (side ? target_sq + 8 : target_sq - 8))) {
                    add_move(move_list, encode_move(source_sq, (side ? target_sq + 8 : target_sq - 8), piece, 0, 0, 1, 0, 0));
                }
            }
        }
        
        // init pawn attacks bitboard
        attacks = pawn_attacks[side][source_sq] & st->occupancies[!side];
        
        // generate pawn captures
        while (attacks) {
            // init target square
            target_sq = get_ls1b_index(attacks);
            
            // pawn promotion
            if ((1ull << source_sq) & promotion_rank) {
                add_move(move_list, encode_move(source_sq, target_sq, piece, Q, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, R, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, B, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece, N, 1, 0, 0, 0));
            }
            else {
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
            }
            // pop ls1b of the pawn attacks
            pop_bit(attacks, target_sq);
        }
        
        // generate enpassant captures
        if (st->enpassant != no_sq) {
            // lookup pawn attacks and bitwise AND with enpassant square (bit)
            uint64_t enpassant_attacks = pawn_attacks[side][source_sq] & (1ULL << st->enpassant);
            
            // make sure enpassant capture available
            if (enpassant_attacks) {
                // init enpassant capture target square
                int target_enpassant = get_ls1b_index(enpassant_attacks);
                add_move(move_list, encode_move(source_sq, target_enpassant, piece, 0, 1, 0, 1, 0));
            }
        }
        
        // pop ls1b from pawn bitboard copy
        pop_bit(bb, source_sq);
    }
}

/// generate all legal king moves in current state.
void generate_king_moves(moves *move_list, Color side){
    int source_sq, target_sq;
    uint64_t bb, attacks;

    Piece piece = (side ? B_KING : W_KING);
    int ksq = (side ? e8 : e1);
    bb = st->bitboards[K+(side*6)];

    while (bb) {
        // init source square
        source_sq = get_ls1b_index(bb);
        
        // init piece attacks in order to get set of target squares
        attacks = king_attacks[source_sq] & ~(st->occupancies[side]);
        
        // loop over target squares available from generated attacks
        while (attacks) {
            // init target square
            target_sq = get_ls1b_index(attacks);    
            
            // quite move
            if (!get_bit(st->occupancies[!side], target_sq))
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
    if (st->castle & (side ? BK : WK)) {
        int sq1 = (side ? f8 : f1);
        int sq2 = (side ? g8 : g1);

        if (!get_bit(st->occupancies[NO_COLOR], sq1) && !get_bit(st->occupancies[NO_COLOR], sq2)) {
            if (!is_square_attacked(ksq, !side) && !is_square_attacked(sq1, !side))
                add_move(move_list, encode_move(ksq, sq2, piece, 0, 0, 0, 0, 1));
                /// @todo check consistancy for all conditions (complete rework on king moves)
                // add_move(move_list, encode_move(h1, sq1, ROOK, 0, 0, 0, 0, 1));
        }
    }
    
    // queen side castling is available
    if (st->castle & (side ? BQ : WQ)) {
        int sq1 = (side ? d8 : d1);
        int sq2 = (side ? c8 : c1);
        int sq3 = (side ? b8 : b1);

        // make sure square between king and queen's rook are empty
        if (!get_bit(st->occupancies[NO_COLOR], sq1) && !get_bit(st->occupancies[NO_COLOR], sq2) && !get_bit(st->occupancies[NO_COLOR], sq3)) {
            // make sure king and the d1 squares are not under attacks
            if (!is_square_attacked(ksq, !side) && !is_square_attacked(sq1, !side))
                add_move(move_list, encode_move(ksq, sq2, piece, 0, 0, 0, 0, 1));
        }
    }
}

/// @todo make a template function for all pieces or find a better way
/// generate all legal moves for sliding pieces and knights.
void generate_moves(moves *move_list, Color side, PieceType pType) {
    int source_sq, target_sq;
    uint64_t bb, attacks;

    Piece piece = (side ? (Piece(pType+side*6)) : Piece(pType));
    bb = st->bitboards[piece];

    while (bb) {
        // init source square
        source_sq = get_ls1b_index(bb);
        
        // init piece attacks in order to get set of target squares
        if (pType == KNIGHT)
            attacks = knight_attacks[source_sq] & ~st->occupancies[side];
        else if (pType == BISHOP)
            attacks = get_bishop_attacks(source_sq, st->occupancies[NO_COLOR]) & ~st->occupancies[side];
        else if (pType == ROOK)
            attacks = get_rook_attacks(source_sq, st->occupancies[NO_COLOR]) & ~st->occupancies[side];
        else if (pType == QUEEN)
            attacks = get_queen_attacks(source_sq, st->occupancies[NO_COLOR]) & ~st->occupancies[side];
        
        // loop over target squares available from generated attacks
        while (attacks) {
            // init target square
            target_sq = get_ls1b_index(attacks);    
            
            // quite move
            if (!get_bit(st->occupancies[!side], target_sq))
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
}

void generate_all(moves *move_list, Color side){
    generate_pawn_moves(move_list, side);
    generate_king_moves(move_list, side);
    generate_moves(move_list, side, KNIGHT);
    generate_moves(move_list, side, BISHOP);
    generate_moves(move_list, side, ROOK);
    generate_moves(move_list, side, QUEEN);
}

} // namespace Kojiro
