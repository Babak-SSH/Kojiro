#include "eval.h"
#include "position.h"
#include <thread>


namespace Kojiro {

uint64_t file_masks[64];

uint64_t rank_masks[64];

uint64_t isolated_masks[64];

uint64_t pawn_passed_masks[2][64];

namespace Eval {


/// initializing general rank and file bitboards for all pieces
/// and isolated files(not protected from left or right) and passed
/// pawns mask for both side.
void init_eval_masks() {
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

/// guessing phase of the game by counting each piece from each side and 
/// suming them according to their value in midgame.
int get_phase_score(const Position& pos) {
    int score = 0;

    for(int piece=W_PAWN;piece <= B_KING;piece++) {
       score += count_bits(pos.bitboards(piece)) * PieceValue[0][piece]; 
    }   

    return score;
}

/// evaluating game score in each state by evaluating each piece from each side.
int evaluation(const Position& pos) {
	int score = 0;
	int sq, double_pawns;
	uint64_t bitboard;

    int phase_score = get_phase_score(pos);

	for(int bb_piece=W_PAWN;bb_piece <= B_KING;bb_piece++) {
		bitboard = pos.bitboards(bb_piece);

        /// Step 1. values of each piece according to the stage of game which
        /// is calculated by the difference of phase score with MidgameLimit,
        /// so if we are close to endgame stage piece values in endgame will
        /// have more impact on the final score.
		while (bitboard) {
			sq = get_ls1b_index(bitboard);

            if(bb_piece >= B_PAWN) {
                score -= (PieceValue[MG][bb_piece] * phase_score + 
                PieceValue[EG][bb_piece] * (MidgameLimit - phase_score))/MidgameLimit;
            }
            else {
                score += (PieceValue[MG][bb_piece] * phase_score + 
                PieceValue[EG][bb_piece] * (MidgameLimit - phase_score))/MidgameLimit;
            }
            
			/// Step 2. positional piece scores. calculating the static positional
            /// score with midgame and endgame difference with phase score for all
            /// pieces and then evaluating specific conditions for each pieces position.
            switch (bb_piece) {
                // evaluate white pieces
                case P: 
                    score += (PawnPosValue[MG][sq] * phase_score +
                            PawnPosValue[EG][sq] * (MidgameLimit - phase_score))/MidgameLimit; 

                    /// check double pawn @todo check double pawn in endgames separately and score difference between double pawns that have protection
                    double_pawns = count_bits(pos.bitboards(P) & file_masks[sq]);
                    if (double_pawns > 1)
                        score += double_pawns * double_pawn_penalty;

                    /// check isolated pawn that doesnt have protection
                    if ((pos.bitboards(P) & isolated_masks[sq]) == 0)
                        score += isolated_pawn_penalty;

                    /// check passed pawns 
                    if ((pawn_passed_masks[WHITE][sq] & pos.bitboards(p)) == 0)
                        score += passed_pawn_bonus[(63-sq)/8];

                    break;

                case N: 
                    score += (KnightPosValue[MG][sq] * phase_score +
                            KnightPosValue[EG][sq] * (MidgameLimit - phase_score))/MidgameLimit;

                    break;

                case B: 
                    score += (BishopPosValue[MG][sq] * phase_score +
                            BishopPosValue[EG][sq] * (MidgameLimit - phase_score))/MidgameLimit;

                    /// mobility of bishop
                    score += count_bits(get_bishop_attacks(sq, pos.occupancies(NO_COLOR)));
                    
                    break;

                case R: 
                    score += (RookPosValue[MG][sq] * phase_score +
                            RookPosValue[EG][sq] * (MidgameLimit - phase_score))/MidgameLimit;

                    /// check for open files
                    if (((pos.bitboards(P) | pos.bitboards(p)) & file_masks[sq]) == 0)
                        score += open_file_score;
                    /// check for semi open files
                    /// @todo needs more parameters
                    // else if (((pos.bitboards(P] & file_masks[sq]) == 0) && ((pos.bitboards(p] & file_masks[sq]) != 0))
                        // score += semi_open_file_score;
                    
                    break;

                case Q:
                    score += (QueenPosValue[MG][sq] * phase_score +
                            QueenPosValue[EG][sq] * (MidgameLimit - phase_score))/MidgameLimit;
                    
                    /// mobility @todo find a better way
                    // score += count_bits(get_queen_attacks(sq, occupancies[NO_COLOR]));

                    break;

                case K: 
                    score += (KingPosValue[MG][sq] * phase_score +
                            KingPosValue[EG][sq] * (MidgameLimit - phase_score))/MidgameLimit;

                    /// @todo
                    // pawn shield and pawn storm
                    // open file
                    // if (((pos.bitboards(P] | pos.bitboards(p]) & file_masks[sq]) == 0)
                        // score -= open_file_score;
                    // semi open file
                    // else if (((pos.bitboards(P] & file_masks[sq]) == 0) && ((pos.bitboards(p] & file_masks[sq]) != 0))
                        // score -= semi_open_file_score;

                    /// king safety bonus
                    score += count_bits(king_attacks[sq] & pos.occupancies(WHITE)) * king_shield_bonus;

                    break;

                // evaluate black pieces
                case p: 
                    score -= (PawnPosValue[MG][Eval::mirror_square(sq)] * phase_score +
                            PawnPosValue[EG][Eval::mirror_square(sq)] * (MidgameLimit - phase_score))/MidgameLimit;
                    
                    /// check double pawn
                    double_pawns = count_bits(pos.bitboards(p) & file_masks[sq]);
                    if (double_pawns > 1)
                        score -= double_pawns * double_pawn_penalty;

                    /// check isolated pawn that doesnt have protection 
                    if ((pos.bitboards(p) & isolated_masks[sq]) == 0)
                        score -= isolated_pawn_penalty;

                    /// check passed pawns 
                    if ((pawn_passed_masks[BLACK][sq] & pos.bitboards(P)) == 0)
                        score -= passed_pawn_bonus[(63-Eval::mirror_square(sq))/8];

                    break;

                case n: 
                    score -= (KnightPosValue[MG][Eval::mirror_square(sq)] * phase_score +
                            KnightPosValue[EG][Eval::mirror_square(sq)] * (MidgameLimit - phase_score))/MidgameLimit;
                    
                    break;

                case b: 
                    score -= (BishopPosValue[MG][Eval::mirror_square(sq)] * phase_score +
                            BishopPosValue[EG][Eval::mirror_square(sq)] * (MidgameLimit - phase_score))/MidgameLimit;

                    /// mobility of bishop
                    /// @todo find an efficient way to find sq threatened by bishops
                    score -= count_bits(get_bishop_attacks(sq, pos.occupancies(NO_COLOR)));
                    
                    break;

                case r: 
                    score -= (RookPosValue[MG][Eval::mirror_square(sq)] * phase_score +
                            RookPosValue[EG][Eval::mirror_square(sq)] * (MidgameLimit - phase_score))/MidgameLimit;
                    
                     // open file
                    if (((pos.bitboards(P) | pos.bitboards(p)) & file_masks[sq]) == 0)
                        score -= open_file_score;
                    // semi open file
                    // else if (((pos.bitboards(p] & file_masks[sq]) == 0) && ((pos.bitboards(P] & file_masks[sq]) != 0))
                        // score -= semi_open_file_score;
                    
                    break;

                case q:
                    score -= (QueenPosValue[MG][Eval::mirror_square(sq)] * phase_score +
                            QueenPosValue[EG][Eval::mirror_square(sq)] * (MidgameLimit - phase_score))/MidgameLimit;

                    // mobility
                    /// @todo find better way to evaluate
                    // score -= count_bits(get_queen_attacks(sq, occupancies[NO_COLOR]));

                    break;

                case k: 
                    score -= (KingPosValue[MG][Eval::mirror_square(sq)] * phase_score +
                            KingPosValue[EG][Eval::mirror_square(sq)] * (MidgameLimit - phase_score))/MidgameLimit;
                    
                    // pawn shield and pawn storm
                    // open file
                    // if (((pos.bitboards(P] | pos.bitboards(p]) & file_masks[sq]) == 0)
                        // score += open_file_score;
                    // semi open file
                    // else if (((pos.bitboards(p] & file_masks[sq]) == 0) && ((pos.bitboards(P] & file_masks[sq]) != 0))
                    //     score += semi_open_file_score;

                    // king safety bonus
                    score -= count_bits(king_attacks[sq] & pos.occupancies(BLACK)) * king_shield_bonus;

                    break;
            }

			pop_bit(bitboard, sq);
		}
	}

	return (pos.side()) ? -score : score;
}

} // namespace Eval

} // namespace Kojiro