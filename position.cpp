#include <sstream>

#include "position.h"


using std::string;

// ASCII pieces
const string ascii_pieces("PNBRQKpnbrqk");

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
                if (get_bit(bitboards[bb_piece], sq))
                    piece = bb_piece;
            }
            
            printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        printf("\n");
    }
    printf("   a b c d e f g h\n\n");

      
    // print side to move
    printf("     Side:     %s\n", !side ? "white" : "black");
    
    // print enpassant square
    printf("     Enpassant:   %s\n", (enpassant != no_sq) ? convert_to_square[enpassant] : "no");
    
    // print castling rights
    printf("     Castling:  %c%c%c%c\n\n", (castle & WK) ? 'K' : '-',
                                           (castle & WQ) ? 'Q' : '-',
                                           (castle & BK) ? 'k' : '-',
                                           (castle & BQ) ? 'q' : '-');
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

std::string get_fen(){
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

    int square = 0, idx;
    unsigned char token;
    std::istringstream iss (fen);
    // don't skip whitespaces
    iss >> std::skipws;

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
        
    }
}
