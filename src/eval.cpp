#include "eval.h"

int Eval::evaluation(){
	int score = 0;
	int sq;
	uint64_t bitboard;

	for(int bb_piece=W_PAWN;bb_piece < B_KING;bb_piece++){
		bitboard = st->bitboards[bb_piece];

		while (bitboard)
		{
			sq = get_ls1b_index(bitboard);

			score += material_score[bb_piece];

			// score positional piece scores
            switch (bb_piece)
            {
                // evaluate white pieces
                case P: score += pawn_score[sq]; break;
                case N: score += knight_score[sq]; break;
                case B: score += bishop_score[sq]; break;
                case R: score += rook_score[sq]; break;
                case K: score += king_score[sq]; break;

                // evaluate black pieces
                case p: score -= pawn_score[Eval::mirror_square(sq)]; break;
                case n: score -= knight_score[Eval::mirror_square(sq)]; break;
                case b: score -= bishop_score[Eval::mirror_square(sq)]; break;
                case r: score -= rook_score[Eval::mirror_square(sq)]; break;
                case k: score -= king_score[Eval::mirror_square(sq)]; break;
            }

			pop_bit(bitboard, sq);
		}
	}
	return (st->side) ? -score : score;
}