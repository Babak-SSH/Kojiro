#include "bitboard.h"
#include "misc.h"
#include "thread.h"
#include <atomic>
#include <cstddef>
#include <istream>
#include <sstream>
#include <string>
#include <unistd.h>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "cons.h"
#include "logger.h"
#include "types.h"
#include "position.h"
#include "thread.h"


using std::string;

namespace Kojiro {

namespace Zobrist {
	uint64_t psq[12][64];
	uint64_t enpassant[64];
	uint64_t castling[16];
	uint64_t side;
}

/// logging board in a file with ascii(alphabet) represention.
void Position::log_board(){
    int sq;
    std::stringstream board;

    board << "board info:\n";
    for (int rank = 0; rank < 8; rank++){
        board << fmt::format("{} ", 8-rank);
        
        for (int file = 0; file < 8; file++){
            sq = rank * 8 + file;
            // define piece variable
            int piece = -1;
            
            // loop over all piece bitboards
            for (int bb_piece = W_PAWN; bb_piece <= B_KING; bb_piece++)
            {
                if (get_bit(bitboards(bb_piece), sq))
                    piece = bb_piece;
            }
            
            board << fmt::format(" {}", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        board << fmt::format("\n");
    }
    board << fmt::format("   a b c d e f g h\n\n");

      
    // print side to move
    board << fmt::format("   Side:     {}\n", !side() ? "white" : "black");

    // print enpassant square
    board << fmt::format("   Enpassant:   {}\n", (enpassant() != no_sq) ? convert_to_square[enpassant()] : "no");

    // print castling rights
    board << fmt::format("   Castling:  {}{}{}{}\n", (castle() & WK) ? 'K' : '-',
                                           (castle() & WQ) ? 'Q' : '-',
                                           (castle() & BK) ? 'k' : '-',
                                           (castle() & BQ) ? 'q' : '-');

    // print turn count
    board << fmt::format("   turn: {}\n", play_count());

    // print hash key of the position
    board << fmt::format("   hash key: {:x}\n", key());

    board << fmt::format("   fen: {}\n\n", get_fen());

    logger.logIt(board.str(), INFO);
}

/// generate "almost" unique position ID aka hash key for state of the game
/// with the help of the zobrist hash keys.
uint64_t Position::generate_hash_key() {
    // final hash key
    uint64_t final_key = 0ULL;
    
    // temp piece bitboard copy
    uint64_t bitboard;
    
    // loop over piece bitboards
    for (int piece = P; piece <= k; piece++)
    {
        // init piece bitboard copy
        bitboard = bitboards(piece);
        
        // loop over the pieces within a bitboard
        while (bitboard)
        {
            // init square occupied by the piece
            int square = get_ls1b_index(bitboard);
            
            // hash piece
            final_key ^= Zobrist::psq[piece][square];
            
            // pop LS1B
            pop_bit(bitboard, square);
        }
    }
    
    // if enpassant square is on board
    if (enpassant() != no_sq){
        // hash enpassant
        final_key ^= Zobrist::enpassant[enpassant()];
	}
    
    // hash castling rights
    final_key ^= Zobrist::castling[castle()];
    
    // hash the side only if black is to move
    if (side() == BLACK) final_key ^= Zobrist::side;
    
    // return generated hash key
    return final_key;
}

void Position::init_hash() {
	for(int bb_piece=W_PAWN;bb_piece <= B_KING;bb_piece++) {
		for(int sq=0; sq < 64; sq++){
			Zobrist::psq[bb_piece][sq] = get_random_U64_number();
		}
	}

	for(int sq=0; sq < 64; sq++) {
		Zobrist::enpassant[sq] = get_random_U64_number();
	}

	for(int i=0; i < 16; i++) {
		Zobrist::castling[i] = get_random_U64_number();
	}

	Zobrist::side = get_random_U64_number();
}

void Position::take_back() {
    if (st->previous != NULL)
        st = st->previous;
    ply--;
    repetitionIndex--;
}

string Position::get_fen() {
    std::stringstream fen;
    bool empty = true;
    int space = 0;

    // encode pieces
    for(int sq = 0; sq < 64; sq++) {
        for(int piece = W_PAWN; piece <= B_KING; piece++) {
            if (get_bit(st->bitboards[piece], sq)) {
                if (space > 0) {
                    fen << space;
                    space = 0;
                }
                fen << ascii_pieces[piece];
                empty = false;
            }
        }
        
        if (empty)
            space++;

        if (sq < 63 && (sq+1) % 8 == 0) {
            if (space > 0) {
                fen << space;
                space = 0;
            }
            fen << '/';
        }

        empty = true;
    }

    // encode side
    if (st->side)
        fen << " b ";
    else
        fen << " w ";

    // encode castling rights
    if ((st->castle & castlingRights(WK)) == castlingRights(WK))
        fen << 'K';
    if ((st->castle & castlingRights(WQ)) == castlingRights(WQ))
        fen << 'Q';
    if ((st->castle & castlingRights(BK)) == castlingRights(BK))
        fen << 'k';
    if ((st->castle & castlingRights(BQ)) == castlingRights(BQ))
        fen << 'q';
    if (!(st->castle & 15))
        fen << '-'; 

    fen << ' ';

    // encode enpassant
    if (st->enpassant != no_sq)
        fen << convert_to_square[st->enpassant] << ' ';
    else
        fen << "- ";

    // encode rule50 and play count
    fen << fmt::to_string(st->rule50);
    fen << ' ';
    fen << fmt::to_string((st->play_count+1)/2);
    
    return fen.str();
}

/// initialiaing the position and state for the current thread by parsing the given fen string.
Position& Position::parse_fen(const string& fen, StateInfo* state, Thread* th){
    memset(this, 0ULL, sizeof(Position));
    memset(state, 0, sizeof(StateInfo));
    memset(state->bitboards, 0ULL, sizeof(st->bitboards));
    memset(state->occupancies, 0ULL, sizeof(st->occupancies));
    st = state; 

    // reset game state variables
    st->side = WHITE;
    st->enpassant = no_sq;
    st->castle = 0;
    
    ply = 0;
    memset(repetitionTable, 0, sizeof(repetitionTable));
    repetitionIndex = 0;

    int square = 0, file, rank;
    std::size_t idx;
    unsigned char token;
    std::istringstream iss (fen);
    // don't skip whitespaces
    iss >> std::noskipws;

    while ((iss >> token) && !isspace(token)){
        if (isdigit(token)){
            square += token - '0'; // advance the given number of files
        }
        // consider '/' ?
        else {
            idx = ascii_pieces.find(token);
            if (idx != string::npos){
                set_piece(Piece(idx), square);
                square++;
            }
        }
        
    }

    // side
    iss >> token;
    st->side = (token == 'w' ? WHITE : BLACK);
    iss >> token;

    // castling
    while ((iss >> token) && !isspace(token)){
        if (token == 'K')
            st->castle |= castlingRights(WK);
        else if (token == 'Q'){
            st->castle |= castlingRights(WQ);
        }
        else if (token == 'k'){
            st->castle |= castlingRights(BK);
        }
        else if (token == 'q'){
            st->castle |= castlingRights(BQ);
        }
        else if (token == '-'){
			st->castle = 0;
		}
    }

    // enpassant
    iss >> token;
    if (token != '-'){
        file =  token - 'a';
        iss >> token;
        rank = 8 - (token - '0');
        st->enpassant = rank * 8 + file;
    }
    else {
        st->enpassant = no_sq;
    }

    iss >> token;
    iss >> st->rule50;
    iss >> token;
    iss >> st->play_count;

    st->key = Position::generate_hash_key();
    thisTh = th;

    return *this;
}

bool Position::is_check() {
    int square = (st->side == WHITE) ? 
        get_ls1b_index(st->bitboards[K]) : get_ls1b_index(st->bitboards[k]);

    return is_square_attacked(square, (st->side) ^ 1);
}

/// @todo enhance color(side) finding
void Position::set_piece(Piece p, int sq){
    set_bit(st->bitboards[p], sq);
    
    st->occupancies[(p > 5)] |= st->bitboards[p];
    st->occupancies[2] |= st->bitboards[p];
}


/// @todo check if assinging a variable is faster than getting it from structure.
/// change the state and position according to the given move if the move is legal and assigning 
/// the current state to the previous state so we can undo the move in take_back function.
int Position::make_move(int move, int move_flag, StateInfo& newSt, int depth){
    moveInfo m = decode_move(move);
    long captures_flag = 0;
    long enpassant_flag = 0;
    long castles_flag = 0;

	if(move_flag == 2 && !m.capture)
		return 0;

    if (get_bit(st->occupancies[st->side], m.source)){
        // preserving the previous state
        memcpy(&newSt, st, offsetof(StateInfo, previous));
        newSt.previous = st;
        st = &newSt;

        repetitionTable[repetitionIndex] = st->key;

        pop_bit(st->bitboards[m.piece], m.source);
		st->key ^= Zobrist::psq[m.piece][m.source];
        set_bit(st->bitboards[m.piece], m.target);
		st->key ^= Zobrist::psq[m.piece][m.target];

        if (m.capture){
            for (int bb_piece = W_PAWN + (6*!st->side); bb_piece <= 5 + (6*!st->side); bb_piece++){
                if (get_bit(st->bitboards[bb_piece], m.target)){
                    captures_flag = 1;

                    pop_bit(st->bitboards[bb_piece], m.target);
					st->key ^= Zobrist::psq[bb_piece][m.target];

                    break;
                }
            }
        }
        
        if (m.promoted){
            pop_bit(st->bitboards[(st->side) ? p : P], m.target);
			st->key ^= Zobrist::psq[(st->side) ? p : P][m.target];
            set_bit(st->bitboards[(st->side) ? m.promoted + 6: m.promoted], m.target);
			st->key ^= Zobrist::psq[(st->side) ? m.promoted + 6: m.promoted][m.target];
        }

        if (m.enpassant){
            (st->side) ? pop_bit(st->bitboards[P], m.target - 8) :
                    pop_bit(st->bitboards[p], m.target + 8);

			(st->side) ? st->key ^= Zobrist::psq[P][m.target - 8] :
					st->key ^= Zobrist::psq[p][m.target + 8];

            enpassant_flag = 1;
            captures_flag = 1;
        }

		if(st->enpassant != no_sq) {
			st->key ^= Zobrist::enpassant[st->enpassant];
		}

        st->enpassant = no_sq;

        if (m.double_push){
            if (((not_H_file&(1<<m.target)) && (st->bitboards[(st->side) ? P : p] & (1ULL << (m.target+1)))) || 
                ((not_A_file&(1<<m.target)) && (st->bitboards[(st->side) ? P : p] & (1ULL << (m.target-1))))) {
                    st->enpassant = (st->side) ? m.target - 8 : m.target + 8;

			        (st->side) ? st->key ^= Zobrist::enpassant[m.target - 8] :
                        st->key ^= Zobrist::enpassant[m.target + 8];
            }
        }

        if (m.castling){
            // switch target square
            switch (m.target)
            {
                // white castles king side
                case (g1):
                    // move H rook
                    pop_bit(st->bitboards[R], h1);
					st->key ^= Zobrist::psq[R][h1];
                    set_bit(st->bitboards[R], f1);
					st->key ^= Zobrist::psq[R][f1];
                    break;
                
                // white castles queen side
                case (c1):
                    // move A rook
                    pop_bit(st->bitboards[R], a1);
					st->key ^= Zobrist::psq[R][a1];
                    set_bit(st->bitboards[R], d1);
					st->key ^= Zobrist::psq[R][d1];
                    break;
                
                // black castles king side
                case (g8):
                    // move H rook
                    pop_bit(st->bitboards[r], h8);
					st->key ^= Zobrist::psq[r][h8];
                    set_bit(st->bitboards[r], f8);
					st->key ^= Zobrist::psq[r][f8];
                    break;
                
                // black castles queen side
                case (c8):
                    // move A rook
                    pop_bit(st->bitboards[r], a8);
					st->key ^= Zobrist::psq[r][a8];
                    set_bit(st->bitboards[r], d8);
					st->key ^= Zobrist::psq[r][d8];
                    break;
            } 
            castles_flag = 1;
        }

        // update castling rights
		st->key ^= Zobrist::castling[st->castle];
        st->castle &= castling_rights[m.source];
        st->castle &= castling_rights[m.target];
		st->key ^= Zobrist::castling[st->castle];
        
        // reset occupancies
        memset(st->occupancies, 0ULL, 24);
        
        // loop over white pieces bitboards
        for (int bb_piece = P; bb_piece <= K; bb_piece++)
            // update white occupancies
            st->occupancies[WHITE] |= st->bitboards[bb_piece];

        // loop over black pieces bitboards
        for (int bb_piece = p; bb_piece <= k; bb_piece++)
            // update black occupancies
            st->occupancies[BLACK] |= st->bitboards[bb_piece];

        // update both sides occupancies
        st->occupancies[NO_COLOR] |= st->occupancies[WHITE];
        st->occupancies[NO_COLOR] |= st->occupancies[BLACK];
        
        // change side
        st->side ^= 1;
		st->key ^= Zobrist::side;
        st->play_count++;
        
        // make sure that king has not been exposed into a check
        if (is_square_attacked((st->side == WHITE) ? 
            get_ls1b_index(st->bitboards[k]) : get_ls1b_index(st->bitboards[K]), st->side)){
            // take move back
            ply++; /// @todo change it its dogshit.
            repetitionIndex++;
            take_back();
            
            captures_flag=0;
            enpassant_flag = 0;
            castles_flag = 0;

            // return illegal move
            return 0;
        }
        else {
            if (depth == 1 && move_flag == 1) {  
                if (captures_flag)
                    thisTh->capture_count++;
                if (enpassant_flag)
                    thisTh->enpassant_count++;
                if (castles_flag)
                    thisTh->castle_count++;
            }
            
            thisTh->nodes.fetch_add(1, std::memory_order_relaxed);
            ply++;
            repetitionIndex++;

            return 1;
        }
    }
    else{
        // not this sides turn
        return 0;
    }
}

} // namespace Kojiro