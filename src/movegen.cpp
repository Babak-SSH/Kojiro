#include "movegen.h"
#include "bitboard.h"
#include "position.h"
#include "types.h"


namespace Kojiro {

namespace MoveGen {

/// @todo bitboards instead of <> operaters in if statements and methods for numric operations.
/// @todo shatter to small functions.
/// generate all legal pawn moves in current state.
void generate_pawn_moves(moves *move_list, const Position& pos) {
    uint64_t bb, promotion_rank, double_push, attacks;
    int source_sq, target_sq;
    Color side = Color(pos.side());

    Piece piece = (side ? B_PAWN : W_PAWN);
    bb = pos.bitboards(P+(side*6));
    promotion_rank = (side ? RANK2 : RANK7);
    double_push = (side ? RANK7 : RANK2); 

    while (bb) {
        source_sq = get_ls1b_index(bb);
        target_sq = (side ? source_sq + 8 : source_sq - 8);

        // generate quite pawn moves
        if (!(target_sq < a8) && !get_bit(pos.occupancies(NO_COLOR), target_sq)) {
            // pawn promotion
            if ((1ull << source_sq) & promotion_rank) {
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         QUEEN, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         ROOK, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         BISHOP, 0, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         KNIGHT, 0, 0, 0, 0));
            }
            else {
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 0, 0, 0, 0));

                // two squares ahead pawn move
                if (((1ull << source_sq) & double_push) && !get_bit(pos.occupancies(NO_COLOR), (side ? target_sq + 8 : target_sq - 8))) {
                    add_move(move_list, encode_move(source_sq, (side ? target_sq + 8 : target_sq - 8), piece, 0, 0, 1, 0, 0));
                }
            }
        }
        
        // init pawn attacks bitboard
        attacks = pawn_attacks[side][source_sq] & pos.occupancies(!side);
        
        // generate pawn captures
        while (attacks) {
            // init target square
            target_sq = get_ls1b_index(attacks);
            
            // pawn promotion
            if ((1ull << source_sq) & promotion_rank) {
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         QUEEN, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         ROOK, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         BISHOP, 1, 0, 0, 0));
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         KNIGHT, 1, 0, 0, 0));
            }
            else {
                // one square ahead pawn move
                add_move(move_list, encode_move(source_sq, target_sq, piece, 0, 1, 0, 0, 0));
            }
            // pop ls1b of the pawn attacks
            pop_bit(attacks, target_sq);
        }
        
        // generate enpassant captures
        if (pos.enpassant() != no_sq) {
            // lookup pawn attacks and bitwise AND with enpassant square (bit)
            uint64_t enpassant_attacks = pawn_attacks[side][source_sq] & (1ULL << pos.enpassant());
            
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
void generate_king_moves(moves *move_list, const Position& pos){
    int source_sq, target_sq;
    uint64_t bb, attacks;
    Color side = Color(pos.side());

    Piece piece = (side ? B_KING : W_KING);
    int ksq = (side ? e8 : e1);
    bb = pos.bitboards(K+(side*6));

    while (bb) {
        // init source square
        source_sq = get_ls1b_index(bb);
        
        // init piece attacks in order to get set of target squares
        attacks = king_attacks[source_sq] & ~(pos.occupancies(side));
        
        // loop over target squares available from generated attacks
        while (attacks) {
            // init target square
            target_sq = get_ls1b_index(attacks);    
            
            // quite move
            if (!get_bit(pos.occupancies(!side), target_sq))
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         0, 0, 0, 0, 0));
            else
                // capture move
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         0, 1, 0, 0, 0));
            
            // pop ls1b in current attacks set
            pop_bit(attacks, target_sq);
        }

        // pop ls1b of the current piece bitboard copy
        pop_bit(bb, source_sq);
    }

     // king side castling is available
    if (pos.castle() & (side ? BK : WK)) {
        int sq1 = (side ? f8 : f1);
        int sq2 = (side ? g8 : g1);

        if (!get_bit(pos.occupancies(NO_COLOR), sq1) && !get_bit(pos.occupancies(NO_COLOR), sq2)) {
            if (!pos.is_square_attacked(ksq, !side) && !pos.is_square_attacked(sq1, !side))
                add_move(move_list, encode_move(ksq, sq2, piece,
                         0, 0, 0, 0, 1));
                /// @todo check consistancy for all conditions (complete rework on king moves)
                // add_move(move_list, encode_move(h1, sq1, ROOK, 0, 0, 0, 0, 1));
        }
    }
    
    // queen side castling is available
    if (pos.castle() & (side ? BQ : WQ)) {
        int sq1 = (side ? d8 : d1);
        int sq2 = (side ? c8 : c1);
        int sq3 = (side ? b8 : b1);

        // make sure square between king and queen's rook are empty
        if (!get_bit(pos.occupancies(NO_COLOR), sq1) && !get_bit(pos.occupancies(NO_COLOR), sq2) && !get_bit(pos.occupancies(NO_COLOR), sq3)) {
            // make sure king and the d1 squares are not under attacks
            if (!pos.is_square_attacked(ksq, !side) && !pos.is_square_attacked(sq1, !side))
                add_move(move_list, encode_move(ksq, sq2, piece,
                         0, 0, 0, 0, 1));
        }
    }
}

/// @todo make a template function for all pieces or find a better way
/// generate all legal moves for sliding pieces and knights.
void generate_moves(moves *move_list, const Position& pos, PieceType pType) {
    int source_sq, target_sq;
    uint64_t bb, attacks;
    Color side = Color(pos.side());

    Piece piece = (side ? (Piece(pType+side*6)) : Piece(pType));
    bb = pos.bitboards(piece);

    while (bb) {
        // init source square
        source_sq = get_ls1b_index(bb);
        
        // init piece attacks in order to get set of target squares
        if (pType == KNIGHT)
            attacks = knight_attacks[source_sq] & ~pos.occupancies(side);
        else if (pType == BISHOP)
            attacks = get_bishop_attacks(source_sq, pos.occupancies(NO_COLOR)) & ~pos.occupancies(side);
        else if (pType == ROOK)
            attacks = get_rook_attacks(source_sq, pos.occupancies(NO_COLOR)) & ~pos.occupancies(side);
        else if (pType == QUEEN)
            attacks = get_queen_attacks(source_sq, pos.occupancies(NO_COLOR)) & ~pos.occupancies(side);
        
        // loop over target squares available from generated attacks
        while (attacks) {
            // init target square
            target_sq = get_ls1b_index(attacks);    
            
            // quite move
            if (!get_bit(pos.occupancies(!side), target_sq))
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         0, 0, 0, 0, 0));
            else
                // capture move
                add_move(move_list, encode_move(source_sq, target_sq, piece,
                         0, 1, 0, 0, 0));
            
            // pop ls1b in current attacks set
            pop_bit(attacks, target_sq);
        }
        
        
        // pop ls1b of the current piece bitboard copy
        pop_bit(bb, source_sq);
    }
}

void generate_all(moves *move_list, const Position& pos){
    generate_pawn_moves(move_list, pos);
    generate_king_moves(move_list, pos);
    generate_moves(move_list, pos, KNIGHT);
    generate_moves(move_list, pos, BISHOP);
    generate_moves(move_list, pos, ROOK);
    generate_moves(move_list, pos, QUEEN);
}

} // namespcae MoveGen

} // namespace Kojiro
