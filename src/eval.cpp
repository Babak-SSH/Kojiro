#include "eval.h"


namespace Kojiro {

uint64_t file_masks[64];

uint64_t rank_masks[64];

uint64_t isolated_masks[64];

uint64_t pawn_passed_masks[2][64];

void Eval::init_eval_masks() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            
            file_masks[square] |= set_file(file);
            rank_masks[square] |= set_rank(rank);

            isolated_masks[square] |= (set_file(file-1) | set_file(file+1));
        }
    }

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            pawn_passed_masks[WHITE][square] |= (set_file(file-1) | set_file(file) | set_file(file+1));
            for (int i = 0; i < (8 - rank); i++)
                pawn_passed_masks[WHITE][square] &= ~rank_masks[i * 8 + file];

            pawn_passed_masks[BLACK][square] |= (set_file(file-1) | set_file(file) | set_file(file+1));
            for (int i = 0; i < rank + 1; i++)
                pawn_passed_masks[BLACK][square] &= ~rank_masks[(7 - i) * 8 + file];  
        }
    }
}

int Eval::evaluation(){
	int score = 0;
	int sq, double_pawns;
	uint64_t bitboard;

	for(int bb_piece=W_PAWN;bb_piece <= B_KING;bb_piece++){
		bitboard = st->bitboards[bb_piece];

		while (bitboard)
		{
			sq = get_ls1b_index(bitboard);

			score += material_score[bb_piece];

			// score positional piece scores
            switch (bb_piece)
            {
                // evaluate white pieces
                case P: 
                    score += pawn_score[sq]; 

                    /// check double pawn @todo check double pawn in endgames separately and score difference between double pawns that have protection
                    double_pawns = count_bits(st->bitboards[P] & file_masks[sq]);
                    if (double_pawns > 1)
                        score += double_pawns * double_pawn_penalty;

                    // check isolated pawn 
                    if ((st->bitboards[P] & isolated_masks[sq]) == 0)
                        score += isolated_pawn_penalty;

                    // check passed pawns 
                    if ((pawn_passed_masks[WHITE][sq] & st->bitboards[p]) == 0)
                        score += passed_pawn_bonus[(63-sq)/8];

                    break;

                case N: 
                    score += knight_score[sq];

                    break;

                case B: 
                    score += bishop_score[sq];

                    // mobility of bishop
                    score += count_bits(get_bishop_attacks(sq, occupancies[NO_COLOR]));
                    
                    break;

                case R: 
                    score += rook_score[sq];

                    // open file
                    if (((st->bitboards[P] | st->bitboards[p]) & file_masks[sq]) == 0)
                        score += open_file_score;
                    // semi open file
                    /// @todo needs more parameters
                    // else if (((st->bitboards[P] & file_masks[sq]) == 0) && ((st->bitboards[p] & file_masks[sq]) != 0))
                        // score += semi_open_file_score;
                    
                    break;

                case Q:
                    // mobility
                    // score += count_bits(get_queen_attacks(sq, occupancies[NO_COLOR]));

                    break;

                case K: 
                    score += king_score[sq];

                    // pawn shield and pawn storm
                    // open file
                    // if (((st->bitboards[P] | st->bitboards[p]) & file_masks[sq]) == 0)
                        // score -= open_file_score;
                    // semi open file
                    // else if (((st->bitboards[P] & file_masks[sq]) == 0) && ((st->bitboards[p] & file_masks[sq]) != 0))
                        // score -= semi_open_file_score;

                    // king safety bonus
                    score += count_bits(king_attacks[sq] & occupancies[WHITE]) * king_shield_bonus;

                    break;

                // evaluate black pieces
                case p: 
                    score -= pawn_score[Eval::mirror_square(sq)]; 
                    
                    // check double pawn
                    double_pawns = count_bits(st->bitboards[p] & file_masks[sq]);
                    if (double_pawns > 1)
                        score -= double_pawns * double_pawn_penalty;

                    // check isolated pawn 
                    if ((st->bitboards[p] & isolated_masks[sq]) == 0)
                        score -= isolated_pawn_penalty;

                    // check passed pawns 
                    if ((pawn_passed_masks[BLACK][sq] & st->bitboards[P]) == 0)
                        score -= passed_pawn_bonus[(63-Eval::mirror_square(sq))/8];

                    break;

                case n: 
                    score -= knight_score[Eval::mirror_square(sq)]; 
                    
                    break;

                case b: 
                    score -= bishop_score[Eval::mirror_square(sq)]; 

                    // mobility of bishop
                    /// @todo find an efficient way to find sq threatened by bishops
                    score -= count_bits(get_bishop_attacks(sq, occupancies[NO_COLOR]));
                    
                    break;

                case r: 
                    score -= rook_score[Eval::mirror_square(sq)]; 
                    
                     // open file
                    if (((st->bitboards[P] | st->bitboards[p]) & file_masks[sq]) == 0)
                        score -= open_file_score;
                    // semi open file
                    // else if (((st->bitboards[p] & file_masks[sq]) == 0) && ((st->bitboards[P] & file_masks[sq]) != 0))
                        // score -= semi_open_file_score;
                    
                    break;

                case q:
                    // mobility
                    /// @todo find better way to evaluate
                    // score -= count_bits(get_queen_attacks(sq, occupancies[NO_COLOR]));

                    break;

                case k: 
                    score -= king_score[Eval::mirror_square(sq)]; 
                    
                    // pawn shield and pawn storm
                    // open file
                    // if (((st->bitboards[P] | st->bitboards[p]) & file_masks[sq]) == 0)
                        // score += open_file_score;
                    // semi open file
                    // else if (((st->bitboards[p] & file_masks[sq]) == 0) && ((st->bitboards[P] & file_masks[sq]) != 0))
                    //     score += semi_open_file_score;

                    // king safety bonus
                    score -= count_bits(king_attacks[sq] & occupancies[BLACK]) * king_shield_bonus;

                    break;
            }

			pop_bit(bitboard, sq);
		}
	}

	return (st->side) ? -score : score;
}

} // namespace Kojiro