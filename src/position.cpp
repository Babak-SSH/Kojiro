#include <sstream>

#include "position.h"


using std::string;

namespace Kojiro {

namespace Zobrist {
	uint64_t psq[12][64];
	uint64_t enpassant[64];
	uint64_t castling[16];
	uint64_t side;
}

// ASCII pieces
const string ascii_pieces("PNBRQKpnbrqk");

StateInfo* st;

// piece bitboards
uint64_t bitboards[12];

// occupancy bitboards
uint64_t occupancies[3];

// side to move
int side = WHITE;

// enpassant square
int enpassant = no_sq; 

// castling rights
int castle = 0;

int rule50 = 0;

int play_count = 0;

void print_board(){
    int sq;
    for (int rank = 0; rank < 8; rank++){
        printf("%d ", 8-rank);
        for (int file = 0; file < 8; file++){
            sq = rank * 8 + file;
            // define piece variable
            int piece = -1;
            
            // loop over all piece bitboards
            for (int bb_piece = W_PAWN; bb_piece <= B_KING; bb_piece++)
            {
                if (get_bit(st->bitboards[bb_piece], sq))
                    piece = bb_piece;
            }
            
            printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        printf("\n");
    }
    printf("   a b c d e f g h\n\n");

      
    // print side to move
    printf("     Side:     %s\n", !st->side ? "white" : "black");
    
    // print enpassant square
    printf("     Enpassant:   %s\n", (st->enpassant != no_sq) ? convert_to_square[st->enpassant] : "no");
    
    // print castling rights
    printf("     Castling:  %c%c%c%c\n", (st->castle & WK) ? 'K' : '-',
                                           (st->castle & WQ) ? 'Q' : '-',
                                           (st->castle & BK) ? 'k' : '-',
                                           (st->castle & BQ) ? 'q' : '-');

    // print turn count
    printf("     turn: %d\n\n", st->play_count);
	printf("\nhash: %llx\n", generate_hash_key());
}

// generate "almost" unique position ID aka hash key from scratch
uint64_t generate_hash_key()
{
    // final hash key
    uint64_t final_key = 0ULL;
    
    // temp piece bitboard copy
    uint64_t bitboard;
    
    // loop over piece bitboards
    for (int piece = P; piece <= k; piece++)
    {
        // init piece bitboard copy
        bitboard = st->bitboards[piece];
        
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
    if (enpassant != no_sq)
        // hash enpassant
        final_key ^= Zobrist::enpassant[enpassant];
    
    // hash castling rights
    final_key ^= Zobrist::castling[castle];
    
    // hash the side only if black is to move
    if (side == BLACK) final_key ^= Zobrist::side;
    
    // return generated hash key
    return final_key;
}

void init_hash() {
	// random seed. 1804289383 is the first number generated with rand() function in "linux"
	unsigned int seed_state = 1804289383;
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

void init_start(){
    // set black pawns
    set_bit(bitboards[W_PAWN], a2);
    set_bit(bitboards[W_PAWN], b2);
    set_bit(bitboards[W_PAWN], c2);
    set_bit(bitboards[W_PAWN], d2);
    set_bit(bitboards[W_PAWN], e2);
    set_bit(bitboards[W_PAWN], f2);
    set_bit(bitboards[W_PAWN], g2);
    set_bit(bitboards[W_PAWN], h2);
    
    // set black knights
    set_bit(bitboards[W_KNIGHT], b1);
    set_bit(bitboards[W_KNIGHT], g1);
    
    // set black bishops
    set_bit(bitboards[W_BISHOP], c1);
    set_bit(bitboards[W_BISHOP], f1);
    
    // set black rooks
    set_bit(bitboards[W_ROOK], a1);
    set_bit(bitboards[W_ROOK], h1);
    
    // set black queen & king
    set_bit(bitboards[W_QUEEN], d1);
    set_bit(bitboards[W_KING], e1);
    
    // set white pawns
    set_bit(bitboards[B_PAWN], a7);
    set_bit(bitboards[B_PAWN], b7);
    set_bit(bitboards[B_PAWN], c7);
    set_bit(bitboards[B_PAWN], d7);
    set_bit(bitboards[B_PAWN], e7);
    set_bit(bitboards[B_PAWN], f7);
    set_bit(bitboards[B_PAWN], g7);
    set_bit(bitboards[B_PAWN], h7);
    
    // set white knights
    set_bit(bitboards[B_KNIGHT], b8);
    set_bit(bitboards[B_KNIGHT], g8);
    
    // set white bishops
    set_bit(bitboards[B_BISHOP], c8);
    set_bit(bitboards[B_BISHOP], f8);
    
    // set white rooks
    set_bit(bitboards[B_ROOK], a8);
    set_bit(bitboards[B_ROOK], h8);
    
    // set white queen & king
    set_bit(bitboards[B_QUEEN], d8);
    set_bit(bitboards[B_KING], e8);
    
    // init side
    side = WHITE;
    
    // init enpassant
    enpassant = no_sq;
    
    // init castling
    castle |= WK;
    castle |= WQ;
    castle |= BK;
    castle |= BQ;
}

void init_state(){
    st = new StateInfo();

    memcpy(st->bitboards, bitboards, 96);
    memcpy(st->occupancies, occupancies, 24);
    
    st->castle = castle;
    st->enpassant = enpassant;
    st->rule50 = rule50;
    st->side = side;
    st->play_count = play_count;

    st->previous = NULL;
}

void take_back(){
    if (st->previous != NULL)
        st = st->previous;
}

string get_fen(){
    return "";
}

void parse_fen(const string& fen){
    // reset board position (bitboards)
    memset(bitboards, 0ULL, sizeof(bitboards));
    
    // reset occupancies (bitboards)
    memset(occupancies, 0ULL, sizeof(occupancies));
    
    // reset game state variables
    side = WHITE;
    enpassant = no_sq;
    castle = 0;

    int square = 0, idx, file, rank;
    unsigned char token;
    std::istringstream iss (fen);
    // don't skip whitespaces
    iss >> std::noskipws;

    while ((iss >> token) && !isspace(token)){
        if (isdigit(token)){
            square += token - '0'; // advance the given number of files
        }
        // consider '/' ?
        else if ((idx = ascii_pieces.find(token)) != string::npos){
            set_piece(Piece(idx), square);
            square++;
        }
    }
    
    // side
    iss >> token;
    side = (token == 'w' ? WHITE : BLACK);
    iss >> token;

    //castling
    while ((iss >> token) && !isspace(token)){
        if (token == 'K')
            castle |= castlingRights(WK);
        else if (token == 'Q'){
            castle |= castlingRights(WQ);
        }
        else if (token == 'k'){
            castle |= castlingRights(BK);
        }
        else if (token == 'q'){
            castle |= castlingRights(BQ);
        }
		else if (token == '-'){
			castle = 0;
		}
    }

    iss >> token;
    if (token != '-'){
        file =  token - 'a';
        iss >> token;
        rank = 8 - (token - '0');
        enpassant = rank * 8 + file;
    }
    else {
        enpassant == no_sq;
    }

    iss >> token;
    iss >> rule50;
    iss >> token;
    iss >> play_count;
}

/// @todo get_attacks function with piece parameter to get rid of army of ifs.(maybe template)
bool is_square_attacked(int square, int side){
    // pawn
    if (pawn_attacks[!side][square] & st->bitboards[P+(6*side)]) return 1;
    // knight
    if (knight_attacks[square] & st->bitboards[N+(6*side)]) return 1;
    // bishop
    if (get_bishop_attacks(square, st->occupancies[NO_COLOR]) & st->bitboards[B+(6*side)]) return 1;
    // rook
    if (get_rook_attacks(square, st->occupancies[NO_COLOR]) & st->bitboards[R+(6*side)]) return 1;
    // queen
    if (get_queen_attacks(square, st->occupancies[NO_COLOR]) & st->bitboards[Q+(6*side)]) return 1;
    // king
    if (king_attacks[square] & st->bitboards[K+(6*side)]) return 1;

    return 0;
}
long captures_count = 0;
long captures_flag = 0;
long enpassant_count = 0;
long enpassant_flag = 0;
long castles_count = 0;
long castles_flag = 0;
/// @todo check if assinging a variable is faster than getting it from structure.
int make_move(int move, int move_flag, StateInfo& newST){
    moveInfo m = decode_move(move);

	if(move_flag == 2 && !m.capture)
		return 0;

    if (get_bit(st->occupancies[st->side], m.source)){
        // preserving the previous state
        memcpy(&newST, st, offsetof(StateInfo, previous));

        newST.previous = st;

        st = &newST;

        pop_bit(newST.bitboards[m.piece], m.source);
        set_bit(newST.bitboards[m.piece], m.target);

        if (m.capture){
            for (int bb_piece = W_PAWN + (6*!newST.side); bb_piece <= 5 + (6*!newST.side); bb_piece++){
                if (get_bit(newST.bitboards[bb_piece], m.target)){
                    captures_flag = 1;
                    pop_bit(newST.bitboards[bb_piece], m.target);
                    break;
                }
            }
        }
        
        if (m.promoted){
            pop_bit(newST.bitboards[(newST.side) ? p : P], m.target);
            set_bit(newST.bitboards[(newST.side) ? m.promoted + 6: m.promoted], m.target);
        }

        if (m.enpassant){
            (newST.side) ? pop_bit(newST.bitboards[P], m.target - 8) :
                    pop_bit(newST.bitboards[p], m.target + 8);
            enpassant_flag = 1;
        }

        newST.enpassant = no_sq;

        if (m.double_push){
            (newST.side) ? (newST.enpassant = m.target - 8) :
                    (newST.enpassant = m.target + 8);
        }

        if (m.castling){
            // switch target square
            switch (m.target)
            {
                // white castles king side
                case (g1):
                    // move H rook
                    pop_bit(newST.bitboards[R], h1);
                    set_bit(newST.bitboards[R], f1);
                    break;
                
                // white castles queen side
                case (c1):
                    // move A rook
                    pop_bit(newST.bitboards[R], a1);
                    set_bit(newST.bitboards[R], d1);
                    break;
                
                // black castles king side
                case (g8):
                    // move H rook
                    pop_bit(newST.bitboards[r], h8);
                    set_bit(newST.bitboards[r], f8);
                    break;
                
                // black castles queen side
                case (c8):
                    // move A rook
                    pop_bit(newST.bitboards[r], a8);
                    set_bit(newST.bitboards[r], d8);
                    break;
            } 
            castles_flag = 1;
        }
        // update castling rights
        newST.castle &= castling_rights[m.source];
        newST.castle &= castling_rights[m.target];
        
        // reset occupancies
        memset(newST.occupancies, 0ULL, 24);
        
        // loop over white pieces bitboards
        for (int bb_piece = P; bb_piece <= K; bb_piece++)
            // update white occupancies
            newST.occupancies[WHITE] |= newST.bitboards[bb_piece];

        // loop over black pieces bitboards
        for (int bb_piece = p; bb_piece <= k; bb_piece++)
            // update black occupancies
            newST.occupancies[BLACK] |= newST.bitboards[bb_piece];

        // update both sides occupancies
        newST.occupancies[NO_COLOR] |= newST.occupancies[WHITE];
        newST.occupancies[NO_COLOR] |= newST.occupancies[BLACK];
        
        // change side
        st->side ^= 1;
        st->play_count++;
        
        // make sure that king has not been exposed into a check
        if (is_square_attacked((st->side == WHITE) ? get_ls1b_index(st->bitboards[k]) : get_ls1b_index(st->bitboards[K]), st->side)){
            // take move back
            take_back();
            // printf("checked can't move\n");
            
            captures_flag=0;
            enpassant_flag = 0;
            castles_flag = 0;

            // return illegal move
            return 0;
        }
        else{
            if (captures_flag && move_flag==1)
                captures_count++;
            if (enpassant_flag && move_flag==1)
                enpassant_count++;
            if (castles_flag && move_flag==1)
                castles_count++;

            captures_flag=0;
            enpassant_flag = 0;
            castles_flag = 0;
            return 1;
        }
    }
    else{
        captures_flag=0;
		enpassant_flag = 0;
		castles_flag = 0;
		
        // not this sides turn
        return 0;
    }
}

} // namespace Kojiro